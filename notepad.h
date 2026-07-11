#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <assert.h>

#include <utility>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")


using npchar = wchar_t;

#include "npBase.h"
#include "npAssert.h"

#include "npVec.h"
#include "npInput.h"
#include "npArray.h"
#include "npString.h"
#include "npColor.h"
#include "npUserData.h"
#include "npID.h"
#include "npUnicodeConverter.h"
#include "npUnicodeString.h"

#include "npWinRegistry.h"

#include "npPopup.h"
#include "npImage.h"
#include "npTexture.h"
#include "npRenderer.h"
#include "npDocument.h"
#include "npDocument_text.h"
#include "npDocument_text2.h"
#include "npDocument_binary.h"

#define NOTEPAD_MENUID_EXIT 1
#define NOTEPAD_MENUID_UNDO 2
#define NOTEPAD_MENUID_CUT 3
#define NOTEPAD_MENUID_COPY 4
#define NOTEPAD_MENUID_PASTE 5
#define NOTEPAD_MENUID_DELETE 6
#define NOTEPAD_MENUID_SELECTALL 7
#define NOTEPAD_MENUID_FILE_NEWTEXTFILE 8
#define NOTEPAD_MENUID_FILE_NEWBINFILE 9
#define NOTEPAD_MENUID_FILE_OPENTEXTFILE 10
#define NOTEPAD_MENUID_FILE_OPENBINFILE 11
#define NOTEPAD_MENUID_FILE_SAVE 12
#define NOTEPAD_MENUID_FILE_SAVEAS 13
#define NOTEPAD_MENUID_FILE_CLOSEDOC 14
#define NOTEPAD_MENUID_REDO 15
#define NOTEPAD_MENUID_EDIT_FIND 16
#define NOTEPAD_MENUID_EDIT_FINDNEXT 17
#define NOTEPAD_MENUID_FILE_CLOSEDOCOTHER 18

struct npColorTheme
{
	npColor m_docBarBG;
	npColor m_docBarItemBG;
	npColor m_docBarItemBGMouseHover;
	npColor m_docBarItemText;

	npColor m_editorBG;
	npColor m_editorScrollBG;
	npColor m_editorScrollButtonBG;
	npColor m_editorScrollRectangle;
	npColor m_editorText;
	npColor m_editorSelectBG;
	npColor m_editorSelectText;
	npColor m_editorCursor;
	npColor m_editorCurrLineBG;

	npColor m_editorBottomBarBG;
	
	npColor m_editorLineBarBG;
	
	npColor m_hexeditorBBG;
	npColor m_hexeditorBText;
	npColor m_hexeditorTText;
};

struct npOpenFIleDialogFileTypeDesc
{
	wchar_t m_title[100];
	wchar_t m_extensions[1000];
};

struct npSearchInFilesEntry
{
	wchar_t m_filePath[1024];
	wchar_t m_title[1024*2];
	uint32_t m_line = 1;
	uint32_t m_col = 1;
};

enum class npCursorType : uint32_t
{
	Arrow,
	Cross,
	Hand,
	Help,
	IBeam,
	No,
	Size,
	SizeNESW,
	SizeNS,
	SizeNWSE,
	SizeWE,
	UpArrow,
	Wait,

	_num
};

struct IFileSaveDialog;
struct IFileOpenDialog;

class notepad
{
	friend class npRenderer_gdi;
	friend class npFont_gdi;


	bool m_run = true;

	npRenderer* m_renderer = 0;
	//gui * m_gui = 0;


	npVec4f m_documentBarRect;
	npTexture* m_docBarBGTexture = 0;
	float32_t m_docBarHeight = 20.f;

	npVec4f m_rightPanelRect;

	npColorTheme m_colorTheme_light;


	npFont* m_fontUI = 0;

	HCURSOR m_cursors[(uint32_t)npCursorType::_num];

	npArray<npDocument*> m_documents;

	npDocument* m_activeDocument = 0;
	float m_dt = 0.f;

	bool m_clickedLMB = false;
	bool m_secondWindow = false;

	IFileSaveDialog* m_fileSaveDialog = 0;
	IFileOpenDialog* m_fileOpenDialog = 0;
public:
	notepad();
	~notepad();

	void OnDraw();
	void OnWindowSizing();
	void OnWindowSizeChanged();
	void OnWindowMaximize();
	void OnWindowMinimize();
	void OnWindowRestore();
	void OnWindowActivate();
	void OnWindowDeactivate();
	void OnSetCursor();

	bool Init(bool secondWindow);
	void Run();
	void Draw();
	void Update();
	
	void WriteRegistry();
	void ReadRegistry();

	void OnRun();
	void OnUpdate();
	uint64_t GetTime();

	void OnWindowClose();

	npFont* GetFontUI() { return m_fontUI; }
	
	void CopyTextToClipboard(npString_base<npchar>*);
	void GetTextFromClipboard(npString_base<npchar>*);

	npCursorType m_currentCursor = npCursorType::Arrow;

	npVec2u m_clientSize;
	npVec2u m_clientSizeOld;
	npInput m_input;
	HWND m_hwnd;
	HWND m_hDlg_find;
	HDC m_dc = 0;
	//UINT m_uMsgWakeUp = 0;
	int m_transparencyForFindWindow = 0;
	void ShowFindWindow();
	void FindNext();

	// все иконки в текстуре
	npTexture* m_GUITexture = 0;

	npVec4f m_documentAreaRect;

	//npPopup* m_popupTextEditorPopup = 0;
	npColorTheme* m_currColorTheme;

	npchar m_textSearchBuffer[1025];
	npchar m_textSearchBuffer_extensions[1025];
	bool m_textSearch_isCaseMatch = false;
	bool m_textSearch_inSubDirs = true;
	npStringW m_textSearch_path;
	std::vector<npSearchInFilesEntry> m_textSearchInFiles_data;


	void OnTextSearch(bool down);
	void OnTextSearchInFiles();
	void OnTextSearchInFilesOpenResult(uint32_t);

	void ActivateCursorIBeam();
	void ActivateCursorArrow();

	void OnPopupCommand(uint32_t);

	void CreateNewTextDocument(
		const wchar_t* title, 
		const char* filePath,
		npUnicodeString* text,
		uint32_t format);
	void CreateNewBinaryDocument();
	void _createNewBinaryDocument(const char*);
	void OpenText();
	void OpenText(const char*);
	void OpenBinary();
	void OpenBinary(const char*);

	bool PointInRect(float32_t x, float32_t y, const npVec4f& rect);
	float32_t Normalize(float32_t max, float32_t val)
	{
		return val * (1.f / max);
	}
	float32_t Distance2D(
		float32_t p1x,
		float32_t p1y,
		float32_t p2x,
		float32_t p2y)
	{
		p1x -= p2x;
		p1y -= p2y;

		return sqrtf((p1x * p1x) + (p1y * p1y));
	}

	void RepaintWindow();

	void OpenSaveFileDialog(
		const char* title,
		const char* okButtonLabel,
		const char* extension,
		npStringW*);
	void OpenOpenFileDialog(
		const char* title,
		const char* okButtonLabel,
		npOpenFIleDialogFileTypeDesc* descs,
		uint32_t descsSz,
		npStringW*);

	void CloseDocument();
	void CloseDocumentOther();
	void ShowDocBarItemPopup(npDocument*);
};

