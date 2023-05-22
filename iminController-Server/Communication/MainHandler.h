#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <thread>
#include <vector>
#include <WinSock2.h>

#include "../Bots/Bot.h"
#include "../Util/Variable.h"
#include "../Communication/Packet.h"
#include "../Bots/BotHandler.h"

class MainHandler {
private:
  bool isNewClientSignature(char *Payload);
  void decideConnectType(SOCKET ClientHandle, SOCKADDR_IN ClientAddr);
  void acceptConnect(SOCKET ListenHandle);
  void startListen();

  static MainHandler *Instance;

public:
  static MainHandler *getInstance();
  void startListenThread();
};