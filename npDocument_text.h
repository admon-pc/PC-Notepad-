#pragma once
#include "npDocument.h"
class npDocument_text : public npDocument
{
	// тут текст
	npString_base<npchar> m_text;

	// это область окна отданная целиком для этого документа
	npVec4f m_editorArea;
	// только для текста
	npVec4f m_editorAreaText;

	// количество строк
	// минимум одна так как в пустом документе
	// курсор уже стоит на линии
	uint32_t m_numOfLines = 1;

	// для работы скроллбара.
	// если количество линий превышает некое значение
	// то m_needToDrawVScroll становится true
	bool m_needToDrawVScroll = false;
	// область целиком включая кнопки
	npVec4f m_vscrollBGRect;
	// область кнопки стрелка_вверх
	npVec4f m_vscroll_upBtnRect;
	// область кнопки стрелка_вниз
	npVec4f m_vscroll_downBtnRect;
	// ширина
	float32_t m_vscrollSize = 14.f;
	// область ползунка
	npVec4f m_vscrollRectangle;
	// для запоминания что нажали на ползунок
	bool m_cursorHoldVScroll = false;
	// надо сохранять значения чтобы динамично прокручивать
	// в соответствии с позицией курсора мишки
	float32_t m_vscrollSaveOnClick = 0.f;
	float32_t m_vscrollSaveOnClickYMouseCoord = 0.f;

	// аналогично для горизонтального скроллбара
	bool m_needToDrawHScroll = false;
	float32_t m_hscrollSize = 12.f;
	npVec4f m_hscrollBGRect;
	npVec4f m_hscrollRectangle;
	npVec4f m_hscroll_leftBtnRect;
	npVec4f m_hscroll_rightBtnRect;
	float32_t m_hscroll_widthLen = 0.f;
	bool m_cursorHoldHScroll = false;
	float32_t m_hscrollSaveOnClick = 0.f;
	float32_t m_hscrollSaveOnClickXMouseCoord = 0.f;
	float32_t m_hscrollRightSideIndent = 150.f;
	
	npVec4f m_bottomBarBG;
	float32_t m_bottomBarHeight = 14.f;

	// максимальное количество видимых строк
	// нужен для: вертикального скроллбара, pageup pagedown
	// значение равно высота_области_редактора / на высоту_строки
	uint32_t m_maxNumOfVisibleLines = 0;

	// длинна самой широкой строки.
	// для горизонтального скроллбара
	float32_t m_maxLineWidth = 0.f;

	// высота текста в пикселях
	// от первого до последнего нарисованного.
	// нужен для вертикального скроллбара и при прокрутке колесом мышки
	float m_textHeight = 0.f; // only from 0 to last visible

	// данное значение будет "передвигать" текст вверх вниз
	float m_vertical_offset = 0.f;
	void _findVerticalOffset();
	// так-же но по горизонтали
	float m_horizontal_offset = 0.f;
	void _findHorizontalOffset();
	// некоторые операции нужно производить в след. кадре
	// как например нахождение значения m_horizontal_offset
	// some operations I need to do in next frame
	bool m_needToFindHorizontalOffset = false;

	// попал ли курсор в область текста
	bool m_cursorInTextEditor = false;

	// шрифт
	// у каждого документа уникальный шрифт, так как масштаб
	// можно менять, а это значит что нужно пересоздавать шрифт.
	npFont* m_fontEditor = 0;
	int32_t m_fontEditorSize = 10;

	// запоминаем что нажали ЛКМ
	bool m_clickedLMB = false;

	// может быть такое что ввод символов должен производиться куда-то
	// но не в документ. Возможно нужно добавить активацию и деактивацию
	// документа. Пока он постоянно активен.
	bool m_isEditActive = true;

	// для того чтобы выделять мышкой текст, и его прокручивать,
	// нужно знать ближайший к курсору мышки символ.
	// чтобы это знать, нужно обрабатывать несколько случаев.
	// эти несколько случаев проще использовать используя одну(плюс _rect)
	// переменную
	uint32_t m_char_nextCursorPosition = -1;
	npVec4f m_char_nextCursorPosition_rect;

	// позиция текстового курсора. Текстовый курсор это индекс в массиве.
	// например 0 значит вставка в начало.
	uint32_t m_textCursorPosition = 0;
	// сохраняем индекс когда щёлкаем мышкой
	uint32_t m_textCursorPositionWhenClick = 0;
	void _showTextCursor();
	// область чтобы его рисовать
	npVec4f m_textCursorRect;
	// чтобы курсор мигал, нужно отсчитывать время
	float32_t m_textCursorTimer = 0.f;
	// мигает каждые полсекунды
	float32_t m_textCursorTimerLimit = 0.5f;
	// рисует текстовый курсор когда true
	bool m_drawTextCursor = false;

