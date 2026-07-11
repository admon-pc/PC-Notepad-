#include "notepad.h"
#include "resource.h"
#include "npRenderer.h"
#include "npRenderer_gdi.h"
#include <vector>
#include <chrono>
#include <filesystem>
#include <windowsx.h>
#include <shobjidl.h> 
#include "Shlwapi.h"
#include <shellapi.h>

#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib,"Comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

notepad* g_app = 0;

LRESULT CALLBACK SystemWindow_WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK FindDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

notepad::notepad()
{
	g_app = this;

	//m_cursorIBeam = LoadCursor(0, IDC_IBEAM);
	//m_cursorArrow = LoadCursor(0, IDC_ARROW);

	m_colorTheme_light.m_docBarBG = 0xFFECF4FC;
	m_colorTheme_light.m_docBarItemBG = 0xFFB7D5FF;
	m_colorTheme_light.m_docBarItemBGMouseHover = 0xFFD6E7FF;
	m_colorTheme_light.m_docBarItemText = 0xFF000000;
	
	m_colorTheme_light.m_editorBG = 0xFFFFFFFF;
	m_colorTheme_light.m_editorScrollBG = 0xFFD7DEF0;
	m_colorTheme_light.m_editorScrollButtonBG = 0xFFD7DEF0;
	m_colorTheme_light.m_editorScrollRectangle = 0xFFA7AEB0;
	m_colorTheme_light.m_editorText = 0xFF000000;
	m_colorTheme_light.m_editorSelectBG = 0xFF3399FF;
	m_colorTheme_light.m_editorSelectText = 0xFFFFFFFF;
	m_colorTheme_light.m_editorCursor = 0xFF000000;
	m_colorTheme_light.m_editorBottomBarBG = 0xFFECF4FC;
	m_colorTheme_light.m_editorLineBarBG = 0xFFECF4FC;
	m_colorTheme_light.m_editorCurrLineBG = 0xFFD2DFF7;
	

	m_colorTheme_light.m_hexeditorBBG = 0xFFFFFFFF;
	m_colorTheme_light.m_hexeditorBText = 0xFF000000;
	m_colorTheme_light.m_hexeditorTText = 0xFF000000;

	m_currColorTheme = &m_colorTheme_light;

}

notepad::~notepad()
{
	if (m_GUITexture)
		delete m_GUITexture;

	if (m_fontUI)
		delete m_fontUI;

	//if (m_popupTextEditorPopup)
	//	delete m_popupTextEditorPopup;

	if (m_renderer)
		delete m_renderer;

	if (m_fileSaveDialog) m_fileSaveDialog->Release();
	if (m_fileOpenDialog) m_fileOpenDialog->Release();
	CoUninitialize();
	
	if (m_hwnd)
	{
		ReleaseDC(m_hwnd, m_dc);
		UnregisterClass(L"notepadcls", GetModuleHandle(NULL));
		DestroyWindow(m_hwnd);
	}

}

bool notepad::PointInRect(float32_t x, float32_t y, const npVec4f& rect)
{
	if (x >= rect.x)
	{
		if (x <= rect.z)
		{
			if (y >= rect.y)
			{
				if (y <= rect.w)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool notepad::Init(bool secondWnd)
{
	m_secondWindow = secondWnd;

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_WIN95_CLASSES; // Required for Trackbar
	InitCommonControlsEx(&icex);

	WNDCLASSEXW wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = SystemWindow_WndProc;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = 0;// (HBRUSH)(COLOR_WINDOW + 1);
	  // wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
	wcex.lpszClassName = L"notepadcls";
	//wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	if (!RegisterClassExW(&wcex))
		return false;
	m_hwnd = CreateWindowW(
		wcex.lpszClassName,
		L"Notepad++",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		800,
		600,
		nullptr,
		nullptr,
		wcex.hInstance,
		nullptr);
	SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
	m_dc = GetDC(m_hwnd);

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr))
		return false;

	hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&m_fileSaveDialog));
	if (FAILED(hr))
		return false;

	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&m_fileOpenDialog));
	if (FAILED(hr))
		return false;

	m_cursors[(uint32_t)npCursorType::Arrow] = LoadCursor(0, IDC_ARROW);
	m_cursors[(uint32_t)npCursorType::Cross] = LoadCursor(0, IDC_CROSS);
	m_cursors[(uint32_t)npCursorType::Hand] = LoadCursor(0, IDC_HAND);
	m_cursors[(uint32_t)npCursorType::Help] = LoadCursor(0, IDC_HELP);
	m_cursors[(uint32_t)npCursorType::IBeam] = LoadCursor(0, IDC_IBEAM);
	m_cursors[(uint32_t)npCursorType::No] = LoadCursor(0, IDC_NO);
	m_cursors[(uint32_t)npCursorType::Size] = LoadCursor(0, IDC_SIZE);
	m_cursors[(uint32_t)npCursorType::SizeNESW] = LoadCursor(0, IDC_SIZENESW);
	m_cursors[(uint32_t)npCursorType::SizeNS] = LoadCursor(0, IDC_SIZENS);
	m_cursors[(uint32_t)npCursorType::SizeNWSE] = LoadCursor(0, IDC_SIZENWSE);
	m_cursors[(uint32_t)npCursorType::SizeWE] = LoadCursor(0, IDC_SIZEWE);
	m_cursors[(uint32_t)npCursorType::UpArrow] = LoadCursor(0, IDC_UPARROW);
	m_cursors[(uint32_t)npCursorType::Wait] = LoadCursor(0, IDC_WAIT);

	m_transparencyForFindWindow = 100;

	RAWINPUTDEVICE device;
	device.usUsagePage = 0x01;
	device.usUsage = 0x02;
	device.dwFlags = 0;
	device.hwndTarget = m_hwnd;
	RegisterRawInputDevices(&device, 1, sizeof device);

	HMENU menu_file = CreateMenu();
	AppendMenu(menu_file, MF_STRING, NOTEPAD_MENUID_FILE_NEWTEXTFILE, L"New Text File");
	AppendMenu(menu_file, MF_STRING, NOTEPAD_MENUID_FILE_NEWBINFILE, L"Create In HEX Editor");
	AppendMenu(menu_file, MF_SEPARATOR, 0, 0);
	AppendMenu(menu_file, MF_STRING, NOTEPAD_MENUID_FILE_CLOSEDOC, L"Close");
	AppendMenu(menu_file, MF_SEPARATOR, 0, 0);
	AppendMenu(menu_file, MF_STRING, NOTEPAD_MENUID_FILE_OPENTEXTFILE, L"Open");
	AppendMenu(menu_file, MF_STRING, NOTEPAD_MENUID_FILE_OPENBINFILE, L"Open In HEX Editor");
	AppendMenu(menu_file, MF_SEPARATOR, 0, 0);
	AppendMenu(menu_file, MF_STRING, NOTEPAD_MENUID_FILE_SAVE, L"Save");
	AppendMenu(menu_file, MF_STRING, NOTEPAD_MENUID_FILE_SAVEAS, L"Save As...");
	AppendMenu(menu_file, MF_SEPARATOR, 0, 0);
	AppendMenu(menu_file, MF_STRING, NOTEPAD_MENUID_EXIT, L"E&xit");

	HMENU menu_edit = CreateMenu();
	AppendMenu(menu_edit, MF_STRING, NOTEPAD_MENUID_CUT, L"Cut");
	AppendMenu(menu_edit, MF_STRING, NOTEPAD_MENUID_COPY, L"Copy");
	AppendMenu(menu_edit, MF_STRING, NOTEPAD_MENUID_PASTE, L"Paste");
	AppendMenu(menu_edit, MF_STRING, NOTEPAD_MENUID_DELETE, L"Delete");
	AppendMenu(menu_edit, MF_SEPARATOR, 0, 0);
	AppendMenu(menu_edit, MF_STRING, NOTEPAD_MENUID_SELECTALL, L"Select All");
	AppendMenu(menu_edit, MF_SEPARATOR, 0, 0);
	AppendMenu(menu_edit, MF_STRING, NOTEPAD_MENUID_EDIT_FIND, L"Find\tCtrl+F");
	AppendMenu(menu_edit, MF_STRING, NOTEPAD_MENUID_EDIT_FINDNEXT, L"Find Next\tF3");

	HMENU menu_view = CreateMenu();
	AppendMenu(menu_view, MF_STRING, 0, L"1");
	AppendMenu(menu_view, MF_STRING, 0, L"2");
	AppendMenu(menu_view, MF_STRING, 0, L"3");

	HMENU menu_help = CreateMenu();
	AppendMenu(menu_help, MF_STRING, 0, L"About");

	HMENU mMainMenu = CreateMenu();
	AppendMenu(mMainMenu, MF_STRING | MF_POPUP, (UINT)menu_file, L"&File");
	AppendMenu(mMainMenu, MF_STRING | MF_POPUP, (UINT)menu_edit, L"&Edit");
	AppendMenu(mMainMenu, MF_STRING | MF_POPUP, (UINT)menu_view, L"View");
	AppendMenu(mMainMenu, MF_STRING | MF_POPUP, (UINT)menu_help, L"Help");
	SetMenu(m_hwnd, mMainMenu);

	/*m_popupTextEditorPopup = new npPopup;
	m_popupTextEditorPopup->AddItem(L"Undo", NOTEPAD_MENUID_UNDO, L"Ctrl+Z");
	m_popupTextEditorPopup->AddSeparator();
	m_popupTextEditorPopup->AddItem(L"Cut", NOTEPAD_MENUID_CUT, L"Ctrl+X");
	m_popupTextEditorPopup->AddItem(L"Copy", NOTEPAD_MENUID_COPY, L"Ctrl+C");
	m_popupTextEditorPopup->AddItem(L"Paste", NOTEPAD_MENUID_PASTE, L"Ctrl+V");
	m_popupTextEditorPopup->AddItem(L"Delete", NOTEPAD_MENUID_DELETE, L"Del");
	m_popupTextEditorPopup->AddSeparator();
	m_popupTextEditorPopup->AddItem(L"Select All", NOTEPAD_MENUID_SELECTALL, L"Ctrl+A");*/

	ShowWindow(m_hwnd, SW_SHOW);
	DrawMenuBar(m_hwnd);
	UpdateWindow(m_hwnd);

	//m_uMsgWakeUp = RegisterWindowMessage(L"MY_APP_WAKEUP_MSG");

	if (m_hwnd)
	{
	}

	m_renderer = new npRenderer_gdi;
	if (!m_renderer->Init())
	{
		return false;
	}

	{
		npImage guiIcons;
		guiIcons.Create(150, 150);
		npColor palette[2] = { 0x00000000, 0xFFFFFFFF };
		uint8_t indices_up[] = {
			0,0,0,0,1,0,0,0,0,
			0,0,0,1,1,1,0,0,0,
			0,0,1,1,1,1,1,0,0,
			0,1,1,1,1,1,1,1,0,
			1,1,1,1,1,1,1,1,1,
		};
		guiIcons.Fill(indices_up, palette, npVec2u(9, 5), npVec2u(0, 0), 0);
		uint8_t indices_down[] = {
			1,1,1,1,1,1,1,1,1,
			0,1,1,1,1,1,1,1,0,
			0,0,1,1,1,1,1,0,0,
			0,0,0,1,1,1,0,0,0,
			0,0,0,0,1,0,0,0,0,
		};
		guiIcons.Fill(indices_down, palette, npVec2u(9, 5), npVec2u(9, 0), 0);
		uint8_t indices_left[] = {
			0,0,0,0,1,
			0,0,0,1,1,
			0,0,1,1,1,
			0,1,1,1,1,
			1,1,1,1,1,
			0,1,1,1,1,
			0,0,1,1,1,
			0,0,0,1,1,
			0,0,0,0,1,
		};
		guiIcons.Fill(indices_left, palette, npVec2u(5, 9), npVec2u(18, 0), 0);
		uint8_t indices_right[] = {
			1,0,0,0,0,
			1,1,0,0,0,
			1,1,1,0,0,
			1,1,1,1,0,
			1,1,1,1,1,
			1,1,1,1,0,
			1,1,1,0,0,
			1,1,0,0,0,
			1,0,0,0,0,
		};
		guiIcons.Fill(indices_right, palette, npVec2u(5, 9), npVec2u(23, 0), 0);

		m_GUITexture = m_renderer->CreateNPTexture(&guiIcons);
	}

	m_fontUI = m_renderer->CreateNPFont("Segoe UI", false, false, 9);

	{
		npImage img;
		img.Create(4, 30);
		npColor palette[22] = 
		{
			0xFFFAFCFD,
			0xFFFDFEFF,
			0xFFF9FCFF,
			0xFFF5FAFF,
			0xFFF4F9FF,
			0xFFF3F8FE,
			0xFFF1F8FE,
			0xFFEEF6FD,
			0xFFEDF5FD,
			0xFFECF4FC,
			0xFFEAF3FC,
			0xFFE9F2FB,
			0xFFE7F1FB,
			0xFFE6F0FA,

			0xFFDCE6F4, //6
			0xFFDCE6F5, //2
			0xFFDCE7F5, //3
			0xFFDCE7F6, //1
			0xFFDDE9F7, //1
			0xFFE4EFFB,
			0xFFCDDAEA,
			0xFFA0AFC3,
		};
		uint8_t indices[] = {
			0,0,0,0,
			1,1,1,1,
			2,2,2,2,
			3,3,3,3,
			4,4,4,4,
			5,5,5,5,
			6,6,6,6,
			7,7,7,7,
			8,8,8,8,
			9,9,9,9,
			10,10,10,10,
			11,11,11,11,
			12,12,12,12,
			13,13,13,13,
			14,14,14,14, // 0xFFDCE6F4
			14,14,14,14,
			14,14,14,14,
			14,14,14,14,
			14,14,14,14,
			14,14,14,14,
			15,15,15,15,
			15,15,15,15,
			16,16,16,16,
			16,16,16,16,
			16,16,16,16,
			17,17,17,17,
			18,18,18,18,
			19,19,19,19,
			20,20,20,20,
			21,21,21,21,
		};
		img.Fill(indices, palette, npVec2u(4, 30), npVec2u(0, 0), 0);
		img.FlipPixel();
		m_docBarBGTexture = m_renderer->CreateNPTexture(&img);
	}
