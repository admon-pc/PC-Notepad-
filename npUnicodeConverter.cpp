#include "notepad.h"

void npUnicodeConverter::_find16From32()
{
	if (m_32 > 0xFFFF)
	{
		char32_t _u = m_32 - 0x10000;
		m_16[0] = 0xD800 + (_u >> 10);
		m_16[1] = 0xDC00 + (_u % 0x400);
		m_16Num = 2;
	}
	else
	{
		m_16[0] = m_32;
		m_16[1] = 0;
		m_16Num = 1;
	}
}

void npUnicodeConverter::_find8From32()
{
	// u vvvv    wwww xxxx    yyyy zzzz
	char32_t _u = m_32;

	if (m_32 > 0xFFFF)
	{

		uint32_t vvvv = (_u & 0xF0000) >> 16;
		uint32_t wwww = (_u & 0xF000) >> 12;
		uint32_t xxxx = (_u & 0xF00) >> 8;
		uint32_t yyyy = (_u & 0xF0) >> 4;
		uint32_t zzzz = (_u & 0xF);

		m_8[0] = 0xF0 + ((_u & 0x100000) >> 18);
		m_8[0] |= vvvv >> 2;

		m_8[1] = 0x80 + wwww;
		m_8[1] |= (vvvv & 0x3) << 4;

		m_8[2] = 0x80 + (xxxx << 2);
		m_8[2] |= yyyy >> 2;

		m_8[3] = 0x80 + (zzzz);
		m_8[3] |= (yyyy & 0x3) << 4;

		m_8Num = 4;
	}
	else if (m_32 > 0x7FF)
	{
		uint32_t wwww = (_u & 0xF000) >> 12;
		uint32_t xxxx = (_u & 0xF00) >> 8;
		uint32_t yyyy = (_u & 0xF0) >> 4;
		uint32_t zzzz = (_u & 0xF);

		m_8[0] = 0xE0 + wwww;

		m_8[1] = 0x80 + (xxxx << 2);
		m_8[1] |= yyyy >> 2;

		m_8[2] = 0x80 + (zzzz);
		m_8[2] |= (yyyy & 0x3) << 4;

		m_8[3] = 0;

		m_8Num = 3;
	}
	else if (m_32 > 0x7F)
	{
		uint32_t xxxx = (_u & 0xF00) >> 8;
		uint32_t yyyy = (_u & 0xF0) >> 4;
		uint32_t zzzz = (_u & 0xF);

		m_8[0] = 0xC0 + ((xxxx & 0x7) << 2);
		m_8[0] |= (yyyy >> 2);

		m_8[1] = 0x80 + (zzzz);
		m_8[1] |= (yyyy & 0x3) << 4;

		m_8[2] = 0;
		m_8[3] = 0;

		m_8Num = 2;
	}
	else
	{
		uint32_t yyyy = (_u & 0xF0) >> 4;
		uint32_t zzzz = (_u & 0xF);

		m_8[0] = zzzz;
		m_8[0] |= (yyyy & 0x7) << 4;

		m_8[1] = 0;
		m_8[2] = 0;
		m_8[3] = 0;

		m_8Num = 1;
	}
}

uint32_t npUnicodeConverter::Set(const npVec4_t<uint32_t>& c)
{
	uint32_t c1 = c.x;
	uint32_t c2 = c.y;
	uint32_t c3 = c.z;
	uint32_t c4 = c.w;

	m_32 = 0;
	m_16[0] = 0;
	m_16[1] = 0;
	m_8[0] = 0;
	m_8[1] = 0;
	m_8[2] = 0;
	m_8[3] = 0;
	m_16Num = 0;
	m_8Num = 0;
	// u vvvv    wwww xxxx    yyyy zzzz

	if ((c1 & 0xF8) == 0xF0)
	{
		// this is MUST BE 4 bytes utf8
		if ((c2 & 0xC0) == 0x80)
		{
			if ((c3 & 0xC0) == 0x80)
			{
				if ((c4 & 0xC0) == 0x80)
				{
					m_32 = (c1 & 0x7) << 18;
					m_32 |= (c2 & 0x3F) << 12;
					m_32 |= (c3 & 0x3F) << 6;
					m_32 |= (c3 & 0x3F);
					m_8[0] = c1;
					m_8[1] = c2;
					m_8[2] = c3;
					m_8[3] = c4;
					m_8Num = 4;
				}
			}
		}
	}
	else if ((c1 & 0xF0) == 0xE0)
	{
		// this is MUST BE 3 bytes utf8
		if ((c2 & 0xC0) == 0x80)
		{
			if ((c3 & 0xC0) == 0x80)
			{
				m_32 = (c1 & 0xF) << 12;
				m_32 |= (c2 & 0x3F) << 6;
				m_32 |= (c3 & 0x3F);
				m_8[0] = c1;
				m_8[1] = c2;
				m_8[2] = c3;
				m_8Num = 3;
			}
		}
	}
	else if ((c1 & 0xE0) == 0xC0)
	{
		// this is MUST BE 2 bytes utf8
		if ((c2 & 0xC0) == 0x80)
		{
			m_32 = (c1 & 0x1F) << 6;
			m_32 |= (c2 & 0x3F);
			m_8[0] = c1;
			m_8[1] = c2;
			m_8Num = 2;
		}
	}
	else
	{
		m_32 = c1;
		m_8[0] = c1;
		m_8Num = 1;
	}

	_find16From32();

	return m_8Num;
}

uint32_t npUnicodeConverter::Set(char16_t c1, char16_t c2)
{
	m_32 = 0;
	m_16[0] = 0;
	m_16[1] = 0;
	m_8[0] = 0;
	m_8[1] = 0;
	m_8[2] = 0;
	m_8[3] = 0;
	m_16Num = 0;
	m_8Num = 0;

	if ((c1 & 0xFC00) == 0xD800)
	{
		if ((c2 & 0xFC00) == 0xDC00)
		{
			// 2 char16_t
			m_32 = (c1 & 0x3FF) << 10;
			m_32 |= (c2 & 0x3FF);
			m_16Num = 2;
		}
	}
	else
	{
		m_32 = c1;
		m_16Num = 1;
	}

	_find8From32();

	return m_16Num;
}

void npUnicodeConverter::wchar_to_char(const wchar_t* str, size_t sz, npStringA* out)
{
	npUnicodeConverter uc;
	if (str && sz && out)
	{
		out->clear();
		for (size_t i = 0; i < sz; )
		{
			wchar_t c1 = str[i];
			wchar_t c2 = 0;
			if (i + 1 < sz)
				c2 = str[i + 1];

			auto n = uc.Set(c1,c2);
			if (uc.m_8Num)
			{
				for (size_t o = 0; o < uc.m_8Num; ++o)
				{
					out->push_back(uc.m_8[o]);
				}
			}
			i += n;
		}
	}
}
void npUnicodeConverter::char_to_wchar(const char* str, size_t sz, npStringW* out)
{
	npUnicodeConverter uc;
	if (str && sz && out)
	{
		out->clear();
		for (size_t i = 0; i < sz; )
		{
			char c1 = str[i];
			char c2 = 0;
			char c3 = 0;
			char c4 = 0;

			if (i + 1 < sz) c2 = str[i + 1];
			if (i + 2 < sz) c3 = str[i + 2];
			if (i + 3 < sz) c4 = str[i + 3];

			auto n = uc.Set(npVec4u(c1, c2, c3, c4));
			if (uc.m_16Num)
			{
				for (size_t o = 0; o < uc.m_16Num; ++o)
				{
					out->push_back(uc.m_16[o]);
				}
			}
			i += n;
		}
	}
}

