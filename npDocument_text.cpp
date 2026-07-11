#include "notepad.h"

#include <filesystem>
extern notepad* g_app;

npDocument_text::npDocument_text(npRenderer* r,
	const char* filePath,
	npUnicodeString* text,
	uint32_t format)
	:
	npDocument(r)
{
	switch (format)
	{
	default:
	case 1:
		m_format = format_utf8;
		break;
	case 2:
		m_format = format_utf16;
		break;
	case 3:
		m_format = format_utf16be;
		break;
	case 4:
		m_format = format_utf32;
		break;
	case 5:
		m_format = format_utf32be;
		break;
	}

	if (filePath)
	{
		m_isSaved = true;
		m_filePath = filePath;
		npUnicodeConverter uc;
		uc.char_to_wchar(m_filePath.c_str(), m_filePath.size(), &m_filePathW);
		
		_setTitleFromPath();
	}

	if (text)
	{
		text->ToUTF16(m_text);
		m_needUpdate = true;
	}

	m_fontEditor = m_renderer->CreateNPFont("Consolas", false, false, m_fontEditorSize);

	
}

npDocument_text::~npDocument_text()
{
	if (m_fontEditor)
		delete m_fontEditor;
}

void npDocument_text::Update(float dt)
{
	m_editorArea = g_app->m_documentAreaRect;
	m_bottomBarBG.x = m_editorArea.x;
	m_bottomBarBG.y = m_editorArea.w - m_bottomBarHeight;
	m_bottomBarBG.z = m_editorArea.z;
	m_bottomBarBG.w = m_editorArea.w;

	m_editorAreaText = m_editorArea;
	g_app->m_currentCursor = npCursorType::Arrow;
	

	auto input = &g_app->m_input;

	if (g_app->PointInRect(input->m_cursorCoordsForGUI.x,
		input->m_cursorCoordsForGUI.y, m_vscrollRectangle))
	{
		if (input->m_isLMBDown)
		{
			m_cursorHoldVScroll = true;
			m_vscrollSaveOnClick = m_vscrollRectangle.y - m_vscroll_upBtnRect.w;
			m_vscrollSaveOnClickYMouseCoord = input->m_cursorCoordsForGUI.y - m_vscroll_upBtnRect.w;
		}
	}
	else if (g_app->PointInRect(input->m_cursorCoordsForGUI.x,
		input->m_cursorCoordsForGUI.y, m_hscrollRectangle))
	{
		if (input->m_isLMBDown)
		{
			m_cursorHoldHScroll = true;
			m_hscrollSaveOnClick = m_hscrollRectangle.x - m_hscroll_leftBtnRect.z;
			m_hscrollSaveOnClickXMouseCoord = input->m_cursorCoordsForGUI.x - m_hscroll_leftBtnRect.z;
		}
	}

	//printf("%f\n", m_editorAreaText.z);

	if (m_needToDrawVScroll)
	{
		m_vscrollBGRect.x = m_editorArea.z - m_vscrollSize;
		m_vscrollBGRect.y = m_editorArea.y;
		m_vscrollBGRect.z = m_editorArea.z;
		m_vscrollBGRect.w = m_editorArea.w;
		m_vscroll_upBtnRect.x = m_vscrollBGRect.x;
		m_vscroll_upBtnRect.y = m_vscrollBGRect.y;
		m_vscroll_upBtnRect.z = m_vscrollBGRect.z;
		m_vscroll_upBtnRect.w = m_vscroll_upBtnRect.y + 10.f;
		m_vscroll_downBtnRect.x = m_vscrollBGRect.x;
		m_vscroll_downBtnRect.y = m_vscrollBGRect.w - 10.f;
		m_vscroll_downBtnRect.z = m_vscrollBGRect.z;
		m_vscroll_downBtnRect.w = m_vscrollBGRect.w;
		m_vscrollRectangle = m_vscrollBGRect;
		m_vscrollRectangle.y = m_vscroll_upBtnRect.w;
		m_vscrollRectangle.w = m_vscroll_downBtnRect.y;
		m_vscrollRectangle.x += 2.f;
		m_vscrollRectangle.z -= 2.f;
		float32_t d = g_app->Normalize((float32_t)m_numOfLines + m_maxNumOfVisibleLines, (float32_t)m_maxNumOfVisibleLines);
		float32_t rectHeight = m_vscrollRectangle.w - m_vscrollRectangle.y;
		m_vscrollRectangle.w = m_vscrollRectangle.y + (rectHeight * d);
		// нельзя слишком маленький
		rectHeight = m_vscrollRectangle.w - m_vscrollRectangle.y;
		if (rectHeight < 30.f)
			m_vscrollRectangle.w = m_vscrollRectangle.y + 30.f;
		rectHeight = m_vscrollRectangle.w - m_vscrollRectangle.y;
		// мне нужно вычислить чему равно перемещение скролл прямоугольника
		// на 1 пиксель. какому количеству m_vertical_offset.
		// и наоборот, зная m_vertical_offset нужно правильно вычислить
		// количество пикселей.
		
		m_editorAreaText.z -= m_vscrollSize;

		// количество пикселей после прямоугольника.
		// до нижней кнопки
		float32_t emptySpaceInPixels = m_vscroll_downBtnRect.y - m_vscrollRectangle.w;
		auto vscrollRectangleSave = m_vscrollRectangle;
		float32_t ntext = g_app->Normalize(m_textHeight, m_vertical_offset);
		// m_textHeight даёт высоту всего текста в пикселях
		float32_t pixels = emptySpaceInPixels * ntext;
		m_vscrollRectangle.y += pixels;
		m_vscrollRectangle.w += pixels;

		if (m_cursorHoldVScroll && (input->m_mouseDelta.y != 0.f))
		{
			//float32_t ntext = normalize(m_textHeight, m_vertical_offset);
			float32_t ycoord = input->m_cursorCoordsForGUI.y - m_vscroll_upBtnRect.w;
			float32_t coord_offset = ycoord - m_vscrollSaveOnClickYMouseCoord;
			pixels = m_vscrollSaveOnClick + coord_offset;
			float32_t nscroll = g_app->Normalize(emptySpaceInPixels, pixels);

			m_vertical_offset = m_textHeight * nscroll;
			if (m_vertical_offset > m_textHeight)
				m_vertical_offset = m_textHeight;
			if (m_vertical_offset < 0.f)
				m_vertical_offset = 0.f;

			m_needUpdate = true;
		}
	}

	if (m_needToDrawHScroll)
	{
		m_hscrollBGRect.x = m_editorArea.x;
		m_hscrollBGRect.y = m_editorArea.w - m_hscrollSize;
		m_hscrollBGRect.z = m_editorArea.z - m_hscrollRightSideIndent;
		m_hscrollBGRect.w = m_editorArea.w;
		m_hscroll_leftBtnRect.x = m_hscrollBGRect.x;
		m_hscroll_leftBtnRect.y = m_hscrollBGRect.y;
		m_hscroll_leftBtnRect.z = m_hscroll_leftBtnRect.x + 10.f;
		m_hscroll_leftBtnRect.w = m_hscrollBGRect.w;
		m_hscroll_rightBtnRect.x = m_hscrollBGRect.z - 10.f;
		m_hscroll_rightBtnRect.y = m_hscrollBGRect.y;
		m_hscroll_rightBtnRect.z = m_hscrollBGRect.z;
		m_hscroll_rightBtnRect.w = m_hscrollBGRect.w;
		m_hscrollRectangle.x = m_hscroll_leftBtnRect.z;
		m_hscrollRectangle.y = m_hscroll_leftBtnRect.y;
		m_hscrollRectangle.z = m_hscroll_rightBtnRect.x;
		m_hscrollRectangle.w = m_hscroll_rightBtnRect.w;
		m_hscrollRectangle.y += 2.f;
		m_hscrollRectangle.w -= 2.f;

		// вначале ползунок имеет размер во всю ширину.
		// Когда в ширину символы выходят за определённую границу
		// этот ползунок нужно укоротить, чтобы можно было сдвинуть текст.
		// .z координата равняется .x + (укороченная ширина)

		// ширина текстовой области
		auto textAreaWidth = m_editorAreaText.z - m_editorAreaText.x;
		// максимальная ширина текста + некое значение лимита
		// чтобы прокрутить хотяб немного
		float32_t maxPixelsWidth = 100.f + m_maxLineWidth;
		// преобразование в подобие процента, от 0 до 1
		float32_t d = g_app->Normalize(maxPixelsWidth, textAreaWidth);
		float32_t rectWidth = m_hscrollRectangle.z - m_hscrollRectangle.x;
		m_hscrollRectangle.z = m_hscrollRectangle.x + (rectWidth * d);

		// сдвиг ползунка используя текущее значение горизонтального смещения
		float32_t emptySpaceInPixels = m_hscroll_rightBtnRect.x - m_hscrollRectangle.z;
		float32_t ntext = g_app->Normalize(m_hscroll_widthLen, m_horizontal_offset);
		float32_t pixels = emptySpaceInPixels * ntext;
		m_hscrollRectangle.x += pixels;
		m_hscrollRectangle.z += pixels;

		// двигаем мышкой
		if (m_cursorHoldHScroll && (input->m_mouseDelta.x != 0.f))
		{
			float32_t xcoord = input->m_cursorCoordsForGUI.x - m_hscroll_leftBtnRect.z;
			float32_t coord_offset = xcoord - m_hscrollSaveOnClickXMouseCoord;
			pixels = m_hscrollSaveOnClick + coord_offset;
			float32_t nscroll = g_app->Normalize(emptySpaceInPixels, pixels);

			auto ww = maxPixelsWidth;
			m_horizontal_offset = m_hscroll_widthLen * nscroll;
			if (m_horizontal_offset > m_hscroll_widthLen)
				m_horizontal_offset = m_hscroll_widthLen;
			if (m_horizontal_offset < 0.f)
				m_horizontal_offset = 0.f;
		}
	}

	m_editorAreaText.w -= m_bottomBarHeight;

	if (g_app->PointInRect(input->m_cursorCoordsForGUI.x,
		input->m_cursorCoordsForGUI.y, m_editorAreaText))
	{
		if (!m_cursorInTextEditor)
		{
			m_cursorInTextEditor = true;
		}
		g_app->m_currentCursor = npCursorType::IBeam;

		if (input->m_wheelDelta)
		{
			if (input->m_kbm == npKeyboardModifier::Ctrl)
			{
				if (input->m_wheelDelta > 0.f)
				{
					m_fontEditorSize += 1;
				}
				if (input->m_wheelDelta < 0.f)
				{
					m_fontEditorSize -= 1;
				}
				if (m_fontEditorSize < 6)
					m_fontEditorSize = 6;
				if (m_fontEditorSize > 26)
					m_fontEditorSize = 26;

				if (m_fontEditor)
					delete m_fontEditor;
				m_fontEditor = m_renderer->CreateNPFont("Consolas", false, false, m_fontEditorSize);
			}
			else
			{
				//auto fontSize = m_fontEditor->GetFontSize();
				//printf("m_wheelDelta %f\n", input->m_wheelDelta);
				if (input->m_wheelDelta > 0.f)
				{
					//m_needUpdate = true;
					//m_vertical_offset -= fontSize.y * 3;
					_moveUpView(3);
				}
				if (input->m_wheelDelta < 0.f)
				{
					//m_vertical_offset += fontSize.y * 3;
					_moveDownView(3);
				}
				/*if (m_vertical_offset < 0)
				{
					m_vertical_offset = 0;
				}
				if (m_vertical_offset > m_textHeight)
				{
					m_vertical_offset = m_textHeight;
				}*/
			}
		}

		if (input->m_isLMBDown)
		{
			if (!m_clickedLMB)
				m_clickedLMB = true;
		}
		if (input->m_isRMBUp)
		{
			/*g_app->m_popupTextEditorPopup->Show(
				(uint32_t)input->m_cursorCoordsForGUI.x,
				(uint32_t)input->m_cursorCoordsForGUI.y);*/
		}
	}
	else
	{
		if (m_cursorInTextEditor)
		{
			m_cursorInTextEditor = false;
			g_app->ActivateCursorArrow();
		}
	}

	if (input->m_isLMBUp)
	{
		m_clickedLMB = false;
		m_cursorHoldVScroll = false;
		m_cursorHoldHScroll = false;
	}

	if (m_isEditActive && m_clickedLMB)
	{
		auto fontSize = m_fontEditor->GetFontSize();
		bool mouseMove = false;
		if (input->m_mouseDelta.x != 0.f || input->m_mouseDelta.y != 0.f)
			mouseMove = true;

		if (m_char_nextCursorPosition != -1)
		{
			auto textCursorBeforeClick = m_textCursorPosition;
			if ((input->m_isLMBHold && mouseMove) || input->m_isLMBDown)
			{
				_showTextCursor();
				m_textCursorPosition = m_char_nextCursorPosition;
				m_textCursorRect.x = m_char_nextCursorPosition_rect.x;
				m_textCursorRect.y = m_char_nextCursorPosition_rect.y;
				m_textCursorRect.z = m_textCursorRect.x + 1;
				m_textCursorRect.w = m_textCursorRect.y + fontSize.y;
				_findVerticalOffset();
				_findHorizontalOffset();
				
				m_getCursorColPrefer = true;

				if (input->m_kbm != npKeyboardModifier::Shift)
				{
					DeselectAll();
					if (input->m_isLMBDown)
						m_textCursorPositionWhenClick = m_textCursorPosition;
				}

				if (mouseMove)
				{
					if (!m_isSelected)
					{
						m_isSelected = true;
						m_selectionStart = m_textCursorPositionWhenClick;
					}
					m_selectionEnd = m_textCursorPosition;
				}
				else
				{
					if (input->m_kbm == npKeyboardModifier::Shift)
					{
						if (m_isSelected)
						{
							m_selectionEnd = m_textCursorPosition;
						}
						else
						{
							m_isSelected = true;
							m_selectionStart = textCursorBeforeClick;
							m_selectionEnd = m_textCursorPosition;
						}
					}
				}
			}
		}
		else
		{
			_findVerticalOffset();
		}
	}


	if (m_isEditActive)
	{
		m_textCursorTimer += dt;
		if (m_textCursorTimer > m_textCursorTimerLimit)
		{
			m_textCursorTimer = 0.f;
			m_drawTextCursor = m_drawTextCursor ? false : true;
		}

		auto* text = &m_text;
		auto text_size = text->size();
		if (text_size)
		{
			if (input->IsKeyHit(npInputKey::K_LEFT))
				_goLeft();
			else if (input->IsKeyHit(npInputKey::K_RIGHT))
				_goRight();
			else if (input->IsKeyHit(npInputKey::K_UP))
				_goUp();
			else if (input->IsKeyHit(npInputKey::K_DOWN))
				_goDown();
			else if (input->IsKeyHit(npInputKey::K_DELETE)
				&& input->m_kbm != npKeyboardModifier::Shift)
				_delete();
			else if (input->IsKeyHit(npInputKey::K_BACKSPACE))
				_backspace();
			else if (input->IsKeyHit(npInputKey::K_HOME))
				_goHome();
			else if (input->IsKeyHit(npInputKey::K_END))
				_goEnd();
			else if (input->IsKeyHit(npInputKey::K_PGUP))
				_goPageUp();
			else if (input->IsKeyHit(npInputKey::K_PGDOWN))
				_goPageDown();
		}

		if (input->m_kbm == npKeyboardModifier::Ctrl)
		{
			if (input->IsKeyHit(npInputKey::K_A))
			{
				_goEndOfText();
				SelectAll();
			}
			else if (input->IsKeyHit(npInputKey::K_X))
				CutToClipboard();
			else if (input->IsKeyHit(npInputKey::K_C))
				CopyToClipboard();
			else if (input->IsKeyHit(npInputKey::K_V))
				PasteFromClipboard();
			else if (input->IsKeyHit(npInputKey::K_INSERT))
				CopyToClipboard();
			else if (input->IsKeyHit(npInputKey::K_S))
				Save();
		}
		else if (input->m_kbm == npKeyboardModifier::ShiftCtrl)
		{
			if (input->IsKeyHit(npInputKey::K_S))
				SaveAs();
		}
		else
		{
			if (input->m_kbm == npKeyboardModifier::Shift)
			{
				if (input->IsKeyHit(npInputKey::K_INSERT))
					PasteFromClipboard();
				if (input->IsKeyHit(npInputKey::K_DELETE))
					CutToClipboard();
			}
			if (input->m_character)
			{
				if (input->m_character != L'\n'
					&& input->m_character != 8 // backspace
					&& input->m_character != 13 // enter
					&& input->m_character != 27 // escape
					)
				{
					bool ok = true;
					//wprintf(L"%c %i\n", g_engine->m_inputContext->m_character,(s32)g_engine->m_inputContext->m_character);
					//*if (!OnCharacter(input->m_character))
					//	ok = false;

						//if (m_charLimit > 0)
						//{
						//	if (m_text.Size() >= m_charLimit)
						//		ok = false;
						//}

					if (ok)
					{
						if (m_isSelected)
							DeleteSelected();
						m_isSaved = false;
						m_text.Insert(input->m_character, m_textCursorPosition);
						m_getCursorColPrefer = true;
						++m_textCursorPosition;
						_showTextCursor();
						_updateTextCursorOnKeyboardKey();
					}
				}
			}
		}

		bool enter = input->IsKeyHit(npInputKey::K_ENTER);

		if (input->IsKeyHit(npInputKey::K_ESCAPE))
		{
			_updateTextCursorOnKeyboardKey();
			if (m_isSelected)
				DeselectAll();
			//else
			//	Deactivate(false);
		}
		//*else if (enter && m_oneLine)
		//{
		///	DeselectAll();
		//	Deactivate(true);
		//}
		else if (enter)
		{
			_updateTextCursorOnKeyboardKey();
			if (m_isSelected)
				DeleteSelected();

			m_isSaved = false;
			m_text.Insert(L'\n', m_textCursorPosition);
			++m_textCursorPosition;
			_showTextCursor();
		}
	}
}

