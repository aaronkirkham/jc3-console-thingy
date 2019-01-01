// https://github.com/xforce/jc3-handling-editor/blob/master/src/hide_from_debugger_patch.cpp

#include <Windows.h>
#include <Winternl.h>

#include <cassert>
#include <cstdint>

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L) // ntsubauth

static void* origQIP = nullptr;
typedef NTSTATUS (*ZwSetInformationThreadType)(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass,
                                               PVOID ThreadInformation, ULONG ThreadInformationLength);

NTSTATUS ZwSetInformationThreadHook(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass,
                                    PVOID ThreadInformation, ULONG ThreadInformationLength)
{
    // Don't hide from debugger
    if (ThreadInformationClass == 0x11) {
        return STATUS_SUCCESS;
    } else {
        return ((ZwSetInformationThreadType)origQIP)(ThreadHandle, ThreadInformationClass, ThreadInformation,
                                                     ThreadInformationLength);
    }
}

// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma pack(push, 1)
const ULONG   kMmovR10EcxMovEax = 0xB8D18B4C;
const USHORT  kSyscall          = 0x050F;
const BYTE    kRetNp            = 0xC3;
const ULONG64 kMov1             = 0x54894808244C8948;
const ULONG64 kMov2             = 0x4C182444894C1024;
const ULONG   kMov3             = 0x20244C89;
const USHORT  kTestByte         = 0x04F6;
const BYTE    kPtr              = 0x25;
const BYTE    kRet              = 0xC3;
const USHORT  kJne              = 0x0375;

// Service code for 64 bit systems.
struct ServiceEntry {
    // This struct contains roughly the following code:
    // 00 mov     r10,rcx
    // 03 mov     eax,52h
    // 08 syscall
    // 0a ret
    // 0b xchg    ax,ax
    // 0e xchg    ax,ax
    ULONG  mov_r10_rcx_mov_eax; // = 4C 8B D1 B8
    ULONG  service_id;
    USHORT syscall;     // = 0F 05
    BYTE   ret;         // = C3
    BYTE   pad;         // = 66
    USHORT xchg_ax_ax1; // = 66 90
    USHORT xchg_ax_ax2; // = 66 90
};

// Service code for 64 bit Windows 8.
struct ServiceEntryW8 {
    // This struct contains the following code:
    // 00 48894c2408      mov     [rsp+8], rcx
    // 05 4889542410      mov     [rsp+10], rdx
    // 0a 4c89442418      mov     [rsp+18], r8
    // 0f 4c894c2420      mov     [rsp+20], r9
    // 14 4c8bd1          mov     r10,rcx
    // 17 b825000000      mov     eax,25h
    // 1c 0f05            syscall
    // 1e c3              ret
    // 1f 90              nop
    ULONG64 mov_1;               // = 48 89 4C 24 08 48 89 54
    ULONG64 mov_2;               // = 24 10 4C 89 44 24 18 4C
    ULONG   mov_3;               // = 89 4C 24 20
    ULONG   mov_r10_rcx_mov_eax; // = 4C 8B D1 B8
    ULONG   service_id;
    USHORT  syscall; // = 0F 05
    BYTE    ret;     // = C3
    BYTE    nop;     // = 90
};

// Service code for 64 bit systems with int 2e fallback.
struct ServiceEntryWithInt2E {
    // This struct contains roughly the following code:
    // 00 4c8bd1           mov     r10,rcx
    // 03 b855000000       mov     eax,52h
    // 08 f604250803fe7f01 test byte ptr SharedUserData!308, 1
    // 10 7503             jne [over syscall]
    // 12 0f05             syscall
    // 14 c3               ret
    // 15 cd2e             int 2e
    // 17 c3               ret
    ULONG  mov_r10_rcx_mov_eax; // = 4C 8B D1 B8
    ULONG  service_id;
    USHORT test_byte; // = F6 04
    BYTE   ptr;       // = 25
    ULONG  user_shared_data_ptr;
    BYTE   one;              // = 01
    USHORT jne_over_syscall; // = 75 03
    USHORT syscall;          // = 0F 05
    BYTE   ret;              // = C3
    USHORT int2e;            // = CD 2E
    BYTE   ret2;             // = C3
};

// Service code for 64 bit systems with int 2e fallback.
struct PatchCode {
    // This struct contains roughly the following code:
    uint16_t mov_rax;
    uint64_t ptr;
    uint16_t jmp_rax;
};

// We don't have an internal thunk for x64.
struct ServiceFullThunk {
    union {
        ServiceEntry          original;
        ServiceEntryW8        original_w8;
        ServiceEntryWithInt2E original_int2e_fallback;
    };
};
#pragma pack(pop)

bool IsService(const void* source)
{
    const ServiceEntry* service = reinterpret_cast<const ServiceEntry*>(source);
    return (kMmovR10EcxMovEax == service->mov_r10_rcx_mov_eax && kSyscall == service->syscall
            && kRetNp == service->ret);
}

bool IsServiceW8(const void* source)
{
    const ServiceEntryW8* service = reinterpret_cast<const ServiceEntryW8*>(source);
    return (kMmovR10EcxMovEax == service->mov_r10_rcx_mov_eax && kMov1 == service->mov_1 && kMov2 == service->mov_2
            && kMov3 == service->mov_3);
}

bool IsServiceWithInt2E(const void* source)
{
    const ServiceEntryWithInt2E* service = reinterpret_cast<const ServiceEntryWithInt2E*>(source);
    return (kMmovR10EcxMovEax == service->mov_r10_rcx_mov_eax && kTestByte == service->test_byte && kPtr == service->ptr
            && kJne == service->jne_over_syscall && kSyscall == service->syscall && kRet == service->ret
            && kRet == service->ret2);
}

struct meow {
    virtual ~meow()
    {
        if (origQIP) {
            auto target = (ServiceFullThunk*)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "ZwSetInformationThread");
            memcpy(target, origQIP, sizeof(PatchCode));
            origQIP = nullptr;
        }
    }
};

static meow m;

void HookZwSetInformationThread()
{
    ServiceFullThunk* code =
        (ServiceFullThunk*)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "ZwSetInformationThread");

    assert(code != 0);

    size_t size = 0;

    if (IsService(code)) {
        size = sizeof ServiceEntry;
    } else if (IsServiceW8(code)) {
        size = sizeof ServiceEntryW8;
    } else if (IsServiceWithInt2E(code)) {
        size = sizeof ServiceEntryWithInt2E;
    }

    origQIP = (ServiceFullThunk*)VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    memcpy(origQIP, code, size);

    DWORD oldProtect;
    VirtualProtect(code, size, PAGE_EXECUTE_READWRITE, &oldProtect);

    PatchCode patch;
    patch.mov_rax = 0xB848;
    patch.ptr     = (uint64_t)ZwSetInformationThreadHook;
    patch.jmp_rax = 0xE0FF;

    memcpy(code, &patch, sizeof(PatchCode));
}
