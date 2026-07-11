#include "notepad.h"
#include "npTexture_gdi.h"

npTexture_gdi::npTexture_gdi()
{
}

npTexture_gdi::~npTexture_gdi()
{
	if (m_bitmap)
		delete m_bitmap;
}
