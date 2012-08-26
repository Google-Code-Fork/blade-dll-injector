#include "Injector.h"
#include "Logger.h"

using namespace System::Threading;

int main() 
{
	Injector^ injector = gcnew Injector();

	Logger::Instance->Log("Blade is starting.");

	injector->Inject("opera", "Poison.dll");

	Thread::Sleep(1000);

	injector->CallExport("opera", "Poison.dll", "Initialize");

	Thread::Sleep(1000);

	injector->Eject("opera", "Poison.dll");

	Logger::Instance->Log("Blade is closing.");
	return 0;
}

