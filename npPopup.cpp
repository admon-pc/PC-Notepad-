#include "notepad.h"

#include <string>

extern notepad* g_app;

npPopup::npPopup()
{
	m_hPopupMenu = CreatePopupMenu();
}

npPopup::~npPopup()
{
	for (size_t i = 0, sz = m_subMenus.size(); i < sz; ++i)
	{
		delete m_subMenus[i];
	}
	DestroyMenu(m_hPopupMenu);
}

npPopup* npPopup::CreateSubMenu(const wchar_t* text)
{
	auto newSubMenu = new npPopup;
	m_subMenus.push_back(newSubMenu);
	AppendMenu(m_hPopupMenu, MF_POPUP | MF_BYPOSITION | MF_STRING, (UINT_PTR)newSubMenu->m_hPopupMenu, text);
	return newSubMenu;
}

void npPopup::AddItem(const wchar_t* _text, uint32_t id, const wchar_t* shortcut)
{
	npStringW text = _text;
	if (shortcut)
	{
		text += L"\t";
		text += shortcut;
	}
	AppendMenu(m_hPopupMenu, MF_BYPOSITION | MF_STRING, id, text.data());
}

void npPopup::AddSeparator()
{
	AppendMenu(m_hPopupMenu, MF_SEPARATOR, 0, 0);
}

void npPopup::Show(uint32_t x, uint32_t y)
{
	HWND hWnd = (HWND)g_app->m_hwnd;
	SetForegroundWindow(hWnd);
	POINT pt;
	pt.x = x;
	pt.y = y;
	ClientToScreen(hWnd, &pt);
	TrackPopupMenu(m_hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
}

