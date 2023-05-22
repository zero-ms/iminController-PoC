#include "Packet.h"

uint8_t Packet::Key[] = {0x15, 0x3e, 0x1f, 0xa5, 0x1a, 0xaf, 0xbd, 0x6a,
                         0xbb, 0xe1, 0x0e, 0xc8, 0xfe, 0x1c, 0x1d, 0x3e};
uint8_t Packet::IV[] = {0x4a, 0x3f, 0xcc, 0x1d, 0xda, 0x52, 0x35, 0x18,
                        0xc9, 0xb0, 0x0a, 0x4d, 0x7e, 0xf2, 0x1c, 0xdc};

void Packet::clearPayload() {
  if (this->Payload != nullptr) {
    delete[] this->Payload;
    this->Payload = nullptr;
    PayloadSize = 0;
  }
}

bool Packet::isPacketReady() {
  return (PayloadSize > 0 && PayloadSize <= PACKET_SIZE);
}

Packet::Packet(SOCKET Socket) {
  this->Socket = Socket;
  // this->Payload = new char[1024];
}

Packet::~Packet() { clearPayload(); }

bool Packet::receivePacket() {
  clear();
  this->Payload = new char[PACKET_SIZE];
  this->PayloadSize = PACKET_SIZE;
  memset(Payload, 0, PACKET_SIZE);

  DWORD Result = recv(this->Socket, Payload, PACKET_SIZE, 0);

  if (Result == SOCKET_ERROR) {
    return false;
  } else if (Result == PACKET_SIZE) {
    uint8_t *DecryptPayload = (uint8_t *)Payload;
    struct AES_ctx CTX;
    AES_init_ctx_iv(&CTX, Key, IV);
    AES_CBC_decrypt_buffer(&CTX, DecryptPayload, PACKET_SIZE);

    return true;
  } else {
    return false;
  }
}

bool Packet::sendPacket() {
  if (isPacketReady()) {
    uint8_t *EncryptPayload = (uint8_t *)Payload;
    struct AES_ctx CTX;
    AES_init_ctx_iv(&CTX, Key, IV);
    AES_CBC_encrypt_buffer(&CTX, EncryptPayload, PACKET_SIZE);

    DWORD Result = send(this->Socket, (char *)EncryptPayload, PACKET_SIZE, 0);

    if (Result == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET) {
      return false;
    } else if (Result == PACKET_SIZE) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

Variable::PacketType Packet::getPacketType() { return this->Type; }

void Packet::setPacketType(Variable::PacketType Type) { this->Type = Type; }

Variable::Signature Packet::parseSignatureFromPayload() {
  if (isPacketReady()) {
    this->Sig = Variable::getInstance()->findSignature(this->Payload);
    return this->Sig;
  } else {
    return Variable::Signature::NONE;
  }
}

void Packet::setSignature(Variable::Signature sig) {
  clearPayload();
  this->Payload = new char[PACKET_SIZE];
  this->PayloadSize = PACKET_SIZE;
  this->Sig = sig;
  memset(Payload, 0, 1024);
  Variable::getInstance()->allocateSignature(sig, this->Payload);
}

char *Packet::getPayload() { return this->Payload; }

void Packet::setPayload(char *Payload, int Size) {
  if (isPacketReady() && this->Sig != Variable::Signature::NONE) {
    if (Size <= PACKET_SIZE) {
      memcpy(&(this->Payload[4]), Payload, Size);
    }
  } else {
    clearPayload();
    this->Payload = Payload;
    this->PayloadSize = Size;
  }
}

void Packet::addParam(std::vector<char *> Params) {
  if (isPacketReady() && this->Sig != Variable::Signature::NONE) {
    int ArrayIndex = 4;
    for (char *element : Params) {
      memcpy(&(this->Payload[ArrayIndex]),
             Variable::getInstance()->getSplitter(),
             strlen(Variable::getInstance()->getSplitter()));
      ArrayIndex += strlen(Variable::getInstance()->getSplitter());
      memcpy(&(this->Payload[ArrayIndex]), element, strlen(element));
      ArrayIndex += strlen(element);
    }
  }
}

void Packet::parseBySplitter(std::string *RawPayload,
                             std::vector<std::string> *Params) {
  size_t pos = 0;
  std::string delimiter(Variable::getInstance()->getSplitter());
  std::string token;
  while ((pos = RawPayload->find(delimiter)) != std::string::npos) {
    token = RawPayload->substr(0, pos);
    Params->push_back(token);
    RawPayload->erase(0, pos + delimiter.length());
  }
  Params->push_back(*RawPayload);
}

void Packet::clear() {
  this->Type = Variable::PacketType::NONE;
  this->Sig = Variable::Signature::NONE;
  clearPayload();
}

std::vector<std::string> Packet::parseParams() {
  std::vector<std::string> Params;
  std::string RawPayload(&(this->Payload[4]));
  parseBySplitter(&RawPayload, &Params);

  return Params;
}