//	m_gui = new gui;
//	if (!m_gui->init())
//		return false;
//	m_gui->get_renderer()->OnWindowSize();
	
	// first update
	// because ReadRegistry will create new documents
	// and documents must know m_documentAreaRect
	Update();
	
	ReadRegistry();

	int argc = 0;
	// Parses the full command line into a classic array format
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
		{
			std::filesystem::path p(argv[i]);
			if (std::filesystem::exists(p))
			{
				OpenText(p.generic_string().c_str());
			}
		}
	}


	OnWindowSizing();
	RepaintWindow();
	return true;
}

uint64_t notepad::GetTime()
{
	static bool isInit = false;
	static uint64_t baseTime;
	if (!isInit)
	{
		auto now = std::chrono::high_resolution_clock::now();
		baseTime = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

		isInit = true;
	}
	auto now = std::chrono::high_resolution_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

	return ms - baseTime;
}

void notepad::OnUpdate()
{
	Update();
	//m_gui->update(1.f);
}

void notepad::ReadRegistry()
{
	if (m_secondWindow)
		return;

	npWinRegistry key_params("Software\\NotepadTest\\params", 0);
	if (key_params.IsGood())
	{
		int posX = 0;
		int posY = 0;
		key_params.GetValue(L"PosX", &posX);
		key_params.GetValue(L"PosY", &posY);
		int szX = 0;
		int szY = 0;
		key_params.GetValue(L"SizeX", &szX);
		key_params.GetValue(L"SizeY", &szY);
		MoveWindow(m_hwnd, posX, posY, szX, szY, TRUE);


	

		RECT r;
		GetClientRect(m_hwnd, &r);
		int clientSizeX = r.right - r.left;
		int clientSizeY = r.bottom - r.top;
		MoveWindow(m_hwnd,
			posX,
			posY,
			szX + (szX - clientSizeX),
			szY + (szY - clientSizeY),
			TRUE);
	}

	npWinRegistry key_opened("Software\\NotepadTest\\opened", 0);
	if (key_opened.IsGood())
	{
		npStringW strw;
		npStringA stra;
		key_opened.GetValue(0,&stra);
		int32_t num = stra.ToInt();
		if (num > 100)
			num = 100;

		key_opened.GetValue("Last", &stra);
		int32_t activeIndex = stra.ToInt();

		if (num)
		{
			for (int32_t i = 0; i < num; ++i)
			{
				char buf[100];
				sprintf_s(buf, 100, "%i", i);

				auto subKey = key_opened.OpenKey(buf);
				if (subKey)
				{
					subKey->GetValue("Type", &stra);
					int type = stra.ToInt();
					subKey->GetValue(L"Path", &strw);
					printf("Type: %s\n", stra.c_str());
					wprintf(L"Path: %s\n", strw.c_str());

					size_t s1 = 0;
					size_t s2 = 0;
					subKey->GetValue(L"S1", &s1);
					subKey->GetValue(L"S2", &s2);

					npDocumentType docType = npDocumentType::Text;
					switch (type)
					{
					default:
					case (int)npDocumentType::Text:
						docType = npDocumentType::Text;
						break;
					case (int)npDocumentType::Binary:
						docType = npDocumentType::Binary;
						break;
					}

					if (std::filesystem::exists(strw.c_str()))
					{
						npUnicodeConverter::wchar_to_char(strw.c_str(), strw.size(), &stra);
						switch (docType)
						{
						case npDocumentType::Text:
							this->OpenText(stra.c_str());
							break;
						case npDocumentType::Binary:
							this->OpenBinary(stra.c_str());
							break;
						case npDocumentType::_end:
							break;
						default:
							break;
						}
					}

					if (m_activeDocument)
					{
						if (s1 != s2)
							m_activeDocument->Select(s1, s2);
					}

					delete subKey;
				}
			}

			if (m_documents.size())
			{
				if (activeIndex >= m_documents.size())
					activeIndex = 0;

				m_activeDocument = m_documents.m_data[activeIndex];
			}
		}
	}
}

