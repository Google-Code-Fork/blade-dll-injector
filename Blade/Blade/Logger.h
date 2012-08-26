#pragma once

using namespace System;
using namespace System::IO;

ref class Logger
{
private:
	Logger();
	static Logger^ instance = gcnew Logger();

	String^ logFile;
	StreamWriter^ streamWriter;

public:
	void Log(String^ message);
	static property Logger^ Instance { Logger^ get() { return instance; } }
};

