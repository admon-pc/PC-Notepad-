#pragma once

#ifdef DrawText
#undef DrawText
#endif

class npFont
{
public:
	npFont() {}
	virtual ~npFont() {}

	virtual bool Init(const char*, bool bold, bool italic, int size) = 0;
	virtual npVec2f GetCharSize(wchar_t) = 0;
	virtual float32_t GetTextLen(const wchar_t*, size_t sz) = 0;
	virtual npVec2f GetFontSize() = 0;
};

class npRenderer
{
public:
	npRenderer() {}
	virtual ~npRenderer() {}
	
	virtual bool Init() = 0;
	virtual void OnWindowSize() = 0;
	virtual npTexture* CreateNPTexture(npImage*) = 0;
	virtual npFont* CreateNPFont(const char*, bool bold, bool italic, int size) = 0;
	virtual void SetNPFont(npFont*) = 0;
	virtual void SetClipRect(const npVec4f&) = 0;
	virtual void BeginDraw() = 0;
	virtual void EndDraw() = 0;
	virtual void DrawRectangle(const npVec4f&, const npColor& color) = 0;
	virtual void DrawText(const npchar*, size_t sz, float32_t x, float32_t y, const npColor& color) = 0;
	virtual void DrawImage(const npVec2f& where, const npVec2i& srcWhere, const npVec2i& srcSize, npTexture*) = 0;
	virtual void DrawImage(const npVec4f& whereRect, npTexture*) = 0;
};