void notepad::WriteRegistry()
{
	if (m_secondWindow)
		return;

	npWinRegistry key_params("Software\\NotepadTest\\params", 0);
	if (key_params.IsGood())
	{
		RECT r;
		GetWindowRect(m_hwnd, &r);
		key_params.SetValue(L"PosX", (int)r.left);
		key_params.SetValue(L"PosY", (int)r.top);
		key_params.SetValue(L"SizeX", (int)m_clientSize.x);
		key_params.SetValue(L"SizeY", (int)m_clientSize.y);
	}

	npWinRegistry key_opened("Software\\NotepadTest\\opened", 0);
	if (key_opened.IsGood())
	{
		size_t activeIndex = 0;
		DWORD num = 0;
		for (size_t i = 0; i < m_documents.m_size; ++i)
		{
			auto item = m_documents.m_data[i];
			if (m_activeDocument == item)
				activeIndex = i;

			if (item->m_filePath.c_str())
			{
			//	if ()
				{
					if (item->m_filePath.size())
					{
						wchar_t wbuf[50];
						wsprintfW(wbuf, L"%u", i);
						auto subKey = key_opened.CreateKey(wbuf);
						if (subKey)
						{
							wsprintfW(wbuf, L"%u", (uint32_t)item->GetType());
							subKey->SetValue(L"Type", wbuf);

							subKey->SetValue(L"Path", item->m_filePathW.c_str());
							subKey->SetValue(L"S1", item->GetSelectionBegin());
							subKey->SetValue(L"S2", item->GetSelectionEnd());

							delete subKey;
							++num;
						}
					}
				}
			}
		}

		wchar_t wbuf[50];
		wsprintfW(wbuf, L"%u", num);
		key_opened.SetValue(0, wbuf);
	
		wsprintfW(wbuf, L"%u", activeIndex);
		key_opened.SetValue(L"Last", wbuf);
	}
}

void notepad::Run()
{
	while (m_run)
	{
		static uint64_t t1 = 0;
		uint64_t t2 = GetTime();
		float32_t tick = float32_t(t2 - t1);
		t1 = t2;
		m_dt = tick / 1000.f;
		
		m_input.Reset();

		MSG msg;
		
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, 0, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		m_currentCursor = npCursorType::Arrow;

		int32_t ctrl_shift_alt = 0;
		if (m_input.IsKeyHold(npInputKey::K_LALT) || m_input.IsKeyHold(npInputKey::K_RALT))
			ctrl_shift_alt |= 1;

		if (m_input.IsKeyHold(npInputKey::K_LSHIFT) || m_input.IsKeyHold(npInputKey::K_RSHIFT))
			ctrl_shift_alt |= 2;

		if (m_input.IsKeyHold(npInputKey::K_LCTRL) || m_input.IsKeyHold(npInputKey::K_RCTRL))
			ctrl_shift_alt |= 4;

		switch (ctrl_shift_alt)
		{
		default:
		case 0:  m_input.m_kbm = npKeyboardModifier::None;          break;
		case 1:  m_input.m_kbm = npKeyboardModifier::Alt;           break;
		case 2:  m_input.m_kbm = npKeyboardModifier::Shift;         break;
		case 3:  m_input.m_kbm = npKeyboardModifier::ShiftAlt;      break;
		case 4:  m_input.m_kbm = npKeyboardModifier::Ctrl;          break;
		case 5:  m_input.m_kbm = npKeyboardModifier::CtrlAlt;       break;
		case 6:  m_input.m_kbm = npKeyboardModifier::ShiftCtrl;     break;
		case 7:  m_input.m_kbm = npKeyboardModifier::ShiftCtrlAlt;  break;
		}

		//CEvent event;
		//while (PollEvent(event))
		//{
		//	if (event.m_type == EEventType::GUI)
		//	{
		//		switch (event.m_event_gui.m_event)
		//		{
		//		case SEvent_GUI::cursor_entered_textEditor:
		//			this->ActivateCursorIBeam();
		//			break;
		//		case SEvent_GUI::cursor_leaved_textEditor:
		//			this->ActivateCursorArrow();
		//			break;
		//		}
		//	}
		//}

	//	m_gui->update(0);
		RepaintWindow();
		Update();
	}
}

void notepad::Update()
{
	m_documentBarRect.x = 0.f;
	m_documentBarRect.y = 0.f;
	m_documentBarRect.z = (float32_t)m_clientSize.x;
	m_documentBarRect.w = m_documentBarRect.x + m_docBarHeight;
	if (PointInRect(m_input.m_cursorCoordsForGUI.x, m_input.m_cursorCoordsForGUI.y,
		m_documentBarRect))
	{
		//m_currentCursor = npCursorType::Help;
	}

	m_rightPanelRect.x = m_clientSize.x - 200;
	m_rightPanelRect.y = 0;
	m_rightPanelRect.z = m_clientSize.x;
	m_rightPanelRect.w = m_clientSize.y;
	m_documentBarRect.z -= m_rightPanelRect.z - m_rightPanelRect.x;

	m_documentAreaRect.x = m_documentBarRect.x;
	m_documentAreaRect.y = m_documentBarRect.w;
	m_documentAreaRect.z = m_documentBarRect.z;
	m_documentAreaRect.w = (float32_t)m_clientSize.y;

	m_documentAreaRect.z = m_rightPanelRect.x;

	if (m_documents.m_size)
	{
		float32_t pos_x = 0.f;
		bool moveItemInDocBar = false;
		size_t lastDocItemFullVisible = 0;
		size_t activeDocIndex = 0;
		for (size_t i = 0; i < m_documents.m_size; ++i)
		{
			auto item = m_documents.m_data[i];
			npVec4f docbarItemRect;
			docbarItemRect.x = m_documentBarRect.x + pos_x;
			docbarItemRect.y = m_documentBarRect.y;
			docbarItemRect.z = docbarItemRect.x + item->m_titleLenInPixels + 20;
			docbarItemRect.w = m_documentBarRect.w - 3;
			item->m_docbarItemRect = docbarItemRect;
			if (m_activeDocument == item)
			{
				activeDocIndex = i;
				if (item->m_docbarItemRect.z > m_documentBarRect.z)
				{
					moveItemInDocBar = true;
				}
			}

			if (item->m_docbarItemRect.z < m_documentBarRect.z)
			{
				lastDocItemFullVisible = i;
			}

			if (item->m_docbarItemRect.z > m_documentBarRect.z)
				item->m_docbarItemRect.z = m_documentBarRect.z;
			if (item->m_docbarItemRect.x > m_documentBarRect.z)
				item->m_docbarItemRect.x = m_documentBarRect.z;

			if (PointInRect(m_input.m_cursorCoordsForGUI.x,
				m_input.m_cursorCoordsForGUI.y,
				item->m_docbarItemRect))
			{ 
				if (item != m_activeDocument)
				{
					if (m_input.m_isLMBDown
						|| m_input.m_isRMBDown)
					{
						m_activeDocument = item;
						m_activeDocument->OnActivate();
						
					}
				}
				else
				{
					if (m_input.m_isRMBDown)
					{
						ShowDocBarItemPopup(item);
					}
				}
			}
			pos_x += item->m_docbarItemRect.z - item->m_docbarItemRect.x;
		}

		if (moveItemInDocBar)
		{
			auto old = m_documents.m_data[lastDocItemFullVisible];
			m_documents.m_data[lastDocItemFullVisible]
				= m_documents.m_data[activeDocIndex];
			m_documents.m_data[activeDocIndex] = old;
		}
	}
	//m_clickedLMB

	if (m_activeDocument)
	{
		m_activeDocument->Update(m_dt);

		if (m_input.IsKeyHit(npInputKey::K_F) && m_input.m_kbm == npKeyboardModifier::Ctrl)
			ShowFindWindow();
		if (m_input.IsKeyHit(npInputKey::K_F3))
			FindNext();
	}
}

