#pragma once
#include <string>
#include <Windows.h>

class CFileAction
{
public:
	CFileAction(DWORD lAction, std::wstring&& lFile) noexcept;

	const DWORD m_action;
	const std::wstring m_file;
};
