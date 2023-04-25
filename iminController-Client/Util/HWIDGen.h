#pragma once

#include <Windows.h>
#include <iostream>
#include <string>

static void genRandomHWID(char *Buffer, int Length) {
  static const char kAlphaNum[] = "0123456789";
  srand((unsigned)time(NULL));
  for (int i = 0; i < Length; ++i) {
    Buffer[i] = kAlphaNum[rand() % (sizeof(kAlphaNum) - 1)];
  }
}

static std::string getHWID() {
  DWORD SerialNumber = 0;
  if (GetVolumeInformationW(L"C:\\", NULL, MAX_PATH + 1, &SerialNumber, NULL,
                            NULL, NULL, MAX_PATH + 1)) {
    return std::to_string(SerialNumber);
  } else {
    char Buffer[10];
    genRandomHWID(Buffer, 10);
    return std::string(Buffer);
  }
}