void notepad::Draw()
{
	m_renderer->BeginDraw();
	m_renderer->SetClipRect(npVec4f(0, 0, (float)g_app->m_clientSize.x, (float)g_app->m_clientSize.y));
	m_renderer->DrawRectangle(npVec4f(0, 0, (float)g_app->m_clientSize.x, (float)g_app->m_clientSize.y), npColor(0xFFCCCCCC));

	// DRAW DOCUMENTS BAR
	m_renderer->SetClipRect(m_documentBarRect);
	m_renderer->DrawRectangle(m_documentBarRect, m_currColorTheme->m_docBarBG);
	m_renderer->DrawImage(m_documentBarRect, m_docBarBGTexture);
	if (m_documents.m_size)
	{
		m_renderer->SetNPFont(m_fontUI);
		for (size_t i = 0; i < m_documents.m_size; ++i)
		{
			auto item = m_documents.m_data[i];
			
			
				

			if (m_activeDocument == item)
			{
				m_renderer->DrawRectangle(item->m_docbarItemRect, m_currColorTheme->m_docBarItemBG);
			}
			else
			{
				if (PointInRect(m_input.m_cursorCoordsForGUI.x,
					m_input.m_cursorCoordsForGUI.y,
					item->m_docbarItemRect))
				{
					m_renderer->DrawRectangle(item->m_docbarItemRect, m_currColorTheme->m_docBarItemBGMouseHover);
				}
			}

			if (item->m_title.size())
			{
				
				m_renderer->DrawText(
					item->m_title.data(), 
					item->m_title.size(),
					item->m_docbarItemRect.x+2,
					item->m_docbarItemRect.y,
					m_currColorTheme->m_docBarItemText);
				if(item->IsNeedToSave())
					m_renderer->DrawText(L"*", 1,
						item->m_docbarItemRect.x + item->m_titleLenInPixels + 2,
						item->m_docbarItemRect.y, m_currColorTheme->m_docBarItemText);
			}

		}
		npVec4f documentBarRectBottom;
		documentBarRectBottom.x = m_documentBarRect.x;
		documentBarRectBottom.y = m_documentBarRect.w - 3;
		documentBarRectBottom.z = m_documentBarRect.z;
		documentBarRectBottom.w = m_documentBarRect.w;
		m_renderer->DrawRectangle(documentBarRectBottom, m_currColorTheme->m_docBarItemBG);

		if (m_activeDocument)
		{
			m_activeDocument->Draw(m_dt);
		}
	}

	m_renderer->EndDraw();
}

void notepad::RepaintWindow()
{
	InvalidateRect(m_hwnd, 0, TRUE);
}

void notepad::ShowDocBarItemPopup(npDocument* doc)
{
	HMENU menu = CreatePopupMenu();
	AppendMenu(menu, MF_BYPOSITION | MF_STRING, NOTEPAD_MENUID_FILE_CLOSEDOC, L"Close");
	AppendMenu(menu, MF_BYPOSITION | MF_STRING, NOTEPAD_MENUID_FILE_CLOSEDOCOTHER, L"Close Other Documents");

	if (doc->GetType() == npDocumentType::Text)
	{
		AppendMenu(menu, MF_BYPOSITION | MF_STRING, NOTEPAD_MENUID_FILE_CLOSEDOC, L"Open in HEX");
	}

	HWND hWnd = (HWND)m_hwnd;
	SetForegroundWindow(hWnd);
	POINT pt;
	pt.x = m_input.m_cursorCoords.x;
	pt.y = m_input.m_cursorCoords.y;
	ClientToScreen(hWnd, &pt);
	TrackPopupMenu(menu, TPM_TOPALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
	DestroyMenu(menu);
}

void notepad::OnWindowClose()
{

	bool askNeedSave = false;
	for (size_t i = 0; i < m_documents.m_size; ++i)
	{
		auto item = m_documents.m_data[i];
		if (item->IsNeedToSave())
		{
			askNeedSave = true;
		}
	}

	if (askNeedSave)
	{
		int ret = MessageBoxW(
			m_hwnd,
			L"Do you want to save all not saved documents?",
			L"Not saved documents",
			MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_DEFBUTTON1 | MB_APPLMODAL);

		if (ret == IDYES)
		{
			for (size_t i = 0; i < m_documents.m_size; ++i)
			{
				auto item = m_documents.m_data[i];
				if (item->IsNeedToSave())
				{
					item->Save();
				}
			}
			m_run = false;
		}
		else if (ret == IDNO)
		{
			m_run = false;
		}
	}
	else
	{
		m_run = false;
	}

	this->WriteRegistry();
}




void notepad::OnDraw()
{
	if (m_renderer)
		Draw();
}
void notepad::OnWindowSizing()
{
	if (m_activeDocument)
		m_activeDocument->OnWindowSize();

	if (m_renderer)
	{
		// из за Windows стиля программы, при клике на рамку,
		// управление программой переходит к ОС и программа перестаёт 
		// выполнять код в главном цикле, вызова
		// m_input.Reset(); не будет.
		m_input.Reset();

		m_renderer->OnWindowSize();
		Update();
		Draw();
	}
}

void notepad::OnWindowSizeChanged()
{
	Update();
	if (m_renderer)
		m_renderer->OnWindowSize();
	if (m_activeDocument)
		m_activeDocument->OnWindowSize();
}

void notepad::OnWindowMaximize()
{
	Update();
	if (m_renderer)
		m_renderer->OnWindowSize();
	if (m_activeDocument)
		m_activeDocument->OnWindowSize();
}

void notepad::OnWindowMinimize()
{
}

void notepad::OnWindowRestore()
{
	if(m_renderer)
		m_renderer->OnWindowSize();
}

void notepad::OnWindowActivate()
{
	m_input.ResetHold();
	if (m_activeDocument)
		m_activeDocument->OnWindowActivate();
}

void notepad::OnWindowDeactivate()
{
	if (m_activeDocument)
		m_activeDocument->OnWindowDeactivate();
}

void notepad::OnSetCursor()
{
	SetCursor(m_cursors[(uint32_t)m_currentCursor]);
}

void notepad::CopyTextToClipboard(npStringW* str)
{
	assert(str);
	if (!str->size())
		return;
	if (!OpenClipboard(0))
		return;

	auto len = str->size();
	EmptyClipboard();
	HGLOBAL clipbuffer;
	clipbuffer = GlobalAlloc(GMEM_DDESHARE, (len + 1) * sizeof(WCHAR));

	wchar_t* buffer;
	buffer = (wchar_t*)GlobalLock(clipbuffer);

	memcpy(buffer, str->data(), str->size() * sizeof(wchar_t));
	buffer[len] = 0;

	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_UNICODETEXT, clipbuffer);
	CloseClipboard();
}

void notepad::GetTextFromClipboard(npStringW* str)
{
	assert(str);
	if (!OpenClipboard(0))
		return;

	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	char16_t* buffer = (char16_t*)GlobalLock(hData);
	GlobalUnlock(hData);
	CloseClipboard();

	str->assign(buffer);
}

void notepad::ActivateCursorIBeam()
{
	//::SetCursor(m_cursorIBeam);
}

void notepad::ActivateCursorArrow()
{
	//::SetCursor(m_cursorArrow);
}

void notepad::OnPopupCommand(uint32_t cmd)
{
	switch (cmd)
	{
	case NOTEPAD_MENUID_EXIT:
		OnWindowClose();
		break;
	case NOTEPAD_MENUID_EDIT_FIND:
	{
		ShowFindWindow();
		break;
	}
	case NOTEPAD_MENUID_EDIT_FINDNEXT:
	{
		g_app->OnTextSearch(true);
		break;
	}
	case NOTEPAD_MENUID_COPY:
	case NOTEPAD_MENUID_CUT:
	case NOTEPAD_MENUID_DELETE:
	case NOTEPAD_MENUID_PASTE:
	case NOTEPAD_MENUID_SELECTALL:
	case NOTEPAD_MENUID_UNDO:
	case NOTEPAD_MENUID_REDO:
		if (m_activeDocument)
			m_activeDocument->OnPopupCommand(cmd);
		break;
	case NOTEPAD_MENUID_FILE_NEWTEXTFILE:
		CreateNewTextDocument(L"New Text", 0, 0, 0);
		break;
	case NOTEPAD_MENUID_FILE_NEWBINFILE:
		CreateNewBinaryDocument();
		break;
	case NOTEPAD_MENUID_FILE_OPENTEXTFILE:
		OpenText();
		break;
	case NOTEPAD_MENUID_FILE_OPENBINFILE:
		this->OpenBinary();
		break;
	case NOTEPAD_MENUID_FILE_SAVE:
		if (m_activeDocument)
			m_activeDocument->Save();
		break;
	case NOTEPAD_MENUID_FILE_SAVEAS:
		if (m_activeDocument)
			m_activeDocument->SaveAs();
		break;
	case NOTEPAD_MENUID_FILE_CLOSEDOC:
			CloseDocument();
		break;
	case NOTEPAD_MENUID_FILE_CLOSEDOCOTHER:
		CloseDocumentOther();
		break;
	}
}

