#include "Uniface.h"

#include <iostream>
#include "Notifications.h"

using namespace std::string_literals;

void CUniface::_resetFileInfo() noexcept
{
	// Clear down the last status
	m_lastFileActioned.clear();
	m_bDeleted = false;
	m_bImported = false;
}

// Function to be executed by thread function
void CUniface::run()
{
	if (!m_bStarted)
	{
		// It is expected that Uniface would be running by now. 
		// Uniface should bave been started by a call to getWASFolder()
		throw std::runtime_error("Uniface is not started");
	}

	while(!m_fileActionList->closed())
	{
		std::this_thread::yield();

		// Close the Uniface transcript when the queue ia empty
		if (m_fileActionList->empty())
		{
			HWND window = GetActiveWindow();
			if (window)
				SendMessage(window, WM_CLOSE, 0, 0);

			_resetFileInfo();
		}

		// Process the actions. pull_front() will wait for new actions if the list is empty
		try
		{
			addFileAction(m_fileActionList->pull_front());
		}
		catch(boost::sync_queue_is_closed)
		{ 
			// The listener queue has been closed so we are exiting the application
			_stopUniface();
		}
	}
}

std::wstring const& CUniface::getWASFolder()
{
	try
	{
		_startUniface();
		return _getWASFolder();
	}
	catch (...)
	{
		_stopUniface();
		throw;
	}
}

CUniface::CUniface(CCommandLine&& pCmdLine,
                   std::shared_ptr<boost::concurrent::sync_deque<CFileAction>> fileActionList) noexcept
	: m_fileActionList(std::move(fileActionList)), m_pCmdLine(std::move(pCmdLine))
{
}

void CUniface::addFileAction(CFileAction const& fileAction)
{
	auto const file{std::filesystem::path{m_workAreaFolder} / fileAction.m_file};

	// We are not interested in modifications to a folder ( Create and delete can be usefull)
	if (fileAction.m_action != FILE_ACTION_MODIFIED || !is_directory(file))
	{
		auto l_bDelete = false;
		auto l_bImport = false;
		// Reset the lags if this is a new fileAction
		if (file != m_lastFileActioned)
			_resetFileInfo();

		// Decide if there should be a delete, import of both
		if (fileAction.m_action == FILE_ACTION_ADDED)
			l_bImport = true;
		else if (fileAction.m_action == FILE_ACTION_REMOVED)
			l_bDelete = true;
		else if (fileAction.m_action == FILE_ACTION_MODIFIED)
		{
			l_bDelete = true;
			l_bImport = true;
		}

		// If the fileToAction has already been imported or deleted then don't do it again
		if (m_bDeleted) // Already deleted
			l_bDelete = false;
		if (m_bImported)
			l_bImport = false;

		// Remember what has been done alrady 
		if (l_bDelete)
			m_bDeleted = true;
		if (l_bImport)
		{
			// If we are importing then we don't want to delete later
			m_bDeleted = true;
			m_bImported = true;
		}

		// call Uniface if we need to
		if (l_bDelete || l_bImport)
			_callUnifaceVC(file, l_bDelete ? TRUE : FALSE, l_bImport ? TRUE : FALSE);
	}
}

std::wstring CUniface::s2ws(const std::string& str)
{
	const int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstrTo[0], size_needed);
	return wstrTo;
}

/* -- External functions */
std::wstring const& CUniface::_getWASFolder()
{
	long procError{ 0 };
	long oprStatus{ -1 };
	UHACT hOp{nullptr};
	UCHAR* tempFilePath{nullptr};

	if (!m_workAreaFolder.empty())
		return m_workAreaFolder;

	UCHAR get_was_folder[] = "GETWASFOLDER";
	auto status = UINSTOPR(m_hIn, /* instance handle */
	                       get_was_folder, /* operation name */
	                       0, /* reserved for future use */
	                       &hOp); /* operation handle */

	/* Activate operation */
	if (status == UACT_SUCCESS)
	{
		status = UOPRACT(hOp, /* operation handle */
		                 &oprStatus, /* $status */
		                 &procError); /* $procerror */

		/* Retrieve parameters */
		if (status == UACT_SUCCESS && oprStatus == 0)
		{
			status = UUF2STR(hOp, 1, &tempFilePath);
			if (status == UACT_SUCCESS)
			{

				m_workAreaFolder = s2ws(reinterpret_cast<const char*>(tempFilePath));
			}
			unifree(hOp, tempFilePath);
		}
	}

	// TODO: Use return value
	UFREEH(&hOp);

	if (m_workAreaFolder.empty())
	{
		throw std::runtime_error{ "Unable to get the WorkArea folder location from Uniface" };
	}
	return m_workAreaFolder;
}

