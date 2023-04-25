#pragma once

#include <WinSock2.h>
#include <stdexcept>
#include <string>
#include <vector>

#include "../Util/Aes.h"
#include "../Util/Variable.h"

class Packet {
private:
  char *Payload = nullptr;
  int PayloadSize = 0;
  enum Variable::PacketType Type = Variable::PacketType::NONE;
  enum Variable::Signature Sig = Variable::Signature::NONE;
  SOCKET Socket;

  void clearPayload();
  bool isPacketReady();

  static uint8_t Key[];
  static uint8_t IV[];

public:
  Packet(SOCKET Socket, char *Payload) {
    this->Socket = Socket;
    this->Payload = Payload;
  }
  Packet(SOCKET Socket);
  ~Packet();

  bool receivePacket();
  bool sendPacket();

  Variable::PacketType getPacketType();
  void setPacketType(Variable::PacketType Type);

  Variable::Signature parseSignatureFromPayload();
  void setSignature(Variable::Signature sig);

  char *getPayload();
  void setPayload(char *Payload, int Size);
  void addParam(std::vector<char *> Params);

  void clear();

  void parseBySplitter(std::string *RawPayload,
                       std::vector<std::string> *Params);
  std::vector<std::string> parseParams();
};
