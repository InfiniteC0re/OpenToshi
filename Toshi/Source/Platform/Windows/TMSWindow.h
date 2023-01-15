#pragma once
#include "Toshi/Core/TObject.h"
#include "Toshi/Render/TRender.h"
#include "Toshi/Strings/TString8.h"

#include <dbt.h>
#include <windowsx.h>
#include <resource.h>

namespace Toshi
{
	class TMSWindow :
		public TGenericClassDerived<TMSWindow, TObject, "TMSWindow", TMAKEVERSION(1, 0), false>
	{
	public:
		TMSWindow() = default;
		TMSWindow(const TMSWindow&) = default;

		void Enable();
		void Disable();
		void Update();

		bool RegisterWindowClass(TRender* renderer, LPCSTR title);
		void UnregisterWindowClass();

		bool Flag1() const { return m_Flag1; }
		bool IsWindowed() const { return m_IsWindowed; }
		HWND GetHWND() const { return m_HWND; }
		
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		static constexpr GUID WceusbshGUID = { 0x25dbce51, 0x6c8f, 0x4a72, 0x8a, 0x6d, 0xb5, 0x4c, 0x2b, 0x4f, 0xc8, 0x35 };

		static bool ms_bIsFocused;
		static STICKYKEYS ms_StickyKeys;
		static HDEVNOTIFY ms_hDeviceNotify;

	private:
		HWND m_HWND;               // 0x04
		TRender* m_Render;         // 0x08
		bool m_IsWindowed;         // 0x0C
		bool m_Flag1;              // 0x0D
		HMODULE m_ModuleHandle;    // 0x10
		bool m_bUnk;               // 0x14
		int m_xPos;                // 0x18
		int m_yPos;                // 0x1C
		bool m_bUnk2;              // 0x20
		bool m_bPopupWindow;       // 0x21
	};
}


