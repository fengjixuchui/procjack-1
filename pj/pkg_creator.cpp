#include <windows.h>
#include <versionhelpers.h>
#include <cstdint>
#include <string>
#include <stdio.h>
#include "../common.h"
#include "pkg_creator.h"

#define LOGERROR wprintf
#define LOGINFO LOGERROR

bool AddPermissionForAppContainer(LPCWSTR Filename);

template<typename T>
static T *at(void *base, uint32_t offset) {
  return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(base) + offset);
}

void PackageCreator::FillShellCode(bool is_64bit, uint16_t ordinal) {
  static uint8_t CODE64[] = {
    0x53, 0x48, 0x83, 0xec, 0x20, 0x65, 0x48, 0x8b, 0x04, 0x25, 0x30, 0x00, 0x00, 0x00, 0x48, 0x89,
    0xcb, 0x48, 0x8b, 0x50, 0x60, 0x48, 0x89, 0x91, 0x00, 0x10, 0x00, 0x00, 0x4c, 0x8b, 0x52, 0x18,
    0x48, 0x8b, 0x91, 0x08, 0x10, 0x00, 0x00, 0x49, 0x83, 0xc2, 0x20, 0x4d, 0x8b, 0x0a, 0x4d, 0x39,
    0xd1, 0x74, 0x78, 0x49, 0x8b, 0x41, 0x50, 0x48, 0x85, 0xd2, 0x75, 0x2a, 0x81, 0x38, 0x4b, 0x00,
    0x45, 0x00, 0x75, 0x22, 0x81, 0x78, 0x04, 0x52, 0x00, 0x4e, 0x00, 0x75, 0x19, 0x81, 0x78, 0x08,
    0x45, 0x00, 0x4c, 0x00, 0x75, 0x10, 0x81, 0x78, 0x0c, 0x33, 0x00, 0x32, 0x00, 0x75, 0x07, 0x66,
    0x83, 0x78, 0x10, 0x2e, 0x74, 0x2d, 0x81, 0x38, 0x6b, 0x00, 0x65, 0x00, 0x49, 0x89, 0xd0, 0x75,
    0x30, 0x81, 0x78, 0x04, 0x72, 0x00, 0x6e, 0x00, 0x75, 0x27, 0x81, 0x78, 0x08, 0x65, 0x00, 0x6c,
    0x00, 0x75, 0x1e, 0x81, 0x78, 0x0c, 0x33, 0x00, 0x32, 0x00, 0x75, 0x15, 0x66, 0x83, 0x78, 0x10,
    0x2e, 0x75, 0x0e, 0x49, 0x8b, 0x51, 0x20, 0x48, 0x89, 0x91, 0x08, 0x10, 0x00, 0x00, 0x49, 0x89,
    0xd0, 0x4d, 0x8b, 0x09, 0x4d, 0x39, 0xd1, 0x75, 0x8a, 0xeb, 0x03, 0x49, 0x89, 0xd0, 0xb8, 0x4d,
    0x5a, 0x00, 0x00, 0x66, 0x41, 0x39, 0x00, 0x0f, 0x85, 0xbd, 0x01, 0x00, 0x00, 0x41, 0x8b, 0x50,
    0x3c, 0x42, 0x81, 0x3c, 0x02, 0x50, 0x45, 0x00, 0x00, 0x0f, 0x85, 0xab, 0x01, 0x00, 0x00, 0x8d,
    0x42, 0x18, 0xb9, 0x0b, 0x01, 0x00, 0x00, 0x42, 0x0f, 0xb7, 0x04, 0x00, 0x66, 0x39, 0xc8, 0x75,
    0x07, 0xb8, 0x78, 0x00, 0x00, 0x00, 0xeb, 0x13, 0xb9, 0x0b, 0x02, 0x00, 0x00, 0x66, 0x39, 0xc8,
    0x0f, 0x85, 0x84, 0x01, 0x00, 0x00, 0xb8, 0x88, 0x00, 0x00, 0x00, 0x48, 0x89, 0x74, 0x24, 0x40,
    0x01, 0xd0, 0x48, 0x89, 0x7c, 0x24, 0x48, 0x45, 0x31, 0xc9, 0x46, 0x8b, 0x14, 0x00, 0x4d, 0x01,
    0xc2, 0x45, 0x8b, 0x5a, 0x20, 0x41, 0x8b, 0x7a, 0x24, 0x4d, 0x01, 0xc3, 0x41, 0x8b, 0x72, 0x1c,
    0x4c, 0x01, 0xc7, 0x4c, 0x01, 0xc6, 0x45, 0x39, 0x4a, 0x18, 0x0f, 0x86, 0xb9, 0x00, 0x00, 0x00,
    0x42, 0x0f, 0xb7, 0x04, 0x4f, 0x43, 0x8b, 0x0c, 0x8b, 0x4c, 0x01, 0xc1, 0x8b, 0x14, 0x86, 0x4c,
    0x01, 0xc2, 0x48, 0x83, 0xbb, 0x10, 0x10, 0x00, 0x00, 0x00, 0x75, 0x29, 0x81, 0x39, 0x4c, 0x6f,
    0x61, 0x64, 0x75, 0x21, 0x81, 0x79, 0x04, 0x4c, 0x69, 0x62, 0x72, 0x75, 0x18, 0x81, 0x79, 0x08,
    0x61, 0x72, 0x79, 0x57, 0x75, 0x0f, 0x80, 0x79, 0x0c, 0x00, 0x75, 0x09, 0x48, 0x89, 0x93, 0x10,
    0x10, 0x00, 0x00, 0xeb, 0x67, 0x48, 0x83, 0xbb, 0x18, 0x10, 0x00, 0x00, 0x00, 0x75, 0x23, 0x81,
    0x39, 0x46, 0x72, 0x65, 0x65, 0x75, 0x1b, 0x81, 0x79, 0x04, 0x4c, 0x69, 0x62, 0x72, 0x75, 0x12,
    0x81, 0x79, 0x08, 0x61, 0x72, 0x79, 0x00, 0x75, 0x09, 0x48, 0x89, 0x93, 0x18, 0x10, 0x00, 0x00,
    0xeb, 0x3a, 0x48, 0x83, 0xbb, 0x20, 0x10, 0x00, 0x00, 0x00, 0x75, 0x30, 0x81, 0x39, 0x47, 0x65,
    0x74, 0x50, 0x75, 0x28, 0x81, 0x79, 0x04, 0x72, 0x6f, 0x63, 0x41, 0x75, 0x1f, 0x81, 0x79, 0x08,
    0x64, 0x64, 0x72, 0x65, 0x75, 0x16, 0x8b, 0x41, 0x0c, 0x25, 0xff, 0xff, 0xff, 0x00, 0x3d, 0x73,
    0x73, 0x00, 0x00, 0x75, 0x07, 0x48, 0x89, 0x93, 0x20, 0x10, 0x00, 0x00, 0x41, 0xff, 0xc1, 0x45,
    0x3b, 0x4a, 0x18, 0x0f, 0x82, 0x47, 0xff, 0xff, 0xff, 0x48, 0x8b, 0x83, 0x10, 0x10, 0x00, 0x00,
    0x48, 0x8d, 0x8b, 0x00, 0x08, 0x00, 0x00, 0xff, 0xd0, 0x48, 0x8b, 0x74, 0x24, 0x40, 0x48, 0x89,
    0xc7, 0x48, 0x85, 0xc0, 0x74, 0x25, 0x4c, 0x8b, 0x83, 0x20, 0x10, 0x00, 0x00, 0xba, 0xad, 0xde,
    0x00, 0x00, 0x48, 0x89, 0xc1, 0x41, 0xff, 0xd0, 0x48, 0x85, 0xc0, 0x74, 0x05, 0x48, 0x89, 0xd9,
    0xff, 0xd0, 0x48, 0x89, 0xf9, 0xff, 0x93, 0x18, 0x10, 0x00, 0x00, 0x48, 0x8b, 0x83, 0x20, 0x10,
    0x00, 0x00, 0x48, 0x8d, 0x93, 0x8c, 0x02, 0x00, 0x00, 0x48, 0x8b, 0x8b, 0x08, 0x10, 0x00, 0x00,
    0xff, 0xd0, 0x4c, 0x8b, 0x83, 0x20, 0x10, 0x00, 0x00, 0x48, 0x8d, 0x93, 0x80, 0x02, 0x00, 0x00,
    0x48, 0x8b, 0x8b, 0x08, 0x10, 0x00, 0x00, 0x48, 0x89, 0xc7, 0x41, 0xff, 0xd0, 0x48, 0x85, 0xc0,
    0x74, 0x13, 0x48, 0x85, 0xff, 0x74, 0x0e, 0x31, 0xd2, 0x41, 0xb8, 0x00, 0x80, 0x00, 0x00, 0x48,
    0x89, 0xd9, 0x57, 0xff, 0xe0, 0x48, 0x8b, 0x7c, 0x24, 0x48, 0x48, 0x83, 0xc4, 0x20, 0x5b, 0xc3,
    0x56, 0x69, 0x72, 0x74, 0x75, 0x61, 0x6c, 0x46, 0x72, 0x65, 0x65, 0x00, 0x45, 0x78, 0x69, 0x74,
    0x54, 0x68, 0x72, 0x65, 0x61, 0x64, 0x00,
  };

  static uint8_t CODE32[] = {
    0x64, 0xa1, 0x30, 0x00, 0x00, 0x00, 0x83, 0xec, 0x10, 0x53, 0x55, 0x56, 0x57, 0x8b, 0x7c, 0x24,
    0x24, 0x89, 0x87, 0x00, 0x10, 0x00, 0x00, 0x8b, 0x68, 0x0c, 0x8b, 0x97, 0x08, 0x10, 0x00, 0x00,
    0x83, 0xc5, 0x14, 0x8b, 0x5d, 0x00, 0x39, 0xeb, 0x74, 0x75, 0x66, 0x0f, 0x1f, 0x04, 0x00, 0x8b,
    0x43, 0x28, 0x85, 0xd2, 0x75, 0x2a, 0x81, 0x38, 0x4b, 0x00, 0x45, 0x00, 0x75, 0x22, 0x81, 0x78,
    0x04, 0x52, 0x00, 0x4e, 0x00, 0x75, 0x19, 0x81, 0x78, 0x08, 0x45, 0x00, 0x4c, 0x00, 0x75, 0x10,
    0x81, 0x78, 0x0c, 0x33, 0x00, 0x32, 0x00, 0x75, 0x07, 0x66, 0x83, 0x78, 0x10, 0x2e, 0x74, 0x2c,
    0x81, 0x38, 0x6b, 0x00, 0x65, 0x00, 0x89, 0xd6, 0x75, 0x2d, 0x81, 0x78, 0x04, 0x72, 0x00, 0x6e,
    0x00, 0x75, 0x24, 0x81, 0x78, 0x08, 0x65, 0x00, 0x6c, 0x00, 0x75, 0x1b, 0x81, 0x78, 0x0c, 0x33,
    0x00, 0x32, 0x00, 0x75, 0x12, 0x66, 0x83, 0x78, 0x10, 0x2e, 0x75, 0x0b, 0x8b, 0x53, 0x10, 0x89,
    0xd6, 0x89, 0x97, 0x08, 0x10, 0x00, 0x00, 0x8b, 0x1b, 0x39, 0xeb, 0x75, 0x92, 0xeb, 0x02, 0x89,
    0xd6, 0xb8, 0x4d, 0x5a, 0x00, 0x00, 0x66, 0x39, 0x06, 0x0f, 0x85, 0x96, 0x01, 0x00, 0x00, 0x8b,
    0x4e, 0x3c, 0x81, 0x3c, 0x0e, 0x50, 0x45, 0x00, 0x00, 0x0f, 0x85, 0x86, 0x01, 0x00, 0x00, 0x0f,
    0xb7, 0x44, 0x0e, 0x18, 0xba, 0x0b, 0x01, 0x00, 0x00, 0x66, 0x39, 0xd0, 0x75, 0x07, 0xb8, 0x74,
    0x00, 0x00, 0x00, 0xeb, 0x13, 0xba, 0x0b, 0x02, 0x00, 0x00, 0x66, 0x39, 0xd0, 0x0f, 0x85, 0x62,
    0x01, 0x00, 0x00, 0xb8, 0x84, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x31, 0xdb, 0x8b, 0x6c, 0x08, 0x04,
    0x8b, 0x44, 0x35, 0x20, 0x01, 0xf5, 0x01, 0xf0, 0x89, 0x44, 0x24, 0x10, 0x8b, 0x55, 0x24, 0x8b,
    0x4d, 0x1c, 0x01, 0xf2, 0x01, 0xf1, 0x89, 0x54, 0x24, 0x14, 0x89, 0x4c, 0x24, 0x24, 0x39, 0x5d,
    0x18, 0x0f, 0x86, 0xbc, 0x00, 0x00, 0x00, 0x0f, 0x1f, 0x04, 0x00, 0x8b, 0x0c, 0x98, 0x0f, 0xb7,
    0x04, 0x5a, 0x01, 0xf1, 0x8b, 0x54, 0x24, 0x24, 0x8b, 0x14, 0x82, 0x01, 0xf2, 0x83, 0xbf, 0x10,
    0x10, 0x00, 0x00, 0x00, 0x75, 0x28, 0x81, 0x39, 0x4c, 0x6f, 0x61, 0x64, 0x75, 0x20, 0x81, 0x79,
    0x04, 0x4c, 0x69, 0x62, 0x72, 0x75, 0x17, 0x81, 0x79, 0x08, 0x61, 0x72, 0x79, 0x57, 0x75, 0x0e,
    0x80, 0x79, 0x0c, 0x00, 0x75, 0x08, 0x89, 0x97, 0x10, 0x10, 0x00, 0x00, 0xeb, 0x63, 0x83, 0xbf,
    0x18, 0x10, 0x00, 0x00, 0x00, 0x75, 0x22, 0x81, 0x39, 0x46, 0x72, 0x65, 0x65, 0x75, 0x1a, 0x81,
    0x79, 0x04, 0x4c, 0x69, 0x62, 0x72, 0x75, 0x11, 0x81, 0x79, 0x08, 0x61, 0x72, 0x79, 0x00, 0x75,
    0x08, 0x89, 0x97, 0x18, 0x10, 0x00, 0x00, 0xeb, 0x38, 0x83, 0xbf, 0x20, 0x10, 0x00, 0x00, 0x00,
    0x75, 0x2f, 0x81, 0x39, 0x47, 0x65, 0x74, 0x50, 0x75, 0x27, 0x81, 0x79, 0x04, 0x72, 0x6f, 0x63,
    0x41, 0x75, 0x1e, 0x81, 0x79, 0x08, 0x64, 0x64, 0x72, 0x65, 0x75, 0x15, 0x8b, 0x41, 0x0c, 0x25,
    0xff, 0xff, 0xff, 0x00, 0x3d, 0x73, 0x73, 0x00, 0x00, 0x75, 0x06, 0x89, 0x97, 0x20, 0x10, 0x00,
    0x00, 0x8b, 0x44, 0x24, 0x10, 0x43, 0x8b, 0x54, 0x24, 0x14, 0x3b, 0x5d, 0x18, 0x0f, 0x82, 0x48,
    0xff, 0xff, 0xff, 0x8b, 0x8f, 0x10, 0x10, 0x00, 0x00, 0x8d, 0x87, 0x00, 0x08, 0x00, 0x00, 0x50,
    0xff, 0xd1, 0x89, 0xc6, 0x85, 0xf6, 0x74, 0x1e, 0x8b, 0x8f, 0x20, 0x10, 0x00, 0x00, 0x68, 0xad,
    0xde, 0x00, 0x00, 0x56, 0xff, 0xd1, 0x85, 0xc0, 0x74, 0x03, 0x57, 0xff, 0xd0, 0x8b, 0x87, 0x18,
    0x10, 0x00, 0x00, 0x56, 0xff, 0xd0, 0x8b, 0x87, 0x20, 0x10, 0x00, 0x00, 0x8d, 0x8f, 0x5b, 0x02,
    0x00, 0x00, 0x51, 0xff, 0xb7, 0x08, 0x10, 0x00, 0x00, 0xff, 0xd0, 0x8b, 0x8f, 0x20, 0x10, 0x00,
    0x00, 0x89, 0xc6, 0x8d, 0x87, 0x4f, 0x02, 0x00, 0x00, 0x50, 0xff, 0xb7, 0x08, 0x10, 0x00, 0x00,
    0xff, 0xd1, 0x85, 0xc0, 0x74, 0x0f, 0x85, 0xf6, 0x74, 0x0b, 0x68, 0x00, 0x80, 0x00, 0x00, 0x6a,
    0x00, 0x57, 0x56, 0xff, 0xe0, 0x5f, 0x5e, 0x5d, 0x5b, 0x83, 0xc4, 0x10, 0xc2, 0x04, 0x00, 0x56,
    0x69, 0x72, 0x74, 0x75, 0x61, 0x6c, 0x46, 0x72, 0x65, 0x65, 0x00, 0x45, 0x78, 0x69, 0x74, 0x54,
    0x68, 0x72, 0x65, 0x61, 0x64, 0x00,
  };

  static_assert(SHELLCODE_CAPACITY >= sizeof(CODE64),
                "Shellcode is too big.  Consider to increase SHELLCODE_CAPACITY.");
  static_assert(SHELLCODE_CAPACITY >= sizeof(CODE32),
                "Shellcode is too big.  Consider to increase SHELLCODE_CAPACITY.");

  if (is_64bit) {
    *at<uint32_t>(CODE64, 0x20e) = ordinal;
    CopyMemory(package_->nw.shellcode, CODE64, sizeof(CODE64));
  }
  else {
    *at<uint32_t>(CODE32, 0x1ef) = ordinal;
    CopyMemory(package_->nw.shellcode, CODE32, sizeof(CODE32));
  }
}

