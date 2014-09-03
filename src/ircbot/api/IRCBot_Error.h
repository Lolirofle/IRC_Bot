#ifndef __LOLIROFLE_IRCBOT_IRCBOT_ERROR_H_INCLUDED__
#define __LOLIROFLE_IRCBOT_IRCBOT_ERROR_H_INCLUDED__

#include <lolien/types.h>
#include <lolien/seq/StringP.h>

struct IRCBot_Error{
	ushort code;
	StringP message;
};

#define IRCBot_Error_init ((struct IRCBot_Error){0,StringP_init})

#define IRCBOT_ERROR_CONNECT    1
#define IRCBOT_ERROR_MEMORY     2
#define IRCBOT_ERROR_DISCONNECT 3

#endif
