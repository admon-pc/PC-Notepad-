#pragma once
class npUnicodeConverter
{
	void _find16From32();
	void _find8From32();
public:
	npUnicodeConverter() {}
	~npUnicodeConverter() {}

	void Set(char32_t c)
	{
		m_32 = c;
		_find16From32();
		_find8From32();
	}

	void Set(char16_t c)
	{
		m_32 = c;
		m_16[0] = c;
		m_16[1] = 0;
		m_16Num = 1;
		_find8From32();
	}

	void Set(char8_t c)
	{
		m_32 = c;
		m_16[0] = c;
		m_16[1] = 0;
		m_16Num = 1;
		m_8[0] = c;
		m_8[1] = 0;
		m_8[2] = 0;
		m_8[3] = 0;
		m_8Num = 1;
	}

	void Set(char c)
	{
		m_32 = c;
		m_16[0] = c;
		m_16[1] = 0;
		m_16Num = 1;
		m_8[0] = c;
		m_8[1] = 0;
		m_8[2] = 0;
		m_8[3] = 0;
		m_8Num = 1;
	}

	void Set(wchar_t c)
	{
		Set((char32_t)c);
	}

	static void wchar_to_char(const wchar_t* str, size_t sz, npStringA* out);
	static void char_to_wchar(const char* str, size_t sz, npStringW* out);

	uint32_t Set(const npVec4_t<uint32_t>& c);
	uint32_t Set(char16_t c1, char16_t c2);

	uint32_t m_32 = 0;
	uint16_t m_16[2] = { 0, 0 };
	uint8_t m_8[4] = { 0, 0, 0, 0 };

	uint32_t m_16Num = 0;
	uint32_t m_8Num = 0;
};