void npDocument_text::Draw(float dt)
{
	m_renderer->SetClipRect(m_editorArea);
	m_renderer->DrawRectangle(m_editorArea, g_app->m_currColorTheme->m_editorBG);

	auto editorHeight = m_editorArea.w - m_editorArea.y;
	auto input = &g_app->m_input;

	m_renderer->DrawRectangle(m_bottomBarBG, g_app->m_currColorTheme->m_editorBottomBarBG);
	m_renderer->SetNPFont(g_app->GetFontUI());
	{
		npchar buf[100];
		int sz = wsprintfW(buf, L"Ln:%u", m_textCursorLine);
		float32_t offset = 0.f;
		m_renderer->DrawText(
			buf, sz,
			m_editorArea.z - m_hscrollRightSideIndent,
			m_editorArea.w - 14,
			g_app->m_currColorTheme->m_docBarItemText);
		offset = 50;
		sz = wsprintfW(buf, L"Col:%u", m_textCursorCol);
		m_renderer->DrawText(
			buf, sz,
			m_editorArea.z - m_hscrollRightSideIndent + offset,
			m_editorArea.w - 14,
			g_app->m_currColorTheme->m_docBarItemText);
	}

	if (m_needToDrawVScroll)
	{
		m_renderer->DrawRectangle(m_vscrollBGRect, g_app->m_currColorTheme->m_editorScrollBG);
		m_renderer->DrawRectangle(m_vscroll_upBtnRect, g_app->m_currColorTheme->m_editorScrollButtonBG);
		m_renderer->DrawImage(
			npVec2f(2 + m_vscroll_upBtnRect.x, 2 + m_vscroll_upBtnRect.y),
			npVec2i(0, 0),
			npVec2i(9, 5),
			g_app->m_GUITexture);
		m_renderer->DrawRectangle(m_vscroll_downBtnRect, g_app->m_currColorTheme->m_editorScrollButtonBG);
		m_renderer->DrawImage(
			npVec2f(2 + m_vscroll_downBtnRect.x, 2 + m_vscroll_downBtnRect.y),
			npVec2i(9, 0),
			npVec2i(9, 5),
			g_app->m_GUITexture);
		m_renderer->DrawRectangle(m_vscrollRectangle, g_app->m_currColorTheme->m_editorScrollRectangle);
	}

	if (m_needToDrawHScroll)
	{
		m_renderer->DrawRectangle(m_hscrollBGRect, g_app->m_currColorTheme->m_editorScrollBG);

		m_renderer->DrawRectangle(m_hscroll_leftBtnRect, g_app->m_currColorTheme->m_editorScrollButtonBG);
		m_renderer->DrawImage(
			npVec2f(2 + m_hscroll_leftBtnRect.x, 2 + m_hscroll_leftBtnRect.y),
			npVec2i(18, 0),
			npVec2i(5, 9),
			g_app->m_GUITexture);
		m_renderer->DrawRectangle(m_hscroll_rightBtnRect, g_app->m_currColorTheme->m_editorScrollButtonBG);
		m_renderer->DrawImage(
			npVec2f(2 + m_hscroll_rightBtnRect.x, 2 + m_hscroll_rightBtnRect.y),
			npVec2i(23, 0),
			npVec2i(5, 9),
			g_app->m_GUITexture);
		m_renderer->DrawRectangle(m_hscrollRectangle, g_app->m_currColorTheme->m_editorScrollRectangle);
	}

	m_renderer->SetClipRect(m_editorAreaText);
	m_renderer->SetNPFont(m_fontEditor);

	auto textAreaSizeX = m_editorAreaText.z - m_editorAreaText.x;
	auto textAreaSizeY = m_editorAreaText.w - m_editorAreaText.y;
	bool isTextCursorWasVisible = false;
	auto fontSize = m_fontEditor->GetFontSize();
	m_nextCharPositionOffset = fontSize.x;
	m_nextLinePositionOffset = fontSize.y;

	m_maxNumOfVisibleLines = (uint32_t)floorf(editorHeight / m_nextLinePositionOffset);
	if (!m_maxNumOfVisibleLines)
		m_maxNumOfVisibleLines = 1;
	else if (m_maxNumOfVisibleLines > 1)
		m_maxNumOfVisibleLines -= 1;

	m_tabCounter = 0;

	auto* text = &m_text;
	if (text->size())
	{
		float carriagePosition = 0.f;
		float rowPosition = 0.f;

		uint32_t selminimum = m_selectionStart;
		uint32_t selmaximum = m_selectionEnd;
		if (m_selectionStart > m_selectionEnd)
		{
			selminimum = m_selectionEnd;
			selmaximum = m_selectionStart;
		}


		size_t char_underMouse = -1;
		npVec4f char_underMouse_rect;
		size_t char_closestMouse = -1;
		npVec4f char_closestMouse_rect;
		size_t char_closestOnLineMouse = -1;
		npVec4f char_closestOnLineMouse_rect;
		size_t char_closestOnLastLineMouse = -1;
		npVec4f char_closestOnLastLineMouse_rect;

		m_char_nextCursorPosition = -1;
		m_char_nextCursorPosition_rect = npVec4f();

		m_textCursorPositionLocation = 0;
		
		{
			m_textCursorPositionY = 0.f;
		}
		
		float textHeight = 0;

		float32_t lineWidth = 0.f;

		bool isVisiblePartWasDrawn = false;
		bool formouseclick_lineFound = false;
		bool formouseclick_lineEnd = false;
		auto textSize = text->size();
		auto textArraySize = text->size() + 1;
		float32_t distanceClosest = FLT_MAX;
		float32_t distanceClosestOnLine = FLT_MAX;
		float32_t distanceClosestOnLastLine = FLT_MAX;

		size_t firstDrawableChar = -1;
		size_t firstDrawableCharLine = 0;
		float32_t firstDrawableChar_rowPos = 0.f;
		
		if (m_needUpdate)
		{
			m_drawStartIndex = 0;
			m_firstVisibleCharLine = 0;
			m_startRowPosition = 0.f;
			m_numOfLines = 1;
		//	m_lineCounter = 0;
		}
		
		uint32_t firstVisibleCharLine_counter = m_firstVisibleCharLine;

		uint32_t numOfLines = m_numOfLines;

		size_t startIndex = m_drawStartIndex;
		//uint32_t lineCounter = m_lineCounter;
		
		uint32_t charCol = 0;
		uint32_t charColAdd = 1;

		rowPosition = m_startRowPosition;
		for (size_t i = startIndex; i < textArraySize; ++i)
		{
			//if (!i)
			//	printf("a");

			wchar_t currChar = text->c_str()[i];

			npVec4f charRect;
			charRect.x = m_editorAreaText.x + carriagePosition;
			charRect.y = m_editorAreaText.y + rowPosition;

			if (i == m_textCursorPosition)
			{
				m_textCursorPositionY = charRect.y - m_editorAreaText.y;
				//printf("%f %f\n", m_textCursorPositionY, m_vertical_offset);
			}

			charRect.x -= m_horizontal_offset;
			charRect.y -= m_vertical_offset;
			//charRect.y -= m_vertical_scroll;

			charRect.z = charRect.x + fontSize.x;
			charRect.w = charRect.y + fontSize.y;

			float32_t charRectHalfLen = (charRect.z - charRect.x) * 0.5f;

			npVec4f charRectForPick = charRect;
			charRectForPick.x -= charRectHalfLen;
			charRectForPick.z -= charRectHalfLen;

			auto centerX = charRect.x + ((charRect.z - charRect.x) * 0.5f);
			auto centerY = charRect.y + ((charRect.w - charRect.y) * 0.5f);



			bool charVisible = false;
			if (charRect.y > m_editorAreaText.y - fontSize.y - fontSize.y)
				charVisible = true;

			if (charRect.w > m_editorAreaText.w + fontSize.y + fontSize.y)
				charVisible = false;

			if (charVisible)
			{
				isVisiblePartWasDrawn = true;

				if (firstDrawableChar==-1)
				{
					firstDrawableChar = i;
					firstDrawableChar_rowPos = rowPosition;
					firstDrawableCharLine = firstVisibleCharLine_counter;
				//	printf("%u %u\n", firstDrawableChar, firstDrawableCharLine);
				//	if (i)
				//		printf("a");
				}

				if (i == m_textCursorPosition)
				{
					m_textCursorPositionLocation = 1;
				}


				npColor textColor = g_app->m_currColorTheme->m_editorText;

				if (m_isSelected)
				{
					if (i >= selminimum && i < selmaximum)
					{
						npVec4f rect;
						rect.x = charRect.x;
						rect.y = charRect.y;
						rect.z = rect.x;
						rect.w = rect.y;
						rect.z += m_nextCharPositionOffset;
						rect.w += m_nextLinePositionOffset;

						if (currChar == '\t')
						{
							for (size_t o = m_tabCounter; o < m_tabSize; ++o)
							{
								rect.z += m_nextCharPositionOffset;
							}
						}

						m_renderer->DrawRectangle(rect, g_app->m_currColorTheme->m_editorSelectBG);

						textColor = g_app->m_currColorTheme->m_editorSelectText;
					}
				}
				m_renderer->DrawText(&currChar, 1, charRect.x, charRect.y, textColor);

				if (g_app->PointInRect(input->m_cursorCoordsForGUI.x,
					input->m_cursorCoordsForGUI.y, charRectForPick))
				{
					char_underMouse = i;
					char_underMouse_rect = charRect;
				}

				auto d = g_app->Distance2D(centerX, centerY,
					input->m_cursorCoordsForGUI.x,
					input->m_cursorCoordsForGUI.y);
				if (d < distanceClosest)
				{
					distanceClosest = d;
					char_closestMouse = i;
					char_closestMouse_rect = charRect;
				}

				if (input->m_cursorCoordsForGUI.y >= charRect.y
					&& input->m_cursorCoordsForGUI.y < charRect.w)
				{
					if (d < distanceClosestOnLine)
					{
						distanceClosestOnLine = d;
						char_closestOnLineMouse = i;
						char_closestOnLineMouse_rect = charRect;
					}
				}


				if (d < distanceClosestOnLastLine)
				{
					distanceClosestOnLastLine = d;
					char_closestOnLastLineMouse = i;
					char_closestOnLastLineMouse_rect = charRect;
				}
				if (currChar == L'\n')
				{
					distanceClosestOnLastLine = FLT_MAX;
				}


				if (i == m_textCursorPosition)
				{
					isTextCursorWasVisible = true;
					m_textCursorRect.x = charRect.x;
					m_textCursorRect.y = charRect.y;
					m_textCursorRect.z = m_textCursorRect.x + 1;
					m_textCursorRect.w = m_textCursorRect.y + fontSize.y;
				}
			}
			else
			{
				//if (m_needUpdate)
				{
					if (isVisiblePartWasDrawn)
					{
						if (i == m_textCursorPosition)
							m_textCursorPositionLocation = 2;
					}
				}
			}


			if (currChar == '\t')
			{
				charColAdd = 0;
				for (size_t o = m_tabCounter; o < m_tabSize; ++o)
				{
					carriagePosition += m_nextCharPositionOffset;
					++charColAdd;
				}
				m_tabCounter = 0;
			}
			else
			{
				carriagePosition += m_nextCharPositionOffset;

				++m_tabCounter;
				if (m_tabCounter == m_tabSize)
					m_tabCounter = 0;
			}

			lineWidth += m_nextCharPositionOffset;
			
			if (m_textCursorPosition == i)
			{
				m_textCursorCol = charCol;
			//	m_textCursorLine = lineCounter;
				printf("%i\n", m_textCursorLine);
				//m_lineCounter = lineCounter;

				if (m_getCursorColPrefer)
				{
					m_getCursorColPrefer = false;
					m_textCursorCol_prefer = m_textCursorCol;
				}
			}
			charCol += charColAdd;
			charColAdd = 1;

			if (currChar == L'\n')
			{
				/*if (i <= m_textCursorPosition)
					++lineCounter;*/

				m_tabCounter = 0;
				carriagePosition = 0.f;
				lineWidth = 0.f;
				rowPosition += m_nextLinePositionOffset;
				charCol = 0;

				if (firstDrawableChar == -1)
					++firstVisibleCharLine_counter;

				{
					++numOfLines;
					textHeight += m_nextLinePositionOffset;
				}

				if (formouseclick_lineFound)
				{
					formouseclick_lineEnd = true;
				}
			}
			
			if (lineWidth > m_maxLineWidth)
				m_maxLineWidth = lineWidth;

			if (charVisible)
			{
				m_lastDrawnCharRect = charRect;
			}

			// если видимая часть была нарисована
			// и текущий символ не видим
			// и не нужно обновление
			// то выход из цикла
			if (isVisiblePartWasDrawn 
				&& !charVisible
				&& !m_needUpdate)
			{
				break;
			}
		}

		m_drawStartIndex = firstDrawableChar;
		m_firstVisibleCharLine = firstDrawableCharLine;
		m_startRowPosition = firstDrawableChar_rowPos;
		if (m_needUpdate)
		{
			m_textHeight = textHeight;
			m_numOfLines = numOfLines;
		//	m_lineCounter = lineCounter;
		}
		
		if (m_finishPageUp)
		{
			m_finishPageUp = false;
		}

		if (m_numOfLines > 1)
			m_needToDrawVScroll = true;
		else
			m_needToDrawVScroll = false;

		//printf("%f\n", m_maxLineWidth);

		if (m_maxLineWidth > (textAreaSizeX - 100.f))
		{
			m_needToDrawHScroll = true;
			m_hscroll_widthLen = m_maxLineWidth - (textAreaSizeX - 100.f);
		}
		else
			m_needToDrawHScroll = false;

		if (char_underMouse != -1)
		{
			m_char_nextCursorPosition = char_underMouse;
			m_char_nextCursorPosition_rect = char_underMouse_rect;
		}
		else
		{
			if (char_closestOnLineMouse != -1)
			{
				m_char_nextCursorPosition = char_closestOnLineMouse;
				m_char_nextCursorPosition_rect = char_closestOnLineMouse_rect;
			}
			else
			{
				if (input->m_cursorCoordsForGUI.y >= m_lastDrawnCharRect.y)
				{
					m_char_nextCursorPosition = char_closestOnLastLineMouse;
					m_char_nextCursorPosition_rect = char_closestOnLastLineMouse_rect;
				}
				else
				{
					m_char_nextCursorPosition = char_closestMouse;
					m_char_nextCursorPosition_rect = char_closestMouse_rect;
				}
			}
		}

		if (m_needToFindHorizontalOffset)
		{
			m_needToFindHorizontalOffset = false;
			_findHorizontalOffset();
		}

		if (m_needUpdate)
			m_needUpdate = false;

		if (m_updateTextCursorOnKeyboardKey)
		{
			m_updateTextCursorOnKeyboardKey = false;

			if (m_textCursorPositionLocation == 0)
			{
				m_vertical_offset = m_textCursorPositionY;
				m_needUpdate = true;
			}
			else if (m_textCursorPositionLocation == 1)
			{
				if (m_textCursorRect.w > m_editorAreaText.w)
					m_vertical_offset += m_textCursorRect.w - m_editorAreaText.w;
				else if (m_textCursorRect.y < m_editorAreaText.y)
				{
					m_vertical_offset -= m_editorAreaText.y - m_textCursorRect.y;
					m_needUpdate = true;
				}
			}
			else if (m_textCursorPositionLocation == 2)
			{
				m_needUpdate = true;
				m_vertical_offset = m_textCursorPositionY - textAreaSizeY + fontSize.y;
			}

			if (m_vertical_offset < 0)
				m_vertical_offset = 0;

			m_needToFindHorizontalOffset = true;
		}



	}
	else
	{
		m_needToDrawVScroll = false;
		isTextCursorWasVisible = true;
		m_textCursorRect.x = m_editorAreaText.x;
		m_textCursorRect.y = m_editorAreaText.y;
		m_textCursorRect.z = m_textCursorRect.x + 1;
		m_textCursorRect.w = m_textCursorRect.y + fontSize.y;

		m_vertical_offset = 0.f;
	}

	

	if (m_isEditActive && m_drawTextCursor && isTextCursorWasVisible)
	{
		m_renderer->DrawRectangle(m_textCursorRect, g_app->m_currColorTheme->m_editorCursor);
	}

	//printf("%u\n", m_textCursorCol_prefer);
}

