#include "TaskHandler.h"

TaskHandler::TaskMap TaskHandler::TaskPool;
int TaskHandler::Order = 1;

void TaskHandler::registerTaskToAllBots(Task *task) {
  BotHandler::addTaskToAllBots(task);
}

uint64_t TaskHandler::getCurrentTimeMillis() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
      .count();
}

void TaskHandler::startTaskThread() {
  std::thread TaskThread(taskThread);
  TaskThread.detach();
}

void TaskHandler::taskThread() {
  while (true) {
    for (auto iter = getTaskPoolIteratorBegin();
         iter != getTaskPoolIteratorEnd(); iter++) {
      if ((iter->second->getLastExecuteMillis() - getCurrentTimeMillis()) >=
          iter->second->getTaskPeriodAsMinute() * 60000) {
        iter->second->setLastExecuteMillis(
            getCurrentTimeMillis() +
            iter->second->getTaskPeriodAsMinute() * 60000);
        registerTaskToAllBots(iter->second);
      }
    }
    Sleep(1000);
  }
}

void TaskHandler::addTask(Task *task) {
  task->setLastExecuteMillis(getCurrentTimeMillis() +
                             task->getTaskPeriodAsMinute() * 60000);
  TaskPool.insert(TaskPair(Order, task));
  printf("Order: %d\n", Order);
  Order += 1;
  registerTaskToAllBots(task);
}

void TaskHandler::removeTask(int Number) {
  Task *task = TaskPool[Number];
  TaskPool.erase(Number);
  delete task;
}

TaskHandler::TaskMapIter TaskHandler::getTaskPoolIteratorBegin() {
  return TaskPool.begin();
}

TaskHandler::TaskMapIter TaskHandler::getTaskPoolIteratorEnd() {
  return TaskPool.end();
}
