#include "Notifications.h"

#include "resource.h"


void CNotifications::ShowDirty(std::wstring const& filename) const noexcept
{
	// Display a balloon message for a print job with a custom icon
	NOTIFYICONDATA nid{};
	nid.cbSize = sizeof nid;
	nid.uFlags = NIF_INFO;
	nid.hWnd = m_hWnd;
	nid.uID = m_uID;
	nid.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON;
	LoadString(HINST_THISCOMPONENT, IDS_DIRTY_TITLE, nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle));
	wcsncpy_s(nid.szInfo, filename.c_str(), filename.length());
	LoadIconMetric(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDI_WASLISTENER), LIM_LARGE, &nid.hBalloonIcon);
	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

BOOL CNotifications::AddNotificationIcon(HWND const lhWnd, DWORD const luID) noexcept
{
	m_hWnd = lhWnd;
	m_uID = luID;

	// Declare NOTIFYICONDATA details.
	NOTIFYICONDATA nid {};
	nid.cbSize = sizeof nid;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP;
	nid.hWnd = m_hWnd;
	nid.uID = m_uID;
	nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;

	// Set the tooltip to be the working folder
	GetCurrentDirectory(ARRAYSIZE(nid.szTip), nid.szTip);

	// Load the icon for high DPI.
	LoadIconMetric(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDI_SMALL), LIM_SMALL, &nid.hIcon);

	// Show the notification.
	Shell_NotifyIcon(NIM_ADD, &nid);

	// NOTIFYICON_VERSION_4 is prefered
	nid.uVersion = NOTIFYICON_VERSION_4;
	return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

void CNotifications::DeleteNotificationIcon() const noexcept
{
	NOTIFYICONDATA nid{};
	nid.cbSize = sizeof nid;
	nid.hWnd = m_hWnd;
	nid.uID = m_uID;
	Shell_NotifyIcon(NIM_DELETE, &nid);
}
