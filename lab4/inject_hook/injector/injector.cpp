// injector.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"

int Wait();

int _tmain(int argc, _TCHAR* argv[])
{
    // ����� �� ����������, ������� ����� �������������.
    DWORD processId = 55;
    char* dllName = "C:\\Users\\admin\\Documents\\Visual Studio 2010\\Projects\\inject_hook\\Debug\\inj_dll.dll";


    // ����������� PID �������� ���� ����� �������������.
    printf("Enter PID to inject dll: ");
    std::cin >> processId;

    // �������� ������ � ��������.
    HANDLE openedProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (openedProcess == NULL)
    {
        printf("OpenProcess error code: %d\r\n", GetLastError());
        return Wait();
    }

    // ���� kernel32.dll
    HMODULE kernelModule = GetModuleHandleW(L"kernel32.dll");
    if (kernelModule == NULL)
    {
        printf("GetModuleHandleW error code: %d\r\n", GetLastError());
        return Wait();
    }

    // ���� LoadLibrary (������� A �������� ��� �������� � ANSI, ���� ���� �� ������)
    LPVOID loadLibraryAddr = GetProcAddress(kernelModule, "LoadLibraryA");
    if (loadLibraryAddr == NULL)
    {
        printf("GetProcAddress error code: %d\r\n", GetLastError());
        return Wait();
    }

    // �������� ������ ��� �������� LoadLibrary, � ������ - ������ � ������� ������������� DLL
    LPVOID argLoadLibrary = (LPVOID)VirtualAllocEx(openedProcess, NULL, strlen(dllName), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (argLoadLibrary == NULL)
    {
        printf("VirtualAllocEx error code: %d\r\n", GetLastError());
        return Wait();
    }

    // ����� ����� �� ���������� ������.
    int countWrited = WriteProcessMemory(openedProcess, argLoadLibrary, dllName, strlen(dllName), NULL);
    if (countWrited == NULL)
    {
        printf("WriteProcessMemory error code: %d\r\n", GetLastError());
        return Wait();
    }

    // ������� �����, �������� ����� LoadLibrary � ����� �� ���������
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

    // ��������� �����.
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
