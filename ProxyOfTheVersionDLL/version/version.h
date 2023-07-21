#pragma once
#define WIN32_LEAN_AND_MEAN
#include <cstdio>
#include <Windows.h>

HINSTANCE m_hinst_dll = nullptr;
extern "C" UINT_PTR mProcs[17]{ 0 };

LPCSTR import_names[] = {
    "GetFileVersionInfoA",
    "GetFileVersionInfoByHandle",
    "GetFileVersionInfoExA",
    "GetFileVersionInfoExW",
    "GetFileVersionInfoSizeA",
    "GetFileVersionInfoSizeExA",
    "GetFileVersionInfoSizeExW",
    "GetFileVersionInfoSizeW",
    "GetFileVersionInfoW",
    "VerFindFileA",
    "VerFindFileW",
    "VerInstallFileA",
    "VerInstallFileW",
    "VerLanguageNameA",
    "VerLanguageNameW",
    "VerQueryValueA",
    "VerQueryValueW"
};

void setupWrappers()
{
    CHAR sys_dir[MAX_PATH];
    GetSystemDirectoryA(sys_dir, MAX_PATH);
    char buffer[MAX_PATH];
    sprintf_s(buffer, "%s\\version.dll", sys_dir);
    m_hinst_dll = LoadLibraryA(buffer);
    for (int i = 0; i < 17; i++) {
        mProcs[i] = reinterpret_cast<UINT_PTR>(GetProcAddress(m_hinst_dll, import_names[i]));
    }
}

extern "C" void GetFileVersionInfoA_wrapper();
extern "C" void GetFileVersionInfoByHandle_wrapper();
extern "C" void GetFileVersionInfoExA_wrapper();
extern "C" void GetFileVersionInfoExW_wrapper();
extern "C" void GetFileVersionInfoSizeA_wrapper();
extern "C" void GetFileVersionInfoSizeExA_wrapper();
extern "C" void GetFileVersionInfoSizeExW_wrapper();
extern "C" void GetFileVersionInfoSizeW_wrapper();
extern "C" void GetFileVersionInfoW_wrapper();
extern "C" void VerFindFileA_wrapper();
extern "C" void VerFindFileW_wrapper();
extern "C" void VerInstallFileA_wrapper();
extern "C" void VerInstallFileW_wrapper();
extern "C" void VerLanguageNameA_wrapper();
extern "C" void VerLanguageNameW_wrapper();
extern "C" void VerQueryValueA_wrapper();
extern "C" void VerQueryValueW_wrapper();