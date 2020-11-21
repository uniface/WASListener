#pragma once

#include "h3gl.h"
#include <mutex>
#include <filesystem>
#include "CommandLine.h"
#include <boost/thread/concurrent_queues/sync_deque.hpp>
#include "FileAction.h"

class CUniface
{
public:
	CUniface(CCommandLine&& pCmdLine, std::shared_ptr<boost::concurrent::sync_deque<CFileAction>> fileActionList) noexcept;
	CUniface(CUniface&& other) noexcept = default;

	void run();
	std::wstring const& getWASFolder();

private:

	void addFileAction(CFileAction const& fileAction);

	//Make a call
	std::wstring const& _getWASFolder();
	DWORD _callUnifaceVC(std::filesystem::path const& file, BOOL bDelete, BOOL bImport);
	void _stopUniface() noexcept;
	void _startUniface();
	void _resetFileInfo() noexcept;

	//Uniface Environment
	bool m_bStarted{false};
	UHACT m_hUniEnv{nullptr};
	UHACT m_hIn{nullptr};

	// Concurrency
	std::shared_ptr<boost::concurrent::sync_deque<CFileAction>> m_fileActionList;

	std::wstring m_workAreaFolder;
	bool m_bDeleted{false};
	bool m_bImported{false};
	std::filesystem::path m_lastFileActioned;
	CCommandLine m_pCmdLine;
	std::wstring CUniface::s2ws(const std::string& str);
};