void notepad::CloseDocumentOther()
{
	bool needSave = false;
	for (size_t i = 0; i < m_documents.m_size; ++i)
	{
		if (m_documents.m_data[i]->IsNeedToSave())
		{
			needSave = true;
			break;
		}
	}
	if (needSave)
	{
		int ret = MessageBoxW(
			m_hwnd,
			L"Do you want to save this document?",
			L"Document is not saved!",
			MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_DEFBUTTON1 | MB_APPLMODAL);

		if (ret == IDYES)
		{
			for (size_t i = 0; i < m_documents.m_size; ++i)
			{
				if (m_documents.m_data[i]->IsNeedToSave())
				{
					m_documents.m_data[i]->Save();
				}
			}
		}
	}
	
	for (size_t i = 0; i < m_documents.m_size; ++i)
	{
		if (m_documents.m_data[i] != m_activeDocument)
		{
			delete m_documents.m_data[i];
		}
	}

	m_documents.clear();
	m_documents.push_back(m_activeDocument);
}

void notepad::CloseDocument()
{
	if (m_activeDocument)
	{
		bool removeFromArray = true;
		if(m_activeDocument->IsNeedToSave())
		{
			int ret = MessageBoxW(
				m_hwnd,
				L"Do you want to save this document?",
				L"Document is not saved!",
				MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_DEFBUTTON1 | MB_APPLMODAL);

			if (ret == IDYES)
			{
				m_activeDocument->Save();
			}
			else
			{
				if (ret == IDCANCEL)
				{
					removeFromArray = false;
				}
			}
		}

		if (removeFromArray)
		{
			size_t index = 0;
			for (size_t i = 0; i < m_documents.m_size; ++i)
			{
				if (m_documents.m_data[i] == m_activeDocument)
				{
					index = i;
					break;
				}
			}
			if (index)
				--index;
			m_documents.EraseFirst(m_activeDocument);
			delete m_activeDocument;

			if (m_documents.m_size)
			{
				m_activeDocument = m_documents.m_data[index];
			}
			else
			{
				m_activeDocument = 0;
			}
		}
	}
}

void notepad::OpenSaveFileDialog(
	const char* title,
	const char* okButtonLabel,
	const char* extension, 
	npStringW* returnPath)
{
	assert(title);
	assert(okButtonLabel);
	npStringW titleW;
	titleW = title;
	npStringW okButtonLabelW;
	okButtonLabelW = okButtonLabel;
	npStringW extensionTitleW;
	extensionTitleW = extension;
	m_fileSaveDialog->SetTitle(titleW.data());
	m_fileSaveDialog->SetOkButtonLabel(okButtonLabelW.data());
	COMDLG_FILTERSPEC rgSpec;
	rgSpec.pszName = extensionTitleW.data();

	npStringW wstr;
	wstr = "*.";
	wstr += extension;
	rgSpec.pszSpec = wstr.data();

	m_fileSaveDialog->SetFileTypes(1, &rgSpec);
	auto hr = m_fileSaveDialog->Show(m_hwnd);
	if (SUCCEEDED(hr))
	{
		IShellItem* pItem;
		hr = m_fileSaveDialog->GetResult(&pItem);
		if (SUCCEEDED(hr))
		{
			PWSTR pszFilePath;
			hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
			if (SUCCEEDED(hr))
			{
				returnPath->append((const char16_t*)pszFilePath);
				CoTaskMemFree(pszFilePath);
			}
			pItem->Release();
		}
	}
	m_fileSaveDialog->Release();
	CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&m_fileSaveDialog));
}

void notepad::OpenOpenFileDialog(
	const char* title,
	const char* okButtonLabel,
	npOpenFIleDialogFileTypeDesc* descs,
	uint32_t descsSz,
	npStringW* returnPath)
{
	assert(title);
	assert(okButtonLabel);

	//npArray<npStringW> extensions_array;
	//np::stringGetWords<npStringW>(&extensions_array, npStringW(extensions));

	npStringW titleW;
	npStringW okButtonLabelW;
	//npStringW extensionTitleW;
	titleW = title;
	okButtonLabelW = okButtonLabel;
	//extensionTitleW = extensionTitle;

	m_fileOpenDialog->SetTitle(titleW.data());
	m_fileOpenDialog->SetOkButtonLabel(okButtonLabelW.data());

	COMDLG_FILTERSPEC* rgSpec = 0;
	if (descs)
	{
		rgSpec = new COMDLG_FILTERSPEC[descsSz];
		/*npStringW wstr;
		for (uint32_t i = 0, sz = (uint32_t)extensions_array.size(); i < sz; ++i)
		{
			wstr += L"*.";
			wstr += extensions_array[i].data();
			if (i < sz - 1)
				wstr += L";";
		}*/
		for (uint32_t i = 0; i < descsSz; ++i)
		{
			rgSpec[i].pszName = descs[i].m_title;
			rgSpec[i].pszSpec = descs[i].m_extensions;
		}
		m_fileOpenDialog->SetFileTypes(descsSz, rgSpec);
	}
	else
	{
		FILEOPENDIALOGOPTIONS options;
		auto hr = m_fileOpenDialog->GetOptions(&options);
		if (SUCCEEDED(hr))
		{
			m_fileOpenDialog->SetOptions(options | FOS_PICKFOLDERS);
		}
	}

	auto hr = m_fileOpenDialog->Show((HWND)m_hwnd);
	if (SUCCEEDED(hr))
	{
		IShellItem* pItem;
		hr = m_fileOpenDialog->GetResult(&pItem);
		if (SUCCEEDED(hr))
		{
			PWSTR pszFilePath;
			hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
			if (SUCCEEDED(hr))
			{
				returnPath->append((const char16_t*)pszFilePath);
				CoTaskMemFree(pszFilePath);
			}
			pItem->Release();
		}
	}
	if(rgSpec)
		delete[]rgSpec;
	m_fileOpenDialog->Release();
	CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
		IID_IFileOpenDialog, reinterpret_cast<void**>(&m_fileOpenDialog));
}

void notepad::CreateNewTextDocument(
	const wchar_t* title,
	const char* filePath,
	npUnicodeString* text,
	uint32_t format)
{
	npDocument_text2* doc = new npDocument_text2(m_renderer, filePath, text, format);
	m_documents.push_back(doc);

	if(!filePath)
	{
		doc->SetTitle(title);
	}

	doc->m_titleLenInPixels = m_fontUI->GetTextLen(doc->m_title.data(), doc->m_title.size());

	m_activeDocument = doc;
}

void notepad::CreateNewBinaryDocument()
{
	npStringW wstr;
	OpenSaveFileDialog(
		"Save file first",
		"Save",
		"*",
		&wstr);
	if (wstr.size())
	{
		npStringA stra;
		npUnicodeConverter::wchar_to_char(wstr.data(), wstr.size(), &stra);
		_createNewBinaryDocument(stra.c_str());
	}

}

void notepad::OpenText(const char* f)
{
	for (size_t i = 0; i < m_documents.m_size; ++i)
	{
		if (strcmp(m_documents.m_data[i]->m_filePath.c_str(), f) == 0)
		{
			m_activeDocument = m_documents.m_data[i];
			m_activeDocument->OnActivate();
			return;
		}
	}

	npUnicodeString ustr;
	uint32_t format = ustr.ReadFromFile(f);
	if (ustr.Size())
		CreateNewTextDocument(L"New Text", f, &ustr, format);
	else
		CreateNewTextDocument(L"New Text", f, 0, format);
}

void notepad::OpenText()
{
	npOpenFIleDialogFileTypeDesc descs[3];
	wsprintfW(descs[0].m_title, L"All files");
	wsprintfW(descs[0].m_extensions, L"*");
	wsprintfW(descs[1].m_title, L"Text File");
	wsprintfW(descs[1].m_extensions, L"*.txt");
	wsprintfW(descs[2].m_title, L"C/C++ Files");
	wsprintfW(descs[2].m_extensions, L"*.c;*.cpp;*.h;*.hpp;*.hxx;*.inl");

	npStringW wstr;
	OpenOpenFileDialog(
		"Text File",
		"Open", 
		&descs[0],
		3,
		&wstr);
	if (wstr.size())
	{
		npStringA astr;
		npUnicodeConverter::wchar_to_char(wstr.data(), wstr.size(), &astr);
		astr.FlipSlash();
		OpenText(astr.c_str());
	}
}

