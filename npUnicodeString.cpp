#include "notepad.h"

namespace np_internal
{
	const float64_t string_to_float_table_d[17] =
	{
		0.,
		0.1,
		0.01,
		0.001,
		0.0001,
		0.00001,
		0.000001,
		0.0000001,
		0.00000001,
		0.000000001,
		0.0000000001,
		0.00000000001,
		0.000000000001,
		0.0000000000001,
		0.00000000000001,
		0.000000000000001,
		0.0000000000000001,
	};
}

void npUnicodeString::_reallocate(size_t new_allocated)
{
	char32_t* new_data = (char32_t*)malloc(new_allocated * sizeof(char32_t));
	if (m_data)
	{
		memcpy(new_data, m_data, m_size * sizeof(char32_t));
		free(m_data);
	}
	else
	{
		memset(new_data, 0, new_allocated);
	}
	m_data = new_data;
	m_allocated = new_allocated;
}

npUnicodeString::npUnicodeString()
{
	_reallocate(np_internal::string_wordSize);
}

npUnicodeString::npUnicodeString(const char8_t* str)
{
	NP_ASSERT_ST(str);
	_reallocate(np_internal::string_wordSize);
	Append(str);
}

npUnicodeString::npUnicodeString(const char* str)
{
	NP_ASSERT_ST(str);
	_reallocate(np_internal::string_wordSize);
	Append(str);
}

npUnicodeString::npUnicodeString(const char16_t* str)
{
	NP_ASSERT_ST(str);
	_reallocate(np_internal::string_wordSize);
	Append(str);
}

npUnicodeString::npUnicodeString(const wchar_t* str)
{
	NP_ASSERT_ST(str);
	_reallocate(np_internal::string_wordSize);
	Append(str);
}

npUnicodeString::npUnicodeString(const char32_t* str)
{
	NP_ASSERT_ST(str);
	_reallocate(np_internal::string_wordSize);
	Append(str);
}

npUnicodeString::npUnicodeString(const npUnicodeString& str)
{
	_reallocate(np_internal::string_wordSize);
	Append(str);
}

npUnicodeString::npUnicodeString(npUnicodeString&& str) noexcept
{
	m_allocated = str.m_allocated;
	m_data = str.m_data;
	m_size = str.m_size;
	str.m_allocated = 0;
	str.m_data = 0;
	str.m_size = 0;
}

npUnicodeString::~npUnicodeString()
{
	_free();
}

void npUnicodeString::_free()
{
	if (m_data)
	{
		free(m_data);
		m_data = 0;
	}
}

void npUnicodeString::Assign(const char* v)
{
	Clear();
	Append(v);
}

void npUnicodeString::Assign(const char8_t* v)
{
	Clear();
	Append(v);
}

void npUnicodeString::Assign(const char16_t* v)
{
	Clear();
	Append(v);
}

void npUnicodeString::Assign(const wchar_t* v)
{
	Clear();
	Append(v);
}

void npUnicodeString::Assign(const char32_t* v)
{
	Clear();
	Append(v);
}

void npUnicodeString::Assign(const npUnicodeString& v)
{
	Clear();
	Append(v);
}

size_t npUnicodeString::Size()
{
	return m_size;
}

size_t npUnicodeString::Capacity()
{
	return m_allocated;
}

void npUnicodeString::Reserve(size_t s)
{
	if (s > m_allocated)
	{
		_reallocate((s + 1) + np_internal::string_wordSize);
	}
}

void npUnicodeString::Clear()
{
	m_size = 0;
	m_data[m_size] = 0;
}

void npUnicodeString::PopBack()
{
	if (m_size)
	{
		--m_size;
		m_data[m_size] = 0;
	}
}

void npUnicodeString::PushBack(char32_t c)
{
	size_t new_size = m_size + 1;
	if ((new_size + 1u) > m_allocated)
		_reallocate((new_size + 1) + (1 + (size_t)(m_size * 0.5f)));
	m_data[m_size] = c;
	m_size = new_size;
	m_data[m_size] = 0;
}

char32_t* npUnicodeString::Data()
{
	return m_data;
}

const char32_t* npUnicodeString::c_str()
{
	return m_data;
}

