#include "Injector.h"
#include "Logger.h"

using namespace System::Threading;

int main() 
{
	Injector^ injector = gcnew Injector();

	Logger::Instance->Log("Blade is starting.");

	injector->Inject("iexplore", "Poison.dll");

	Thread::Sleep(1000);

	injector->CallExport("iexplore", "Poison.dll", "Initialize");

	Thread::Sleep(1000);

	injector->Eject("iexplore", "Poison.dll");

	Logger::Instance->Log("Blade is closing.");
	return 0;
}

