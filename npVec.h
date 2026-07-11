#pragma once

template<typename Type>
class npVec2_t
{
public:
	npVec2_t<Type>() {}
	npVec2_t<Type>(Type _x, Type _y) :x(_x), y(_y) {}
	npVec2_t<Type>(Type val) : x(val), y(val) {}
	Type x = 0;
	Type y = 0;

	void Set(Type _x, Type _y) { x = _x; y = _y; }
	npVec2_t<Type> operator*(const npVec2_t<Type>& v)const { npVec2_t<Type> r; r.x = x * v.x; r.y = y * v.y; return r; }
	npVec2_t<Type> operator-(const npVec2_t<Type>& v)const { npVec2_t<Type> r; r.x = x - v.x; r.y = y - v.y; return r; }
	npVec2_t<Type> operator+(const npVec2_t<Type>& v)const { npVec2_t<Type> r; r.x = x + v.x; r.y = y + v.y; return r; }
	npVec2_t<Type> operator/(const npVec2_t<Type>& v)const { npVec2_t<Type> r; r.x = x / v.x; r.y = y / v.y; return r; }

	void operator+=(const npVec2_t<Type>& v) { x += v.x; y += v.y; }
	void operator-=(const npVec2_t<Type>& v) { x -= v.x; y -= v.y; }
	void operator*=(const npVec2_t<Type>& v) { x *= v.x; y *= v.y; }
	void operator/=(const npVec2_t<Type>& v) { x /= v.x; y /= v.y; }
	bool operator==(const npVec2_t<Type>& v)const { if (x != v.x)return false; if (y != v.y)return false; return true; }
	Type Distance(const npVec2_t<Type>& from)const { return npVec2_t<Type>(x - from.x, y - from.y).Length(); }
	Type Length()const { return sqrt(LengthSqrt()); }
	Type LengthSqrt()const { return(x * x) + (y * y); }
	Type	Dot()const { return (x * x) + (y * y); }
	void	Normalize2()
	{
		Type len = sqrt(Dot());
		if (len > 0)
			len = 1.0f / len;
		x *= len; y *= len;
	}
	npVec2_t<Type> operator*(Type v)const { npVec2_t<Type> r; r.x = x * v; r.y = y * v; return r; }
};

template<typename Type>
class npVec3_t
{
public:
	npVec3_t<Type>() {}
	//npVec3_t<Type>(const npVec4_t<Type>& v);// : x(v.x), y(v.y), z(v.z) {}
	npVec3_t<Type>(Type _v) :x(_v), y(_v), z(_v) {}
	npVec3_t<Type>(Type _x, Type _y, Type _z) : x(_x), y(_y), z(_z) {}
	Type x = 0;
	Type y = 0;
	Type z = 0;