void npUnicodeString::Append(const char8_t* str)
{
	size_t new_size = Getlen(str) + m_size;

	if ((new_size + 1u) > m_allocated)
		_reallocate((new_size + 1) + (1 + (size_t)(m_size * 0.5f)));

	npUnicodeConverter uc;

	const char8_t* p = str;
	while (*p)
	{
		auto p_begin = p;

		npVec4u vec;
		vec.x = *p;
		++p;
		if (*p)
		{
			vec.y = *p;
			++p;
			if (*p)
			{
				vec.z = *p;
				++p;
				if (*p)
					vec.w = *p;
			}
		}

		auto ret = uc.Set(vec);
		if (!ret)
			break;

		PushBack(uc.m_32);

		p = p_begin + ret;
	}
}

void npUnicodeString::Append(const char* str)
{
	Append((const char8_t*)str);
}

void npUnicodeString::Append(const wchar_t* str)
{
	Append((const char16_t*)str);
}

void npUnicodeString::Append(const char16_t* str)
{
	size_t new_size = Getlen(str) + m_size;

	if ((new_size + 1u) > m_allocated)
		_reallocate((new_size + 1) + (1 + (size_t)(m_size * 0.5f)));

	npUnicodeConverter uc;

	const char16_t* p = str;
	while (*p)
	{
		auto p_begin = p;

		char16_t c1 = 0;
		char16_t c2 = 0;

		c1 = *p;

		++p;
		if (*p)
			c2 = *p;

		auto ret = uc.Set(c1, c2);
		if (!ret)
			break;

		PushBack(uc.m_32);

		p = p_begin + ret;
	}
}

// it can be that str will not have zero at the end
void npUnicodeString::Append(const char32_t* str, size_t size)
{
	size_t new_size = size + m_size;

	if ((new_size + 1u) > m_allocated)
		_reallocate((new_size + 1) + (1 + (size_t)(m_size * 0.5f)));
	const char32_t* p = str;
	//while (*p)
	for (size_t i = 0; i < size; ++i)
	{
		PushBack(*p);
		++p;
	}
}

void npUnicodeString::Append(const char32_t* str)
{
	Append(str, Getlen(str));
}

void npUnicodeString::Append(const npUnicodeString& str)
{
	size_t new_size = str.m_size + m_size;
	if ((new_size + 1u) > m_allocated)
		_reallocate((new_size + 1) + (1 + (size_t)(m_size * 0.5f)));
	const char32_t* p = str.m_data;
	while (*p)
	{
		PushBack(*p);
		++p;
	}
}

void npUnicodeString::Append(char32_t v)
{
	PushBack(v);
}

void npUnicodeString::Append(uint32_t v)
{
	char buf[20];
	sprintf_s(buf, 20, "%u", v);
	Append(buf);
}

void npUnicodeString::Append(uint64_t v)
{
	char buf[20];
	sprintf_s(buf, 20, "%llu", v);
	Append(buf);
}

void npUnicodeString::Append(int32_t v)
{
	char buf[20];
	sprintf_s(buf, 20, "%i", v);
	Append(buf);
}

void npUnicodeString::Append(int64_t v)
{
	char buf[20];
	sprintf_s(buf, 20, "%lli", v);
	Append(buf);
}

void npUnicodeString::Append(float32_t v)
{
	char buf[20];
	sprintf_s(buf, 20, "%f", v);
	Append(buf);
}

void npUnicodeString::AppendFloat(float32_t v)
{
	char buf[20];
	sprintf_s(buf, 20, "%.7f", v);
	Append(buf);
}

void npUnicodeString::Append(float64_t v)
{
	char buf[20];
	sprintf_s(buf, 20, "%.14f", v);
	Append(buf);
}

int32_t npUnicodeString::ToInt()
{
	size_t len = m_size;
	int32_t result = 0;
	int32_t mul_val = 1;
	bool is_neg = m_data[0] == U'-';
	for (size_t i = 0, last = len - 1; i < len; ++i)
	{
		int32_t char_value = (int32_t)m_data[last] - 0x30;
		if (char_value >= 0 && char_value <= 9)
		{
			result += char_value * mul_val;
			mul_val *= 10;
		}
		--last;
	}
	if (is_neg)
		result = -result;
	return result;
}

