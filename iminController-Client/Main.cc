#include "Main.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpszCmdParam, int nCmdShow) {
  if (isDuplicateRunning()) {
    return -1;
  }

  LoadLibraryW(xorstr_(L"wininet.dll"));

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
