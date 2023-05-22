#include "TaskHandler.h"

TaskHandler *TaskHandler::Instance = nullptr;

void TaskHandler::registerTaskToAllBots(Task *task) {
  BotHandler::getInstance()->addTaskToAllBots(task);
}

uint64_t TaskHandler::getCurrentTimeMillis() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
      .count();
}

void TaskHandler::startTaskThread() {
  std::thread TaskThread(&TaskHandler::taskThread, this);
  TaskThread.detach();
}

int TaskHandler::getSize() { return TaskPool.size(); }

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

TaskHandler *TaskHandler::getInstance() {
  if (Instance == NULL) {
    Instance = new TaskHandler();
  }

  return Instance;
}

void TaskHandler::addTask(Task *task) {
  task->setLastExecuteMillis(getCurrentTimeMillis() +
                             task->getTaskPeriodAsMinute() * 60000);
  TaskPool.insert(TaskPair(Order, task));
  printf("New Task Order: %d\n", Order);
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