uint32_t npUnicodeString::ToUint()
{
	if (m_data[0] == U'-')
		return 0;
	size_t len = m_size;
	int32_t result = 0;
	int32_t mul_val = 1;
	for (size_t i = 0, last = len - 1; i < len; ++i)
	{
		int32_t char_value = (int32_t)m_data[last] - 0x30;
		if (char_value >= 0 && char_value <= 9)
		{
			result += char_value * mul_val;
			mul_val *= 10;
		}
		--last;
	}
	return result;
}

float32_t npUnicodeString::ToFloat()
{
	float32_t result = 0.f;
	auto ptr = m_data;
	bool is_negative = *ptr == U'-';

	if (is_negative)
		++ptr;

	int64_t i = 0;
	while (*ptr >= U'0' && *ptr <= U'9')
	{
		i *= 10;
		i += *ptr - U'0';
		++ptr;
	}
	result = (float32_t)i;

	i = 0;

	if (*ptr == U'.')
		++ptr;

	int64_t part_2_count = 0;
	while (*ptr >= U'0' && *ptr <= U'9')
	{
		i *= 10;
		i += *ptr - U'0';
		++ptr;
		++part_2_count;
	}
	result += (float32_t)i * np_internal::string_to_float_table[part_2_count];

	return is_negative ? -result : result;
}

float64_t npUnicodeString::ToFloat64()
{
	float64_t result = 0.;
	auto ptr = m_data;
	bool is_negative = *ptr == U'-';

	if (is_negative)
		++ptr;

	int64_t i = 0;
	while (*ptr >= U'0' && *ptr <= U'9')
	{
		i *= 10;
		i += *ptr - U'0';
		++ptr;
	}
	result = (float64_t)i;

	i = 0;

	if (*ptr == U'.')
		++ptr;

	int32_t part_2_count = 0;
	while (*ptr >= U'0' && *ptr <= U'9')
	{
		i *= 10;
		i += *ptr - U'0';
		++ptr;
		++part_2_count;
	}
	result += (float64_t)i * np_internal::string_to_float_table_d[part_2_count];

	return is_negative ? -result : result;
}

void npUnicodeString::ToUTF8(npStringA& str)
{
	npUnicodeConverter uc;
	for (size_t i = 0; i < m_size; ++i)
	{
		uc.Set(m_data[i]);

		if (!uc.m_8Num)
			break;

		for (size_t o = 0; o < uc.m_8Num; ++o)
		{
			str.push_back(uc.m_8[o]);
		}
	}
}

void npUnicodeString::ToUTF16(npStringW& str)
{
	npUnicodeConverter uc;
	for (size_t i = 0; i < m_size; ++i)
	{
		uc.Set(m_data[i]);

		if (!uc.m_16Num)
			break;

		str.push_back(uc.m_16[0]);
		if (uc.m_16Num == 2)
			str.push_back(uc.m_16[1]);
	}
}

uint32_t npUnicodeString::ReadFromFile(const char* fn)
{
	uint32_t ret = 1;

	NP_ASSERT_ST(fn);
	FILE* f = 0;
	fopen_s(&f, fn, "rb");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		auto fsz = ftell(f);
		if(fsz)
			Reserve(fsz);
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
			ret = 1;
		}
		else if ((bom[0] == 0x00)
			&& (bom[1] == 0x00)
			&& (bom[2] == 0xFE)
			&& (bom[3] == 0xFF))
		{
			fseek(f, 4, SEEK_SET);
			type = type_utf32;
			isBE = true;
			ret = 5;
		}
		else if ((bom[0] == 0xFF)
			&& (bom[1] == 0xFE)
			&& (bom[2] == 0x00)
			&& (bom[3] == 0x00))
		{
			fseek(f, 4, SEEK_SET);
			type = type_utf32;
			ret = 4;
		}
		else if ((bom[0] == 0xFF)
			&& (bom[1] == 0xFE))
		{
			fseek(f, 2, SEEK_SET);
			type = type_utf16;
			ret = 2;
		}
		else if ((bom[0] == 0xFE)
			&& (bom[1] == 0xFF))
		{
			fseek(f, 2, SEEK_SET);
			type = type_utf16;
			isBE = true;
			ret = 3;
		}
		

		union
		{
			uint32_t m_32;
			uint16_t m_16[2];
			uint8_t m_8[4];
		}
		uint_union;

		npUnicodeConverter uc;
		while (true)
		{
			uint8_t buf[4] = { 0,0,0,0 };

			if (type == type_utf8)
			{
				long t = ftell(f);
				size_t r = fread_s(buf, 4, 1, 4, f);
				if (!r)
					break;

				uint32_t b = uc.Set(npVec4u(buf[0], buf[1], buf[2], buf[3]));
				if (!uc.m_32)
					break;

				PushBack(uc.m_32);

				fseek(f, t + b, SEEK_SET);
			}
			else if (type == type_utf16)
			{
				long t = ftell(f);
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

				PushBack(uc.m_32);

				fseek(f, t + (b * 2), SEEK_SET);
			}
			else if (type == type_utf32)
			{
				long t = ftell(f);
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

				PushBack(uint_union.m_32);

				fseek(f, t + 4, SEEK_SET);
			}
			else
			{
				break;
			}
		}

		fclose(f);
	}
	return ret;
}

