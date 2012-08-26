#include "Injector.h"
#include "Logger.h"

int main() 
{
	Injector^ injector = gcnew Injector();

	Logger::Instance->Log("Blade is starting.");

	injector->Inject("iexplore", "Poison.dll");

	Logger::Instance->Log("Blade is closing.");
	return 0;
}

