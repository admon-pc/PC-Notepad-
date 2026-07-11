#pragma once
#include "npDocument.h"

#include <list>

constexpr uint32_t npDocumentTextColors_size = 3;
const npColor npDocumentTextColors_light[] =
{
	0xFF000000, // text
	0xFFFF8A23, // numbers
	0xFFFF6E00, // "text"
	0xFF999900, // punct
};

enum class npDocumentWordTypeBasic
{
	unknown,
	space, // кроме \n
	punct,
	word,
	number,
	new_line,
};

struct npDocumentHistoryNode_text
{
	char32_t* m_string = 0;
	size_t m_charNum = 0;
	size_t m_charPosition = 0;
	bool m_backspace = false;
	
	// Вместо m_tabsLine проще использовать индекс
	// это должен быть m_selectionStart
	size_t m_tabsIndex = 0;
	//size_t m_tabsLine = 1;
	size_t m_tabsNum = 0;
	uint8_t* m_tabsBuffer = 0;

	enum
	{
		type_insert,
		type_delete,
		type_tabsInsert,
		type_tabsDelete,
	};
	uint32_t m_type = 0;
};
class npDocumentHistory_text
{
	/*npArray<npDocumentHistoryNode_text> m_undoData;
	npArray<npDocumentHistoryNode_text> m_redoData;*/
	std::list<npDocumentHistoryNode_text> m_undoData;
	std::list<npDocumentHistoryNode_text> m_redoData;

	npchar m_prevChar = 0;

	void _addText()
	{
		if (m_buffer.Size())
		{
			npDocumentHistoryNode_text node;
			node.m_type = node.type_insert;
			node.m_charPosition = m_firstCharPosition;
			node.m_charNum = (m_buffer.Size());

			size_t sz = (node.m_charNum+1) * sizeof(char32_t);
			node.m_string = (char32_t*)malloc(sz);
			memcpy(node.m_string, m_buffer.c_str(), sz);
			
			m_undoData.push_back(node);

			m_buffer.Clear();
		}

		m_firstCharPosition = -1;
	//	m_lastTextPosition = 0;
	}

	void _deleteRedo()
	{
		for (auto o : m_redoData)
		{
			if (o.m_tabsBuffer)
				free(o.m_tabsBuffer);

			if (o.m_string)
				free(o.m_string);
		}
		m_redoData.clear();
	}
public:
	npDocumentHistory_text() {}
	~npDocumentHistory_text() 
	{
		_deleteRedo();
		for (auto o : m_undoData)
		{
			if (o.m_tabsBuffer)
				free(o.m_tabsBuffer);
			if (o.m_string)
				free(o.m_string);
		}
	}
	npUnicodeString m_buffer;
	size_t m_firstCharPosition = -1;

	bool IsHasUndo() { return m_undoData.size() > 0; }
	bool IsHasRedo() { return m_redoData.size() > 0; }

	void AddUndo()
	{
		_addText();
	}

	void AddUndo(const npDocumentHistoryNode_text& node)
	{
		m_undoData.push_back(node);
	}

	void OnAddChar(
		npchar c, 
		size_t charPosition
		)
	{
		_deleteRedo();

		//m_cursorPositionAfter = cursorPositionAfter;

		if(m_firstCharPosition == -1)
			m_firstCharPosition = charPosition;
	//	m_lastTextPosition = textCursorPosition;

		m_buffer.PushBack(c);
		if (c == ' '
			|| c == '\t'
			|| c == '\r'
			|| c == '\n')
		{
			if (m_prevChar != '\t'
				&& m_prevChar != ' '
				&& m_prevChar != '\r')
			{
				_addText();
			}
			else if (c == '\n')
			{
				_addText();
			}
		}

		m_prevChar = c;
	}
	void OnPasteText(npchar c, size_t charPosition)
	{
		_deleteRedo();

		if (m_firstCharPosition == -1)
			m_firstCharPosition = charPosition;
	//	m_lastTextPosition = textCursorPosition;
		m_buffer.PushBack(c);

		m_prevChar = c;
	}