void npDocument_text::Save()
{
	if (!m_filePath.size())
	{
		SaveAs();
		return;
	}

	if (m_filePath.size())
	{
		npUnicodeString ustr;
		ustr.Assign((const char16_t*)m_text.c_str());

		switch (m_format)
		{
		case format_utf8:
			ustr.SaveToFileUTF8(m_filePath.c_str(), m_withBOM);
			break;
		case format_utf16:
			ustr.SaveToFileUTF16(m_filePath.c_str(), m_withBOM);
			break;
		case format_utf32:
			ustr.SaveToFileUTF32(m_filePath.c_str(), m_withBOM);
			break;
		}

		m_isSaved = true;
	}
}

void npDocument_text::_setTitleFromPath()
{
	std::filesystem::path p(m_filePath.c_str());
	m_title = p.filename().c_str();
}

void npDocument_text::SaveAs()
{
	g_app->OpenSaveFileDialog(
		"Save As...",
		"Save",
		"txt",
		&m_filePathW);
	
	if (m_filePathW.size())
	{
		m_isSaved = true;	

		npUnicodeConverter uc;
		uc.wchar_to_char(m_filePathW.c_str(), m_filePathW.size(), &m_filePath);

		_setTitleFromPath();

		Save();
		m_titleLenInPixels = g_app->GetFontUI()->GetTextLen(m_title.data(), m_title.size());
	}
}

