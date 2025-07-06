#ifndef SIG_SCANNER_HPP
#define SIG_SCANNER_HPP

#include <array>

namespace Scanner
{

    enum SigType
    {
        Relative,
        Absolute,
        Basic,
        Jmp,
    };

    struct Sig
    {
        const char* name;
        const char* signature;
        const int offset;
        const SigType type;

        constexpr Sig(const char* name, const char* signature, const int offset, const SigType type)
            : name(name), signature(signature), offset(offset), type(type) {}
    };

    constexpr std::array<Sig, 1> sigs
    {
        Sig("world", "48 8b 0d ?? ?? ?? ?? 48 81 c1 a8 24 00 00 e8 ?? ?? ?? ?? c1 e8 1f 34 01 48 83 c4 28 c3", 3, SigType::Relative)
    };

    uintptr_t get(const char* name);
    void cache();
}

#endif // SIG_SCANNER_HPP
