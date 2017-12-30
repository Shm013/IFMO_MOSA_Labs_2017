// injector.cpp: îïğåäåëÿåò òî÷êó âõîäà äëÿ êîíñîëüíîãî ïğèëîæåíèÿ.
//

#include "stdafx.h"

int Wait();

int _tmain(int argc, _TCHAR* argv[])
{
    // Ïóñòü äî áèáëèîòåêè, êîòîğóş õîòèì èíúåêòèğîâàòü.
    DWORD processId = 55;
    char* dllName = "C:\\Users\\admin\\Documents\\Visual Studio 2010\\Projects\\inject_hook\\Debug\\inj_dll.dll";


    // Çàïğàøèâàåì PID ïğîöåññà êóäà õîòèì èíúåêòèğîâàòü.
    printf("Enter PID to inject dll: ");
    std::cin >> processId;

    // Ïîëó÷àåì äîñòóï ê ïğîöåññó.
    HANDLE openedProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (openedProcess == NULL)
    {
        printf("OpenProcess error code: %d\r\n", GetLastError());
        return Wait();
    }

    // Èùåì kernel32.dll
    HMODULE kernelModule = GetModuleHandleW(L"kernel32.dll");
    if (kernelModule == NULL)
    {
        printf("GetModuleHandleW error code: %d\r\n", GetLastError());
        return Wait();
    }

    // Èùåì LoadLibrary (Ñóôôèêñ A îçíà÷àåò ÷òî ğàáîòàåì â ANSI, îäèí áàéò íà ñèìâîë)
    LPVOID loadLibraryAddr = GetProcAddress(kernelModule, "LoadLibraryA");
    if (loadLibraryAddr == NULL)
    {
        printf("GetProcAddress error code: %d\r\n", GetLastError());
        return Wait();
    }

    // Âûäåëÿåì ïàìÿòü ïîä àğãóìåíò LoadLibrary, à èìåííî - ñòğîêó ñ àäğåñîì èíúåêòèğóåìîé DLL
    LPVOID argLoadLibrary = (LPVOID)VirtualAllocEx(openedProcess, NULL, strlen(dllName), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (argLoadLibrary == NULL)
    {
        printf("VirtualAllocEx error code: %d\r\n", GetLastError());
        return Wait();
    }

    // Ïèøåì áàéòû ïî óêàçàííîìó àäğåñó.
    int countWrited = WriteProcessMemory(openedProcess, argLoadLibrary, dllName, strlen(dllName), NULL);
    if (countWrited == NULL)
    {
        printf("WriteProcessMemory error code: %d\r\n", GetLastError());
        return Wait();
    }

    // Ñîçäàåì ïîòîê, ïåğåäàåì àäğåñ LoadLibrary è àäğåñ åå àğãóìåíòà
    HANDLE threadID = CreateRemoteThread(openedProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, argLoadLibrary, NULL, NULL);

    if (threadID == NULL)
    {
        printf("CreateRemoteThread error code: %d\r\n", GetLastError());
        return Wait();
    }
    else
    {
        printf("Dll injected!");
    }

    // Çàêğûâàåì ïîòîê.
    CloseHandle(openedProcess);

    return 0;
}

int Wait()
{
    char a;
    printf("Press any key to exit");
    std::cin >> a;
    return 0;
}
