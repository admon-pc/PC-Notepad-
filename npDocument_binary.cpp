#include "notepad.h"
#include <filesystem>

extern notepad* g_app;

const wchar_t g_npDocument_binary_bytes[256][3] = 
{
	L"00",L"01",L"02",L"03",L"04",L"05",L"06",L"07",L"08",L"09",L"0A",L"0B",L"0C",L"0D",L"0E",L"0F",
	L"10",L"11",L"12",L"13",L"14",L"15",L"16",L"17",L"18",L"19",L"1A",L"1B",L"1C",L"1D",L"1E",L"1F",
	L"20",L"21",L"22",L"23",L"24",L"25",L"26",L"27",L"28",L"29",L"2A",L"2B",L"2C",L"2D",L"2E",L"2F",
	L"30",L"31",L"32",L"33",L"34",L"35",L"36",L"37",L"38",L"39",L"3A",L"3B",L"3C",L"3D",L"3E",L"3F",
	L"40",L"41",L"42",L"43",L"44",L"45",L"46",L"47",L"48",L"49",L"4A",L"4B",L"4C",L"4D",L"4E",L"4F",
	L"50",L"51",L"52",L"53",L"54",L"55",L"56",L"57",L"58",L"59",L"5A",L"5B",L"5C",L"5D",L"5E",L"5F",
	L"60",L"61",L"62",L"63",L"64",L"65",L"66",L"67",L"68",L"69",L"6A",L"6B",L"6C",L"6D",L"6E",L"6F",
	L"70",L"71",L"72",L"73",L"74",L"75",L"76",L"77",L"78",L"79",L"7A",L"7B",L"7C",L"7D",L"7E",L"7F",
	L"80",L"81",L"82",L"83",L"84",L"85",L"86",L"87",L"88",L"89",L"8A",L"8B",L"8C",L"8D",L"8E",L"8F",
	L"90",L"91",L"92",L"93",L"94",L"95",L"96",L"97",L"98",L"99",L"9A",L"9B",L"9C",L"9D",L"9E",L"9F",
	L"A0",L"A1",L"A2",L"A3",L"A4",L"A5",L"A6",L"A7",L"A8",L"A9",L"AA",L"AB",L"AC",L"AD",L"AE",L"AF",
	L"B0",L"B1",L"B2",L"B3",L"B4",L"B5",L"B6",L"B7",L"B8",L"B9",L"BA",L"BB",L"BC",L"BD",L"BE",L"BF",
	L"C0",L"C1",L"C2",L"C3",L"C4",L"C5",L"C6",L"C7",L"C8",L"C9",L"CA",L"CB",L"CC",L"CD",L"CE",L"CF",
	L"D0",L"D1",L"D2",L"D3",L"D4",L"D5",L"D6",L"D7",L"D8",L"D9",L"DA",L"DB",L"DC",L"DD",L"DE",L"DF",
	L"E0",L"E1",L"E2",L"E3",L"E4",L"E5",L"E6",L"E7",L"E8",L"E9",L"EA",L"EB",L"EC",L"ED",L"EE",L"EF",
	L"F0",L"F1",L"F2",L"F3",L"F4",L"F5",L"F6",L"F7",L"F8",L"F9",L"FA",L"FB",L"FC",L"FD",L"FE",L"FF",
};
const wchar_t g_npDocument_binary_ascii[256][1] =
{
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
	L'€',L'·',L'‚',L'ƒ',L'„',L'…',L'†',L'‡',L'ˆ',L'‰',L'Š',L'‹',L'Œ',L'·',L'Ž',L'·',
	L'·',L'‘',L'’',L'“',L'”',L'•',L'–',L'—',L'˜',L'™',L'š',L'›',L'œ',L'·',L'ž',L'Ÿ',
	L'·',L'¡',L'¢',L'£',L'¤',L'¥',L'¦',L'§',L'¨',L'©',L'ª',L'«',L'¬',L'·',L'®',L'¯',
	L'°',L'±',L'²',L'³',L'´',L'µ',L'¶',L'·',L'¸',L'¹',L'º',L'»',L'¼',L'½',L'¾',L'¿',
	L'À',L'Á',L'Â',L'Ã',L'Ä',L'Å',L'Æ',L'Ç',L'È',L'É',L'Ê',L'Ë',L'Ì',L'Í',L'Î',L'Ï',
	L'Ð',L'Ñ',L'Ò',L'Ó',L'Ô',L'Õ',L'Ö',L'×',L'Ø',L'Ù',L'Ú',L'Û',L'Ü',L'Ý',L'Þ',L'ß',
	L'à',L'á',L'â',L'ã',L'ä',L'å',L'æ',L'ç',L'è',L'é',L'ê',L'ë',L'ì',L'í',L'î',L'ï',
	L'ð',L'ñ',L'ò',L'ó',L'ô',L'õ',L'ö',L'÷',L'ø',L'ù',L'ú',L'û',L'ü',L'ý',L'þ',L'ÿ',
};

