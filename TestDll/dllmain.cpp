// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include <fstream>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    std::ofstream output_file("D:\\test_dll_output.txt");

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: 
        output_file << "Dll process attach\n";
        break;
    case DLL_THREAD_ATTACH:
        output_file << "Dll thread attach\n";
        break;
    case DLL_THREAD_DETACH:
        output_file << "Dll thread detach\n";
        break;
    case DLL_PROCESS_DETACH:
        output_file << "Dll process detach\n";
        break;
    }

    return TRUE;
}

