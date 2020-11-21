#include "FolderWatcher.h"

#include <iostream>
#include <string>

using namespace std::string_literals;

constexpr auto buffer_size = 4096U;

CFolderWatcher::CFolderWatcher(std::shared_ptr<boost::concurrent::sync_deque<CFileAction>> list) :
	m_fileActionList{std::move(list)}
{
	m_buffer.assign(buffer_size, 0);
}
void CFolderWatcher::setFolder(std::wstring&& folder) noexcept
{
	m_folder = std::move(folder);
}

void CFolderWatcher::run()
{
	std::cout << "Task Start\n";
	// Check if thread is requested to stop ?
	OVERLAPPED ovl;
	ovl.Internal = 0;
	ovl.InternalHigh = 0;
	ovl.hEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
	ovl.Pointer = nullptr;
	ovl.Offset = 0;
	ovl.OffsetHigh = 0;

	if (!OpenDirectory() || ovl.hEvent == nullptr)
		throw std::runtime_error{ "Fails to open the work area folder for listening" };

	ResetEvent(ovl.hEvent);

	if (!ReadDirectoryChangesW(m_hDirectory,
							   m_buffer.data(), static_cast<DWORD>(m_buffer.size()),
							   TRUE,
	                           FILE_NOTIFY_CHANGE_LAST_WRITE
	                           | FILE_NOTIFY_CHANGE_CREATION
	                           | FILE_NOTIFY_CHANGE_FILE_NAME, // filter conditions
	                           nullptr, &ovl, nullptr))
	{
		auto const error = GetLastError();
		if (error != ERROR_IO_PENDING)
		{
			throw std::runtime_error{ "Unknown error from ReadDirectoryChangesW: "s + std::to_string(error) };
		}
	}

	while (!m_fileActionList->closed())
	{
		if (ovl.hEvent == nullptr)
		{
			throw std::runtime_error{ "ovl.hEvent = nullptr" };
		}

		if (WaitForSingleObject(ovl.hEvent, 100) == WAIT_OBJECT_0)
		{
			DWORD dw;
			GetOverlappedResult(m_hDirectory, &ovl, &dw, FALSE);
			ProcessNotification();
			ResetEvent(ovl.hEvent);

			ReadDirectoryChangesW(m_hDirectory,
			                      m_buffer.data(), static_cast<DWORD>(m_buffer.size()),
			                      TRUE,
			                      FILE_NOTIFY_CHANGE_LAST_WRITE
			                      | FILE_NOTIFY_CHANGE_CREATION
			                      | FILE_NOTIFY_CHANGE_FILE_NAME, // filter conditions
			                      nullptr, &ovl, nullptr);
		}
	}
}

bool CFolderWatcher::OpenDirectory() noexcept
{
	// Allow this routine to be called redundantly.
	if (m_hDirectory)
		return true;

	m_hDirectory = CreateFileW(
		m_folder.c_str(), // Folder to Watch
		FILE_LIST_DIRECTORY, // access (read/write) mode
		FILE_SHARE_READ // share mode
		| FILE_SHARE_WRITE
		| FILE_SHARE_DELETE,
		nullptr, // security descriptor
		OPEN_EXISTING, // how to create
		FILE_FLAG_BACKUP_SEMANTICS // file attributes
		| FILE_FLAG_OVERLAPPED,
		nullptr); // file with attributes to copy

	return m_hDirectory != INVALID_HANDLE_VALUE;
}

void CFolderWatcher::ProcessNotification()
{
	auto* p = m_buffer.data();
	for (;;)
	{
		auto const* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(p);
		auto const status{ m_fileActionList->try_push_back(
			CFileAction(info->Action, std::wstring{ info->FileName, info->FileNameLength / sizeof(wchar_t) })
		) };
		if (status == boost::queue_op_status::closed || !info->NextEntryOffset)
		{
			break;
		}

		p += info->NextEntryOffset;
	}
	m_buffer.assign(buffer_size, 0);
}
