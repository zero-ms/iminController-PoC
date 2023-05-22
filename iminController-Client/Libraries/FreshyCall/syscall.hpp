// Copyright (c) 2020 ElephantSe4l. All Rights Reserved.
// Released under MPL-2.0, see LICENCE for more information.

#if !defined(__x86_64) && !defined(_M_AMD64)
#error Only x64 compilations are supported
#endif

#ifndef FRESHYCALLS_SYSCALL_HPP_
#define FRESHYCALLS_SYSCALL_HPP_

#define JM_XORSTR_DISABLE_AVX_INTRINSICS

// compiler intrinsics such as __readgsqword or __movsb
#if defined(__GNUC__) || defined(__GNUG__)
#include <psdk_inc/intrin-impl.h>
#else
#include <intrin.h>
#endif

// #include "function_result.hpp"
#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <windows.h>

using StubMap = std::map<uintptr_t, DWORD64>;
using SyscallMap = std::unordered_map<DWORD64, uint32_t>;
using NtStatus = uint32_t;

extern uint8_t masked_syscall_stub[];

namespace freshycalls {

// Set of utils related to syscalls such as dynamic numbers extraction or
// dynamic invocation.
//
// `Syscall` class is a singleton class. Service numbers differs between OS
// builds nor even between process. It doesn't make sense to have several
// instances of this class, it will always be the same. Remark: As it's a
// singleton, there's no (external) way to construct this class. You need to use
// the instance getter `get_instance()`
//
// Sample usage:
//    auto& syscall = freshycalls::Syscall::get_instance();
//    syscall.CallSyscall("NtTestAlert");

class Syscall {

private:
  static inline StubMap stub_map;
  static inline SyscallMap syscall_map;

  // Iterates the export table of ntdll to find every syscall stub and builds an
  // ordered map with them. Will build an ordered map because
  // `ExtractSyscallsNumbers` needs it this way.

  static void ExtractStubs() noexcept;

  // `stub_map` is ordered from lowest to highest using the stub address.
  // Syscalls numbers are assigned using this ordering too. The lowest stub
  // address will be the stub with the lowest syscall number (0 in this case).
  // We just need to iterate `stub_map` and iterate the syscall number on every
  // iteration.

  static inline void ExtractSyscallsNumbers() noexcept {
    uint32_t syscall_no = 0;

    for (const auto &pair : stub_map) {
      syscall_map.insert({pair.second, syscall_no});
      syscall_no++;
    }
  };

  static DWORD64 djb2(PBYTE str) noexcept {
    DWORD64 dwHash = 0x406275696c64657;
    INT c;

    while (c = *str++) {
      dwHash = ((dwHash << 0x5) + dwHash) + c;
    }
    return dwHash;
  }

  // Tries to locate the syscall instruction inside a stub using some known
  // patterns. Returns the address of the instruction.

  [[nodiscard]] static inline uintptr_t
  FindSyscallInstruction(uintptr_t stub_addr) noexcept {
    uintptr_t instruction_addr;

    // Since Windows 10 TH2
    if (*(reinterpret_cast<unsigned char *>(stub_addr + 0x12)) == 0x0F &&
        *(reinterpret_cast<unsigned char *>(stub_addr + 0x13)) == 0x05) {
      instruction_addr = stub_addr + 0x12;
    }

    // From Windows XP to Windows 10 TH2
    else if (*(reinterpret_cast<unsigned char *>(stub_addr + 0x8)) == 0x0F &&
             *(reinterpret_cast<unsigned char *>(stub_addr + 0x9)) == 0x05) {
      instruction_addr = stub_addr + 0x8;
    } else {
      instruction_addr = 0;
    }

    return instruction_addr;
  };

  // Calls the service associated to the `syscall_no` service number using the
  // `stub_addr` stub and passing `args` as parameters. Returns a FunctionResult
  // that represents the result of the call.

  template <typename... ServiceArgs>
  NtStatus InternalCaller(uint32_t syscall_no, uintptr_t stub_addr,
                          ServiceArgs... args) noexcept {
    using StubDef = NtStatus(__stdcall *)(uint32_t, ServiceArgs...);
    StubDef stub = reinterpret_cast<decltype(stub)>(stub_addr);
    NtStatus return_value =
        stub(syscall_no, std::forward<ServiceArgs>(args)...);

    return return_value;
  }

  // Private constructor
  Syscall() noexcept {
    ExtractStubs();
    ExtractSyscallsNumbers();
  };

public:
  // Disable any other constructor or assignment operator
  Syscall(const Syscall &) = delete;
  Syscall &operator=(const Syscall &) = delete;
  Syscall(Syscall &&) = delete;
  Syscall &operator=(Syscall &&) = delete;

  // Singleton instance getter
  static inline Syscall &get_instance() noexcept {
    static Syscall instance;
    return instance;
  }

  // Iterates `stub_map` in search of `stub_name` address. Before start
  // iterating it lookups the cache.

  [[nodiscard]] uintptr_t GetStubAddr(DWORD64 stub_name);

  // Iterates `syscall_map` in search of the associated service number of
  // `stub_name`. Before start iterating it lookups the cache.

  [[nodiscard]] uint32_t GetSyscallNumber(DWORD64 stub_name);

  // Gets the corresponding service number given the name of the stub and
  // decides when to use the manual or the masked stub depending on if it can
  // find the syscall instruction inside the original stub. Returns a
  // FunctionResult that represents the result of the call.

  template <typename... ServiceArgs>
  NtStatus CallSyscall(DWORD64 stub_name, ServiceArgs... args) {
    uintptr_t spoofed_stub_addr;
    uintptr_t spoofed_syscall_inst_addr;

    spoofed_stub_addr = GetStubAddr(0x7c11e9b1caef31f1);
    spoofed_syscall_inst_addr = FindSyscallInstruction(spoofed_stub_addr);

    uint32_t syscall_no;
    uintptr_t stub_addr;
    uintptr_t syscall_inst_addr;

    stub_addr = GetStubAddr(stub_name);
    syscall_no = GetSyscallNumber(stub_name);
    syscall_inst_addr = FindSyscallInstruction(stub_addr);

    return InternalCaller(
        syscall_no, reinterpret_cast<uintptr_t>(&masked_syscall_stub),
        spoofed_syscall_inst_addr, std::forward<ServiceArgs>(args)...);
  }
};

} // namespace freshycalls

#endif // SISYPHUS_SYSCALL_SYSCALL_HPP_