void notepad::OpenBinary()
{
	npOpenFIleDialogFileTypeDesc descs[1];
	wsprintfW(descs[0].m_title, L"All files");
	wsprintfW(descs[0].m_extensions, L"*");

	npStringW wstr;
	OpenOpenFileDialog(
		"File",
		"Open",
		&descs[0],
		1,
		&wstr);
	if (wstr.size())
	{
		npStringA astr;
		npUnicodeConverter::wchar_to_char(wstr.data(), wstr.size(), &astr);

		OpenBinary(astr.c_str());
	}
}

void notepad::_createNewBinaryDocument(const char* str)
{
	FILE* f = 0;
	fopen_s(&f, str, "rb+");
	if (f)
	{
		npDocument_binary* doc = new npDocument_binary(
			m_renderer,
			f,
			str);

		doc->m_titleLenInPixels = m_fontUI->GetTextLen(doc->m_title.data(), doc->m_title.size());
		m_documents.push_back(doc);
		m_activeDocument = doc;
	}
}

void notepad::OpenBinary(const char* f)
{
	_createNewBinaryDocument(f);
	//CreateNewBinaryDocument();
	/*npUnicodeString ustr;
	uint32_t format = ustr.ReadFromFile(f);
	if (ustr.Size())
		CreateNewTextDocument(L"New Text", f, &ustr, format);
	else
		CreateNewTextDocument(L"New Text", f, 0, format);*/
}

void notepad::OnTextSearch(bool down)
{
	size_t slen = wcslen(m_textSearchBuffer);
	if (slen && m_activeDocument)
	{
		m_activeDocument->OnTextSearch(down);
	}
}

void notepad::OnTextSearchInFilesOpenResult(uint32_t listboxitemindex)
{
	if (listboxitemindex < m_textSearchInFiles_data.size())
	{
		auto& obj = m_textSearchInFiles_data.data()[listboxitemindex];

		npStringA stra;
		npUnicodeConverter::wchar_to_char(obj.m_filePath, wcslen(obj.m_filePath), &stra);
		if (stra.size())
		{
			OpenText(stra.c_str());
			if (m_activeDocument)
			{
				m_activeDocument->GoTo(obj.m_line, obj.m_col, false);
				m_activeDocument->Select(wcslen(m_textSearchBuffer));
				SetFocus(m_hwnd);
			}
		}
	}
}

void notepad::OnTextSearchInFiles()
{
	size_t fintTextSize = wcslen(m_textSearchBuffer);
	if (!fintTextSize)
		return;

	m_textSearchInFiles_data.clear();
	SendDlgItemMessage(m_hDlg_find, IDC_LIST1, LB_RESETCONTENT, 0, 0);
	bool do_tolower = m_textSearch_isCaseMatch ? false : true;

	npUnicodeString unicodeSearchBuffer = m_textSearchBuffer;
	size_t ui = 0;

	std::vector<std::filesystem::path> extensions;
	extensions.push_back(std::filesystem::path(".txt"));
	extensions.push_back(std::filesystem::path(".c"));
	extensions.push_back(std::filesystem::path(".h"));
	extensions.push_back(std::filesystem::path(".cpp"));

	std::vector<std::filesystem::path> files_to_scan;

	std::filesystem::path dirPath = m_textSearch_path.c_str(); 
	if (std::filesystem::exists(dirPath)
		&& std::filesystem::is_directory(dirPath))
	{
		if (m_textSearch_inSubDirs)
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath))
			{
				if (entry.is_regular_file())
				{
					auto ext = entry.path().extension();
					for (const auto& e : extensions)
					{
						if (e == ext)
						{
							files_to_scan.push_back(entry.path());
							break;
						}
					}
				}
			}
		}
		else
		{
			for (const auto& entry : std::filesystem::directory_iterator(dirPath))
			{
				if (entry.is_regular_file())
				{
					auto ext = entry.path().extension();
					for (const auto& e : extensions)
					{
						if (e == ext)
						{
							files_to_scan.push_back(entry.path());
							break;
						}
					}
				}
			}
		}
	}

	for (const auto& p : files_to_scan)
	{
		FILE* f = 0;
		fopen_s(&f, p.generic_string().c_str(), "rb");
		if (f)
		{
			fseek(f, 0, SEEK_END);
			auto fsz = ftell(f);
			fseek(f, 0, SEEK_SET);

			uint8_t bom[4] = { 0,0,0,0 };
			fread_s(bom, 4, 1, 4, f);
			fseek(f, 0, SEEK_SET);

			enum
			{
				type_utf8,
				type_utf16,
				type_utf32,
			};
			bool isBE = false;
			uint32_t type = type_utf8;


			if ((bom[0] == 0xEF)
				&& (bom[1] == 0xBB)
				&& (bom[2] == 0xBF))
			{
				fseek(f, 3, SEEK_SET);
			}
			else if ((bom[0] == 0x00)
				&& (bom[1] == 0x00)
				&& (bom[2] == 0xFE)
				&& (bom[3] == 0xFF))
			{
				fseek(f, 4, SEEK_SET);
				type = type_utf32;
				isBE = true;
			}
			else if ((bom[0] == 0xFF)
				&& (bom[1] == 0xFE)
				&& (bom[2] == 0x00)
				&& (bom[3] == 0x00))
			{
				fseek(f, 4, SEEK_SET);
				type = type_utf32;
			}
			else if ((bom[0] == 0xFF)
				&& (bom[1] == 0xFE))
			{
				fseek(f, 2, SEEK_SET);
				type = type_utf16;
			}
			else if ((bom[0] == 0xFE)
				&& (bom[1] == 0xFF))
			{
				fseek(f, 2, SEEK_SET);
				type = type_utf16;
				isBE = true;
			}
			union
			{
				uint32_t m_32;
				uint16_t m_16[2];
				uint8_t m_8[4];
			}
			uint_union;
			npUnicodeConverter uc;
			size_t line = 1;
			size_t col = 1;
			size_t words_first_col = -1;
			while (true)
			{
				uc.m_32 = 0;
				uint64_t filePosBegin = _ftelli64(f);

				uint8_t buf[4] = { 0,0,0,0 };
				if (type == type_utf8)
				{
					auto t = _ftelli64(f);
					size_t r = fread_s(buf, 4, 1, 4, f);
					if (!r)
						break;

					uint32_t b = uc.Set(npVec4u(buf[0], buf[1], buf[2], buf[3]));
					if (!uc.m_32)
						break;

					_fseeki64(f, t + b, SEEK_SET);
				}
				else if (type == type_utf16)
				{
					auto t = _ftelli64(f);
					size_t r = fread_s(uint_union.m_8, 4, 1, 4, f);
					if (!r)
						break;


					if (isBE)
					{
						auto _0 = uint_union.m_8[0];
						auto _1 = uint_union.m_8[1];
						uint_union.m_8[0] = uint_union.m_8[3];
						uint_union.m_8[3] = _0;
						uint_union.m_8[1] = uint_union.m_8[2];
						uint_union.m_8[2] = _1;
					}

					uint32_t b = uc.Set(uint_union.m_16[0], uint_union.m_16[1]);
					if (!uc.m_32)
						break;

					_fseeki64(f, t + (b * 2), SEEK_SET);
				}
				else if (type == type_utf32)
				{
					auto t = _ftelli64(f);
					size_t r = fread_s(uint_union.m_8, 4, 1, 4, f);
					if (!r)
						break;


					if (isBE)
					{
						auto _0 = uint_union.m_8[0];
						auto _1 = uint_union.m_8[1];
						uint_union.m_8[0] = uint_union.m_8[3];
						uint_union.m_8[3] = _0;
						uint_union.m_8[1] = uint_union.m_8[2];
						uint_union.m_8[2] = _1;
					}

					uc.m_32 = uint_union.m_32;

					_fseeki64(f, t + 4, SEEK_SET);
				}
				
				if (uc.m_32)
				{
					if (uc.m_32 == '\n')
					{
						col = 1;
					//	printf("LINE %i\n", line);
						line++;
					}
					else
					{
						if (words_first_col == -1)
							words_first_col = col;
						col++;
					}


					uint32_t ch1 = uc.m_32;
					uint32_t ch2 = unicodeSearchBuffer.c_str()[ui];
					if (do_tolower)
					{
						if (ch1 < 127 && ch2 < 127)
						{
							ch1 = tolower(ch1);
							ch2 = tolower(ch2);
						}
					}

					if (ch1 == ch2)
					{
						++ui;
						if (ui == fintTextSize)
						{
							// Есть совпадение
							//printf("HIT in %s\n", p.generic_string().c_str());

							npSearchInFilesEntry entry_data;
							wsprintf(entry_data.m_filePath, L"%s", p.generic_wstring().c_str());
							wsprintf(entry_data.m_title, L"%s Line:%u Col:%u", 
								p.generic_wstring().c_str(), 
								line, 
								words_first_col);

							entry_data.m_line = line;
							entry_data.m_col = words_first_col;
							m_textSearchInFiles_data.push_back(entry_data);
							ui = 0;
							words_first_col = -1;
						}
					}
					else
					{
						ui = 0;
						words_first_col = -1;
					}
				}

			}

			fclose(f);
		}
	}

	for (const auto& o : m_textSearchInFiles_data)
	{
		SendDlgItemMessage(m_hDlg_find, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)o.m_title);
	}
}

