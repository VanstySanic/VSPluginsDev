// Copyright VanstySanic. All Rights Reserved.

#pragma once

/**
 * @brief Cross-architecture runtime function hook with trampoline support.
 *
 * Features
 * --------
 * - Works across Windows, Linux, and macOS (including Apple Silicon).
 * - Stop-The-World (STW) best-effort thread suspension on Windows/macOS to reduce instruction tearing.
 * - Instruction cache flushing and page protection helpers (RWX write window).
 * - Apple JIT compatibility: MAP_JIT + pthread_jit_write_protect_np (resolved at runtime when available).
 * - x86/x64: minimal but practical relocation for common patterns:
 *     * CALL/JMP rel32, 0F 8x Jcc rel32
 *     * RIP-relative disp32 memory addressing (mod=00, rm=101)
 * - Jump patching:
 *     * Prefer near jump (E9 rel32) when within ±2 GB
 *     * Fallback to absolute jump (mov r11, imm64 ; jmp r11) on x64
 * - Trampoline allocator biased to place code near the original function (to keep rel32/disp32 in range).
 *
 * Architectures
 * -------------
 * - x86, x86_64: supported with the relocation coverage listed above.
 * - ARM64, ARMv7/Thumb-2: instruction width honored, but PC-relative relocation (B/BL, ADR/ADRP, LDR literal, etc.)
 *   is NOT fully implemented. Use at your own risk on ARM; complex functions may break.
 *
 * Limitations (Important)
 * -----------------------
 * - The x86/x64 instruction parser is intentionally lightweight; rare/complex encodings (VEX/EVEX/AVX-512, exotic
 *   prefixes/patterns) may be mis-parsed. For production-grade coverage, integrate a full decoder (e.g., Zydis/Capstone).
 * - Trampolines do not carry unwind metadata; stack unwinding/SEH through trampolines may be unsafe.
 * - Linux STW is a no-op; patching still relies on atomic jump emission + I-cache flush.
 * - On hardened macOS/iOS processes, MAP_JIT may require proper entitlements.
 *
 * Build
 * -----
 * - Linux/macOS:
 *     g++ -std=c++17 -O2 hotpatch_all_in_one_fixed.cpp -o hotpatch_demo -DHOTPATCH_DEMO
 * - Windows (MSVC):
 *     cl /std:c++17 /O2 hotpatch_all_in_one_fixed.cpp /DHOTPATCH_DEMO
 *
 * Safety Notes
 * ------------
 * - This code modifies executable memory in-process. Ensure you understand the implications (security policies,
 *   code-signing, W^X constraints) before deploying.
 * - Prefer testing in a controlled environment. Use at your own risk.
 */


#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <type_traits>

#if defined(_WIN32) || defined(_WIN64)
#define HP_WINDOWS 1
#include <windows.h>
#include <tlhelp32.h>
#else
#define HP_POSIX 1
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
	#if defined(__APPLE__)
#include <TargetConditionals.h>
#include <pthread.h>
#include <mach/mach.h>
#include <sys/sysctl.h>
		#ifndef MAP_JIT
#define MAP_JIT 0x800
		#endif
	#endif
#endif

/**
 * Architecture detection.
 */
#if defined(__x86_64__) || defined(_M_X64)
#define HP_ARCH_X64 1
#elif defined(__i386) || defined(_M_IX86)
#define HP_ARCH_X86 1
#elif defined(__aarch64__) || defined(_M_ARM64)
#define HP_ARCH_ARM64 1
#elif defined(__arm__) || defined(_M_ARM)
#define HP_ARCH_ARM32 1
#endif

/**
 * Config toggles
 * Stop-The-World while patching to minimize instruction tearing risk.
 */
#ifndef HP_ENABLE_STW
#define HP_ENABLE_STW 1
#endif

/**
 * Apple JIT helpers (resolved dynamically when available).
 */
#if defined(__APPLE__)
#if defined(__has_include)
#if __has_include(<pthread.h>)
#include <pthread.h>
#endif
#if __has_include(<dlfcn.h>)
#include <dlfcn.h>
#endif
#endif
#endif

namespace VS
{
	namespace Function
	{
		namespace Hook
		{
        #if defined(__APPLE__)

		    /**
             * Resolve pthread_jit_write_protect_np at runtime if available.
             */
		    static inline auto HpGetPjwp() -> void(*)(int)
            {
                using pjwp_fn = void(*)(int);
                static pjwp_fn Cached = []() -> pjwp_fn {
            #if defined(PTHREAD_JIT_WRITE_PROTECT_SUPPORTED)
                    return &pthread_jit_write_protect_np;
            #else
                #if defined(RTLD_DEFAULT) && defined(__has_include)
                    #if __has_include(<dlfcn.h>)
                    void* Sym = dlsym(RTLD_DEFAULT, "pthread_jit_write_protect_np");
                    return reinterpret_cast<pjwp_fn>(Sym);
                    #endif
                #endif
                    return nullptr;
            #endif
                }();
                return Cached;
            }

		    static inline void HpJitWriteEnter()
		    {
		        if (auto Fn = HpGetPjwp()) { Fn(0); }
		    }
		    static inline void HpJitWriteLeave()
		    {
		        if (auto Fn = HpGetPjwp()) { Fn(1); }
		    }
        #else
		    static inline void HpJitWriteEnter() {}
		    static inline void HpJitWriteLeave() {}
        #endif
		    
