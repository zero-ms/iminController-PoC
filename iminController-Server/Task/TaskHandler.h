#pragma once

#include <chrono>
#include <iostream>
#include <map>

#include "../Bots/BotHandler.h"

class TaskHandler {
private:
  typedef std::map<int, class Task *> TaskMap;
  typedef TaskMap::iterator TaskMapIter;
  typedef std::pair<int, class Task *> TaskPair;

  TaskMap TaskPool;
  int Order = 1;

  uint64_t getCurrentTimeMillis();
  void registerTaskToAllBots(Task *task);
  void taskThread();

  static TaskHandler *Instance;

public:
  static TaskHandler *getInstance();

  void addTask(Task *task);
  void removeTask(int Number);
  TaskMapIter getTaskPoolIteratorBegin();
  TaskMapIter getTaskPoolIteratorEnd();

  void startTaskThread();

  int getSize();
};
