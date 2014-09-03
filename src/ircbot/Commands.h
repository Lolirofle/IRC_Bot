#ifndef __LOLIROFLE_IRCBOT_COMMANDS_H_INCLUDED__
#define __LOLIROFLE_IRCBOT_COMMANDS_H_INCLUDED__

#include <lolien/types.h>
#include <lolien/seq/DynamicArray.h>
#include "api/Commands.h"

bool initCommands(DynamicArray* commands);
void freeCommands(DynamicArray* commands);

#endif
