#include "FileAction.h"

CFileAction::CFileAction(const DWORD lAction, std::wstring&& lFile) noexcept
	: m_action(lAction), m_file(std::move(lFile)) 
{
}
