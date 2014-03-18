#ifndef __LOLIROFLE_IRCBOT_API_COMMAND_H_INCLUDED__
#define __LOLIROFLE_IRCBOT_API_COMMAND_H_INCLUDED__

#include <lolie/Stringp.h>
#include "CommandArgument.h"
#include "CommandParameter.h"
struct IRCBot;

/**
 * Structure representing a command
 */
struct Command{
	//Name of the command (Call name)
	Stringcp name;

	//Help string
	Stringcp help;

	//Function that the command executes
	bool(*func)(struct IRCBot* bot,Stringcp target,union CommandArgument* arg);

	//Parameter data
	unsigned int parameterCount;
	struct CommandParameter parameters[];
};

#endif
