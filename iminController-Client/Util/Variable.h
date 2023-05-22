#pragma once

#include <Windows.h>

#include "../Libraries/xorstr/xorstr.hpp"

class Variable {
private:
  char ServerAddress[512];
  const int Port = 5235;
  char NewClientSignature[4] = {'\x01', '\x02', '\x03', '\x04'};
  char PingSignature[4] = {'\x04', '\x09', '\x07', '\x06'};
  char SuccessSignature[4] = {'\x01', '\x01', '\x05', '\x06'};
  char FailSignature[4] = {'\x04', '\x06', '\x07', '\x08'};
  char LoadPluginSignature[4] = {'\x08', '\x01', '\x03', '\x05'};
  char SplitterSignature[7] = "NMH523";

  Variable();
  static Variable *Instance;

  bool compareByte(char *Payload, const char *StandardPayload) {
    for (int i = 0; i < 4; i++) {
      if (Payload[i] != StandardPayload[i]) {
        return false;
      }
    }
    return true;
  }

public:
#define PACKET_SIZE 1024
#define REAL_PACKET_SIZE 1300

  typedef enum class Signature {
    NEW_CLIENT,
    NEW_PLUGIN,
    PING,
    RESPONSE_SUCCESS,
    RESPONSE_FAIL,
    LOAD_PLUGIN,
    NONE,
  };

  typedef enum class PacketType {
    ALIVE,
    LOAD_PLUGIN,
    NONE,
  };

  static Variable *getInstance();

  bool compareSignature(Variable::Signature sig, char *Payload);

  Variable::Signature findSignature(char *Payload);

  void allocateSignature(Variable::Signature sig, char *Payload);

  char *getServerAddress();

  int getPort();

  char *getSplitter();
};