	void OnDeleteSelected(const npchar* string, size_t num, size_t textCursorPosition)
	{
		_deleteRedo();
		npDocumentHistoryNode_text node;
		node.m_type = node.type_delete;
		node.m_charPosition = textCursorPosition;
		node.m_backspace = false;
		node.m_charNum = num;
		size_t sz = num * sizeof(char32_t);
		node.m_string = (char32_t*)malloc(sz);
		for (size_t i = num-1, o = 0; i >= 0;)
		{
 			node.m_string[o] = string[i];
			if (!i)
				break;
			--i;
			++o;
		}
		m_undoData.push_back(node);
		m_buffer.Clear();
	}

	void OnDelete(npchar c, size_t textCursorPosition, bool backspace)
	{
		_deleteRedo();

		npDocumentHistoryNode_text node;
		node.m_type = node.type_delete;
		node.m_charPosition = textCursorPosition;
		node.m_backspace = backspace;
		node.m_charNum = 1;
		size_t sz = 1 * sizeof(char32_t);
		node.m_string = (char32_t*)malloc(sz);
		node.m_string[0] = c;
		m_undoData.push_back(node);
		m_buffer.Clear();
	}

	bool Undo(npDocumentHistoryNode_text* node)
	{
		bool r = false;
		if (m_buffer.Size())
		{
			_addText();
		}

		if (m_undoData.size())
		{
			*node = m_undoData.back();
			r = true;
			m_undoData.pop_back();
			m_redoData.push_back(*node);
		}

		return r;
	}

	bool Redo(npDocumentHistoryNode_text* node)
	{
		bool r = false;
		if (m_redoData.size())
		{
			*node = m_redoData.back();
			r = true;
			m_redoData.pop_back();
			m_undoData.push_back(*node);
		}

		return r;
	}
};


class npDocument_text2 : public npDocument
{
	struct scrollbar_data
	{
		bool m_needToDraw = false;
		float32_t m_size = 12.f;
		npVec4f m_rectBG;
		npVec4f m_rectangle;
		npVec4f m_btn1Rect;
		npVec4f m_btn2Rect;
		bool m_cursorHold = false;
		float32_t m_saveOnClick = 0.f;
		float32_t m_saveOnClickMouseCoord = 0.f;
	};
	scrollbar_data m_scrollV;
	scrollbar_data m_scrollH;

	bool m_find_horizontal_offset = false;
	float32_t m_hscroll_widthLen = 0.f;
	float32_t m_horizontal_offset = 0.f;
	float32_t m_maxLineWidth = 0.f;
	void _findCurLineWidth();
	void _findNeedToDrawHScroll();
	void _findHOffsetFromTextCursorPosition();
	
	bool m_textFindMode = false;

	float32_t m_2clickTimer = 0.f;
	uint32_t m_2clickCount = 0;
	float32_t m_3clickTimer = 0.f;
	uint32_t m_3clickCount = 0;

	// тут текст
	npString_base<npchar> m_text;
	// тут информация о
	npString_base<char32_t> m_textData;

	npDocumentHistory_text m_history;

	int m_doubleClick = 0;

	// это область окна отданная целиком для этого документа
	npVec4f m_editorArea;
	// только для текста
	npVec4f m_editorAreaText;
	npVec4f m_bottomBarBG;
	npVec4f m_lineBar;
	wchar_t m_lineBarCharBuffer[50];

	float32_t m_bottomBarHeight = 14.f;

	float32_t m_hscrollRightSideIndent = 150.f;

	// попал ли курсор в область текста
	bool m_cursorInTextEditor = false;

	// шрифт
// у каждого документа уникальный шрифт, так как масштаб
// можно менять, а это значит что нужно пересоздавать шрифт.
	npFont* m_fontEditor = 0;
	int32_t m_fontEditorSize = 10;
	
