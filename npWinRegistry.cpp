#include "notepad.h"

npWinRegistry::npWinRegistry()
{
}

npWinRegistry::npWinRegistry(const char* s, npWinRegistry* opt)
{
	HKEY parent_key = HKEY_CURRENT_USER;
	if (opt)
		parent_key = opt->m_key;

	auto err = RegCreateKeyExA(
		parent_key,
		s,
		0, 0, 0,
		KEY_ALL_ACCESS,
		0,
		&m_key,
		0);
	if (err != ERROR_SUCCESS)
		printf("npWinRegistry err: %u\n", err);
}
npWinRegistry::npWinRegistry(const wchar_t* s, npWinRegistry* opt)
{
	HKEY parent_key = HKEY_CURRENT_USER;
	if (opt)
		parent_key = opt->m_key;

	auto err = RegCreateKeyExW(
		parent_key,
		s,
		0, 0, 0,
		KEY_ALL_ACCESS,
		0,
		&m_key,
		0);
	if (err != ERROR_SUCCESS)
		printf("npWinRegistry err: %u\n", err);
}

npWinRegistry::~npWinRegistry()
{
	RegCloseKey(m_key);
	m_key = 0;
}

void npWinRegistry::DeleteTree()
{
	if(m_key)
		RegDeleteTreeW(m_key, 0);
}

npWinRegistry* npWinRegistry::CreateKey(const char* s)
{
	NP_ASSERT_ST(s);
	npWinRegistry* subKey = new npWinRegistry(s, this);
	if (!subKey->IsGood())
	{
		delete subKey;
		subKey = 0;
	}
	return subKey;
}

npWinRegistry* npWinRegistry::CreateKey(const wchar_t* s)
{
	NP_ASSERT_ST(s);
	npWinRegistry* subKey = new npWinRegistry(s, this);
	if (!subKey->IsGood())
	{
		delete subKey;
		subKey = 0;
	}
	return subKey;
}

npWinRegistry* npWinRegistry::OpenKey(const char* s)
{
	npWinRegistry* subKey = new npWinRegistry();
	RegOpenKeyExA(
		m_key,
		s,
		0,
		KEY_ALL_ACCESS,
		&subKey->m_key);
	if (!subKey->IsGood())
	{
		delete subKey;
		subKey = 0;
	}
	return subKey;
}

npWinRegistry* npWinRegistry::OpenKey(const wchar_t* s)
{
	npWinRegistry* subKey = new npWinRegistry();
	RegOpenKeyExW(
		m_key,
		s,
		0,
		KEY_ALL_ACCESS,
		&subKey->m_key);
	if (!subKey->IsGood())
	{
		delete subKey;
		subKey = 0;
	}
	return subKey;
}


void npWinRegistry::SetValue(const char* name, const char* value)
{
	RegSetValueExA(
		m_key,
		name,
		0,
		REG_SZ,
		(const BYTE*)value,
		(strlen(value) + 1));
}

void npWinRegistry::SetValue(const wchar_t* name, const wchar_t* value)
{
	RegSetValueExW(
		m_key,
		name,
		0,
		REG_SZ,
		(const BYTE*)value,
		(wcslen(value) + 1) * sizeof(wchar_t));
}
void npWinRegistry::SetValue(const wchar_t* s, int v)
{
	wchar_t wbuf[50];
	wsprintfW(wbuf, L"%i", v);
	RegSetValueExW(
		m_key,
		s,
		0,
		REG_SZ,
		(const BYTE*)wbuf,
		(wcslen(wbuf) + 1) * sizeof(wchar_t));
}

void npWinRegistry::GetValue(const char* s, npStringA* out)
{
	char buf[201];
	memset(buf, 0, 201);
	DWORD sz = 200;
	LSTATUS err = RegGetValueA(m_key,0, s, RRF_RT_ANY, 0, buf, &sz);
	if (buf[0])
		out->assign(buf);
}

void npWinRegistry::GetValue(const wchar_t* s, npStringW* out)
{
	wchar_t buf[201];
	memset(buf, 0, 201 * sizeof(wchar_t));
	DWORD sz = 200 * sizeof(wchar_t);
	LSTATUS err = RegGetValueW(m_key, 0, s, RRF_RT_ANY, 0, buf, &sz);
	if (buf[0])
		out->assign(buf);
}
void npWinRegistry::GetValue(const wchar_t* s, int* out)
{
	wchar_t buf[201];
	memset(buf, 0, 201 * sizeof(wchar_t));
	DWORD sz = 200 * sizeof(wchar_t);
	LSTATUS err = RegGetValueW(m_key, 0, s, RRF_RT_ANY, 0, buf, &sz);
	*out = _wtoi(buf);
}

void npWinRegistry::GetValue(const wchar_t* s, size_t* out)
{
	wchar_t buf[201];
	memset(buf, 0, 201 * sizeof(wchar_t));
	DWORD sz = 200 * sizeof(wchar_t);
	LSTATUS err = RegGetValueW(m_key, 0, s, RRF_RT_ANY, 0, buf, &sz);
	*out = _wtoi64(buf);
}