			/**
             * Instruction cache flush.
             */
            static void FlushICache(void* Addr, size_t Size)
            {
            #if HP_WINDOWS
                FlushInstructionCache(GetCurrentProcess(), Addr, Size);
            #else
                __builtin___clear_cache(reinterpret_cast<char*>(Addr), reinterpret_cast<char*>(Addr) + Size);
            #endif
            }

            /**
             * Compute page-aligned protection range for a given [addr, addr+size).
             */
            static void GetPageAlignedRange(void* Addr, size_t Size, void** OutPageStart, size_t* OutFullSize)
            {
            #if defined(HP_POSIX) && HP_POSIX
                size_t PageSize = static_cast<size_t>(sysconf(_SC_PAGESIZE));
            #else
                SYSTEM_INFO SI; GetSystemInfo(&SI);
                size_t PageSize = static_cast<size_t>(SI.dwPageSize);
            #endif
                uintptr_t A   = reinterpret_cast<uintptr_t>(Addr);
                uintptr_t PS  = A & ~(static_cast<uintptr_t>(PageSize) - 1);
                size_t Adjust   = static_cast<size_t>(A - PS);
                size_t Full     = Adjust + Size;
                size_t Rounded  = (Full + PageSize - 1) & ~(PageSize - 1);
                *OutPageStart   = reinterpret_cast<void*>(PS);
                *OutFullSize    = Rounded;
            }

