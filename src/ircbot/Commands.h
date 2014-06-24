#ifndef __LOLIROFLE_IRCBOT_COMMANDS_H_INCLUDED__
#define __LOLIROFLE_IRCBOT_COMMANDS_H_INCLUDED__

#include <lolien/types.h>
#include <lolie/Stringp.h>
#include "api/Commands.h"

bool initCommands(struct DynamicArray* commands);
void freeCommands(struct DynamicArray* commands);

#endif