void npDocument_text::OnPopupCommand(uint32_t cmd)
{
	switch (cmd)
	{
	case NOTEPAD_MENUID_COPY:
		this->CopyToClipboard();
		break;
	case NOTEPAD_MENUID_CUT:
		this->CutToClipboard();
		break;
	case NOTEPAD_MENUID_DELETE:
		this->DeleteSelected();
		break;
	case NOTEPAD_MENUID_PASTE:
		this->PasteFromClipboard();
		break;
	case NOTEPAD_MENUID_SELECTALL:
		this->SelectAll();
		break;
	case NOTEPAD_MENUID_UNDO:
		//this->CopyToClipboard();
		break;
	}
}

void npDocument_text::_findVerticalOffset()
{
	if (m_textCursorRect.w > m_editorAreaText.w)
		m_vertical_offset += m_textCursorRect.w - m_editorAreaText.w;
	else if (m_textCursorRect.y < m_editorAreaText.y)
	{
		m_vertical_offset -= m_editorAreaText.y - m_textCursorRect.y;
		m_needUpdate = true;
	}
	if (m_vertical_offset < 0)
	{
		m_vertical_offset = 0;
	}
}

void npDocument_text::_findHorizontalOffset()
{
	if (m_textCursorRect.z > m_editorAreaText.z)
		m_horizontal_offset += m_textCursorRect.z - m_editorAreaText.z;
	else if (m_textCursorRect.x < m_editorAreaText.x)
		m_horizontal_offset -= m_editorAreaText.x - m_textCursorRect.x;
	if (m_horizontal_offset < 0)
		m_horizontal_offset = 0;
}