            /**
             * Make target memory writable and executable.
             * Returns original protection (Windows) via OutOldProt; POSIX path restores to RX best-effort.
             */
            static bool MakeMemoryWritableExec(void* Addr, size_t Size, unsigned long& OutOldProt)
            {
                if (!Addr || Size == 0) { return false; }
            #if HP_WINDOWS
                DWORD Old = 0;
                if (!VirtualProtect(Addr, Size, PAGE_EXECUTE_READWRITE, &Old)) { return false; }
                OutOldProt = static_cast<unsigned long>(Old);
                return true;
            #else
                void* PageStart = nullptr; size_t FullSize = 0;
                GetPageAlignedRange(Addr, Size, &PageStart, &FullSize);
                HpJitWriteEnter();
                if (mprotect(PageStart, FullSize, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
                    HpJitWriteLeave();
                    return false;
                }
                OutOldProt = 0; /** Not tracked on POSIX path. */
                return true;
            #endif
            }

            /**
             * Restore memory protection after patching.
             */
            static bool RestoreMemoryProtection(void* Addr, size_t Size, unsigned long OldProt)
            {
                if (!Addr || Size == 0) { return false; }
            #if HP_WINDOWS
                DWORD Tmp = 0;
                return VirtualProtect(Addr, Size, static_cast<DWORD>(OldProt), &Tmp) != 0;
            #else
                void* PageStart = nullptr; size_t FullSize = 0;
                GetPageAlignedRange(Addr, Size, &PageStart, &FullSize);
                if (mprotect(PageStart, FullSize, PROT_READ | PROT_EXEC) != 0) {
                    HpJitWriteLeave(); /** Ensure we leave even on failure. */
                    return false;
                }
                HpJitWriteLeave();
                return true;
            #endif
            }

            /**
             * Write memory with temporary RWX, flush ICache, and restore protection.
             */
            static bool WriteMemory(void* Dest, const void* Data, size_t Size)
            {
                if (!Dest || !Data || Size == 0) { return false; }
                unsigned long OldProt = 0;
                if (!MakeMemoryWritableExec(Dest, Size, OldProt)) { return false; }
                std::memcpy(Dest, Data, Size);
                FlushICache(Dest, Size);
                if (!RestoreMemoryProtection(Dest, Size, OldProt)) { return false; }
                return true;
            }

            /**
             * Allocate executable memory (RWX).
             */
            static void* AllocExecMemory(size_t Size)
            {
                if (Size == 0) { return nullptr; }
            #if HP_WINDOWS
                return VirtualAlloc(nullptr, Size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            #else
                int Flags = MAP_PRIVATE | MAP_ANONYMOUS;
                #if defined(__APPLE__)
                Flags |= MAP_JIT; /** Best-effort; may require entitlements. */
                #endif
                void* P = mmap(nullptr, Size, PROT_READ | PROT_WRITE | PROT_EXEC, Flags, -1, 0);
                if (P == MAP_FAILED) { return nullptr; }
                return P;
            #endif
            }

            static void FreeExecMemory(void* P, size_t Size)
            {
                if (!P) { return; }
            #if HP_WINDOWS
                (void)Size; /** Not needed on Windows. */
                VirtualFree(P, 0, MEM_RELEASE);
            #else
                munmap(P, Size);
            #endif
            }

            /**
             * Allocate executable memory near a hint address to keep rel32 in range (mainly for x64).
             */
            static void* AllocExecMemoryNear(void* Hint, size_t Size)
            {
            #if HP_WINDOWS
                SYSTEM_INFO SI; GetSystemInfo(&SI);
                const size_t Gran = SI.dwAllocationGranularity ? SI.dwAllocationGranularity : 0x10000;
                uintptr_t Base = (uintptr_t)Hint;
                const uintptr_t Range = 0x70000000ull; /** ~1.75GB each side, leaving headroom. */
                uintptr_t Lo = (Base > Range) ? Base - Range : 0;
                uintptr_t Hi = Base + Range;

                for (int Dir = 0; Dir < 2; ++Dir) {
                    for (uintptr_t P = Base; ; ) {
                        void* R = VirtualAlloc((void*)P, Size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
                        if (R) { return R; }
                        if (Dir == 0) { /** Downward. */
                            if (P < Lo + Gran) { break; }
                            P -= Gran;
                        } else {         /** Upward. */
                            if (P > Hi - Gran) { break; }
                            P += Gran;
                        }
                    }
                }
                return AllocExecMemory(Size);
            #else
                uintptr_t Base = (uintptr_t)Hint;
                const uintptr_t Step = 1ull << 20; /** 1 MB stepping. */
                for (int i = 0; i < 256; ++i) {
                    uintptr_t Addr = Base + (i % 2 ? + (uintptr_t)i * Step : - (uintptr_t)i * Step);
                    void* P = mmap((void*)Addr, Size, PROT_READ | PROT_WRITE | PROT_EXEC,
                                   MAP_PRIVATE | MAP_ANONYMOUS
            #if defined(__APPLE__)
                                   | MAP_JIT
            #endif
                                   , -1, 0);
                    if (P != MAP_FAILED) {
            #if defined(HP_ARCH_X64)
                        uintptr_t Delta = ((uintptr_t)P > Base) ? ((uintptr_t)P - Base) : (Base - (uintptr_t)P);
                        if (Delta <= 0x7fffffffULL) { return P; }
                        munmap(P, Size);
            #else
                        return P;
            #endif
                    }
                }
                return AllocExecMemory(Size);
            #endif
            }

            /**
             * Best-effort Stop-The-World (STW) to minimize instruction tearing while patching.
             */
            class StopTheWorld
            {
            public:
                StopTheWorld() : bActive(false) {}

                bool Enter()
                {
            #if !HP_ENABLE_STW
                    return true;
            #else
                #if HP_WINDOWS
                    DWORD Pid = GetCurrentProcessId();
                    DWORD SelfTid = GetCurrentThreadId();
                    HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
                    if (Snap == INVALID_HANDLE_VALUE) { return false; }
                    THREADENTRY32 Te; Te.dwSize = sizeof(Te);
                    BOOL Ok = Thread32First(Snap, &Te);
                    while (Ok) {
                        if (Te.th32OwnerProcessID == Pid && Te.th32ThreadID != SelfTid) {
                            HANDLE Th = OpenThread(THREAD_SUSPEND_RESUME, FALSE, Te.th32ThreadID);
                            if (Th) { SuspendThread(Th); CloseHandle(Th); }
                        }
                        Ok = Thread32Next(Snap, &Te);
                    }
                    CloseHandle(Snap);
                    bActive = true;
                    return true;
                #elif defined(__APPLE__)
                    mach_port_t Task = mach_task_self();
                    thread_act_array_t Threads;
                    mach_msg_type_number_t Count = 0;
                    kern_return_t Kr = task_threads(Task, &Threads, &Count);
                    if (Kr != KERN_SUCCESS) { return false; }
                    thread_t Self = mach_thread_self();
                    for (mach_msg_type_number_t i = 0; i < Count; ++i) {
                        if (Threads[i] != Self) { thread_suspend(Threads[i]); }
                    }
                    bActive = true;
                    for (mach_msg_type_number_t i = 0; i < Count; ++i) { mach_port_deallocate(mach_task_self(), Threads[i]); }
                    return true;
                #else
                    /** No portable STW on Linux/other POSIX; noop. */
                    bActive = false;
                    return true;
                #endif
            #endif
                }

                void Leave()
                {
            #if !HP_ENABLE_STW
                    return;
            #else
                #if HP_WINDOWS
                    DWORD Pid = GetCurrentProcessId();
                    DWORD SelfTid = GetCurrentThreadId();
                    HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
                    if (Snap == INVALID_HANDLE_VALUE) { return; }
                    THREADENTRY32 Te; Te.dwSize = sizeof(Te);
                    BOOL Ok = Thread32First(Snap, &Te);
                    while (Ok) {
                        if (Te.th32OwnerProcessID == Pid && Te.th32ThreadID != SelfTid) {
                            HANDLE Th = OpenThread(THREAD_SUSPEND_RESUME, FALSE, Te.th32ThreadID);
                            if (Th) { ResumeThread(Th); CloseHandle(Th); }
                        }
                        Ok = Thread32Next(Snap, &Te);
                    }
                    CloseHandle(Snap);
                    bActive = false;
                #elif defined(__APPLE__)
                    mach_port_t Task = mach_task_self();
                    thread_act_array_t Threads;
                    mach_msg_type_number_t Count = 0;
                    if (task_threads(Task, &Threads, &Count) == KERN_SUCCESS) {
                        thread_t Self = mach_thread_self();
                        for (mach_msg_type_number_t i = 0; i < Count; ++i) {
                            if (Threads[i] != Self) { thread_resume(Threads[i]); }
                        }
                        for (mach_msg_type_number_t i = 0; i < Count; ++i) { mach_port_deallocate(mach_task_self(), Threads[i]); }
                    }
                    bActive = false;
                #else
                    bActive = false; /** Noop. */
                #endif
            #endif
                }

            private:
                bool bActive;
            };

            /**
             * Minimal x86/x64 instruction parser with relocation info.
             * Coverage:
             *  - rel32 immediates for CALL (0xE8), JMP (0xE9), and 0F 8x jcc rel32
             *  - RIP-relative memory with ModR/M mod==0 && rm==5 (disp32)
             */
            #if defined(HP_ARCH_X86) || defined(HP_ARCH_X64)
            struct X86InstrInfo
            {
                size_t Len;             /** Instruction length in bytes. */
                bool   bHasRel32;       /** Valid rel32 immediate present. */
                size_t Rel32Offset;     /** Offset of rel32 immediate (from instruction start). */
                bool   bHasRipDisp32;   /** RIP-relative disp32 present. */
                size_t RipDispOffset;   /** Offset of disp32 (from instruction start). */

                X86InstrInfo()
                    : Len(0), bHasRel32(false), Rel32Offset(0), bHasRipDisp32(false), RipDispOffset(0)
                {}
            };

            static inline bool Readuint8_t(const uint8_t* P, size_t Max, size_t Idx, uint8_t& Out)
            {
                if (Idx >= Max) { return false; }
                Out = P[Idx]; return true;
            }

            static inline bool Readuint32_t(const uint8_t* P, size_t Max, size_t Idx, uint32_t& Out)
            {
                if (Idx + 4 > Max) { return false; }
                std::memcpy(&Out, P + Idx, 4); return true;
            }

            /**
             * Parse a single instruction and extract relocation-related info.
             * Returns decoded length; 0 on failure.
             */
            static size_t X86ParseInstruction(const uint8_t* P, size_t Max, X86InstrInfo& Info)
            {
                Info = X86InstrInfo();
                if (Max == 0) { return 0; }
                size_t I = 0;

                /** 1) Legacy prefixes. */
                bool bPrefix = true;
                while (bPrefix && I < Max) {
                    uint8_t B = P[I];
                    switch (B) {
                        case 0xF0: case 0xF2: case 0xF3:
                        case 0x2E: case 0x36: case 0x3E: case 0x26: case 0x64: case 0x65:
                        case 0x66: case 0x67:
                            ++I; break;
                        default:
                            bPrefix = false; break;
                    }
                }
                if (I >= Max) { return 0; }

                /** 2) REX prefix (x64). */
                uint8_t Rex = 0;
                if (I < Max && (P[I] & 0xF0) == 0x40) { Rex = P[I]; ++I; }

                if (I >= Max) { return 0; }
                uint8_t Op = P[I++];
                bool bTwoByte = false;
                if (Op == 0x0F) {
                    if (I >= Max) { return 0; }
                    Op = P[I++];
                    bTwoByte = true;
                }

                auto Need = [&](size_t N)->bool { if (I + N > Max) { return false; } I += N; return true; };

                /** Helper to parse ModR/M, SIB and detect RIP-relative disp32. */
                auto ParseModRMAndDisps = [&](size_t ModRMIdx)->bool
                {
                    if (ModRMIdx >= Max) { return false; }
                    uint8_t ModRM = P[ModRMIdx];
                    uint8_t Mod = (ModRM >> 6) & 3;
                    uint8_t Rm  = (ModRM) & 7;
                    size_t Cur = ModRMIdx + 1;
                    if (Mod != 3) {
                        if (Rm == 4) {
                            if (Cur >= Max) { return false; }
                            uint8_t SIB = P[Cur++];
                            uint8_t Base = SIB & 7;
                            if (Mod == 0 && Base == 5) {
                                if (Cur + 4 > Max) { return false; }
                                Cur += 4;
                            } else if (Mod == 1) {
                                if (Cur + 1 > Max) { return false; }
                                Cur += 1;
                            } else if (Mod == 2) {
                                if (Cur + 4 > Max) { return false; }
                                Cur += 4;
                            }
                        } else {
                            if (Mod == 0 && Rm == 5) {
                                if (Cur + 4 > Max) { return false; }
            #if defined(HP_ARCH_X64)
                                Info.bHasRipDisp32 = true;
                                Info.RipDispOffset = Cur;
            #endif
                                Cur += 4;
                            } else if (Mod == 1) {
                                if (Cur + 1 > Max) { return false; }
                                Cur += 1;
                            } else if (Mod == 2) {
                                if (Cur + 4 > Max) { return false; }
                                Cur += 4;
                            }
                        }
                    }
                    if (Cur > I) { I = Cur; }
                    return true;
                };

                if (!bTwoByte) {
                    /** CALL/JMP rel32. */
                    if (Op == 0xE8 || Op == 0xE9) {
                        if (!Need(4)) { return 0; }
                        Info.Len = I;
                        Info.bHasRel32 = true;
                        Info.Rel32Offset = Info.Len - 4;
                        return Info.Len;
                    }
                    /** Short Jcc / short JMP. */
                    if ((Op & 0xF0) == 0x70 || Op == 0xEB) {
                        if (!Need(1)) { return 0; }
                        Info.Len = I; return Info.Len;
                    }

                    /** Common ModR/M cases (conservative). */
                    if ((Op & 0xC0) == 0x80 || Op == 0x81 || Op == 0x83 || Op == 0xC7 ||
                        Op == 0x89 || Op == 0x8B || Op == 0x8A || Op == 0x88 ||
                        (Op >= 0x50 && Op <= 0x5F) || (Op >= 0xB8 && Op <= 0xBF))
                    {
                        bool bUsesModRM = false;
                        if (Op == 0x89 || Op == 0x8B || Op == 0x8A || Op == 0x88 || Op == 0xC7 || Op == 0x81 || Op == 0x83) {
                            bUsesModRM = true;
                        }
                        if ((Op >= 0xB8 && Op <= 0xBF)) {
            #if defined(HP_ARCH_X64)
                            size_t Imm = 8;
            #else
                            size_t Imm = 4;
            #endif
                            if (!Need(Imm)) { return 0; }
                            Info.Len = I; return Info.Len;
                        }
                        if (bUsesModRM) {
                            if (I >= Max) { return 0; }
                            size_t ModRMIdx = I;
                            if (!Need(1)) { return 0; }
                            if (!ParseModRMAndDisps(ModRMIdx)) { return 0; }
                            if (Op == 0xC7 || Op == 0x81) {
                                if (!Need(4)) { return 0; }
                            } else if (Op == 0x83) {
                                if (!Need(1)) { return 0; }
                            }
                            Info.Len = I; return Info.Len;
                        }
                        Info.Len = I; return Info.Len;
                    }

                    /** Heuristic ModR/M parse, otherwise treat as single byte. */
                    if ((Op & 0xC0) != 0xC0) {
                        if (I < Max) {
                            size_t Save = I;
                            if (!Need(1)) { Info.Len = I; return Info.Len; }
                            size_t ModRMIdx = Save;
                            if (!ParseModRMAndDisps(ModRMIdx)) {
                                Info.Len = I; return Info.Len;
                            }
                            Info.Len = I; return Info.Len;
                        }
                    }
                    Info.Len = I; return Info.Len;
                }
                else {
                    /** Two-byte: Jcc rel32 (0F 80-8F). */
                    if (Op >= 0x80 && Op <= 0x8F) {
                        if (!Need(4)) { return 0; }
                        Info.Len = I;
                        Info.bHasRel32 = true;
                        Info.Rel32Offset = Info.Len - 4;
                        return Info.Len;
                    }
                    if (I < Max) {
                        size_t ModRMIdx = I;
                        if (!Need(1)) { return 0; }
                        if (!ParseModRMAndDisps(ModRMIdx)) { return 0; }
                        Info.Len = I; return Info.Len;
                    }
                    Info.Len = I; return Info.Len;
                }
            }

            /** Wrapper to get instruction length only. */
            static size_t X86InstrLen(const uint8_t* P, size_t Max)
            {
                X86InstrInfo Info;
                size_t L = X86ParseInstruction(P, Max, Info);
                return L;
            }
            #endif /** x86/x64 */

            /**
             * Thumb-2 instruction width detector (ARM32).
             */
            #if defined(HP_ARCH_ARM32)
            static inline bool ThumbIs32Bit(uint16_t Halfword)
            {
                uint16_t Top5 = (Halfword >> 11) & 0x1F;
                return (Top5 == 0b11101) || (Top5 == 0b11110) || (Top5 == 0b11111);
            }
            #endif

            /**
             * function hook base (templated on function pointer type).
             * Builds trampoline, relocates x86/x64 relocs when needed, and patches entry with a jump.
             */
            template <typename FuncPtrType>
            class FFunctionHook
            {
            public:
                FFunctionHook(FuncPtrType From, FuncPtrType To)
                    : SrcFunc(From)
                    , DstFunc(To)
                    , Trampoline(nullptr)
                    , TrampSize(0)
                    , bHooked(false)
                    , CopyLen(0)
                {
                    static_assert(sizeof(FuncPtrType) == sizeof(void*), "Unsupported ABI: function pointer size differs from void*");
                    std::memset(Saved, 0, sizeof(Saved));
                }

                ~FFunctionHook()
                {
                    UnHook();
                }

                /**
                 * Install the hook.
                 */
                bool Hook()
                {
                    if (bHooked) { return false; }
                    if (!SrcFunc || !DstFunc) { return false; }

                    void* Src = FnToPtr(SrcFunc);
                    void* Dst = FnToPtr(DstFunc);
                    if (!Src || !Dst) { return false; }

                    StopTheWorld STW; STW.Enter();

                    /** Build jump patch at Src. */
                    uint8_t Patch[32]; size_t PatchLen = 0;
                    if (!BuildJumpPatch(Src, Dst, Patch, sizeof(Patch), PatchLen)) {
                        STW.Leave(); return false;
                    }

                    /** Compute safe copy length (no instruction splitting). */
                    const size_t NeedCopy = PatchLen;
                    CopyLen = ComputeSafeCopyLenAndInfos(Src, NeedCopy);
                    if (CopyLen == 0 || CopyLen > sizeof(Saved)) { STW.Leave(); return false; }

                    /** Save original bytes. */
                    std::memcpy(Saved, Src, CopyLen);

                    /** Create trampoline and relocate inner references if needed. */
                    if (!CreateTrampoline(Src, CopyLen)) {
                        STW.Leave(); return false;
                    }

                    /** Patch entry with our jump. */
                    if (!WriteMemory(Src, Patch, PatchLen)) {
                        DestroyTrampoline();
                        STW.Leave();
                        return false;
                    }

                    bHooked = true;
                    STW.Leave();
                    return true;
                }

                /**
                 * Remove the hook and restore original bytes.
                 */
                bool UnHook()
                {
                    if (!bHooked) { return false; }
                    void* Src = FnToPtr(SrcFunc);
                    if (!Src || CopyLen == 0) { return false; }

                    StopTheWorld STW; STW.Enter();

                    bool bOk = WriteMemory(Src, Saved, CopyLen);
                    DestroyTrampoline();
                    bHooked = false;

                    STW.Leave();
                    return bOk;
                }

                /**
                 * Get the trampoline entry (callable as original function).
                 */
                void* GetTrampoline() const { return Trampoline; }

            private:
                static void* FnToPtr(FuncPtrType F) { void* P = nullptr; std::memcpy(&P, &F, sizeof(void*)); return P; }

                /**
                 * Compute minimal bytes to copy (>= MinBytes) without cutting instructions.
                 * For x86/x64 also collect relocation info for later fixups.
                 */
                static size_t ComputeSafeCopyLenAndInfos(void* Code, size_t MinBytes)
                {
            #if defined(HP_ARCH_X64) || defined(HP_ARCH_X86)
                    const uint8_t* P = reinterpret_cast<const uint8_t*>(Code);
                    size_t Off = 0; size_t MaxScan = 64;
                    InstrInfosCount = 0;
                    while (Off < MaxScan) {
                        X86InstrInfo Info;
                        size_t L = X86ParseInstruction(P + Off, MaxScan - Off, Info);
                        if (L == 0) { return 0; }
                        
                        if (InstrInfosCount < MAX_INFOS) {
                            InstrRelInfo& R = Instrs[InstrInfosCount++];
                            R.StartOffset   = Off;
                            R.Len           = Info.Len;
                            R.bHasRel32     = Info.bHasRel32;
                            R.Rel32Offset   = Off + Info.Rel32Offset;
                            R.bHasRipDisp32 = Info.bHasRipDisp32;
                            R.RipDispOffset = Off + Info.RipDispOffset;
                        } else {
                            return 0;
                        }

                        Off += L;
                        if (Off >= MinBytes) { return Off > 64 ? 0 : Off; }
                    }
                    return 0;
            #elif defined(HP_ARCH_ARM64)
                    size_t N = ((MinBytes + 3) & ~size_t(3));
                    return N <= 64 ? N : 0;
            #elif defined(HP_ARCH_ARM32)
                    uintptr_t A = reinterpret_cast<uintptr_t>(Code);
                    if (A & 1) {
                        const uint16_t* HP = reinterpret_cast<const uint16_t*>(A & ~uintptr_t(1));
                        size_t Bytes = 0; size_t MaxBytes = 64;
                        while (Bytes < MaxBytes) {
                            uint16_t HW = *HP;
                            bool bW32 = ThumbIs32Bit(HW);
                            size_t ILen = bW32 ? 4 : 2;
                            Bytes += ILen;
                            HP += (ILen / 2);
                            if (Bytes >= MinBytes) { return Bytes; }
                        }
                        return 0;
                    } else {
                        size_t N = ((MinBytes + 3) & ~size_t(3));
                        return N <= 64 ? N : 0;
                    }
            #else
                    return 0;
            #endif
                }

                /**
                 * Build an absolute jump patch at Src to Dst into Out buffer.
                 * Returns OutLen bytes written.
                 */
                static bool BuildJumpPatch(void* Src, void* Dst, uint8_t* Out, size_t OutCap, size_t& OutLen)
                {
                    OutLen = 0;
            #if defined(HP_ARCH_X64)
                    uintptr_t S = reinterpret_cast<uintptr_t>(Src);
                    uintptr_t D = reinterpret_cast<uintptr_t>(Dst);
                    long long Rel64 = (long long)D - (long long)(S + 5);
                    if (Rel64 >= INT32_MIN && Rel64 <= INT32_MAX) {
                        if (OutCap < 5) { return false; }
                        Out[0] = 0xE9;
                        int32_t R32 = (int32_t)Rel64;
                        std::memcpy(Out + 1, &R32, 4);
                        OutLen = 5; return true;
                    }
                    if (OutCap < 13) { return false; }
                    uint8_t Buf[13] = {0x49,0xBB,0,0,0,0,0,0,0,0, 0x41,0xFF,0xE3}; /** mov r11, imm64; jmp r11 */
                    uint64_t T = (uint64_t)reinterpret_cast<uintptr_t>(Dst);
                    std::memcpy(&Buf[2], &T, 8);
                    std::memcpy(Out, Buf, 13); OutLen = 13; return true;
            #elif defined(HP_ARCH_X86)
                    if (OutCap < 5) { return false; }
                    uintptr_t S = reinterpret_cast<uintptr_t>(Src);
                    uintptr_t D = reinterpret_cast<uintptr_t>(Dst);
                    long long Rel64 = (long long)D - (long long)(S + 5);
                    if (Rel64 < INT32_MIN || Rel64 > INT32_MAX) { return false; }
                    int32_t R32 = (int32_t)Rel64;
                    Out[0] = 0xE9; std::memcpy(Out + 1, &R32, 4); OutLen = 5; return true;
            #elif defined(HP_ARCH_ARM64)
                    if (OutCap < 16) { return false; }
                    uint32_t Ldr = 0x58000000u | ((2u & 0x7FFFFu) << 5) | 17u; /** LDR X17, #8 */
                    uint32_t Br  = 0xD61F0000u | (17u << 5);                   /** BR X17 */
                    uint64_t T   = (uint64_t)reinterpret_cast<uintptr_t>(Dst);
                    std::memcpy(Out + 0, &Ldr, 4);
                    std::memcpy(Out + 4, &Br, 4);
                    std::memcpy(Out + 8, &T, 8);
                    OutLen = 16; return true;
            #elif defined(HP_ARCH_ARM32)
                    uintptr_t S = reinterpret_cast<uintptr_t>(Src);
                    if (S & 1) {
                        /**
                         * Thumb-2 back jump (alignment-safe).
                         * Sequence: LDR.W r12, [PC, #4]; BX r12; NOP; [optional NOP]; .word (target|1)
                         * The optional 16-bit NOP is inserted if A%4==2 to ensure literal is at Align(A+4,4)+4.
                         */
                        if (OutCap < 12) { return false; }

                        uint8_t* P = Out;
                        uintptr_t A = (reinterpret_cast<uintptr_t>(Src) & ~1u);
                        bool bNeedPadNop = ((A & 2u) != 0);

                        uint8_t Ldr[4] = {0xF8, 0xDF, 0xC0, 0x04}; /** LDR.W r12, [PC, #4] */
                        std::memcpy(P, Ldr, 4); P += 4;

                        uint8_t Bx[2] = {0x47, 0x60};             /** BX r12 */
                        std::memcpy(P, Bx, 2); P += 2;

                        uint8_t Nop16[2] = {0xBF, 0x00};          /** NOP */
                        std::memcpy(P, Nop16, 2); P += 2;

                        if (bNeedPadNop) {
                            if ((size_t)(P - Out) + 2 + 4 > OutCap) { return false; }
                            std::memcpy(P, Nop16, 2); P += 2;
                        }

                        if ((size_t)(P - Out) + 4 > OutCap) { return false; }
                        uint32_t T = (uint32_t)((reinterpret_cast<uintptr_t>(Dst)) | 1u); /** Keep Thumb state. */
                        std::memcpy(P, &T, 4); P += 4;

                        OutLen = (size_t)(P - Out); /** 12 or 14 */
                        return true;
                    } else {
                        /** ARM A32: LDR PC, [PC,#-4]; .word target */
                        if (OutCap < 8) { return false; }
                        uint32_t LdrPc = 0xE51FF004u;
                        uint32_t T = (uint32_t)(reinterpret_cast<uintptr_t>(Dst));
                        std::memcpy(Out + 0, &LdrPc, 4);
                        std::memcpy(Out + 4, &T, 4);
                        OutLen = 8; return true;
                    }
            #else
                    (void)Src; (void)Dst; (void)Out; (void)OutCap;
                    return false;
            #endif
                }

                /**
                 * Create trampoline memory near Src, copy Saved bytes, relocate if needed, and append a back jump.
                 */
                bool CreateTrampoline(void* Src, size_t SavedBytes)
                {
                    if (!Src || SavedBytes == 0) { return false; }

                    size_t BackLen = 0;
            #if defined(HP_ARCH_X64)
                    BackLen = 13;
            #elif defined(HP_ARCH_X86)
                    BackLen = 5;
            #elif defined(HP_ARCH_ARM64)
                    BackLen = 16;
            #elif defined(HP_ARCH_ARM32)
                    uintptr_t S = reinterpret_cast<uintptr_t>(Src);
                    BackLen = (S & 1) ? 14 : 8; /** Thumb may need extra NOP. */
            #else
                    return false;
            #endif
                    size_t AllocSize = SavedBytes + BackLen;
                    void*  Mem = AllocExecMemoryNear(Src, AllocSize);
                    if (!Mem) { return false; }

            #if defined(__APPLE__)
                    HpJitWriteEnter();
            #endif

                    /** Copy original prologue bytes into trampoline. */
                    std::memcpy(Mem, Saved, SavedBytes);

            #if defined(HP_ARCH_X86) || defined(HP_ARCH_X64)
                    /** Relocate rel32 and RIP-disp32 within copied instructions. */
                    uintptr_t OrigBase  = reinterpret_cast<uintptr_t>(Src);
                    uintptr_t TrampBase = reinterpret_cast<uintptr_t>(Mem);
                    for (size_t Idx = 0; Idx < InstrInfosCount; ++Idx) {
                        InstrRelInfo& Ri = Instrs[Idx];

                        if (Ri.bHasRel32) {
                            uintptr_t InstrStart = OrigBase + Ri.StartOffset;
                            uintptr_t InstrEnd   = InstrStart + Ri.Len;
                            int32_t OrigRel = 0;
                            std::memcpy(&OrigRel, reinterpret_cast<void*>(OrigBase + Ri.Rel32Offset), 4);
                            uintptr_t Target = InstrEnd + static_cast<int64_t>(OrigRel);

                            uintptr_t NewInstrStart = TrampBase + Ri.StartOffset;
                            uintptr_t NewInstrEnd   = NewInstrStart + Ri.Len;
                            int64_t NewRel64   = (int64_t)Target - (int64_t)NewInstrEnd;
                            if (NewRel64 < INT32_MIN || NewRel64 > INT32_MAX) {
                                std::fprintf(stderr, "[rel32 OOR] tramp=%p target=%p delta=%lld\n",
                                             (void*)NewInstrEnd, (void*)Target, (long long)NewRel64);
                                return false;
                            }
                            int32_t NewRel32 = static_cast<int32_t>(NewRel64);
                            std::memcpy(reinterpret_cast<void*>(TrampBase + Ri.Rel32Offset), &NewRel32, 4);
                        }

                        if (Ri.bHasRipDisp32) {
                            uintptr_t InstrStart = OrigBase + Ri.StartOffset;
                            uintptr_t InstrEnd   = InstrStart + Ri.Len;
                            int32_t OrigDisp    = 0;
                            std::memcpy(&OrigDisp, reinterpret_cast<void*>(OrigBase + Ri.RipDispOffset), 4);
                            uintptr_t Target = InstrEnd + static_cast<int64_t>(OrigDisp);

                            uintptr_t NewInstrStart = TrampBase + Ri.StartOffset;
                            uintptr_t NewInstrEnd   = NewInstrStart + Ri.Len;
                            int64_t NewDisp64  = (int64_t)Target - (int64_t)NewInstrEnd;
                            if (NewDisp64 < INT32_MIN || NewDisp64 > INT32_MAX) {
                                std::fprintf(stderr, "[rip-disp OOR] tramp=%p target=%p delta=%lld\n",
                                             (void*)NewInstrEnd, (void*)Target, (long long)NewDisp64);
                                return false;
                            }
                            int32_t NewDisp32 = static_cast<int32_t>(NewDisp64);
                            std::memcpy(reinterpret_cast<void*>(TrampBase + Ri.RipDispOffset), &NewDisp32, 4);
                        }
                    }
            #endif

                    /** Build back jump to Src + SavedBytes. */
                    uint8_t*  Back = reinterpret_cast<uint8_t*>(Mem) + SavedBytes;
                    uintptr_t RetAddr = reinterpret_cast<uintptr_t>(Src) + SavedBytes;
            #if defined(HP_ARCH_ARM32)
                    if (reinterpret_cast<uintptr_t>(Src) & 1u) {
                        RetAddr |= 1u; /** Keep Thumb state. */
                    }
            #endif
                    void* Ret = reinterpret_cast<void*>(RetAddr);

                    size_t Built = 0;
                    if (!BuildJumpPatch(/*Src=*/Back, /*Dst=*/Ret, Back, BackLen, Built) || Built == 0 || Built > BackLen) {
            #if defined(__APPLE__)
                        HpJitWriteLeave();
            #endif
                        FreeExecMemory(Mem, AllocSize);
                        return false;
                    }

                    FlushICache(Mem, AllocSize);
            #if defined(__APPLE__)
                    HpJitWriteLeave();
            #endif

                    Trampoline = Mem;
                    TrampSize  = AllocSize;
                    return true;
                }

                /**
                 * Free trampoline memory.
                 */
                void DestroyTrampoline()
                {
                    if (Trampoline) {
                        FreeExecMemory(Trampoline, TrampSize);
                        Trampoline = nullptr;
                        TrampSize  = 0;
                    }
                }

            private:
                FuncPtrType SrcFunc;
                FuncPtrType DstFunc;

                uint8_t     Saved[64];
                size_t CopyLen;

                void*  Trampoline;
                size_t TrampSize;
                bool   bHooked;

            #if defined(HP_ARCH_X86) || defined(HP_ARCH_X64)
                /**
                 * Relocation info captured during ComputeSafeCopyLenAndInfos for x86/x64.
                 * Static storage is preserved to avoid extra allocations; single-threaded usage is assumed.
                 */
                struct InstrRelInfo
                {
                    size_t StartOffset;
                    size_t Len;
                    bool   bHasRel32;
                    size_t Rel32Offset;
                    bool   bHasRipDisp32;
                    size_t RipDispOffset;
                };
                static constexpr size_t MAX_INFOS = 32;
                static InstrRelInfo Instrs[MAX_INFOS];
                static size_t       InstrInfosCount;
            #endif
            };

            /** Static members definition. */
            #if defined(HP_ARCH_X86) || defined(HP_ARCH_X64)
            template<typename F> typename FFunctionHook<F>::InstrRelInfo FFunctionHook<F>::Instrs[FFunctionHook<F>::MAX_INFOS];
            template<typename F> size_t FFunctionHook<F>::InstrInfosCount = 0;
            #endif
		}
	}
}

template <typename FuncPtrType>
using FVSFunctionHook = VS::Function::Hook::FFunctionHook<FuncPtrType>;