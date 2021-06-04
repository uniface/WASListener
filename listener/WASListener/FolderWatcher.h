#pragma once

#include <future>

//#include "TDequeConcurrent.h"
#include <boost/thread/concurrent_queues/sync_deque.hpp>
#include "FileAction.h"

class CFolderWatcher
{
public:
	explicit CFolderWatcher(std::shared_ptr<boost::concurrent::sync_deque<CFileAction>> list);

	// Function to be executed by thread function
	void run(std::wstring&& folder);

private:
	bool OpenDirectory() noexcept;
	void ProcessNotification();

private:
	std::wstring m_folder;
	HANDLE m_hDirectory{nullptr};
	std::vector<BYTE> m_buffer;
	std::shared_ptr<boost::concurrent::sync_deque<CFileAction>> m_fileActionList;
};
