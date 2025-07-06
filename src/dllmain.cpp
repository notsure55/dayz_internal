#include <windows.h>
#include <iostream>
#include <sig_scanner.hpp>
#include <print>

void setup() {
    Scanner::cache();
}

class ObjectClass {
public:
    char padding[0x10];
    char* name;
};

class Object {
public:
    char padding[0x8];
    ObjectClass* object_class;
};

class World {
public:
    char padding[0x24A8];
    Object** inventory_items;
    int inventory_item_count;
};

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

    const auto world { *reinterpret_cast<World**>(Scanner::get("world")) };

    while (!GetAsyncKeyState(VK_DELETE)) {
        for (size_t i { 0 }; i < world->inventory_item_count; ++i) {
            std::println("*****************************");
            if (!world->inventory_items[i]) { continue; }
            std::println("object_name: {} | index: {}", world->inventory_items[i]->object_class->name, i);
        }

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
