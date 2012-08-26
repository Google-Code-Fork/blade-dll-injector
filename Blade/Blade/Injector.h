#pragma once
#include <Windows.h>
#include "Logger.h"

using namespace System;

ref class Injector
{
private:
	Logger^ logger;
	HANDLE hProcess, allocAddress, hRemoteThread;

public:
	Injector(void);

	// the DLL file must be in Environment::CurrentDirectory
	bool Inject(String^ processName, String^ dllName);
	bool Eject(String^ processName, String^ dllName);
	String^ GetLastErrorMessage();
};

