#include "notepad.h"


void npAssert::OnAssert(const char* message, const char* file, uint32_t line)
{
	printf("Assertion failed: %s, file %s, line %u\n", message, file, line);
}

void npAssert::OnAssertST(const char* message, const char* file, uint32_t line)
{
	printf("Assertion failed: %s, file %s, line %u\n", message, file, line);
}
