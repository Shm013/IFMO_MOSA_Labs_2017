// dllmain.cpp: ���������� ����� ����� ��� ���������� DLL.
#include "stdafx.h"

// dllmain.cpp : Defines the entry point for the DLL application.

#define SIZE 5

// ���������� ������� � ��������� �����
typedef int (WINAPI *pGetLocalTime)(LPSYSTEMTIME);

void WINAPI get2007Back(_Out_ LPSYSTEMTIME lpSystemTime);
void BeginRedirect(LPVOID);

// ������ ������������ �������
pGetLocalTime pOrigAddress = NULL;
// ���� ������� ������ ���
BYTE oldBytes[SIZE] = { 0 };
// ���� ������� ��� ��� ������� � ������������ �������
BYTE JMP[SIZE] = { 0 };
// ��� ����� �������
DWORD oldProtect, myProtect = PAGE_EXECUTE_READWRITE;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // �������������, ��� ������������ � ��������.
        MessageBoxW(NULL, L"I hook MessageBox!", L"Hello", MB_OK);

        // ���� ����� GetLocalTime
        pOrigAddress = (pGetLocalTime)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetLocalTime");
        
        if (pOrigAddress != NULL)
        {
            BeginRedirect(get2007Back);
        }

        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void BeginRedirect(LPVOID newFunction)
{
    // ������-����� ��� ������ ������� ��������
    BYTE tempJMP[SIZE] = { 0xE9, 0x90, 0x90, 0x90, 0x90 }; // E9 cd - �������� ������������� ������� + 4 ������ �����.
    memcpy(JMP, tempJMP, SIZE);
    // ��������� �������� ������������ ������������ �������
    DWORD JMPSize = ((DWORD)newFunction - (DWORD)pOrigAddress - 5); // 5 ���� - ��� ������ �������.
    // �������� ������ � ������
    VirtualProtect((LPVOID)pOrigAddress, SIZE, PAGE_EXECUTE_READWRITE, &oldProtect);
    // ���������� ������ �����
    memcpy(oldBytes, pOrigAddress, SIZE);
    // ����� 4����� ��������. ��� ��������� ������ �� x86
    memcpy(&JMP[1], &JMPSize, 4);
    // ���������� ������ ������������
    memcpy(pOrigAddress, JMP, SIZE);
    // ��������������� ������ ����� �������
    VirtualProtect((LPVOID)pOrigAddress, SIZE, oldProtect, NULL);
}

// ���������� 2007 ���.
SYSTEMTIME time = {2007,9,6,15,10,30,0,0};
void WINAPI get2007Back(_Out_ LPSYSTEMTIME lpSystemTime)
{
    *lpSystemTime = time;
    return;
}