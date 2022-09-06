#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <windows.h>

uint8_t *buff;
size_t pageSize;
uint64_t *pBuffEnd;
constexpr uint64_t vBuffEnd = 0x0807060504030201U;

LONG signalHandler(PEXCEPTION_POINTERS pExceptionInfo) noexcept {

  if (pExceptionInfo->ExceptionRecord->ExceptionCode ==
      EXCEPTION_ACCESS_VIOLATION) {

    char outStr[100];
    memset(outStr, 0, sizeof(outStr));
    snprintf(outStr, sizeof(outStr), "buffEnd is %llx\n", *pBuffEnd);
    DWORD bytesWritten;
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(out, outStr, sizeof(outStr), &bytesWritten, nullptr);
    if (*pBuffEnd == vBuffEnd) {
        exit(0);
    }
    else {
        exit(3);
    }
  }
  else if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) {
      return EXCEPTION_CONTINUE_SEARCH;
  }

  return EXCEPTION_CONTINUE_SEARCH;
}

int main(int argc, char* argv[]) {
  PVOID handle = AddVectoredExceptionHandler(
      1, reinterpret_cast<PVECTORED_EXCEPTION_HANDLER>(signalHandler));
  if (handle == nullptr) {
    std::cout << "AddVectoredExceptionHandler failed" << std::endl;
    exit(1);
  }

  SYSTEM_INFO sysInfo;
  GetSystemInfo(&sysInfo);

  pageSize = sysInfo.dwPageSize;

  buff = reinterpret_cast<uint8_t *>(
      VirtualAlloc(nullptr, pageSize * 2, MEM_RESERVE, PAGE_READWRITE));

  if (buff == nullptr) {
    std::cout << "VirtualAlloc MEM_RESERVE failed with " << GetLastError()
              << std::endl;
    exit(1);
  }

  DWORD flProtect;
  
  if (argc <= 1) {
      flProtect = PAGE_READWRITE;
  }
  else {
      flProtect = PAGE_READWRITE | PAGE_GUARD;
  }
  

  void *ptr = VirtualAlloc(buff, pageSize, MEM_COMMIT, flProtect);

  if (ptr != buff) {
    std::cout << "VirtualAlloc MEM_COMMIT failed with " << GetLastError()
              << std::endl;
  }

  pBuffEnd =
      reinterpret_cast<uint64_t *>(buff + pageSize - sizeof(uint64_t));
  *pBuffEnd = vBuffEnd;

  uint32_t *probePtr = reinterpret_cast<uint32_t *>(buff + pageSize - 1U);

  *probePtr = 0x5A5A5A5AU;

  return 0;
}