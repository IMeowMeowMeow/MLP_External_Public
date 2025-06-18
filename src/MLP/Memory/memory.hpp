#pragma once
#include <windows.h>
#include <TlHelp32.h>
#include <comdef.h>
#include <iostream>
#include <vector>
#include <cstring>


namespace memory {
    inline DWORD process_id = NULL;
    inline HWND hwnd = NULL;
    inline uintptr_t module_base = NULL;
    inline HANDLE process = NULL;

    inline std::uint64_t get_module_base(const char* name, ULONG pid)
    {
        HANDLE Module = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
        MODULEENTRY32 Entry;
        Entry.dwSize = sizeof(Entry);

        WCHAR* ModuleNameChar;
        int Chars = MultiByteToWideChar(CP_ACP, 0, name, -1, NULL, 0);
        ModuleNameChar = new WCHAR[Chars];
        MultiByteToWideChar(CP_ACP, 0, name, -1, (LPWSTR)ModuleNameChar, Chars);

        while (Module32Next(Module, &Entry)) {
            if (!wcscmp((wchar_t*)Entry.szModule, ModuleNameChar)) {
                CloseHandle(Module);
                return std::uint64_t(Entry.modBaseAddr);
            }
        }

        CloseHandle(Module);
        Entry.modBaseAddr = 0x0;
        return std::uint64_t(Entry.modBaseAddr);
    }

    inline std::uint64_t get_module_size(const char* name, ULONG pid)
    {
        HANDLE Module = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
        MODULEENTRY32 Entry;
        Entry.dwSize = sizeof(Entry);

        WCHAR* ModuleNameChar;
        int Chars = MultiByteToWideChar(CP_ACP, 0, name, -1, NULL, 0);
        ModuleNameChar = new WCHAR[Chars];
        MultiByteToWideChar(CP_ACP, 0, name, -1, (LPWSTR)ModuleNameChar, Chars);

        while (Module32Next(Module, &Entry)) {
            if (!wcscmp((wchar_t*)Entry.szModule, ModuleNameChar)) {
                CloseHandle(Module);
                return std::uint64_t(Entry.modBaseSize);
            }
        }

        CloseHandle(Module);
        return std::uint64_t(NULL);
    }

    template <typename T>
    inline void write(uintptr_t address, const T& buffer) {
        if (address < 0x10000) return;  // Skip invalid addresses

        // Write memory using WriteProcessMemory
        if (!WriteProcessMemory(memory::process, (LPVOID)address, &buffer, sizeof(buffer), NULL)) {
        }
    }


    inline void write(uintptr_t address, const void* buffer, size_t size) {
        if (address < 0x10000 || buffer == nullptr || size == 0) return;

        // Write the raw memory to the target address
        WriteProcessMemory(process, (LPVOID)address, buffer, size, NULL);
    }

    inline void read_raw(uintptr_t address, void* buffer, size_t size) {
        ReadProcessMemory(process, (LPCVOID)address, buffer, size, NULL);
    }

    inline void* virtallocex(LPVOID address, SIZE_T dwSize, DWORD allocation_type, DWORD protection)
    {
        return VirtualAllocEx(process, address, dwSize, allocation_type, protection);
    }

    inline int attach(const char* window_name) {

        hwnd = FindWindowA(NULL, window_name);
        if (!hwnd) return 1;

        GetWindowThreadProcessId(hwnd, &process_id);
        if (!process_id) return 2;

        process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
        if (!process) return 3;

        return 0;
    }

    template <typename T>
    inline T read(uintptr_t address) {
        T data = T();
        memory::read_raw(address, &data, sizeof(T));
        return data;
    }

    template <typename T>
    inline bool read(uintptr_t address, T* buffer) {
        return ReadProcessMemory(process, reinterpret_cast<LPCVOID>(address), buffer, sizeof(T), NULL);
    }

    inline DWORD64 aob(HANDLE hProcess, BYTE* pattern, const char* mask, DWORD64 start, SIZE_T size) {
        std::vector<BYTE> buffer(size);
        SIZE_T bytesRead;

        if (ReadProcessMemory(hProcess, (LPCVOID)start, buffer.data(), size, &bytesRead)) {
            for (SIZE_T i = 0; i < bytesRead - strlen(mask); i++) {
                bool found = true;
                for (SIZE_T j = 0; j < strlen(mask); j++) {
                    if (mask[j] != '?' && buffer[i + j] != pattern[j]) {
                        found = false;
                        break;
                    }
                }
                if (found) return start + i;
            }
        }
        return 0;
    }
}