#ifndef _NP_INPUTH_
#define _NP_INPUTH_

enum class npInputKey : uint32_t
{
	K_NONE = 0, K_BACKSPACE = 8, K_TAB = 9, K_ENTER = 13, K_SHIFT = 16,
	K_CTRL = 17, K_ALT = 18, K_PAUSE = 19, K_CAPS_LOCK = 20, K_ESCAPE = 27,
	K_SPACE = 32, K_PGUP = 33, K_PGDOWN = 34, K_END = 35, K_HOME = 36,
	K_LEFT = 37, K_UP = 38, K_RIGHT = 39, K_DOWN = 40, K_PRTSCR = 44,
	K_INSERT = 45, K_DELETE = 46, K_0 = 48, K_1 = 49, K_2 = 50,
	K_3 = 51, K_4 = 52, K_5 = 53, K_6 = 54, K_7 = 55, K_8 = 56,
	K_9 = 57, K_A = 65, K_B = 66, K_C = 67, K_D = 68, K_E = 69,
	K_F = 70, K_G = 71, K_H = 72, K_I = 73, K_J = 74, K_K = 75,
	K_L = 76, K_M = 77, K_N = 78, K_O = 79, K_P = 80, K_Q = 81,
	K_R = 82, K_S = 83, K_T = 84, K_U = 85, K_V = 86, K_W = 87,
	K_X = 88, K_Y = 89, K_Z = 90, K_WIN = 91, K_CONTEXT = 93,
	K_NUM_0 = 96, K_NUM_1 = 97, K_NUM_2 = 98, K_NUM_3 = 99,
	K_NUM_4 = 100, K_NUM_5 = 101, K_NUM_6 = 102, K_NUM_7 = 103,
	K_NUM_8 = 104, K_NUM_9 = 105, K_NUM_MUL = 106, K_NUM_ADD = 107,
	K_NUM_SUB = 109, K_NUM_DOT = 110, K_NUM_SLASH = 111, K_F1 = 112,
	K_F2 = 113, K_F3 = 114, K_F4 = 115, K_F5 = 116, K_F6 = 117,
	K_F7 = 118, K_F8 = 119, K_F9 = 120, K_F10 = 121, K_F11 = 122,
	K_F12 = 123, K_NUM_LOCK = 144, K_SCROLL_LOCK = 145, K_LSHIFT = 160,
	K_RSHIFT = 161, K_LCTRL = 162, K_RCTRL = 163, K_LALT = 164, K_RALT = 165,
	K_FUNC_MY_COMP = 182, K_FUNC_CALC = 183, K_COLON = 186,
	K_ADD = 187, K_COMMA = 188, K_SUB = 189, K_DOT = 190,
	K_SLASH = 191, K_TILDE = 192, K_FIGURE_OPEN = 219,
	K_BACKSLASH = 220, K_FIGURE_CLOSE = 221, K_QUOTE = 222,

	END,
};

enum class npKeyboardModifier : uint32_t
{
	None,
	Ctrl,
	Alt,
	Shift,
	ShiftAlt,
	ShiftCtrl,
	ShiftCtrlAlt,
	CtrlAlt
};

class npInput
{
public:
	npInput()
	{
		Reset();
		ResetHold();
	}

	bool m_isLMBDown_window = false;


	bool m_isLMBDown = false;
	bool m_isLMBHold = false;
	bool m_isLMBUp = false;
	bool m_isLMBDbl = false;

	bool m_isRMBDown = false;
	bool m_isRMBHold = false;
	bool m_isRMBUp = false;
	bool m_isRMBDbl = false;

	bool m_isMMBDown = false;
	bool m_isMMBHold = false;
	bool m_isMMBUp = false;
	bool m_isMMBDbl = false;

	bool m_isX1MBDown = false;
	bool m_isX1MBHold = false;
	bool m_isX1MBUp = false;
	bool m_isX1MBDbl = false;

	bool m_isX2MBDown = false;
	bool m_isX2MBHold = false;
	bool m_isX2MBUp = false;
	bool m_isX2MBDbl = false;

	npVec2i  m_cursorCoords = npVec2i(-1, -1);
	npVec2f  m_cursorCoordsForGUI = npVec2f(-1.f,-1.f);
	npVec2f  m_mouseDelta;
	float32_t  m_wheelDelta = 0.f;

	char16_t m_character = 0;

	npKeyboardModifier m_kbm = npKeyboardModifier::None;

	uint8_t m_key_hit[256];

	bool IsKeyHit(npInputKey key)
	{
		return m_key_hit[(uint32_t)key] != 0;
	}

	void SetKeyHit(npInputKey key, bool set)
	{
		m_key_hit[(uint32_t)key] = set;
	}

	uint8_t m_key_hold[256];

	bool IsKeyHold(npInputKey key) {
		return m_key_hold[(uint32_t)key] != 0;
	}

	uint8_t m_key_released[256];

	bool IsKeyReleased(npInputKey key) {
		return m_key_released[(uint32_t)key] != 0;
	}

	void ResetHold()
	{
		memset(m_key_hold, 0, sizeof(uint8_t) * 256);
		m_isLMBHold = false;
		m_isRMBHold = false;
		m_isMMBHold = false;
		m_isX1MBHold = false;
		m_isX2MBHold = false;
	}

	void Reset()
	{
		m_character = 0;

		m_isLMBDown = false;
		m_isLMBUp = false;
		m_isLMBDbl = false;

		m_isRMBDown = false;
		m_isRMBUp = false;
		m_isRMBDbl = false;

		m_isMMBDown = false;
		m_isMMBUp = false;
		m_isMMBDbl = false;

		m_isX1MBDown = false;
		m_isX1MBUp = false;
		m_isX1MBDbl = false;

		m_isX2MBDown = false;
		m_isX2MBUp = false;
		m_isX2MBDbl = false;

		memset(m_key_released, 0, sizeof(uint8_t) * 256);
		memset(m_key_hit, 0, sizeof(uint8_t) * 256);
		m_mouseDelta.Set(0.f, 0.f);
		m_wheelDelta = 0.f;


		m_LMBMultipleClickTime = 0.3f;
		m_RMBMultipleClickTime = 0.3f;
		m_MMBMultipleClickTime = 0.3f;
		m_X1MBMultipleClickTime = 0.3f;
		m_X2MBMultipleClickTime = 0.3f;
	}

	float32_t m_LMBMultipleClickTime = 0.f;
	float32_t m_RMBMultipleClickTime = 0.f;
	float32_t m_MMBMultipleClickTime = 0.f;
	float32_t m_X1MBMultipleClickTime = 0.f;
	float32_t m_X2MBMultipleClickTime = 0.f;
	uint32_t m_LMBClickCount = 0;
	uint32_t m_RMBClickCount = 0;
	uint32_t m_MMBClickCount = 0;
	uint32_t m_X1MBClickCount = 0;
	uint32_t m_X2MBClickCount = 0;
};

#endif