	uint32_t m_textCursorLine = 1;
	
	//uint32_t m_textCursorLineOnClick = 1; // save when click lmb
	size_t m_textCursorPosOnClick = 0; // for double\triple click

	uint32_t m_textCursorCol = 1;
	// когда перемещаемся по тексту кнопками вверх вниз, pageup pagedown
	// нужно чтобы текстовый курсор переместился в Col в котором он был ранее.
	// m_textCursorCol_prefer надо устанавливать при перемещении курсора влево вправо
	//   при вводе текста, при вставке, при клике и выборе мышкой
	// m_textCursorCol устанавливается в моменте рисования. Поэтому чтобы установить
	// m_textCursorCol_prefer устанавливаем m_getCursorColPrefer=true
	uint32_t m_textCursorCol_prefer = 1;
	bool m_getCursorColPrefer = false;
	// установка m_textCursorPosition в соответствии с m_textCursorCol_prefer
	void _moveTextCursorToColPrefer();

	// размер tab в символах
	uint32_t m_tabSize = 4;
	// для подсчёта символов для табуляции
	uint32_t m_tabCounter = 0;

	void _showTextCursor();
	// область чтобы его рисовать
	npVec4f m_textCursorRect;
	// чтобы курсор мигал, нужно отсчитывать время
	float32_t m_textCursorTimer = 0.f;
	// мигает каждые полсекунды
	float32_t m_textCursorTimerLimit = 0.5f;
	// рисует текстовый курсор когда true
	bool m_drawTextCursor = false;

	bool m_textMouseScroll = false;
	bool m_textMouseScrollUp = true;
	float32_t m_textMouseScrollTimer = 0.f;
	float32_t m_textMouseScrollTimerLimit = 0.1f;
	// был ли видимым последний символ
	// используется в прокрутке текста при выделении мышкой
	//   чтобы прекратить крутить
	bool m_isLastCharVisible = false;

	// может быть такое что ввод символов должен производиться куда-то
	// но не в документ. Возможно нужно добавить активацию и деактивацию
	// документа. Пока он постоянно активен.
	bool m_isEditActive = true;

	// позиция текстового курсора. Текстовый курсор это индекс в массиве.
	// например 0 значит вставка в начало.
	uint32_t m_textCursorPosition = 0;
	// для Undo
	uint32_t m_textCursorPositionNext = 0;
	// индекс символа который будет взаимодействовать с курсором мышки
	uint32_t m_charForMousePick = 0;
	// сохраняем индекс когда щёлкаем мышкой
	uint32_t m_textCursorPositionOnClick = 0;

	void _setTitleFromPath();
	void _updateRects();
	void _updateData();
	// m_textData вычисляется здесь. по строкам.
	// Начинается с index. Указывать на начало строки нужно самому.
	void _updateData_textData(size_t index);

	// количество строк в документе
	size_t m_lineNum = 1;
	size_t m_numOfVisibleLines = 0;

	float32_t m_textAreaHeight = 0.f;
	float32_t m_nextCharPositionOffset = 0.f;
	float32_t m_nextLinePositionOffset = 0.f;

	// Индекс в массиве с текстом. С этого индекса начинается рисование.
	size_t m_drawStartIndex = 0;
	// Индекс последнего нарисованного символа.
	// Нужно чтобы прокрутить текст к текстовому курсору в
	// случае когда он внизу за пределами видимости.
	// + для Ctrl+PageDown
	size_t m_drawLastIndex = 0;
	// Это номер строки на которой находится m_drawStartIndex.
	// Нужно вычислять тогда когда вычисляется m_drawStartIndex.
	// Используется в подсчёте строк для текстового курсора.
	size_t m_drawStartIndexLine = 1;