DWORD CUniface::_callUnifaceVC(std::filesystem::path const& file, const BOOL bDelete, const BOOL bImport)
{
	UHACT hOp{ UHACT_NULL };
	long procError{ 0 };
	long oprStatus{ -1 };

	UCHAR vc[] = "VC";
	auto status = UINSTOPR(m_hIn, /* instance handle */
	                       vc, /* operation name */
	                       0, /* reserved for future use */
	                       &hOp); /* operation handle */

	/* Supply parameters */
	if (status == UACT_SUCCESS)
		status = USTR2UF(hOp, 1, reinterpret_cast<UCHAR*>(const_cast<CHAR*>(file.u8string().c_str())));
	if (status == UACT_SUCCESS)
		status = UINT2UF(hOp, 2, bDelete);
	if (status == UACT_SUCCESS)
		status = UINT2UF(hOp, 3, bImport);

	/* Activate operation */
	if (status == UACT_SUCCESS)
	{
		status = UOPRACT(hOp, /* operation handle */
		                 &oprStatus, /* $status */
		                 &procError); /* $procerror */
		if (status == UACT_SUCCESS && oprStatus == 2)
		{
			// TODO: Use return value
			gNotifications.ShowDirty(file.wstring());
		}
	}
	// TODO: use return value
	UFREEH(&hOp);

	return oprStatus;
}

void CUniface::_startUniface()
{
	if (!m_bStarted)
	{
		m_bStarted = false;

		const auto uniRet = UECREATE(1, nullptr, m_pCmdLine.getUnifaceCommandLine(), m_pCmdLine.getUnifaceAsnFile(),
		                             nullptr,
		                             nullptr, &m_hUniEnv);
		if (uniRet != UINI_SUCCESS)
		{
			throw std::runtime_error{ "Unable to start Uniface Environment (uecreate): "s + std::to_string(uniRet) };
		}

		long procError{ 0 };
		UCHAR vc_main[] = "VC_MAIN";
		const auto status = UINSTNEW(m_hUniEnv,
		                             vc_main, /* component name */
		                             static_cast<UCHAR*>(nullptr), /* component ID */
		                             static_cast<UCHAR*>(nullptr), /* instance name */
		                             UDEFAULT_COMM_MODE, /* communication mode */
		                             static_cast<UCHAR*>(nullptr), /* instance properties */
		                             &m_hIn, /* instance handle */
		                             &procError); /* $procerror */

		if (status != UACT_SUCCESS)
		{
			_stopUniface();
			throw std::runtime_error{ "Unable in init Component VC_MAIN (UINSTNEW): "s + std::to_string(status) };
		}

		m_bStarted = status == UACT_SUCCESS;
	}
}

void CUniface::_stopUniface() noexcept
{
	/* Delete instance */
	long procError{ 0 };
	if (m_hIn != nullptr)
	{
		// TODO: Use return value
		UINSTDEL(m_hIn, &procError); /* Delete the instance */
	}
	// TODO: Use return value
	UFREEH(&m_hIn);

	if (m_hUniEnv != nullptr)
	{
		// TODO: Use return value
		UEDELETE(m_hUniEnv, -1); /* Close the Uniface environment */
	}
	// TODO: Use return value
	UFREEH(&m_hUniEnv);
	m_hUniEnv = nullptr;

	// Make sure we are not lingering on the fileToAction when Uniface is kicked off again
	_resetFileInfo();
	m_bStarted = false;
}
