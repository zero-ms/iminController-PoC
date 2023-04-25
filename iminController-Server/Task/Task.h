#pragma once

#include <string>
#include <chrono>

#include "../Util/Variable.h"

class Task {
private:
  Variable::TaskType Type = Variable::TaskType::NONE;
  char TaskParam_1[255];
  char TaskParam_2[255];
  char TaskParam_3[255];
  char TaskParam_4[255];
  uint64_t LastExecuteMillis = 0;
  int Period = 0;

public:
  Task(Variable::TaskType Type);

  void setTaskPeriod(int Minutes);
  int getTaskPeriodAsMinute();
  void setLastExecuteMillis(uint64_t millis);
  uint64_t getLastExecuteMillis();

  void setTaskParam_1(char *Param); // normally plugin download URL
  void setTaskParam_2(char *Param); // normally plugin param 1
  void setTaskParam_3(char *Param); // param 2
  void setTaskParam_4(char *Param); // param 3

  char *getTaskParam_1();
  char *getTaskParam_2();
  char *getTaskParam_3();
  char *getTaskParam_4();
};
