#include "notepad.h"
npImage::npImage()
{
}

npImage::~npImage()
{
	Delete();
}

void npImage::Delete()
{
	if (m_data)
		free(m_data);
}

void npImage::Create(uint32_t x, uint32_t y)
{
	Delete();
	m_width = x;
	m_height = y;
	m_pitch = m_width * 4;
	m_dataSize = m_pitch * m_height;
	m_data = (uint8_t*)malloc(m_dataSize);
}

void npImage::FlipVertical()
{
	uint8_t* line = nullptr;
	line = (uint8_t*)malloc(m_pitch);
	uint8_t* p_Up = &m_data[0];
	uint8_t* p_Down = &m_data[m_pitch * m_height - m_pitch];
	for (uint32_t i = 0; i < m_height / 2; ++i)
	{
		memcpy(line, p_Up, m_pitch);
		memcpy(p_Up, p_Down, m_pitch);
		memcpy(p_Down, line, m_pitch);
		p_Up += m_pitch;
		p_Down -= m_pitch;
	}
	free(line);
}

void npImage::FlipPixel()
{
	for (uint32_t i = 0; i < m_dataSize; )
	{
		uint8_t r = m_data[i];
		uint8_t g = m_data[i + 1];
		uint8_t b = m_data[i + 2];
		uint8_t a = m_data[i + 3];
		m_data[i] = b;
		m_data[i + 1] = g;
		m_data[i + 2] = r;
		m_data[i + 3] = a;
		i += 4;
	}
}

void npImage::Fill(const npColor& color)
{
	uint8_t* data = m_data;
	for (uint32_t h = 0; h < m_height; ++h)
	{
		for (uint32_t w = 0; w < m_width; ++w)
		{
			*data = color.GetAsByteRed();   ++data;
			*data = color.GetAsByteGreen(); ++data;
			*data = color.GetAsByteBlue();  ++data;
			*data = color.GetAsByteAlpha(); ++data;
		}
	}
}

void npImage::Fill(
	npImage* img,
	const npVec2u& where,
	npVec2i* outPosition)
{
	uint32_t srcW = img->m_width;
	uint32_t srcH = img->m_height;

	if (outPosition)
	{
		outPosition->x = where.x;
		outPosition->y = where.y;
	}
	rgba* rgbadataSrc = (rgba*)img->m_data;
	rgba* rgbadataDst = (rgba*)m_data;
	for (uint32_t yi = 0; yi < srcH; ++yi)
	{
		uint32_t whereY = where.y + yi;
		if (whereY >= this->m_height)
			continue;

		for (uint32_t xi = 0; xi < srcW; ++xi)
		{
			uint32_t whereX = where.x + xi;


			if (whereX >= this->m_width)
			{
				//break;
				continue;
			}


			uint32_t srcIndex = xi + (yi * srcW);
			uint32_t dstIndex = whereX + (whereY * this->m_width);

			if (rgbadataSrc[srcIndex].a == 0)
				continue;

			rgbadataDst[dstIndex] = rgbadataSrc[srcIndex];
		}
		if (whereY >= this->m_height)
		{
			continue;
			//	break;
		}
	}
}

void npImage::Fill(
	const uint8_t* indices,
	npColor* palette,
	const npVec2u& size,
	const npVec2u& where,
	npVec2i* outPosition)
{
	uint32_t srcW = size.x;
	uint32_t srcH = size.y;

	if (outPosition)
	{
		outPosition->x = where.x;
		outPosition->y = where.y;
	}

	rgba* rgbadataDst = (rgba*)m_data;

	for (uint32_t yi = 0; yi < srcH; ++yi)
	{
		uint32_t whereY = where.y + yi;
		if (whereY >= this->m_height)
			continue;

		for (uint32_t xi = 0; xi < srcW; ++xi)
		{
			uint32_t whereX = where.x + xi;


			if (whereX >= this->m_width)
			{
				//break;
				continue;
			}

			uint32_t srcIndex = xi + (yi * srcW);
			uint32_t dstIndex = whereX + (whereY * this->m_width);

			npColor clr = palette[indices[srcIndex]];

			rgbadataDst[dstIndex].r = clr.GetAsByteRed();
			rgbadataDst[dstIndex].g = clr.GetAsByteGreen();
			rgbadataDst[dstIndex].b = clr.GetAsByteBlue();
			rgbadataDst[dstIndex].a = clr.GetAsByteAlpha();
		}
		if (whereY >= this->m_height)
		{
			continue;
			//	break;
		}
	}
}

void npImage::Fill(const uint8_t* indices, npColor* palette,
	const npVec2u& size, npVec2u* wherePtr, npVec2i* outPosition)
{
	npVec2u where = *wherePtr;
	Fill(indices, palette, size, where, outPosition);
	wherePtr->x += size.x;
}
