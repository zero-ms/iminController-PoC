#pragma once

#include <Windows.h>

class Variable {
private:
  static char *ServerAddress;
  static const int Port = 5235;
  static char NewClientSignature[4];
  static char PingSignature[4];
  static char SuccessSignature[4];
  static char FailSignature[4];
  static char LoadPluginSignature[4];
  static char SplitterSignature[7];

  static bool compareByte(char *Payload, const char *StandardPayload) {
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

  typedef enum class TaskType {
      LOAD_PLUGIN,
      NONE,
  };

  static bool compareSignature(Variable::Signature sig, char *Payload);

  static Variable::Signature findSignature(char *Payload);

  static void allocateSignature(Variable::Signature sig, char *Payload);

  static char *getServerAddress();

  static int getPort();

  static char *getSplitter();
};