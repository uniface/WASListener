// WASListener.cpp : Defines the entry point for the application.
//
#include <Windows.h>
#include <future>
#include <tchar.h>
#include "Notifications.h"
#include "WASListener.h"

#include <iostream>

#include "CommandLine.h"
#include "FolderWatcher.h"
#include "Uniface.h"

#include <boost/thread/concurrent_queues/sync_deque.hpp>

constexpr auto MAX_LOADSTRING = 100;

// TODO: Globals should be constant or they shouldn't be globals
// Global Variables:
HINSTANCE hInst; // current instance
WCHAR szTitle[MAX_LOADSTRING]; // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance) noexcept;
BOOL InitInstance(HINSTANCE, int) noexcept;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM) noexcept;
void ShowContextMenu(HWND, POINT) noexcept;
int parseCommandLine(LPWSTR);
// TODO: Globals should be constant or they shouldn't be globals
std::unique_ptr<CFolderWatcher> watcher;
std::thread workerThreads[2];
auto const list(std::make_shared<boost::concurrent::sync_deque<CFileAction>>());

int APIENTRY wWinMain(_In_ HINSTANCE const hInstance,
                      _In_opt_ HINSTANCE const,
                      _In_ LPWSTR const,
                      _In_ int const nShowCmd)
{
	// Initialize global strings
	hInst = hInstance;
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WASLISTENER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nShowCmd))
	{
		return -1;
	}

	const auto hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WASLISTENER));

	MSG msg;

	CCommandLine commandLine;
	try
	{
		//Creating a threads to execute our tasks
		commandLine.parse(GetCommandLineA());
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Error", MB_OK);
		// TODO: Use return value
		gNotifications.DeleteNotificationIcon();
		return -1;
	}

	// Setup our thread for processing the WorkArea updates in Uniface
	auto workArea_promise{ std::make_shared<std::promise<std::wstring>>() }; // Uniface will return the location of the WorkArea

	workerThreads[0] = std::thread([workArea_promise, uniface{ std::make_unique<CUniface>(std::move(commandLine), list) }] ()
	{
		// Getting the working folder will make a call to Uniface so starting the whole Uniface environment
		workArea_promise->set_value(uniface->getWASFolder());
		uniface->run();
		std::cout << "Finished processing actions\n";
	});

	// Setup our thread for watching the WorkArea
	watcher = std::make_unique<CFolderWatcher>(list);
	workerThreads[1] = std::thread([workArea_promise{ std::move(workArea_promise) }]()
	{
		try
		{
			// Wait for Uniface to return the location of the WorkArea
			watcher->setFolder(workArea_promise->get_future().get());
		}
		catch (const std::exception& e)
		{
			MessageBoxA(nullptr, e.what(), "Error", MB_OK);
		}
		watcher->run(); // Start listening for changes in the WorkArea
		std::cout << "Finished watching\n";
	});


	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return static_cast<int>(msg.wParam);
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance) noexcept
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WASLISTENER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WASLISTENER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE const hInstance, int) noexcept
{
	hInst = hInstance; // Store instance handle in our global variable

	return CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr) ? TRUE : FALSE;
}

void ShowContextMenu(HWND const hwnd, POINT const pt) noexcept
{
	const auto hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDC_CONTEXTMENU));
	if (hMenu)
	{
		const auto hSubMenu = GetSubMenu(hMenu, 0);
		if (hSubMenu)
		{
			// our window must be foreground before calling TrackPopupMenu or the menu will not disappear when the user clicks away
			SetForegroundWindow(hwnd);

			// respect menu drop alignment
			UINT uFlags = TPM_RIGHTBUTTON;
			if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
			{
				uFlags |= TPM_RIGHTALIGN;
			}
			else
			{
				uFlags |= TPM_LEFTALIGN;
			}

			TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, hwnd, nullptr);
		}
		DestroyMenu(hMenu);
	}
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND const hWnd, UINT const message, WPARAM const wParam, LPARAM const lParam)
{
	switch (message)
	{
	case WM_CREATE:

		try
		{
			// create a lock in the current folder to prevent multiple instances
			auto const hFileLock = CreateFile(L".\\.WASListener.lock",
			                       GENERIC_WRITE,
			                       FILE_SHARE_READ,
			                       nullptr,
			                       CREATE_NEW,
			                       FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE |
			                       FILE_ATTRIBUTE_SYSTEM,
			                       nullptr);
			if (hFileLock == INVALID_HANDLE_VALUE)
				throw std::runtime_error{ "Unable to create lock. Another instance is probably running already" };

			// add the notification icon
			if (!gNotifications.AddNotificationIcon(hWnd, 100))
				throw std::runtime_error{ "Unable to register notification icon" };
		}
		catch (const std::runtime_error& msg)
		{
			MessageBoxA(hWnd, msg.what(), "Error", MB_OK);
			return -1;
		}
		break;
	case WM_COMMAND:
		{
			const int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WMAPP_NOTIFYCALLBACK:
		{
			if (LOWORD(lParam) == WM_CONTEXTMENU)
			{
				POINT const pt = {LOWORD(wParam), HIWORD(wParam)};
				ShowContextMenu(hWnd, pt);
			}
			break;
		}
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			/* HDC hdc = */
			BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		// Stop the processing
		list->close();
		workerThreads[0].join();
		workerThreads[1].join();
		watcher.reset();

		// TODO: Use return value
		gNotifications.DeleteNotificationIcon();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND const hDlg, UINT const message, WPARAM const wParam, LPARAM) noexcept
{
	switch (message)
	{
	case WM_INITDIALOG:
		return static_cast<INT_PTR>(TRUE);

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return static_cast<INT_PTR>(TRUE);
		}
		break;
	}
	return static_cast<INT_PTR>(FALSE);
}
