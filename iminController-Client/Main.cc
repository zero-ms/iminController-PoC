#include "Main.h"

int main() {
  if (isDuplicateRunning()) {
    return -1;
  }

  MainHandler::startHandler();
}

bool isDuplicateRunning() {
  HANDLE MutexHandle = NULL;

  MutexHandle = CreateMutexW(NULL, FALSE, L"imin2023unit");
  if (MutexHandle == NULL) {
    return true;
  }

  if (GetLastError() == ERROR_ALREADY_EXISTS) {
    CloseHandle(MutexHandle);
    return true;
  }

  return false;
}
