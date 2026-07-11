#pragma once

class npColor
{
public:
	npColor() {}
	~npColor() {}
	npColor(uint32_t uint_data)
	{
		SetAsInteger(uint_data);
	}

	float r = 0.f;
	float g = 0.f;
	float b = 0.f;
	float a = 1.f;

	const uint8_t GetAsByteRed() const { return static_cast<uint8_t>(r * 255.); }
	const uint8_t GetAsByteGreen() const { return static_cast<uint8_t>(g * 255.); }
	const uint8_t GetAsByteBlue() const { return static_cast<uint8_t>(b * 255.); }
	const uint8_t GetAsByteAlpha() const { return static_cast<uint8_t>(a * 255.); }

	void SetAsByteAlpha(int32_t v) { a = static_cast<float>(v) * 0.00392156862745f; }
	void SetAsByteRed(int32_t v) { r = static_cast<float>(v) * 0.00392156862745f; }
	void SetAsByteGreen(int32_t v) { g = static_cast<float>(v) * 0.00392156862745f; }
	void SetAsByteBlue(int32_t v) { b = static_cast<float>(v) * 0.00392156862745f; }

	void SetAsInteger(uint32_t v)
	{
		this->SetAsByteRed(static_cast<uint8_t>(v >> 16u));
		this->SetAsByteGreen(static_cast<uint8_t>(v >> 8u));
		this->SetAsByteBlue(static_cast<uint8_t>(v));
		this->SetAsByteAlpha(static_cast<uint8_t>(v >> 24u));
	}
};