void npDocument_text::_showTextCursor()
{
	m_drawTextCursor = true;
	m_textCursorTimer = 0.f;
}

void npDocument_text::DeselectAll()
{
	m_selectionStart = 0;
	m_selectionEnd = 0;
	m_isSelected = false;
}

void npDocument_text::DeleteSelected()
{
	if (!m_isSelected)
		return;
	m_getCursorColPrefer = true;
	auto s1 = m_selectionStart;
	auto s2 = m_selectionEnd;
	if (s1 > s2)
	{
		s1 = s2;
		s2 = m_selectionStart;
	}

	auto num_to_delete = s2 - s1;
	auto str_len = m_text.size();
	if (num_to_delete > str_len)
	{
		m_isSaved = false;
		m_textCursorPositionWhenClick = str_len;
		DeselectAll();
		_updateTextCursorOnKeyboardKey();
		return;
	}

	m_isSaved = false;

	//printf("Delete %i %i\n", s1, s2);
	auto buf = m_text.data();
	for (size_t i = s2, i2 = s1; i < str_len; ++i, ++i2)
	{
		buf[i2] = buf[i];
	}

	buf[str_len - num_to_delete] = 0;

	m_textCursorPosition = s1;
	m_text.SetSize(str_len - num_to_delete);

	DeselectAll();
	_updateTextCursorOnKeyboardKey();
}

