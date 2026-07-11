#include "notepad.h"
#include "npRenderer.h"
#include "npRenderer_gdi.h"
#include "npTexture_gdi.h"

extern notepad* g_app;


npFont_gdi::npFont_gdi()
{
}

npFont_gdi::~npFont_gdi()
{
	DeleteObject(m_font);
}

bool npFont_gdi::Init(const char* f, bool bold, bool italic, int size)
{
	m_font = CreateFontA(
		-MulDiv(size, GetDeviceCaps(g_app->m_dc, 
		LOGPIXELSY), 72), 
		0, 0, 0,
		bold ? FW_BOLD : 0,
		italic, 0, 0,
		ANSI_CHARSET, 0, 0,
		DEFAULT_QUALITY,
		0, f);
	m_size = GetCharSize(L'_');
	return true;
}



npVec2f npFont_gdi::GetFontSize()
{
	return m_size;
}

npVec2f npFont_gdi::GetCharSize(npchar c)
{
	npVec2f sz;

	SelectObject(g_app->m_dc, m_font);
	SIZE size;
	GetTextExtentPoint32W(g_app->m_dc, &c, 1, &size);

	sz.x = (float)size.cx;
	sz.y = (float)size.cy;

	return sz;
}

float32_t npFont_gdi::GetTextLen(const wchar_t* text, size_t sz)
{
	float32_t len = .0f;
	if (text && sz)
	{
		SelectObject(g_app->m_dc, m_font);
		SIZE size;
		GetTextExtentPoint32W(g_app->m_dc, text, sz, &size);
		len = (float)size.cx;
	}
	return len;
}


npRenderer_gdi::npRenderer_gdi()
{

}

npRenderer_gdi::~npRenderer_gdi()
{
	DeleteObject(m_clipRegion);
	DeleteDC(m_bitmapWindowDC);
	DeleteObject(m_bitmapWindow);
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

bool npRenderer_gdi::Init()
{
	m_bitmapWindow = CreateCompatibleBitmap(g_app->m_dc, 300, 300);
	m_bitmapWindowDC = CreateCompatibleDC(g_app->m_dc);
	m_clipRegion = CreateRectRgn(
		0,
		0,
		10,
		10);
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
	SetBkMode(m_bitmapWindowDC, TRANSPARENT);
	return true;
}

void npRenderer_gdi::OnWindowSize()
{
	if (g_app->m_clientSize.x && g_app->m_clientSize.y)
	{
		DeleteObject(m_bitmapWindow);
		m_bitmapWindow = CreateCompatibleBitmap(g_app->m_dc, g_app->m_clientSize.x, g_app->m_clientSize.y);
	}
}

npTexture* npRenderer_gdi::CreateNPTexture(npImage* img)
{
	npTexture_gdi* gditexture = new npTexture_gdi;

	gditexture->m_bitmap = new Gdiplus::Bitmap((INT)img->m_width, (INT)img->m_height, PixelFormat32bppARGB);
	Gdiplus::Rect r(0, 0, img->m_width, img->m_height);
	Gdiplus::BitmapData bmpData;
	if (gditexture->m_bitmap->LockBits(&r, Gdiplus::ImageLockModeWrite,
		PixelFormat32bppARGB, &bmpData) == Gdiplus::Ok)
	{
		uint8_t* src = img->m_data;
		uint8_t* dst = (uint8_t*)bmpData.Scan0;
		for (uint32_t i = 0; i < img->m_height; ++i)
		{
			memcpy(dst, src, img->m_pitch);
			dst += img->m_pitch;
			src += img->m_pitch;
		}
		gditexture->m_bitmap->UnlockBits(&bmpData);
	}

	return gditexture;
}

void npRenderer_gdi::SetNPFont(npFont* f)
{
	npFont_gdi* font = (npFont_gdi*)f;
	
	SelectObject(m_bitmapWindowDC, font->m_font);
	SetBkMode(m_bitmapWindowDC, TRANSPARENT);
}

void npRenderer_gdi::SetClipRect(const npVec4f& rect)
{
	DeleteObject(m_clipRegion);
	m_clipRegion = CreateRectRgn(
		(int)rect.x,
		(int)rect.y,
		(int)rect.z,
		(int)rect.w);
	SelectClipRgn(m_bitmapWindowDC, m_clipRegion);
}

void npRenderer_gdi::BeginDraw()
{
	SelectObject(m_bitmapWindowDC, m_bitmapWindow);
}

void npRenderer_gdi::EndDraw()
{
	static int cc = 0;
	if (cc > 1)
	{
		BitBlt(g_app->m_dc, 0, 0, g_app->m_clientSize.x,
			g_app->m_clientSize.y, m_bitmapWindowDC, 0, 0, SRCCOPY);
		cc = 0;
	}
	++cc;

	/*StretchBlt(g_app->m_dc, 0, 0, g_app->m_clientSize.x,
		g_app->m_clientSize.y, m_bitmapWindowDC, 0, 0, g_app->m_clientSize.x,
		g_app->m_clientSize.y, SRCCOPY);*/
}


void npRenderer_gdi::DrawRectangle(const npVec4f& rect, const npColor& color)
{
	SelectObject(m_bitmapWindowDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(m_bitmapWindowDC, RGB(color.GetAsByteRed(), color.GetAsByteGreen(), color.GetAsByteBlue()));

	SelectObject(m_bitmapWindowDC, GetStockObject(DC_PEN));
	SetDCPenColor(m_bitmapWindowDC, RGB(color.GetAsByteRed(), color.GetAsByteGreen(), color.GetAsByteBlue()));

	Rectangle(m_bitmapWindowDC, (int)rect.x, (int)rect.y, (int)rect.z, (int)rect.w);
}

void npRenderer_gdi::DrawText(const npchar* text, size_t sz, float32_t x, float32_t y, const npColor& color)
{
	SetTextColor(m_bitmapWindowDC, RGB(color.GetAsByteRed(), color.GetAsByteGreen(), color.GetAsByteBlue()));
	TextOutW(m_bitmapWindowDC, (int)x, (int)y, text, sz);
}

npFont* npRenderer_gdi::CreateNPFont(const char* f, bool bold, bool italic, int size)
{
	npFont_gdi* fnt = new npFont_gdi;
	if (!fnt->Init(f, bold, italic, size))
	{
		delete fnt;
		fnt = 0;
	}
	return fnt;
}

void npRenderer_gdi::DrawImage(const npVec2f& where, const npVec2i& srcWhere, const npVec2i& srcSize, npTexture* texture)
{
	npTexture_gdi* gditexture = (npTexture_gdi*)texture;

	Gdiplus::Graphics g(m_bitmapWindowDC);
	g.DrawImage(gditexture->m_bitmap,
		Gdiplus::Rect(
			(INT)where.x,
			(INT)where.y,
			srcSize.x,
			srcSize.y),
		srcWhere.x,
		srcWhere.y,
		srcSize.x,
		srcSize.y,
		Gdiplus::Unit::UnitPixel);
}

void npRenderer_gdi::DrawImage(const npVec4f& whereRect, npTexture* t)
{
	npTexture_gdi* gditexture = (npTexture_gdi*)t;

	Gdiplus::Graphics g(m_bitmapWindowDC);
	g.DrawImage(gditexture->m_bitmap,
		Gdiplus::Rect(
			(int)whereRect.x,
			(int)whereRect.y,
			(int)whereRect.z,
			(int)whereRect.w));
}