	void Set(Type _x, Type _y, Type _z) { x = _x; y = _y; z = _z; }
	void Set(Type val) { x = y = z = val; }
	Type* data() { return &x; }
	Type Distance(const npVec3_t<Type>& from)const { return npVec3_t<Type>(x - from.x, y - from.y, z - from.z).Length(); }
	//Type Distance(const npVec4_t<Type>& from)const { return npVec3_t<Type>(x - from.x, y - from.y, z - from.z).Length(); }
	void operator+=(const npVec3_t<Type>& v) { x += v.x; y += v.y; z += v.z; }
	void operator-=(const npVec3_t<Type>& v) { x -= v.x; y -= v.y; z -= v.z; }
	void operator*=(const npVec3_t<Type>& v) { x *= v.x; y *= v.y; z *= v.z; }
	void operator/=(const npVec3_t<Type>& v) { x /= v.x; y /= v.y; z /= v.z; }
	Type operator[](int32_t index)const { assert((index >= 0) && (index < 3)); return (&x)[index]; }
	Type& operator[](int32_t index) { assert((index >= 0) && (index < 3)); return (&x)[index]; }
	npVec3_t<Type> operator+(const npVec3_t<Type>& v)const { npVec3_t<Type> r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; return r; }
	npVec3_t<Type> operator-(const npVec3_t<Type>& v)const { npVec3_t<Type> r; r.x = x - v.x; r.y = y - v.y; r.z = z - v.z; return r; }
	npVec3_t<Type> operator*(const npVec3_t<Type>& v)const { npVec3_t<Type> r; r.x = x * v.x; r.y = y * v.y; r.z = z * v.z; return r; }
	npVec3_t<Type> operator/(const npVec3_t<Type>& v)const { npVec3_t<Type> r; r.x = x / v.x; r.y = y / v.y; r.z = z / v.z; return r; }
	//npVec3_t<Type> operator+(const npVec4_t<Type>& v)const { npVec3_t<Type> r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; return r; }
	//npVec3_t<Type>& operator=(const npVec4_t<Type>& v) { x = v.x; y = v.y; z = v.z; return *this; }
	//void operator+=(const npVec4_t<Type>& v) { x += v.x; y += v.y; z += v.z; }
	bool operator==(const npVec3_t<Type>& v)const {
		if (x != v.x)return false;
		if (y != v.y)return false;
		if (z != v.z)return false;
		return true;
	}
	bool operator!=(const npVec3_t<Type>& v)const {
		if (x != v.x)return true;
		if (y != v.y)return true;
		if (z != v.z)return true;
		return false;
	}
	npVec3_t<Type> Cross(const npVec3_t<Type>& a)const {
		npVec3_t<Type> out;
		out.x = (y * a.z) - (z * a.y);
		out.y = (z * a.x) - (x * a.z);
		out.z = (x * a.y) - (y * a.x);
		return out;
	}
	void Cross2(const npVec3_t<Type>& a, npVec3_t<Type>& out)const {
		out.x = (y * a.z) - (z * a.y);
		out.y = (z * a.x) - (x * a.z);
		out.z = (x * a.y) - (y * a.x);
	}
	void	Normalize2()
	{
		Type len = sqrt(Dot());
		if (len > 0)
			len = 1.0f / len;
		x *= len; y *= len; z *= len;
	}
	Type	Dot(const npVec3_t<Type>& V2)const { return (x * V2.x) + (y * V2.y) + (z * V2.z); }
	Type	Dot()const { return (x * x) + (y * y) + (z * z); }
	npVec3_t<Type> operator-()const { npVec3_t<Type> r; r.x = -x; r.y = -y; r.z = -z; return r; }
	Type Length() const { return sqrt(Length2()); }
	Type Length2() const { return Dot(*this); }

	//void add(const npVec4_t<Type>&);
};

template<typename Type>
class npVec4_t
{
public:
	npVec4_t<Type>() {}
	npVec4_t<Type>(Type _v) :x(_v), y(_v), z(_v), w(_v) {}
	npVec4_t<Type>(Type _x, Type _y, Type _z, Type _w) : x(_x), y(_y), z(_z), w(_w) {}
	npVec4_t<Type>(const npVec3_t<float32_t>& _v) : x(_v.x), y(_v.y), z(_v.z), w(0) {}
	Type x = 0;
	Type y = 0;
	Type z = 0;
	Type w = 0;

	Type* Data() { return &x; }
	Type operator[](int32_t index)const { assert((index >= 0) && (index < 4)); return (&x)[index]; }
	Type& operator[](int32_t index) { assert((index >= 0) && (index < 4)); return (&x)[index]; }

	void Set(Type _x, Type _y, Type _z, Type _w) { x = _x; y = _y; z = _z; w = _w; }
	void Set(Type val) { x = y = z = w = val; }
	void operator+=(const npVec4_t<Type>& v) { x += v.x; y += v.y; z += v.z; w += v.w; }
	void operator-=(const npVec4_t<Type>& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; }
	void operator*=(const npVec4_t<Type>& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; }
	void operator/=(const npVec4_t<Type>& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; }

