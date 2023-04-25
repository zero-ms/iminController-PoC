#pragma once

#include <iostream>
#include <map>
#include <chrono>

#include "../Bots/BotHandler.h"

class TaskHandler {
private:
  typedef std::map<int, class Task *> TaskMap;
  typedef TaskMap::iterator TaskMapIter;
  typedef std::pair<int, class Task *> TaskPair;
  static TaskMap TaskPool;
  static int Order;

  static uint64_t getCurrentTimeMillis();
  static void registerTaskToAllBots(Task *task);
  static void taskThread();

public:
  static void addTask(Task *task);
  static void removeTask(int Number);
  static TaskMapIter getTaskPoolIteratorBegin();
  static TaskMapIter getTaskPoolIteratorEnd();

  static void startTaskThread();
};