	// выбран ли текст
	bool m_isSelected = false;
	// диапазон выбранного текста. 
	// начало и конец
	uint32_t m_selectionStart = 0;
	uint32_t m_selectionEnd = 0;

	// для
	// for _goUp _goDown
	// нужно отсчитывать позицию - столбик
	uint32_t m_textCursorCol = 0;
	void _findTextCursorCol();
	uint32_t m_textCursorCol_prefer = 0;
	bool m_getCursorColPrefer = false;
	void _findCursorColPrefer();
	uint32_t m_textCursorLine = 0;

	// Подсчёт строк. Нужно сохранять так как
	// m_drawStartIndex меняется
	//uint32_t m_lineCounter = 0;

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
	
	// когда ввожу символ то нужно проверит уходит ли текстовый курсор за пределы
	// или когда курсор невиден, прокрутили текст вверх или вниз, то при вводе
	//  надо прокрутить обратно чтобы курсор снова стал виден.
	// из-за особенности редактора делать проще в след. кадре.
	bool m_updateTextCursorOnKeyboardKey = false;
	void _updateTextCursorOnKeyboardKey();

	

	// размер tab в символах
	uint32_t m_tabSize = 4;
	// для подсчёта символов для табуляции
	uint32_t m_tabCounter = 0;

	// когда текст был прокручен и текстовый курсор пропал из виду
	// при вводе символа текст надо прокрутить обратно чтобы
	// текстовый курсор снова стал виден.
	int m_textCursorPositionLocation = 0; // 0 above, 1 visible, 2 below
	// это расстояние используется чтобы 'прокрутить' m_vertical_offset
	// обратно
	float32_t m_textCursorPositionY = 0;

	// для выбора мышкой. для обработки случая когда мышь ушла вниз.
	npVec4f m_lastDrawnCharRect;

	// с этого индекса начинается рисование.
	// он указывает на первый символ строки.
	// Когда происходит рисование, и если символ
	// не видим, то значение меняется.
	// Реализовано так, что, самый первый видимый символ и
	// становится первым в рисовании. Он всегда в начале строки.
	// Получается так, что, если будем крутить текст вниз,
	// то это значение будет автоматически обновляться без нагрузки,
	// ведь, m_drawStartIndex изменился, другие символы уйдут вверх.
	// Но если крутить вверх то нужно подумать какое значение будет
	// принимать m_drawStartIndex. Возможно, надо находить индекс первого
	// символа строки выше. Пока это не реализовано. Реализовано так - 
	// если крутим вверх то обнуляем m_drawStartIndex. От этого
	// появляются тормоза.
	size_t m_drawStartIndex = 0;
	//npVec2f m_drawStartCharPosition;
	// при рисовании, когда спускаемся ниже строкой, делаем отступ,
	// величина отступа = высота шрифта + может быть значение определяющее
	//    расстояние между строками
	// Когда m_drawStartIndex динамически изменяется, надо сохранять
	// значение. 
	//   Или как-то вычислять динамически перед рисованием (надо попробовать)
	float m_startRowPosition = 0.f;
	
	// номер линии на которой находится символ m_drawStartIndex
	uint32_t m_firstVisibleCharLine = 0;

	bool m_finishPageUp = false;

	bool m_needUpdate = true;
	//bool m_skipUpdate = false;

	float m_nextCharPositionOffset = 0.f;
	float m_nextLinePositionOffset = 0.f;

	void _setTitleFromPath();

	// Нужны функции которые помогут в реализации остального функционала
	// Надо уметь
	// * крутить текст на указанное количество строк. Как колесом мышки.
	//		Нужно будет заменить код прокрутки текста мышкой на эти функции.
	void _moveUpView(uint32_t);
	void _moveDownView(uint32_t);
	// * получить индекс символа в начале текущей строки (на которой 
	//   находится символ с индексом index)
	size_t _moveIndexIntoCol0(size_t index);
	void _moveTextCursorUp(uint32_t);
	void _moveTextCursorDown(uint32_t);

public:
	npDocument_text(npRenderer* r,
		const char* filePath,
		npUnicodeString* text,
		uint32_t format);
	virtual ~npDocument_text();

	virtual void Update(float dt) override;
	virtual void Draw(float dt) override;
	virtual void Save() override;
	virtual void SaveAs() override;

	virtual void OnPopupCommand(uint32_t cmd) override;

	void DeselectAll();
	void DeleteSelected();
	void SelectAll();
	void CutToClipboard();
	void CopyToClipboard();
	void PasteFromClipboard();
	bool OnCharacter(bool c) { return true; }

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
};