void npDocument_text::_goUp()
{
	_showTextCursor();
	_findTextCursorCol();
	auto saveCol = m_textCursorCol;
	auto lineColMax = m_textCursorCol;
	auto m_textCursorPosition_old = m_textCursorPosition;
	_updateTextCursorOnKeyboardKey();

	m_textCursorPosition -= m_textCursorCol;
	if (!m_textCursorPosition)
	{
		m_textCursorCol = 0;
		goto selection;
	}

	--m_textCursorPosition;
	_findTextCursorCol();
	lineColMax = m_textCursorCol;
	m_textCursorPosition -= m_textCursorCol;

	if (saveCol > lineColMax)
		saveCol = lineColMax;

	//m_textCursorPosition += saveCol;
	m_textCursorCol = 0;
	_findCursorColPrefer();

	_findTextCursorCol();

selection:
	auto input = &g_app->m_input;
	if (input->m_kbm == npKeyboardModifier::Shift)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			m_isSelected = true;
			m_selectionStart = m_textCursorPosition_old;
		}
		m_selectionEnd = m_textCursorPosition;
	}
	else
	{
		if (m_isSelected)
			DeselectAll();
	}
}

void npDocument_text::_findCursorColPrefer()
{
	auto textPtr = &m_text.data()[m_textCursorPosition];
	uint32_t tabCounter = 0;

	// двигаем m_textCursorPosition
	while (*textPtr)
	{
		if (m_textCursorCol >= m_textCursorCol_prefer)
		{
			m_textCursorCol = m_textCursorCol_prefer;
			break;
		}
		if (*textPtr == U'\n')
			break;

		auto textCursorCol_t = m_textCursorCol;
		if (*textPtr == '\t')
		{
			for (size_t o = tabCounter; o < m_tabSize; ++o)
			{
				if (m_textCursorCol == m_textCursorCol_prefer)
				{
					if (o < 2)
					{
						m_textCursorCol = textCursorCol_t;
						return;
					}
					else
					{
						if (o == tabCounter)
							break;
						for (size_t o2 = o; o2 < m_tabSize; ++o2)
							++m_textCursorCol;
					}
				}
				++m_textCursorCol;
			}
			tabCounter = 0;
		}
		else
		{
			++tabCounter;
			if (tabCounter == m_tabSize)
				tabCounter = 0;
			if (m_textCursorCol >= m_textCursorCol_prefer)
			{
				m_textCursorCol = m_textCursorCol_prefer;
				break;
			}
			++m_textCursorCol;
		}

		++m_textCursorPosition;
		++textPtr;
	}
}

void npDocument_text::_goDown()
{
	_showTextCursor();
	if (m_textCursorPosition == m_text.size())
	{
		_updateTextCursorOnKeyboardKey();
		return;
	}

	auto m_textCursorPosition_old = m_textCursorPosition;

	_findTextCursorCol();

	//auto saveCol = m_textCursorCol;

	// can poit to end 0
	wchar_t* textPtr = &m_text.data()[m_textCursorPosition];
	if (*textPtr)
	{
		// find \n and then go to the next line
		while (*textPtr)
		{
			if (*textPtr == U'\n')
			{
				++textPtr;
				++m_textCursorPosition;
				break;
			}
			++textPtr;
			++m_textCursorPosition;
		}

		if (!*textPtr)
		{
			m_textCursorPosition = m_text.size();
			goto selection;
		}

		// m_textCursorPosition на начале строки
		m_textCursorCol = 0;
		_findCursorColPrefer();
	}

selection:
	auto input = &g_app->m_input;
	if (input->m_kbm == npKeyboardModifier::Shift)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			m_isSelected = true;
			m_selectionStart = m_textCursorPosition_old;
		}
		m_selectionEnd = m_textCursorPosition;
	}
	else
	{
		if (m_isSelected)
			DeselectAll();
	}

	_updateTextCursorOnKeyboardKey();
}

void npDocument_text::_delete()
{

	_showTextCursor();
	if (m_textCursorPosition < m_text.size())
	{
		if (m_isSelected)
		{
			DeleteSelected();
		}
		else
		{
			bool ok = false;
			auto buf = m_text.data();
			auto str_len = m_text.size();
			for (size_t i = m_textCursorPosition; i < str_len; ++i)
			{
				ok = true;
				if (i + 1 == str_len)
					break;
				buf[i] = buf[i + 1];
			}
			if (ok)
			{
				m_isSaved = false;
				buf[str_len - 1] = 0;
				m_text.SetSize(m_text.size() - 1);
			}
		}
	}
	else
	{
		if (m_isSelected)
		{
			DeleteSelected();
		}
	}
	_updateTextCursorOnKeyboardKey();
}

