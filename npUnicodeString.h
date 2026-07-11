#pragma once
class npUnicodeString
{
	size_t m_size = 0;
	size_t m_allocated = 0;
	char32_t* m_data = 0;
	void _reallocate(size_t new_allocated);
	void _free();

	template<typename other_type>
	size_t Getlen(const other_type* str)
	{
		uint32_t len = 0;
		if (str[0] != 0)
		{
			const other_type* p = &str[0];
			while ((size_t)*p++)
				len++;
		}
		return len;
	}

public:
	npUnicodeString();
	npUnicodeString(const char*);
	npUnicodeString(const wchar_t*);
	npUnicodeString(const char8_t*);
	npUnicodeString(const char16_t*);
	npUnicodeString(const char32_t*);
	npUnicodeString(const npUnicodeString&);
	npUnicodeString(npUnicodeString&&) noexcept;
	~npUnicodeString();

	void Assign(const char*);
	void Assign(const wchar_t*);
	void Assign(const char8_t*);
	void Assign(const char16_t*);
	void Assign(const char32_t*);
	void Assign(const npUnicodeString&);

	size_t Size();
	size_t Capacity();
	void Reserve(size_t);
	void Clear();
	void PopBack();
	void PushBack(char32_t);
	char32_t* Data();
	const char32_t* c_str();
	void Flip();

	void Insert(char32_t c, size_t where);

	void Append(const char*);
	void Append(const char8_t*);
	void Append(const wchar_t*);
	void Append(const char16_t*);
	void Append(const char32_t*);
	void Append(const char32_t*, size_t size);
	void Append(const npUnicodeString&);
	void Append(char32_t);
	void Append(uint32_t);
	void Append(uint64_t);
	void Append(int32_t);
	void Append(int64_t);
	void Append(float32_t);
	void AppendFloat(float32_t);
	void Append(float64_t);

	int32_t ToInt();
	uint32_t ToUint();
	float32_t ToFloat();
	float64_t ToFloat64();
	void ToUTF8(npStringA&);
	void ToUTF16(npStringW&);

	// return is encoding type
	// 1 - utf8
	// 2 - utf16
	// 3 - utf16 big endian
	// 4 - utf32
	// 5 - utf32 big endian
	uint32_t ReadFromFile(const char*);

	void SaveToFileUTF8(const char*, bool addBOM = true);
	void SaveToFileUTF16(const char*, bool addBOM = true);
	void SaveToFileUTF32(const char*, bool addBOM = true);

	npUnicodeString& operator=(const npUnicodeString&);
	npUnicodeString& operator=(npUnicodeString&&) noexcept;

	//m_size = size;
	void _set_size(size_t size);
};

