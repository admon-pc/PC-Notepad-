#pragma once
#include "npRenderer.h"

#include <gdiplus.h>

class npFont_gdi : public npFont
{
public:
	npFont_gdi();
	virtual ~npFont_gdi();
	virtual bool Init(const char*, bool bold, bool italic, int size) override;
	virtual npVec2f GetCharSize(wchar_t) override;
	virtual npVec2f GetFontSize() override;
	virtual float32_t GetTextLen(const wchar_t*, size_t sz) override;
	HFONT m_font;
	npVec2f m_size = 0;
};

class npRenderer_gdi : public npRenderer
{
	HBITMAP m_bitmapWindow;
	HDC m_bitmapWindowDC = 0;
	HRGN m_clipRegion;
	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR       m_gdiplusToken;
public:
	npRenderer_gdi();
	virtual ~npRenderer_gdi();
	
	virtual bool Init() override;
	virtual void OnWindowSize() override;

	virtual npTexture* CreateNPTexture(npImage*) override;

	virtual npFont* CreateNPFont(const char*, bool bold, bool italic, int size) override;
	virtual void SetNPFont(npFont*) override;
	virtual void SetClipRect(const npVec4f&) override;
	virtual void BeginDraw() override;
	virtual void EndDraw() override;
	virtual void DrawRectangle(const npVec4f&, const npColor& color) override;
	virtual void DrawText(const npchar*, size_t sz, float32_t x, float32_t y, const npColor& color) override;
	virtual void DrawImage(const npVec2f& where, const npVec2i& srcWhere, const npVec2i& srcSize, npTexture*) override;
	virtual void DrawImage(const npVec4f& whereRect, npTexture*) override;
};