npDocument_binary::npDocument_binary(
	npRenderer* r,
	FILE* f,
	const char* filePath)
	:
	npDocument(r)
{
	m_filePath = filePath;
	npUnicodeConverter::char_to_wchar(m_filePath.data(), m_filePath.size(), &m_filePathW);

	m_file = f;

	_setTitleFromPath();
	m_isSaved = true;
	m_type = npDocumentType::Binary;

	m_fontEditor = m_renderer->CreateNPFont("Consolas", false, false, m_fontEditorSize);
	
	Update(0.f);
	_findMaxNumOfLines();
	_readBytes();
}

npDocument_binary::~npDocument_binary()
{
	if (m_file)
		fclose(m_file);

	if (m_fontEditor)
		delete m_fontEditor;
}

void npDocument_binary::_readBytes()
{
	m_bytes.clear();
	if (m_numOfBytesToDraw)
	{
		_fseeki64(m_file, m_fileFirstDrawBytePosition, SEEK_SET);
		for (uint32_t i = 0; i < m_numOfBytesToDraw; ++i)
		{
			uint8_t b = 0;
			if (!fread(&b, 1, 1, m_file))
			{
				break;
			}
			m_bytes.push_back(b);
		}
	}
}

void npDocument_binary::_findMaxNumOfLines()
{
	auto editorHeight = m_editorArea.w - m_editorArea.y;
	m_maxNumOfLines = (uint32_t)floorf(editorHeight / m_nextLinePositionOffset);
	m_numOfBytesToDraw = m_maxNumOfLines * 16;
}

void npDocument_binary::_setTitleFromPath()
{
	std::filesystem::path p(m_filePath.c_str());
	m_title = p.filename().c_str();
}

void npDocument_binary::Update(float dt)
{
	m_editorArea = g_app->m_documentAreaRect;
	m_editorArea_bytes = m_editorArea;
	m_editorArea_bytes.x += 30;
	m_editorArea_bytes.z -= 200;

	m_editorArea_text = m_editorArea;
	m_editorArea_text.x = m_editorArea_bytes.z;
	auto fontSize = m_fontEditor->GetFontSize();
	m_nextLinePositionOffset = fontSize.y;
}

void npDocument_binary::Draw(float dt)
{
	m_renderer->SetClipRect(m_editorArea);
	m_renderer->DrawRectangle(m_editorArea, g_app->m_currColorTheme->m_hexeditorBBG);
	
	float carriagePositionBytes = 0.f;
	float carriagePositionText = 0.f;
	float rowPosition = 0.f;
	auto fontSize = m_fontEditor->GetFontSize();
	float nextCharPositionOffsetBytes = (fontSize.x * 2) + fontSize.x;
	float nextCharPositionOffsetText = fontSize.x;
	
	m_renderer->SetNPFont(m_fontEditor);

	int count8 = 0;
	int count16 = 0;
	for (size_t i = 0; i < m_bytes.m_size; ++i)
	{
		uint8_t b = m_bytes.m_data[i];

		npVec4f char1Rect;
		char1Rect.x = m_editorArea_bytes.x + carriagePositionBytes;
		char1Rect.y = m_editorArea_bytes.y + rowPosition;
		char1Rect.z = char1Rect.x + fontSize.x;
		char1Rect.w = char1Rect.y + fontSize.y;

		npVec4f char2Rect = char1Rect;
		char2Rect.x += fontSize.x;
		char2Rect.z = char2Rect.x + fontSize.x;

		m_renderer->DrawText(&g_npDocument_binary_bytes[b][0], 1, 
			char1Rect.x, char1Rect.y, g_app->m_currColorTheme->m_hexeditorBText);
		m_renderer->DrawText(&g_npDocument_binary_bytes[b][1], 1, 
			char2Rect.x, char2Rect.y, g_app->m_currColorTheme->m_hexeditorBText);

		npVec4f charTextRect;
		charTextRect.x = m_editorArea_text.x + carriagePositionText;
		charTextRect.y = m_editorArea_text.y + rowPosition;
		charTextRect.z = charTextRect.x + fontSize.x;
		charTextRect.w = charTextRect.y + fontSize.y;
		npchar ch = g_npDocument_binary_ascii[b][0];

		if (ch >= 0 && ch < 32)
			ch = L'·';
		switch (ch)
		{
		case 127:
		case 129:
		case 141:
		case 143:
		case 144:
		case 157:
		case 160:
			ch = L'·';
			break;
		}

		m_renderer->DrawText(&ch, 1,
			charTextRect.x, charTextRect.y, 
			g_app->m_currColorTheme->m_hexeditorTText);

		carriagePositionBytes += nextCharPositionOffsetBytes;
		carriagePositionText += nextCharPositionOffsetText;
		++count8;
		if (count8 == 8)
		{
			count8 = 0;
			carriagePositionBytes += nextCharPositionOffsetBytes;
		}

		++count16;
		if (count16 == 16)
		{
			count16 = 0;
			carriagePositionBytes = 0;
			carriagePositionText = 0;
			rowPosition += m_nextLinePositionOffset;
		}
	}
}

void npDocument_binary::Save()
{
}

void npDocument_binary::SaveAs()
{
}
