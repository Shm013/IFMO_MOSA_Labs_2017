// dllmain.cpp: определяет точку входа для приложения DLL.
#include "stdafx.h"

// dllmain.cpp : Defines the entry point for the DLL application.

#define SIZE 5

// Объявления функций и кастомных типов
typedef int (WINAPI *pGetLocalTime)(LPSYSTEMTIME);

void WINAPI get2007Back(_Out_ LPSYSTEMTIME lpSystemTime);
void BeginRedirect(LPVOID);

// Адресс оригинальной функции
pGetLocalTime pOrigAddress = NULL;
// Сюда запишем старый код
BYTE oldBytes[SIZE] = { 0 };
// Сюда запишем код для вставки в оригинальную функцию
BYTE JMP[SIZE] = { 0 };
// Тут права доступа
DWORD oldProtect, myProtect = PAGE_EXECUTE_READWRITE;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Уведомимление, что подключились к процессу.
        MessageBoxW(NULL, L"I hook MessageBox!", L"Hello", MB_OK);

        // Идем адрес GetLocalTime
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
    // Массив-маска для записи команды перехода
    BYTE tempJMP[SIZE] = { 0xE9, 0x90, 0x90, 0x90, 0x90 }; // E9 cd - Короткий относительный переход + 4 пустых байта.
    memcpy(JMP, tempJMP, SIZE);
    // Вычисляем смещение относительно оригинальной функции
    DWORD JMPSize = ((DWORD)newFunction - (DWORD)pOrigAddress - 5); // 5 байт - это размер вставки.
    // Получаем доступ к памяти
    VirtualProtect((LPVOID)pOrigAddress, SIZE, PAGE_EXECUTE_READWRITE, &oldProtect);
    // Запоминаем старые байты
    memcpy(oldBytes, pOrigAddress, SIZE);
    // Пишем 4байта смещения. Код рассчитан только на x86
    memcpy(&JMP[1], &JMPSize, 4);
    // Записываем вместо оригинальных
    memcpy(pOrigAddress, JMP, SIZE);
    // Восстанавливаем старые права доступа
    VirtualProtect((LPVOID)pOrigAddress, SIZE, oldProtect, NULL);
}

// Возврашаем 2007 год.
SYSTEMTIME time = {2007,9,6,15,10,30,0,0};
void WINAPI get2007Back(_Out_ LPSYSTEMTIME lpSystemTime)
{
    *lpSystemTime = time;
    return;
}