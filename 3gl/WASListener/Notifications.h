#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <strsafe.h>
#include <direct.h>
#include <string>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

inline auto const HINST_THISCOMPONENT{reinterpret_cast<HINSTANCE>(&__ImageBase)};
inline UINT constexpr WMAPP_NOTIFYCALLBACK = WM_APP + 1;

class CNotifications
{
public:
	void ShowDirty(std::wstring const& filename) const noexcept;
	BOOL AddNotificationIcon(HWND lhWnd, DWORD luID) noexcept;
	void DeleteNotificationIcon() const noexcept;

private:
	HWND m_hWnd{nullptr};
	DWORD m_uID{0};
};

inline CNotifications gNotifications;
