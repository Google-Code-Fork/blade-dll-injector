#include <Windows.h>

BOOL WINAPI DllMain(HMODULE, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// do something later    
	}
	return TRUE;
}

extern "C" __declspec(dllexport) void Initialize()
{
    MessageBox(0, L"Injection was successful!", L"Poison", 0);
}