void npDocument_text::_backspace()
{
	_showTextCursor();
	m_getCursorColPrefer = true;
	if (m_isSelected)
	{
		DeleteSelected();
	}
	else
	{
		bool ok = false;
		auto buf = m_text.data();
		auto str_len = m_text.size();
		for (size_t i = m_textCursorPosition; i < str_len; ++i)
		{
			if (i == 0)
				break;
			ok = true;
			buf[i - 1] = buf[i];
		}
		if (m_textCursorPosition == str_len && !ok)
			ok = true;
		if (ok)
		{
			m_isSaved = false;
			if (str_len - 1 >= 0)
			{
				if (m_textCursorPosition)
				{
					--m_textCursorPosition;
					buf[str_len - 1] = 0;
					m_text.SetSize(m_text.size() - 1);
				}
			}
		}
	}
	_updateTextCursorOnKeyboardKey();
}

void npDocument_text::_goHome()
{
	m_getCursorColPrefer = true;
	auto input = &g_app->m_input;
	if (input->m_kbm == npKeyboardModifier::Ctrl
		|| input->m_kbm == npKeyboardModifier::ShiftCtrl)
	{
		_goHomeOfText();
		return;
	}

	_showTextCursor();
	if (input->m_kbm == npKeyboardModifier::Shift
		|| input->m_kbm == npKeyboardModifier::ShiftCtrl)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			m_isSelected = true;
			m_selectionStart = m_textCursorPosition;
		}
	}
	else
	{
		if (m_isSelected)
			DeselectAll();
	}
	//if (!m_oneLine)
	{
		auto ptr = m_text.c_str() + m_textCursorPosition;
		if (m_textCursorPosition && m_text.c_str())
		{
			if (*ptr == 0)
			{
				m_textCursorPosition--;
				ptr--;
			}

			while (*ptr)
			{
				ptr--;
				m_textCursorPosition--;
				if (m_textCursorPosition == 0)
				{
					break;
				}

				if (*ptr == L'\n')
				{
					m_textCursorPosition++;
					break;
				}
			}
		}
	}
	//else
	//{
	//	m_textCursorPosition = 0;
	//}

	if (input->m_kbm == npKeyboardModifier::Shift
		|| input->m_kbm == npKeyboardModifier::ShiftCtrl)
		m_selectionEnd = m_textCursorPosition;

	_updateTextCursorOnKeyboardKey();
}

void npDocument_text::_goEnd()
{
	m_getCursorColPrefer = true;
	auto input = &g_app->m_input;
	if (input->m_kbm == npKeyboardModifier::Ctrl
		|| input->m_kbm == npKeyboardModifier::ShiftCtrl)
	{
		_goEndOfText();
		return;
	}
	_showTextCursor();
	if (input->m_kbm == npKeyboardModifier::Shift)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			m_isSelected = true;
			m_selectionStart = m_textCursorPosition;
		}
	}
	else
	{
		if (m_isSelected)
			DeselectAll();
	}
	//if (!m_oneLine)
	{
		auto ptr = m_text.c_str() + m_textCursorPosition;
		while (*ptr)
		{
			if (*ptr == L'\n')
				break;
			ptr++;
			m_textCursorPosition++;
		}
	}

	if (input->m_kbm == npKeyboardModifier::Shift)
		m_selectionEnd = m_textCursorPosition;

	_updateTextCursorOnKeyboardKey();
}

void npDocument_text::_goEndOfText()
{
	m_getCursorColPrefer = true;
	auto input = &g_app->m_input;
	_showTextCursor();
	if (input->m_kbm == npKeyboardModifier::ShiftCtrl
		|| input->m_kbm == npKeyboardModifier::Shift)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			m_isSelected = true;
			m_selectionStart = m_textCursorPosition;
		}
	}
	else
	{
		if (m_isSelected)
			DeselectAll();
	}

	m_textCursorPosition = m_text.size();

	if (input->m_kbm == npKeyboardModifier::ShiftCtrl
		|| input->m_kbm == npKeyboardModifier::Shift)
		m_selectionEnd = m_textCursorPosition;

	_updateTextCursorOnKeyboardKey();
}

void npDocument_text::_goHomeOfText()
{
	m_getCursorColPrefer = true;
	auto input = &g_app->m_input;
	_showTextCursor();
	if (input->m_kbm == npKeyboardModifier::ShiftCtrl
		|| input->m_kbm == npKeyboardModifier::Shift)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			m_isSelected = true;
			m_selectionStart = m_textCursorPosition;
		}
	}
	else
	{
		if (m_isSelected)
			DeselectAll();
	}
	m_textCursorPosition = 0;

	if (input->m_kbm == npKeyboardModifier::ShiftCtrl
		|| input->m_kbm == npKeyboardModifier::Shift)
		m_selectionEnd = m_textCursorPosition;

	_updateTextCursorOnKeyboardKey();
}

void npDocument_text::_goPageUp()
{
	auto input = &g_app->m_input;
	_showTextCursor();
	uint32_t numLines = m_firstVisibleCharLine;
	if (numLines > m_maxNumOfVisibleLines)
		numLines = m_maxNumOfVisibleLines;

	_moveUpView(numLines);

	uint32_t numLinesForCursor = numLines;

	_moveTextCursorUp(numLinesForCursor);
	m_textCursorCol = 0;
	_findCursorColPrefer();
}

void npDocument_text::_goPageDown()
{
	auto linesRemain = m_numOfLines - m_firstVisibleCharLine;
	auto input = &g_app->m_input;

	//printf("%u[%u][%u] %u %u\n",
	//	m_numOfLines,
	//	m_drawStartIndex,
	//	m_firstVisibleCharLine,
	//	m_maxNumOfVisibleLines, 
	//	linesRemain);

	if (m_numOfLines > m_maxNumOfVisibleLines
		|| linesRemain > m_maxNumOfVisibleLines)
	{
		_showTextCursor();
		if (input->m_kbm == npKeyboardModifier::Shift)
		{
			if ((m_selectionStart == 0) && (m_selectionEnd == 0))
			{
				m_isSelected = true;
				m_selectionStart = m_textCursorPosition;
			}
		}
		else
		{
			if (m_isSelected)
				DeselectAll();
		}

		if (linesRemain > m_maxNumOfVisibleLines)
		{
			//m_vertical_offset += m_maxNumOfVisibleLines * m_nextLinePositionOffset;
			_moveDownView(m_maxNumOfVisibleLines);

			wchar_t* textPtr = &m_text.data()[m_textCursorPosition];
			if (*textPtr)
			{
				uint32_t lineCounter = 0;
				// find \n and then go to the next line
				while (*textPtr)
				{
					if (*textPtr == U'\n')
					{
						++lineCounter;
						if (lineCounter >= m_maxNumOfVisibleLines)
							break;
					}
					++textPtr;
					++m_textCursorPosition;
				}

				_goHome();
				m_getCursorColPrefer = false;
				m_textCursorCol = 0;
				_findCursorColPrefer();
			}
		}
		else
		{
			_goEndOfText();
			_goHome();
			m_getCursorColPrefer = false;
			m_textCursorCol = 0;
			_findCursorColPrefer();
		}

		_updateTextCursorOnKeyboardKey();
	}
	else
	{
		// !тут и выше (вызовы _goEndOfText)
		// !установки на позицию(m_getCursorColPrefer) нет 
		// ---- решение
		// вызов _goHome и вызов _findCursorColPrefer
		_goEndOfText();
		_goHome();
		m_getCursorColPrefer = false;
		m_textCursorCol = 0;
		_findCursorColPrefer();
	}

	if (input->m_kbm == npKeyboardModifier::ShiftCtrl
		|| input->m_kbm == npKeyboardModifier::Shift)
		m_selectionEnd = m_textCursorPosition;
}

