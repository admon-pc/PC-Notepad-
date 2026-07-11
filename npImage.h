#pragma once
class npImage
{
public:
	npImage();
	~npImage();

	// fast creating
	void Create(uint32_t x, uint32_t y);

	// теперь m_data может хранить файл полностью. В случае DDS для DirectXTK.
	// для доступа к массиву с пикселями нужен m_bitDataOffset
	// если это обычные файлы (типа .PNG) то m_data указывает на обычный буфер а m_bitDataOffset = 0
	uint8_t* m_data = nullptr;
	uint32_t m_bitDataOffset = 0;
	uint32_t m_dataSize = 0;
	uint32_t m_fileSize = 0;
	uint32_t m_width = 0;
	uint32_t m_height = 0;
	uint32_t m_bits = 32;
	uint32_t m_mipCount = 1;
	uint32_t m_pitch = 0;

	struct rgba
	{
		uint8_t r, g, b, a;
	};

	void FlipVertical();
	void FlipPixel();
	void Fill(const npColor& color);

	// outPosition and outUV is optional
	// it will write position and texture coordinate.
	void Fill(npImage*, const npVec2u& where, npVec2i* outPosition);

	void Fill(const uint8_t* indices, npColor* palette, const npVec2u& size,
		const npVec2u& where, npVec2i* outPosition);

	// here `where` is a pointer, and method will move position for next draw, it will add value into `where` (size.x and size.y)
	// so, you do not need to care about changin where everytime.
	void Fill(const uint8_t* indices, npColor* palette,
		const npVec2u& size, npVec2u* where, npVec2i* outPosition);

	void Delete();
};

