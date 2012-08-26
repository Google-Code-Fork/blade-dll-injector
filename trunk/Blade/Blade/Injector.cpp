#include <Windows.h>
#include "Injector.h"

#using <System.dll>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Runtime::InteropServices;

Injector::Injector(void)
{
}

String^ Injector::GetLastErrorMessage() 
{
	LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

	IntPtr ptr(lpMsgBuf);
	return Marshal:: PtrToStringUni(ptr);
}

bool Injector::Eject(String^ processName, String^ dllName)
{
	String^ dllPath = Environment::CurrentDirectory + "\\" + dllName;
	array<Process^>^ processes = Process::GetProcessesByName(processName);

	Int32 processId = 0;
	if (processes->Length > 0)
	{
		processId = processes[0]->Id;
		Logger::Instance->Log("Process id of " +  processName + " is " + processId + ".");
	}
	else
	{
		Logger::Instance->Log("No processes named " +  processName + " found.");
		return false;
	}

	ProcessModule^ processModule = nullptr;
	System::Collections::IEnumerator^ module = processes[0]->Modules->GetEnumerator();
	while (module->MoveNext())
	{
		Logger::Instance->Log("Loaded module " + dynamic_cast<ProcessModule^>(module->Current)->FileName  + ".");

		if (dllPath == static_cast<ProcessModule^>(module->Current)->FileName)
		{
			Logger::Instance->Log("DLL " + dllPath + " found. Will remove it.");
			processModule = static_cast<ProcessModule^>(module->Current);
		}
	}

	if (processModule == nullptr)
	{
		Logger::Instance->Log("No DLL " + dllPath + " found. Nothing to do.");
		return false;
	}

	Logger::Instance->Log("Openning process.");
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
		
	if (hProcess == NULL)
	{
		Logger::Instance->Log("Error while openning process with message '" + GetLastErrorMessage() + "'.");
		return false;
	}

	Logger::Instance->Log("Unloading DLL remotely.");
	hRemoteThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE) GetProcAddress(GetModuleHandle(L"kernel32.dll"), "FreeLibrary"), (void*)processModule->BaseAddress, 0, 0);

	if (hRemoteThread == NULL)
	{
		Logger::Instance->Log("Error while calling remote thread with message '" + GetLastErrorMessage() + "'.");
		return false;
	}

	Logger::Instance->Log("Waiting.");
	WaitForSingleObject(hRemoteThread, INFINITE);

	Logger::Instance->Log("Closing.");
	CloseHandle(hProcess);
}

bool Injector::Inject(String^ processName, String^ dllName) 
{
	String^ dllPath = Environment::CurrentDirectory + "\\" + dllName;
	array<Process^>^ processes = Process::GetProcessesByName(processName);

	Int32 processId = 0;
	if (processes->Length > 0)
	{
		processId = processes[0]->Id;
		Logger::Instance->Log("Process id of " +  processName + " is " + processId + ".");

	}
	else
	{
		Logger::Instance->Log("No processes named " +  processName + " found.");
		return false;
	}

	System::Collections::IEnumerator^ module = processes[0]->Modules->GetEnumerator();
	while (module->MoveNext())
	{
		Logger::Instance->Log("Loaded module " + static_cast<ProcessModule^>(module->Current)->FileName  + ".");

		if (dllPath == static_cast<ProcessModule^>(module->Current)->FileName)
		{
			Logger::Instance->Log("DLL " + dllPath + " already injected.");
			return false;
		}
	} 

	Logger::Instance->Log("Openning process.");
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processId);

	if (hProcess == NULL)
	{
		Logger::Instance->Log("Error while openning process with message '" + GetLastErrorMessage() + "'.");
		return false;
	}

	Logger::Instance->Log("Allocating memory for " + dllPath + ".");
	allocAddress = VirtualAllocEx(hProcess, 0, (dllPath->Length)*sizeof(wchar_t), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (allocAddress == NULL)
	{
		Logger::Instance->Log("Error while allocating memory with message '" + GetLastErrorMessage() + "'.");
		return false;
	}

	Logger::Instance->Log("Writing path to remote process.");
	if (WriteProcessMemory(hProcess, (void*)(allocAddress), (void*)Marshal::StringToHGlobalUni(dllPath), (dllPath->Length)*sizeof(wchar_t), 0) == 0)
	{
		Logger::Instance->Log("Error while writing path to remote process with message '" + GetLastErrorMessage() + "'.");
		return false;
	}

	Logger::Instance->Log("Loading DLL remotely.");
	hRemoteThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE) GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW"), allocAddress, 0, 0);

	if (hRemoteThread == NULL)
	{
		Logger::Instance->Log("Error while calling remote thread with message '" + GetLastErrorMessage() + "'.");
		return false;
	}

	Logger::Instance->Log("Waiting.");
	WaitForSingleObject(hRemoteThread, INFINITE);

	Logger::Instance->Log("Freeing.");
	VirtualFreeEx(hProcess, allocAddress, (dllPath->Length)*sizeof(wchar_t), MEM_DECOMMIT);

	Logger::Instance->Log("Closing.");
	CloseHandle(hProcess);

	return false;
}