PackageCreator::PackageCreator()
  : blob_(HeapAlloc(GetProcessHeap(), /*dwFlags*/HEAP_ZERO_MEMORY, sizeof(Package)))
{}

PackageCreator::~PackageCreator() {
  if (blob_) {
    HeapFree(GetProcessHeap(), /*dwFlags*/0, blob_);
  }
}

bool PackageCreator::Fill(bool is_64bit,
                          LPCWSTR filepath,
                          int32_t ordinal,
                          const std::string &args) {
  if (!blob_) return false;

  // Use ordinal to decide to inject blob as DLL or a flat binary
  if (ordinal >= 0 && ordinal <= 0xFFFF) {
    // Inject as DLL
    if (IsWindows8OrGreater())
      AddPermissionForAppContainer(filepath);
    if (!GetFullPathName(filepath,
                         MAX_PATH,
                         reinterpret_cast<LPWSTR>(package_->nw.dllpath),
                         /*lpFilePart*/nullptr)) {
      LOGERROR(L"GetFullPathName failed - %08x\n", GetLastError());
      return false;
    }
    args.copy(package_->nw.args, min(args.size(), ARGS_CAPACITY), 0);
    FillShellCode(is_64bit, static_cast<WORD>(ordinal));
    return true;
  }

  // Inject as a flat binary
  HANDLE h = CreateFile(filepath,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        /*lpSecurityAttributes*/nullptr,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        /*hTemplateFile*/nullptr);
  if (h == INVALID_HANDLE_VALUE) {
    LOGERROR(L"CreateFile failed - %08x\n", GetLastError());
    return false;
  }

  bool is_success = false;
  DWORD written = 0;
  if (ReadFile(h,
               blob_,
               sizeof(Package),
               &written,
               /*lpOverlapped*/nullptr))
    is_success = true;
  else
    LOGERROR(L"ReadFile failed - %08x\n", GetLastError());

  CloseHandle(h);
  return is_success;
}

