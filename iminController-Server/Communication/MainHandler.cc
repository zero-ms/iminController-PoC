#include "MainHandler.h"

bool MainHandler::isNewClientSignature(char *Payload) {
  return Variable::compareSignature(Variable::Signature::NEW_CLIENT, Payload);
}

void MainHandler::decideConnectType(SOCKET ClientHandle,
                                    SOCKADDR_IN ClientAddr) {
  Packet *InitPacket = new Packet(ClientHandle);
  if (InitPacket->receivePacket()) {
    switch (InitPacket->parseSignatureFromPayload()) {
    case Variable::Signature::NEW_CLIENT: {
      std::vector<std::string> Params = InitPacket->parseParams();
      Bot *NewBot =
          new Bot(ClientHandle, inet_ntoa(ClientAddr.sin_addr), Params.at(1));
      BotHandler::addBot(NewBot);
      NewBot->startLooping();
      delete InitPacket;
      break;
    }
    case Variable::Signature::LOAD_PLUGIN: {
      break;
    }
    }
  }
}

void MainHandler::acceptConnect(SOCKET ListenHandle) {
  SOCKADDR_IN ClientAddr = {};
  int ClientAddrSize = sizeof(ClientAddr);
  while (true) {
    SOCKET ClientHandle = INVALID_SOCKET;
    ClientHandle =
        accept(ListenHandle, (SOCKADDR *)&ClientAddr, &ClientAddrSize);
    if (ClientHandle != INVALID_SOCKET) {
      decideConnectType(ClientHandle, ClientAddr);
    } else if (ClientHandle == INVALID_SOCKET) {
      continue;
    } else {
      continue;
    }
  }
}

void MainHandler::startListenThread() {
  std::thread ListenThread(&startListen);
  ListenThread.detach();
}

void MainHandler::startListen() {
  WSADATA WsaData;
  WSAStartup(MAKEWORD(2, 2), &WsaData);

  SOCKET ListenHandle;
  ListenHandle = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  SOCKADDR_IN ListenAddr = {};
  ListenAddr.sin_family = AF_INET;
  ListenAddr.sin_port = htons(Variable::getPort());
  ListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  bind(ListenHandle, (SOCKADDR *)&ListenAddr, sizeof(ListenAddr));
  listen(ListenHandle, SOMAXCONN);

  acceptConnect(ListenHandle);
}