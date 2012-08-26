#include "Logger.h"

using namespace System;
using namespace System::IO;

Logger::Logger(void)
{
	logFile = Environment::CurrentDirectory + "\\Blade.log";
	streamWriter = gcnew StreamWriter(logFile, true);	
}

void Logger::Log(String^ message) 
{
	streamWriter->WriteLine(DateTime::Now.ToString("yyyy-MM-dd HH:mm:ss:") + DateTime::Now.Millisecond + " " + message);
	streamWriter->Flush();

	// I want to see this in output window
	Console::WriteLine(DateTime::Now.ToString("yyyy-MM-dd HH:mm:ss:") + DateTime::Now.Millisecond + " " + message);
}