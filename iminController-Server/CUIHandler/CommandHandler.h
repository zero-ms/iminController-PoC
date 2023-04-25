#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../Bots/BotHandler.h"
#include "../Task/Task.h"
#include "../Task/TaskHandler.h"

class CommandHandler {
private:
  std::vector<std::string> splitCommand(std::string Command, char Delimiter);
  std::vector<std::string> SplittedCommand;
  std::string RawCommand;

public:
  CommandHandler(std::string RawCommand) { this->RawCommand = RawCommand; }
  int checkCommand();
  int executeCommand();
};