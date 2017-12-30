// target.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	std::string value;
	while (true)
	{
		// Output the current process Id
        HANDLE currentThread = GetCurrentThread();
		std::cout << "Target.exe process id: ";
		std::cout << GetProcessIdOfThread(currentThread);
		std::cout << "\n";
		CloseHandle(currentThread);
        
        std::cout << "Press <enter> to Beep (Ctrl-C to exit): ";
		std::getline(std::cin, value);
		Beep(500, 500);
	}
	return 0;
}
