#pragma once

class npPopup
{
public:
	npPopup();
	~npPopup();

	npPopup* CreateSubMenu(const wchar_t* text) ;
	void AddItem(const wchar_t*, uint32_t id, const wchar_t* shortcut) ;
	void AddSeparator() ;
	void Show(uint32_t x, uint32_t y) ;


	HMENU m_hPopupMenu = 0;
	npArray<npPopup*> m_subMenus;
};