void notepad::FindNext()
{
	size_t slen = wcslen(m_textSearchBuffer);
	if (slen && m_activeDocument)
	{
		m_activeDocument->OnTextSearch(true);
	}
}

void notepad::ShowFindWindow()
{
	m_hDlg_find = CreateDialog(GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDD_DIALOG1),
		0,
		FindDialogProc);
	ShowWindow(m_hDlg_find, SW_SHOW);
}

LRESULT CALLBACK SystemWindow_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam);
	notepad* app = (notepad*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	/*if (message == g_app->m_uMsgWakeUp)
	{
		if (IsIconic(hWnd))
			ShowWindow(hWnd, SW_RESTORE);
		SetForegroundWindow(hWnd);
		return 0;
	}*/

	switch (message)
	{
	case WM_COPYDATA: {
		PCOPYDATASTRUCT pCDS = (PCOPYDATASTRUCT)lParam;

		// Safety check: Validate the identifier code
		if (pCDS->dwData == 1) {
			LPWSTR receivedCmdLine = (LPWSTR)pCDS->lpData;

			// --- PROCESS YOUR COMMAND LINE HERE ---
			// Example: Parse args or open a file passed via receivedCmdLine
			MessageBox(hWnd, receivedCmdLine, L"Arguments Received", MB_OK);

			// Restore and flash window to notify the user
			if (IsIconic(hWnd)) ShowWindow(hWnd, SW_RESTORE);
			SetForegroundWindow(hWnd);

			return TRUE; // Tell the OS the message was handled
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		auto input = &g_app->m_input;
		input->m_isLMBDown_window = true;
		return 0;
	}
	case WM_LBUTTONUP:
	{
		auto input = &g_app->m_input;
		input->m_isLMBDown_window = false;
		return 0;
	}
	//case WM_MOUSEWHEEL:
	//{
	//	auto input = &g_app->m_input;
	//	input->m_wheelDelta = (float32_t)GET_WHEEL_DELTA_WPARAM(wParam);// / (float32_t)WHEEL_DELTA;
	//}
	//break;
	//case WM_MOUSEMOVE:
	//{
	//	auto xPos = GET_X_LPARAM(lParam);
	//	auto yPos = GET_Y_LPARAM(lParam);
	//	auto input = &g_app->m_input;
	//	input->m_cursorCoordsForGUI.x = (float32_t)xPos;
	//	input->m_cursorCoordsForGUI.y = (float32_t)yPos;
	//}	break;
	case WM_DESTROY:
	{
		return 0;
	}
	case WM_CAPTURECHANGED:
	case WM_NCLBUTTONDBLCLK:
	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCMBUTTONDBLCLK:
	case WM_NCMBUTTONDOWN:
	case WM_NCMBUTTONUP:
	case WM_NCRBUTTONDBLCLK:
	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONUP:
	case WM_NCMOUSEHOVER:
	case WM_NCMOUSELEAVE:
	{
		break;
	}
	case WM_EXITSIZEMOVE:
	{
		if (app)
			app->OnWindowSizing();
		break;
	}
	case WM_SETCURSOR:
	{
		auto id = LOWORD(lParam);
		switch (id)
		{
		case HTCLIENT:
		{
			if (app)
			{
				app->OnSetCursor();
				return TRUE;
			}
		}break;
		case HTLEFT:
		case HTRIGHT:
		case HTTOP:
			break;
		case HTBOTTOM:
			break;
		case HTTOPLEFT:
			break;
		case HTBOTTOMRIGHT:
			break;
		case HTBOTTOMLEFT:
			break;
		case HTTOPRIGHT:
			break;
		case HTHELP:
			break;
		}
	}break;
	case WM_CHAR:
	{
		switch (wParam)
		{
		case 0x08: // or '\b'
		// Process a backspace.
			break;
		case 0x0A: // or '\n'
			// Process a linefeed.
			break;
		case 0x0D:
			// Process a carriage return.
			break;
		case 0x1B:
			// Process an escape.
			break;
		default:{
			if (app)
				app->m_input.m_character = (TCHAR)wParam;
		}break;
		}
		break;
	}
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		bool isPress = true;

		npInputKey key = (npInputKey)wParam;

		if (message == WM_SYSKEYUP) isPress = false;
		if (message == WM_KEYUP) isPress = false;

		const UINT MY_MAPVK_VSC_TO_VK_EX = 3;

		if (key == npInputKey::K_SHIFT)
		{ // shift -> lshift rshift
			key = static_cast<npInputKey>(MapVirtualKey((static_cast<UINT>(lParam >> 16) & 255u), MY_MAPVK_VSC_TO_VK_EX));
		}
		if (key == npInputKey::K_CTRL)
		{ // ctrl -> lctrl rctrl
			key = static_cast<npInputKey>(MapVirtualKey((static_cast<UINT>(lParam >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX));
			if (lParam & 0x1000000)
				key = static_cast<npInputKey>(163);
		}

		if (key == npInputKey::K_ALT)
		{ // alt -> lalt ralt
			key = static_cast<npInputKey>(MapVirtualKey((static_cast<UINT>(lParam >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX));
			if (lParam & 0x1000000)
				key = static_cast<npInputKey>(165);
			//printf("alt = %i\n",(int)ev.keyboardEvent.key);
		}

		//uint8_t keys[256u];
		//GetKeyboardState(keys);
		//WORD chars[2];


		auto input = &g_app->m_input;
		if (isPress)
		{
			if ((uint32_t)key < 256)
			{
				input->m_key_hold[(uint32_t)key] = 1;
				input->m_key_hit[(uint32_t)key] = 1;
			}
		}
		else
		{
			if ((uint32_t)key < 256)
			{
				input->m_key_hold[(uint32_t)key] = 0;
				input->m_key_released[(uint32_t)key] = 1;
			}
		}

		if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP)
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		else
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		//	return 0;
		}
	}break;
	case WM_INPUT:
		if (app)
		{
			static npArray<char> m_RawInputMessageData;

			HRAWINPUT hRawInput = (HRAWINPUT)lParam;
			UINT dataSize;
			GetRawInputData(
				hRawInput, RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

			if (dataSize == 0)
				break;
			if (dataSize > m_RawInputMessageData.size())
				m_RawInputMessageData.reserve(dataSize);

			void* dataBuf = &m_RawInputMessageData[0];
			GetRawInputData(
				hRawInput, RID_INPUT, dataBuf, &dataSize, sizeof(RAWINPUTHEADER));

			const RAWINPUT* raw = (const RAWINPUT*)dataBuf;
			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				auto input = &g_app->m_input;
				HANDLE deviceHandle = raw->header.hDevice;

				const RAWMOUSE& mouseData = raw->data.mouse;

				USHORT flags = mouseData.usButtonFlags;
				short wheelDelta = (short)mouseData.usButtonData;
				LONG x = mouseData.lLastX, y = mouseData.lLastY;

				/*wprintf(
					L"Mouse: Device=0x%08X, Flags=%04x, WheelDelta=%d, X=%d, Y=%d\n",
					deviceHandle, flags, wheelDelta, x, y);*/

				input->m_mouseDelta.x = (float32_t)x;
				input->m_mouseDelta.y = (float32_t)y;
				if (wheelDelta)
					input->m_wheelDelta = (float32_t)wheelDelta / (float32_t)WHEEL_DELTA;

				POINT cursorPoint;
				GetCursorPos(&cursorPoint);
				ScreenToClient(hWnd, &cursorPoint);
				input->m_cursorCoords.x = cursorPoint.x;
				input->m_cursorCoords.y = cursorPoint.y;
				input->m_cursorCoordsForGUI.x = (float32_t)input->m_cursorCoords.x;
				input->m_cursorCoordsForGUI.y = (float32_t)input->m_cursorCoords.y;

				if (flags)
				{
					input->m_isLMBDown = (flags & 0x1) == 0x1;
				//	printf("\n%i\n", input->m_isLMBDown);
					input->m_isLMBUp = (flags & 0x2) == 0x2;
					if (input->m_isLMBDown) input->m_isLMBHold = true;
					if (input->m_isLMBUp)    input->m_isLMBHold = false;

					input->m_isRMBDown = (flags & 0x4) == 0x4;
					input->m_isRMBUp = (flags & 0x8) == 0x8;
					if (input->m_isRMBDown)  input->m_isRMBHold = true;
					if (input->m_isRMBUp)    input->m_isRMBHold = false;

					input->m_isMMBDown = (flags & 0x10) == 0x10;
					input->m_isMMBUp = (flags & 0x20) == 0x20;
					if (input->m_isMMBDown)  input->m_isMMBHold = true;
					if (input->m_isMMBUp)    input->m_isMMBHold = false;

					input->m_isX1MBDown = (flags & 0x100) == 0x100;
					input->m_isX1MBUp = (flags & 0x200) == 0x200;
					if (input->m_isX1MBDown)  input->m_isX1MBHold = true;
					if (input->m_isX1MBUp)    input->m_isX1MBHold = false;

					input->m_isX2MBDown = (flags & 0x40) == 0x40;
					input->m_isX2MBUp = (flags & 0x80) == 0x80;
					if (input->m_isX2MBDown)  input->m_isX2MBHold = true;
					if (input->m_isX2MBUp)    input->m_isX2MBHold = false;
				}
			}
		}
		break;
	case WM_COMMAND:
	{
		if (app)
			app->OnPopupCommand(LOWORD(wParam));
	}return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		if (hWnd == app->m_hwnd)
		{
			if (app)
				app->OnDraw();
		}
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_SIZE:
	{
		if (app)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			app->m_clientSize.x = rc.right - rc.left;
			app->m_clientSize.y = rc.bottom - rc.top;

			if (app->m_clientSize.x <= 0)
				app->m_clientSize.x = 1;
			if (app->m_clientSize.y <= 0)
				app->m_clientSize.y = 1;

			if (app->m_clientSize.x != app->m_clientSizeOld.x || app->m_clientSize.y != app->m_clientSizeOld.y)
			{
				app->m_clientSizeOld = app->m_clientSize;
				if (wmId != SIZE_MINIMIZED)
					app->OnWindowSizeChanged();
			}
		}
		switch (wmId)
		{
		case SIZE_MAXIMIZED:
			if (app)
				app->OnWindowMaximize();
			break;
		case SIZE_MINIMIZED:
			if (app)
				app->OnWindowMinimize();
			break;
		case SIZE_RESTORED:
			if (app)
				app->OnWindowRestore();
			break;
		}
		
		return 0;
	}break;
	
	case WM_SIZING:
	{
		if (app)
		{
			app->OnWindowSizing();
		}
	}
	break;
	case WM_ACTIVATE:
		switch (wParam)
		{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			if (app)
				app->OnWindowActivate();
			break;
		case WA_INACTIVE:
			if (app)
				app->OnWindowDeactivate();
			break;
		}
		break;
	case WM_ERASEBKGND:
		return 0;
	case WM_CLOSE:
	{
		if (app)
		{
			app->OnWindowClose();
		}
		return 0;
	}break;
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = 300;
		lpMMI->ptMinTrackSize.y = 200;
	}break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

INT_PTR CALLBACK FindDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		// Initialization code here (e.g., setting text, checkboxes)
		SetLayeredWindowAttributes(hDlg, 0, 155 + g_app->m_transparencyForFindWindow, LWA_ALPHA);
		SendDlgItemMessage(hDlg, IDC_SLIDER1, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
		SendDlgItemMessage(hDlg, IDC_SLIDER1, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)g_app->m_transparencyForFindWindow);

		CheckDlgButton(hDlg, IDC_CHECK1, g_app->m_textSearch_isCaseMatch ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hDlg, IDC_CHECK2, g_app->m_textSearch_inSubDirs ? BST_CHECKED : BST_UNCHECKED);

		//SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)L"File1");
		//SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)L"File2");
		//SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)L"File3");

		//очистка списка
		//SendDlgItemMessage(hDlg, IDC_LIST1, LB_RESETCONTENT, 0, 0);

		// сдвиг окна в центр монитора
		RECT r;
		GetWindowRect(hDlg, &r);
		int W = r.right - r.left;
		int H = r.bottom - r.top;
		if (W < 2)W = 2;
		if (H < 2)H = 2;
		RECT rD;
		GetWindowRect(GetDesktopWindow(), &rD);
		int WD = rD.right - rD.left;
		int HD = rD.bottom - rD.top;
		if (WD < 2)WD = 2;
		if (HD < 2)HD = 2;
		MoveWindow(hDlg, (WD/2) - (W/2), (HD/2) - (H/2), W, H, TRUE);

	}return (INT_PTR)TRUE;
	case WM_SHOWWINDOW:
		SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
		break;
	case WM_COMMAND:
		// Handle control clicks (buttons, menus)
		switch (LOWORD(wParam))
		{
		case IDC_LIST1:
		{
			switch (HIWORD(wParam))
			{
			case LBN_SELCHANGE: // Sent when selection changes
			{
				/*int index = (int)SendDlgItemMessage(hDlg, IDC_LIST1, LB_GETCURSEL, 0, 0);
				if (index != LB_ERR) {
				}*/
			}
			return (INT_PTR)TRUE;
			case LBN_DBLCLK: // Sent on double-click (Requires LBS_NOTIFY)
			{
				g_app->OnTextSearchInFilesOpenResult((int)SendDlgItemMessage(hDlg, IDC_LIST1, LB_GETCURSEL, 0, 0));
			}
			return (INT_PTR)TRUE;
			}
		}break;
		case IDC_CHECK1: {
			UINT uState = IsDlgButtonChecked(hDlg, IDC_CHECK1);
			g_app->m_textSearch_isCaseMatch = (uState == BST_CHECKED) ? true : false;
			SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
		}break;
		case IDC_CHECK2: {
			UINT uState = IsDlgButtonChecked(hDlg, IDC_CHECK2);
			g_app->m_textSearch_inSubDirs = (uState == BST_CHECKED) ? true : false;
			SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
		}break;
		case IDC_BUTTON1://find up
		{
			auto edit = GetDlgItem(hDlg, IDC_EDIT1);
			//g_app->m_textSearchBuffer[1024] = 0;
			GetWindowText(edit, g_app->m_textSearchBuffer, 1024);
			g_app->OnTextSearch(false);
		}
			break;
		case IDC_BUTTON2: //find down
		{
			auto edit = GetDlgItem(hDlg, IDC_EDIT1);
			//g_app->m_textSearchBuffer[1024] = 0;
			GetWindowText(edit, g_app->m_textSearchBuffer, 1024);
			g_app->OnTextSearch(true);
		}
			break;
		case IDC_BUTTON3: { //clear
			auto edit = GetDlgItem(hDlg, IDC_EDIT1);
			SetWindowTextW(edit, 0);
			SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
		}break;
		case IDC_BUTTON4: { //browse dir
			g_app->m_textSearch_path.clear();
			g_app->OpenOpenFileDialog(
			"Select Dir",
				"Select",
				0,
				0,
				&g_app->m_textSearch_path);
			if (g_app->m_textSearch_path.size())
			{
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT2), g_app->m_textSearch_path.c_str());
			}
			break;
		}
		case IDC_BUTTON5: { //start search
			if (g_app->m_textSearch_path.size())
			{
				GetWindowText(GetDlgItem(hDlg, IDC_EDIT2), g_app->m_textSearchBuffer, 1024);
				g_app->m_textSearch_path.clear();
				g_app->m_textSearch_path.assign(g_app->m_textSearchBuffer);

				GetWindowText(GetDlgItem(hDlg, IDC_EDIT1), g_app->m_textSearchBuffer, 1024);
				GetWindowText(GetDlgItem(hDlg, IDC_EDIT3), g_app->m_textSearchBuffer_extensions, 1024);

				g_app->OnTextSearchInFiles();
			}
			break;
		}
		case IDOK:

		case IDCANCEL:
			// Terminate the modal dialog box
			DestroyWindow(hDlg);
			return (INT_PTR)TRUE;
		}
		break;

	case WM_HSCROLL: {
		// Verify the message came from our specific slider control
		HWND hSlider = (HWND)lParam;
		if (hSlider == GetDlgItem(hDlg, IDC_SLIDER1)) {
			// Get the current slider position
			int pos = (int)SendMessage(hSlider, TBM_GETPOS, 0, 0);

			g_app->m_transparencyForFindWindow = pos;
			SetLayeredWindowAttributes(hDlg, 0, 155 + g_app->m_transparencyForFindWindow, LWA_ALPHA);
		}
		break;
	}
	}
	return (INT_PTR)FALSE; // Return FALSE for unhandled messages
}
