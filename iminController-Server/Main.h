#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <sstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <thread>

#include "CUIHandler/CommandHandler.h"
#include "Communication/MainHandler.h"
#include "Bots/BotHandler.h"
#include "Task/TaskHandler.h"

bool isDuplicateRunning();
void titleThread();
void startAllThread();