#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <Windows.h>

#include "../Util/Variable.h"
#include "../Communication/Packet.h"
#include "../Util/HWIDGen.h"
#include "../Plugin/PluginHandler.h"

class MainHandler {
private:
  static SOCKET ServerSocket;

  static bool sendInitPacket();
  static bool sendPingPacket(Packet *AlivePacket);
  static void handleRequest(char *Payload);
  static void waitConnect();
  static bool tryConnect();

public:
  static void startHandler();
};
