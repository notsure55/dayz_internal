#include <sig_scanner.hpp>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include <chrono>
#include <iostream>
#include <sstream>
#include <vector>
#include <windows.h>
#include <psapi.h>
#include <thread>

using std::cout;
using std::endl;

namespace Scanner
{
    std::unordered_map<std::string, uintptr_t> cached_sigs;

    uintptr_t get(const char* name) {
        return cached_sigs.find(name)->second;
    }

    void cache()
    {
        for (const auto sig: sigs)
        {
            std::vector<int> pattern;
            std::stringstream ss(sig.signature);
            std::string byte;

            // reads each byte, stores as int, so if ?? store a -1 byte so we can differentiate in compare later
            while (ss >> byte) {
                // ida scans give a ? not two ? lol :C
                if (byte == "??" || byte == "?") {
                    pattern.push_back(-1); // wildcard
                }
                else {
                    pattern.push_back(std::stoi(byte, nullptr, 16));
                }
            }

            const HMODULE hModule = GetModuleHandleA(nullptr);

            if (!hModule)
                return;

            MODULEINFO modInfo { nullptr };

            if (!GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(modInfo))) {
                return;
            }

            // BYTE* so we can index through bytes of the module
            const BYTE* start = static_cast<BYTE*>(modInfo.lpBaseOfDll);
            const size_t size = modInfo.SizeOfImage;

            // - pattern.size so we don't go outside of module and read null and swallow a load
            for (size_t i = 0; i < size - pattern.size(); ++i) {

                bool found = true;

                for (size_t j = 0; j < pattern.size(); ++j) {
                    // if !wildcard and != to byte at start[i + j] we break, and go next iter
                    if (pattern[j] != -1 && start[i + j] != pattern[j]) {
                        found = false;
                        break;
                    }
                }

                uintptr_t address = 0;

                if (found) {
                    if (sig.type == SigType::Absolute) {
                        const uintptr_t pattern_address = reinterpret_cast<uintptr_t>(&start[i]) + sig.offset;
                        const int jump_offset = *reinterpret_cast<int*>(pattern_address);
                        address = pattern_address + jump_offset + sizeof(int);
                    }
                    else if (sig.type == SigType::Relative) {
                        const uintptr_t pattern_address = reinterpret_cast<uintptr_t>(&start[i]);
                        const uintptr_t rip = pattern_address + 7;
                        const int offset = *reinterpret_cast<int*>(pattern_address + sig.offset);
                        address = rip + offset;
                    }
                    else if (sig.type == SigType::Jmp) {
                        const uintptr_t pattern_address = reinterpret_cast<uintptr_t>(&start[i]);
                        const uintptr_t rip = pattern_address + 5;
                        const int offset = *reinterpret_cast<int*>(pattern_address + sig.offset);
                        address = rip + offset;
                    }
                    else {
                        address = reinterpret_cast<uintptr_t>(&start[i]) + sig.offset;
                    }

                    std::cout << std::format("[*]Found a Sig! Name: {}", sig.name);
                    std::cout << std::format(" Address: 0x{:X}", address) << std::endl;
                    cached_sigs.emplace(sig.name, address);
                }
            }
        }
    }
}
