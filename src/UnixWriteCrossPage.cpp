#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

uint8_t *buff;
size_t pageSize;
uint64_t *pBuffEnd;
constexpr uint64_t vBuffEnd = 0x0807060504030201U;

void signalHandler(int32_t const signalId, siginfo_t *const si,
                   void *const ptr) noexcept {
  if (signalId == SIGSEGV) {
    char outStr[100];
    memset(outStr, 0, sizeof(outStr));
    snprintf(outStr, sizeof(outStr), "buffEnd is %lx\n", *pBuffEnd);
    write(STDOUT_FILENO, outStr, sizeof(outStr));
    if (*pBuffEnd == vBuffEnd) {
      exit(0);
    } else {
      exit(3);
    }
  }
}

int main() {
  struct sigaction sa = {};
  sa.sa_sigaction = signalHandler;
  sa.sa_flags = SA_SIGINFO;
  sigfillset(&sa.sa_mask);
  sigaction(SIGSEGV, &sa, nullptr);

  pageSize = sysconf(_SC_PAGE_SIZE);

  buff = reinterpret_cast<uint8_t *>(mmap(nullptr, pageSize * 2U, PROT_NONE,
                                          MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));

  if (buff == nullptr) {
    perror("mmap PROT_NONE failed\n");
    exit(1);
  }

  const int32_t error = mprotect(buff, pageSize, PROT_READ | PROT_WRITE);

  if (error != 0) {
    perror("mprotect failed");
    exit(1);
  }

  pBuffEnd = reinterpret_cast<uint64_t *>(buff + pageSize - sizeof(uint64_t));
  *pBuffEnd = vBuffEnd;

  uint32_t *probePtr = reinterpret_cast<uint32_t *>(buff + pageSize - 1U);

  *probePtr = 0x5A5A5A5AU;

  return 0;
}
