#pragma once

#ifdef _WIN32
#define NP_PLATFORM_WIN32
#else
#error This platform is not supported
#endif

#ifdef _MSC_VER
#define NP_FORCE_INLINE __forceinline
#define NP_CDECL _cdecl
#ifdef NP_EXPORT
#define NP_API _declspec(dllexport)
#else
#define NP_API _declspec(dllimport)
#endif
#else
#define NP_FORCE_INLINE inline
#define NP_API
#define NP_CDECL
#endif

#ifdef NP_PLATFORM_WIN32
#ifdef _WIN64
#define NP_PLATFORMBIT64
#endif
#else
#error This platform is not supported
#endif

#ifdef _DEBUG
#define NP_DEBUG
#endif

#ifdef NP_DEBUG
#define NP_DEBUGBREAK __debugbreak();
#else
#define NP_DEBUGBREAK
#endif

#define NP_FILE __FILE__
#define NP_LINE __LINE__
#define NP_FUNCTION __FUNCTION__

#define NP_QUOTE(s) #s

using float32_t = float;
using float64_t = double;
using real_t = float64_t;

#define NP_MAKEFOURCC( ch0, ch1, ch2, ch3 )\
	((uint32_t)(uint8_t)(ch0)|((uint32_t)(uint8_t)(ch1)<<8)|\
	((uint32_t)(uint8_t)(ch2)<<16)|((uint32_t)(uint8_t)(ch3)<<24))

#ifndef BIT
#define BIT(x)0x1<<(x)
#endif

