#include "Task.h"

Task::Task(Variable::TaskType Type) { this->Type = Type;
  memset(this->TaskParam_1, 0, 255);
  memset(this->TaskParam_2, 0, 255);
  memset(this->TaskParam_3, 0, 255);
  memset(this->TaskParam_4, 0, 255);
}

void Task::setTaskPeriod(int Minutes) { this->Period = Minutes; }

int Task::getTaskPeriodAsMinute() { return this->Period; }

void Task::setLastExecuteMillis(uint64_t millis) {
  this->LastExecuteMillis = millis;
}

uint64_t Task::getLastExecuteMillis() { return this->LastExecuteMillis; }

void Task::setTaskParam_1(char *Param) {
  memcpy(this->TaskParam_1, Param, strlen(Param));
}

void Task::setTaskParam_2(char *Param) {
  memcpy(this->TaskParam_2, Param, strlen(Param));
}

void Task::setTaskParam_3(char *Param) {
  memcpy(this->TaskParam_3, Param, strlen(Param));
}

void Task::setTaskParam_4(char *Param) {
  memcpy(this->TaskParam_4, Param, strlen(Param));
}

char *Task::getTaskParam_1() { return this->TaskParam_1; }

char *Task::getTaskParam_2() { return this->TaskParam_2; }

char *Task::getTaskParam_3() { return this->TaskParam_3; }

char *Task::getTaskParam_4() { return this->TaskParam_4; }
