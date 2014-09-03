#ifndef __LOLIROFLE_IRCBOT_API_COMMAND_H_INCLUDED__
#define __LOLIROFLE_IRCBOT_API_COMMAND_H_INCLUDED__

#include <lolien/seq/StringP.h>
#include "CommandParameter.h"
#include "CommandArgument.h"
struct IRCBot;

/**
 * Structure representing a command
 */
struct Command{
	//Name of the command (Call name)
	StringCP name;

	//Help string
	StringCP help;

	//Function that the command executes
	bool(*func)(struct IRCBot* bot,StringCP target,struct CommandArgument* args);

	//Parameter data
	unsigned int parameterCount;
	struct CommandParameter parameters[];
};

#endif