	// Нужны функции которые помогут в реализации остального функционала
	// Надо уметь
	// * крутить текст на указанное количество строк. Как колесом мышки.
	//		Нужно будет заменить код прокрутки текста мышкой на эти функции.
	void _moveUpView(uint32_t);
	void _moveDownView(uint32_t);
	// Когда текстовый курсор за пределами видимости, нужно прокрутить текст
	// так чтобы курсор снова стал видимым. Это нужно делать например при вводе 
	// символа, при перемещении текстового курсора  и т.д.
	// По сути простое изменение m_drawStartIndex
	void _moveViewIntoTextCursor();
	void _moveViewToLine(uint32_t);
	// * получить индекс символа в начале текущей строки (на которой 
	//   находится символ с индексом index)
	size_t _moveIndexIntoCol1(size_t index);
	// * получить индекс символа в конце текущей строки
	size_t _moveIndexIntoEndOfTheLine(size_t index);
	void _moveTextCursorUp(uint32_t);
	void _moveTextCursorDown(uint32_t);

	// выбран ли текст
	bool m_isSelected = false;
	// диапазон выбранного текста. 
	// начало и конец
	uint32_t m_selectionStart = 0;
	uint32_t m_selectionEnd = 0;

	// запоминаем что нажали ЛКМ
	bool m_clickedLMB = false;

	void _goLeft();
	void _goRight();
	void _goUp();
	void _goDown();
	void _delete();
	void _backspace();
	void _goHome();
	void _goEnd();
	void _goEndOfText(); // ctrl + end
	void _goHomeOfText(); // ctrl + home
	void _goPageUp();
	void _goPageDown();
	
	void _onTab();
	void _onTab_multiInsert(size_t lineNum, uint8_t* set, uint8_t* get);
	bool _onTab_multiDelete(size_t lineNum, uint8_t* set);

	void _moveTextUp();
	void _moveTextDown();

	void _showTextEditPopup();

	size_t _getLineFromIndex(size_t);
public:
	npDocument_text2(npRenderer* r,
		const char* filePath,
		npUnicodeString* text,
		uint32_t format);
	virtual ~npDocument_text2();

	virtual void Update(float dt) override;
	virtual void Draw(float dt) override;
	virtual void Save() override;
	virtual void SaveAs() override;

	virtual void OnPopupCommand(uint32_t cmd) override;
	virtual void OnWindowSize() override;
	virtual void OnActivate() override;
	virtual void OnWindowActivate() override;
	virtual void OnWindowDeactivate() override;
	virtual void OnTextSearch(bool down) override;
	virtual void GoTo(uint32_t line, uint32_t col, bool tabs) override;
	virtual void Select(size_t begin, size_t end) override;
	virtual void Select(size_t num) override;
	virtual size_t GetSelectionBegin() { return m_selectionStart; };
	virtual size_t GetSelectionEnd() { return m_selectionEnd; };

	void DeselectAll();
	void DeleteSelected(bool addToHistory = true);
	void SelectAll();
	void CutToClipboard();
	void CopyToClipboard();
	void PasteFromClipboard();
	bool OnCharacter(char32_t c) { if (c == '\r')return false; return true; }

	void Undo();
	void Redo();

	void InsertChar(char32_t c);

	//void InsertText(const char32_t* text, size_t charNum, bool addToHistory = true);
	//void InsertText(const wchar_t* text, size_t charNum, bool addToHistory = true);

	enum
	{
		format_utf8,
		format_utf16,
		format_utf16be,
		format_utf32,
		format_utf32be,
	};
	uint32_t m_format = format_utf32;
	bool m_withBOM = true;

	struct WordInfo
	{
		size_t m_begin = 0;
		size_t m_end = 0;
		npDocumentWordTypeBasic m_type = npDocumentWordTypeBasic::unknown;
		uint32_t m_color = 0;
	};
	// получить информацию о слове по индексу
	// под словом понимается всё что попадает в курсор
	// и пробел и число.
	void GetWordInfo(size_t pos, WordInfo*);

	void OnCtrlX();
};

