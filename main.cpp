/*
123456789ABCDEFG
0000000000000000
	000000000000	0000
0	000000000000000
00	00000000000000
000	0000000000000
0	00			0000000000000
	000	0000000000000
000	0000000000000
*/
#include "notepad.h"

int __stdcall WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd)
{
	bool scondWindow = false;

	HANDLE hMutex = CreateMutex(NULL, FALSE, L"Global\\NOTPADPPMutex");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		scondWindow = true;
		//LPWSTR cmdLine = GetCommandLineW();
		//COPYDATASTRUCT cds;
		//cds.dwData = 1; // Custom identifier code for your app
		//cds.cbData = (lstrlenW(cmdLine) + 1) * sizeof(WCHAR); // Size in bytes
		//cds.lpData = cmdLine; // Pointer to the data
		//
		//HWND hPrevWnd = FindWindow(L"notepadcls", NULL);
		//if (hPrevWnd) 
		//{
		//	// SendMessage blocks until the first instance receives and processes it
		//	SendMessage(hPrevWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);


		//	// If minimized, restore it
		//	if (IsIconic(hPrevWnd)) {
		//		ShowWindow(hPrevWnd, SW_RESTORE);
		//	}
		//	// Bring it to the front and focus it
		//	SetForegroundWindow(hPrevWnd);
		//}
		//return 0;
	}

	bool allocConsole = true;

	if (allocConsole)
	{
		AllocConsole();

		FILE* f = 0;
		freopen_s(&f, "CONOUT$", "w", stdout);
		printf("%s : %s\n", __DATE__, __TIME__);
	}
	notepad app;
	if (app.Init(scondWindow))
	{
		app.Run();
	}
	
	if (allocConsole)
	{
		FreeConsole();
	}
	CloseHandle(hMutex);

	return 1;
}

//int main()
//{
//
//	notepad app;
//	if (app.Init())
//	{
//		app.Run();
//	}
//
//	return 1;
//}