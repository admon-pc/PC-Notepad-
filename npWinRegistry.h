#pragma once
class npWinRegistry
{
public:
	npWinRegistry();
	npWinRegistry(const char*, npWinRegistry* opt);
	npWinRegistry(const wchar_t*, npWinRegistry* opt);
	~npWinRegistry();

	void DeleteTree();
	npWinRegistry* CreateKey(const char*);
	npWinRegistry* CreateKey(const wchar_t*);

	npWinRegistry* OpenKey(const char*);
	npWinRegistry* OpenKey(const wchar_t*);

	void SetValue(const char*, const char*);
	void SetValue(const wchar_t*, const wchar_t*);
	void SetValue(const wchar_t*, int);

	void GetValue(const char*, npStringA*);
	void GetValue(const wchar_t*, npStringW*);
	void GetValue(const wchar_t*, int*);
	void GetValue(const wchar_t*, size_t*);

	bool IsGood() { return m_key != 0; }
	HKEY m_key = 0;
};

