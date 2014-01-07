#ifndef __LOLIROFLE_IRCBOT_IRCBOT_H_INCLUDED__
#define __LOLIROFLE_IRCBOT_IRCBOT_H_INCLUDED__

#include "api/IRCBot.h"

#include <lolie/TypeAliases.h>
#include <lolie/Stringp.h>
#include "ircinterface/irc.h"

bool IRCBot_initialize(struct IRCBot* bot);
bool IRCBot_free(struct IRCBot* bot);

bool IRCBot_connect(struct IRCBot* bot,Stringcp host,unsigned short port);
bool IRCBot_disconnect(struct IRCBot* bot);

#endif