void npUnicodeString::SaveToFileUTF8(const char* fn, bool addBOM)
{
	FILE* f = 0;
	fopen_s(&f, fn, "wb");
	if (f && m_data && m_size)
	{
		if (addBOM)
		{
			uint8_t bom[4] = { 0xEF, 0xBB, 0xBF, 0 };
			fwrite(bom, 1, 3, f);
		}

		npUnicodeConverter uc;
		for (size_t i = 0; i < m_size; ++i)
		{
			uc.Set(m_data[i]);
			for(size_t o = 0; o < uc.m_8Num; ++o)
			{
				fwrite(&uc.m_8[o], 1, 1, f);
			}
		}
	}

	if (f)
		fclose(f);
}

void npUnicodeString::SaveToFileUTF16(const char* fn, bool addBOM)
{
	FILE* f = 0;
	fopen_s(&f, fn, "wb");
	if (f && m_data && m_size)
	{
		if (addBOM)
		{
			uint8_t bom[4] = { 0xFF, 0xFE, 0, 0 };
			fwrite(bom, 1, 2, f);
		}

		npUnicodeConverter uc;
		for (size_t i = 0; i < m_size; ++i)
		{
			uc.Set(m_data[i]);
			if (uc.m_16Num)
			{
				fwrite(&uc.m_16[0], sizeof(char16_t), 1, f);
				if(uc.m_16Num > 1)
					fwrite(&uc.m_16[1], sizeof(char16_t), 1, f);
			}
		}
	}

	if (f)
		fclose(f);
}

void npUnicodeString::SaveToFileUTF32(const char* fn, bool addBOM)
{
	FILE* f = 0;
	fopen_s(&f, fn, "wb");
	if (f && m_data && m_size)
	{
		if (addBOM)
		{
			uint8_t bom[4] = { 0xFF, 0xFE, 0, 0 };
			fwrite(bom, 1, 4, f);
		}

		fwrite(m_data, sizeof(char32_t), m_size, f);
	}

	if(f)
		fclose(f);
}

npUnicodeString& npUnicodeString::operator=(const npUnicodeString& str)
{
	_reallocate(np_internal::string_wordSize);
	Clear();
	Append(str);
	return *this;
}

npUnicodeString& npUnicodeString::operator=(npUnicodeString&& str) noexcept
{
	_free();
	m_allocated = str.m_allocated;
	m_data = str.m_data;
	m_size = str.m_size;
	str.m_allocated = 0;
	str.m_data = 0;
	str.m_size = 0;
	return *this;
}

void npUnicodeString::Flip()
{
	if (!m_size)
		return;
	npUnicodeString flippedStr;
	for (size_t i = m_size - 1; i >= 0; --i)
	{
		flippedStr.Append(m_data[i]);
		if (!i)
			break;
	}
	*this = std::move(flippedStr);
}

void npUnicodeString::Insert(char32_t c, size_t where)
{
	size_t new_size = m_size + 1;
	if ((new_size + 1u) > m_allocated)
		_reallocate((new_size + 1) + (1 + (size_t)(m_size * 0.5f)));

	size_t i = m_size;
	while (i >= where)
	{
		auto next = i + 1;
		if (next < m_allocated)
			m_data[next] = m_data[i];

		if (i == 0)
			break;
		--i;
	}
	m_data[where] = c;
	m_size = new_size;
	m_data[m_size] = 0;
}

void npUnicodeString::_set_size(size_t size)
{
	m_size = size;
}

