#pragma once
class npAssert
{
public:
	static void OnAssert(const char* message, const char* file, uint32_t line);
	static void OnAssertST(const char* message, const char* file, uint32_t line);
};

#ifdef NP_DEBUG
#define NP_ASSERT(x)if(!!(x)==false){npAssert::OnAssert(#x, NP_FILE, (uint32_t)NP_LINE); NP_DEBUGBREAK;}
#define NP_ASSERT_ST(x)if(!!(x)==false){npAssert::OnAssertST(#x, NP_FILE, (uint32_t)NP_LINE); NP_DEBUGBREAK;}
#else
#define NP_ASSERT(x)((void)0)
#define NP_ASSERT_ST(x)((void)0)
#endif