	npVec4_t<Type> operator+(const npVec4_t<Type>& v)const { npVec4_t<Type> r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; r.w = w + v.w; return r; }
	npVec4_t<Type> operator-(const npVec4_t<Type>& v)const { npVec4_t<Type> r; r.x = x - v.x; r.y = y - v.y; r.z = z - v.z; r.w = w - v.w; return r; }
	npVec4_t<Type> operator*(const npVec4_t<Type>& v)const { npVec4_t<Type> r; r.x = x * v.x; r.y = y * v.y; r.z = z * v.z; r.w = w * v.w; return r; }
	npVec4_t<Type> operator/(const npVec4_t<Type>& v)const { npVec4_t<Type> r; r.x = x / v.x; r.y = y / v.y; r.z = z / v.z; r.w = w / v.w; return r; }

	npVec4_t<Type> operator-()const { npVec4_t<Type> r; r.x = -x; r.y = -y; r.z = -z; r.w = -w; return r; }

	//npVec4_t<Type> operator+(const npVec4_t<float64_t>& v)const { npVec4_t<Type> r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; r.w = w + v.w; return r; }

	Type	Dot(const npVec4_t<Type>& V2)const { return (x * V2.x) + (y * V2.y) + (z * V2.z) + (w * V2.w); }
	Type	Dot()const { return (x * x) + (y * y) + (z * z) + (w * w); }
	void	Normalize2()
	{
		Type len = sqrt(Dot());
		if (len > 0)
			len = 1.0f / len;
		x *= len; y *= len; z *= len; w *= len;
	}
	void Cross2(const npVec4_t<Type>& a, npVec4_t<Type>& out)const {
		out.x = (y * a.z) - (z * a.y);
		out.y = (z * a.x) - (x * a.z);
		out.z = (x * a.y) - (y * a.x);
	}
	Type LengthSqrt()const { return(x * x) + (y * y) + (z * z); }
	Type Length()const { return sqrt(LengthSqrt()); }
	Type Distance(const npVec4_t<Type>& from)const { return npVec4_t<Type>(x - from.x, y - from.y, z - from.z, 1.f).Length(); }
	bool operator==(const npVec4_t<Type>& v)const {
		if (x != v.x)return false;
		if (y != v.y)return false;
		if (z != v.z)return false;
		if (w != v.w)return false;
		return true;
	}
};
inline npVec4_t<float32_t> operator*(const float32_t& s, const npVec4_t<float32_t>& v) { return v * s; }
inline npVec4_t<float64_t> operator*(const float64_t& s, const npVec4_t<float64_t>& v) { return v * s; }
inline npVec4_t<float64_t> operator+(const npVec4_t<float64_t>& s, const npVec4_t<float32_t>& v)
{
	npVec4_t<float64_t> r;
	r.x = s.x + v.x;
	r.y = s.y + v.y;
	r.z = s.z + v.z;
	r.w = s.w + v.w;
	return r;
}
inline npVec4_t<float32_t> operator-(const npVec4_t<float32_t>& s, const npVec4_t<float64_t>& v)
{
	npVec4_t<float32_t> r;
	r.x = s.x - (float32_t)v.x;
	r.y = s.y - (float32_t)v.y;
	r.z = s.z - (float32_t)v.z;
	r.w = s.w - (float32_t)v.w;
	return r;
}

using npVec2f = npVec2_t<float32_t>;
using npVec2i = npVec2_t<int32_t>;
using npVec2u = npVec2_t<uint32_t>;
using npVec3f = npVec3_t<float32_t>;
using npVec3i = npVec3_t<int32_t>;
using npVec3u = npVec3_t<uint32_t>;
using npVec4f = npVec4_t<float32_t>;
using npVec4i = npVec4_t<int32_t>;
using npVec4u = npVec4_t<uint32_t>;

