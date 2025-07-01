#include <windows.h>
#include <iostream>
void setup() {
}

void cleanup(const LPVOID hModule, FILE* f) {
    // cleanup
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(static_cast<HMODULE>(hModule), 0);
}

DWORD WINAPI entry_point(const LPVOID hModule) {

    AllocConsole();
    FILE* f { nullptr };
    freopen_s(&f, "CONOUT$", "w", stdout);

    setup();

    while (!GetAsyncKeyState(VK_DELETE)) {
        Sleep(5);
    }

    cleanup(hModule, f);

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call != DLL_PROCESS_ATTACH) {
        return false;
    }

    const HANDLE handle = CreateThread(
        nullptr,
        0,
        entry_point,
        hModule,
        0,
        nullptr
    );

    if (handle) {
        CloseHandle(handle);
    }

    return TRUE;
}
