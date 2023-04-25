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
  static bool isNewClientSignature(char *Payload);
  static void decideConnectType(SOCKET ClientHandle, SOCKADDR_IN ClientAddr);
  static void acceptConnect(SOCKET ListenHandle);
  static void startListen();

public:
  static void startListenThread();
};