void npDocument_text::_findTextCursorCol()
{
//	printf("%s\n", __FUNCTION__);
	m_textCursorCol = 0;
	auto ptr = m_text.data();
	for (uint32_t i = 0, sz = m_text.size(); i < sz; ++i)
	{
		if (i == m_textCursorPosition)
			break;

		if (*ptr == U'\n')
			m_textCursorCol = 0;
		else
			++m_textCursorCol;

		++ptr;
	}
}

void npDocument_text::_updateTextCursorOnKeyboardKey()
{
	m_updateTextCursorOnKeyboardKey = true;
	m_needUpdate = true;
}

void npDocument_text::_goLeft()
{
	_showTextCursor();
	auto input = &g_app->m_input;
	if (m_textCursorPosition)
	{
		if (input->m_kbm == npKeyboardModifier::Shift)
		{
			if ((m_selectionStart == 0) && (m_selectionEnd == 0))
			{
				m_isSelected = true;
				m_selectionStart = m_textCursorPosition;
			}
			m_selectionEnd = m_textCursorPosition - 1;
		}
		else
		{
			if (m_isSelected)
			{
				if (m_selectionStart < m_selectionEnd)
					m_textCursorPosition = m_selectionStart + 1;
				else
					++m_textCursorPosition;
				DeselectAll();
			}
		}

		--m_textCursorPosition;
		m_getCursorColPrefer = true;
		/*if(m_textCursorCol)
			m_textCursorCol_prefer = m_textCursorCol-1;*/
	}
	else
	{
		if (m_isSelected && input->m_kbm != npKeyboardModifier::Shift)
			DeselectAll();
	}

	_updateTextCursorOnKeyboardKey();
}

void npDocument_text::_goRight()
{
	_showTextCursor();
	auto input = &g_app->m_input;
	if (m_textCursorPosition < m_text.size())
	{
		if (input->m_kbm == npKeyboardModifier::Shift)
		{
			if ((m_selectionStart == 0) && (m_selectionEnd == 0))
			{
				m_isSelected = true;
				m_selectionStart = m_textCursorPosition;
			}
			m_selectionEnd = m_textCursorPosition + 1;
		}
		else
		{
			if (m_isSelected)
			{
				if (m_selectionStart > m_selectionEnd)
					m_textCursorPosition = m_selectionStart - 1;
				else
					--m_textCursorPosition;

				DeselectAll();
			}
		}

		++m_textCursorPosition;
		m_getCursorColPrefer = true;

		/*m_textCursorCol_prefer = m_textCursorCol+1;
		if (m_textCursorPosition+1 < m_text.size())
		{
			if (m_text.c_str()[m_textCursorPosition+1] == '\n')
				m_textCursorCol_prefer = 0;
		}*/
	}
	else
	{
		if (m_isSelected && input->m_kbm != npKeyboardModifier::Shift)
			DeselectAll();
	}
	_updateTextCursorOnKeyboardKey();
}

void npDocument_text::SelectAll()
{
	if (m_text.size())
	{
		_goEndOfText();

		m_selectionStart = 0;
		m_selectionEnd = m_text.size();
		m_isSelected = true;
		_updateTextCursorOnKeyboardKey();
	}
}

void npDocument_text::CutToClipboard()
{
	if (!m_isSelected)
		return;
	m_getCursorColPrefer = true;
	CopyToClipboard();
	this->DeleteSelected();
	_updateTextCursorOnKeyboardKey();
}

void npDocument_text::CopyToClipboard()
{
	if (!m_isSelected)
		return;
	npStringW str;
	size_t s1 = m_selectionStart;
	size_t s2 = m_selectionEnd;
	if (s1 > s2)
	{
		s1 = s2;
		s2 = m_selectionStart;
	}
	auto buf = m_text.data();
	auto num_to_select = s2 - s1;
	for (size_t i = s1; i < s2; ++i)
	{
		str.append(buf[i]);
	}
	g_app->CopyTextToClipboard(&str);
}

void npDocument_text::PasteFromClipboard()
{
	npStringW text;
	g_app->GetTextFromClipboard(&text);
	auto text_size = text.size();
	if (text_size)
	{
		this->DeleteSelected();


		auto ptr = text.data();
		for (size_t i = 0; i < text_size; ++i)
		{
			if (OnCharacter(ptr[i]))
			{
				m_isSaved = false;
				m_text.Insert(ptr[i], m_textCursorPosition);
				++m_textCursorPosition;
			}
		}
		//	m_textCursorPosition += text_size;
	}
	_updateTextCursorOnKeyboardKey();
	m_needUpdate = true;
}

void npDocument_text::_moveUpView(uint32_t num)
{
	auto fontSize = m_fontEditor->GetFontSize();
	m_vertical_offset -= fontSize.y * num;
	if (m_vertical_offset < 0)
		m_vertical_offset = 0;
	m_startRowPosition = m_vertical_offset- fontSize.y;
	if (m_startRowPosition < 0)
		m_startRowPosition = 0.f;
	// change m_drawStartIndex
	if (m_drawStartIndex)
	{
		for (uint32_t i = 0; i < num; ++i)
		{
			// move up on the end of the line above
			--m_drawStartIndex;
			if (!m_drawStartIndex)
				break;
			m_drawStartIndex = _moveIndexIntoCol0(m_drawStartIndex);

			if (!m_drawStartIndex)
				break;
		}
	}
}

void npDocument_text::_moveDownView(uint32_t num)
{
	auto fontSize = m_fontEditor->GetFontSize();
	m_vertical_offset += fontSize.y * num;
	if (m_vertical_offset > m_textHeight)
		m_vertical_offset = m_textHeight;
}

size_t npDocument_text::_moveIndexIntoCol0(size_t index)
{
	if (!index)
		return 0;

	size_t result = index;

	wchar_t* textPtr = &m_text.data()[0];
	while (true)
	{
		if (result == 0)
			break;

		if (textPtr[result - 1] == L'\n')
		{
			--result;
			break;
		}

		--result;
	}
	return result;
}

void npDocument_text::_moveTextCursorUp(uint32_t num)
{
	for (uint32_t i = 0; i < num; ++i)
	{
		m_textCursorPosition = _moveIndexIntoCol0(m_textCursorPosition);
		if (m_textCursorPosition)
			--m_textCursorPosition;
	}
}

void npDocument_text::_moveTextCursorDown(uint32_t num)
{
}