PVOID PackageCreator::Inject(HANDLE target_process) {
  bool ret = false;

  PVOID promise_land = VirtualAllocEx(target_process,
                                      nullptr,
                                      sizeof(Package),
                                      MEM_RESERVE | MEM_COMMIT,
                                      PAGE_EXECUTE_READWRITE);
  if (!promise_land) {
    LOGERROR(L"VirtualAllocEx failed - %08x\n", GetLastError());
    goto cleanup;
  }

  SIZE_T written = 0;
  if (!WriteProcessMemory(target_process,
                          promise_land,
                          blob_,
                          sizeof(Package),
                          &written)) {
    LOGERROR(L"WriteProcessMemory failed - %08x\n", GetLastError());
    goto cleanup;
  }

  DWORD old_protect;
  if (!VirtualProtectEx(target_process,
                        promise_land,
                        sizeof(Package::NonWritable),
                        PAGE_EXECUTE_READ,
                        &old_protect)) {
    LOGERROR(L"VirtualProtectEx(RE) failed - %08x\n", GetLastError());
    goto cleanup;
  }
  if (!VirtualProtectEx(target_process,
                        at<uint8_t>(promise_land, sizeof(Package::NonWritable)),
                        sizeof(Package) - sizeof(Package::NonWritable),
                        PAGE_READWRITE,
                        &old_protect)) {
    LOGERROR(L"VirtualProtectEx(RW) failed - %08x\n", GetLastError());
    goto cleanup;
  }

  ret = true;

cleanup:
  if (!ret) {
    VirtualFreeEx(target_process, promise_land, 0, MEM_RELEASE);
    promise_land = nullptr;
  }
  return promise_land;
}
