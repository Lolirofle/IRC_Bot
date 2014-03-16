#ifndef __LOLIROFLE_IRCBOT_API_IRCBOT_H_INCLUDED__
#define __LOLIROFLE_IRCBOT_API_IRCBOT_H_INCLUDED__

#include <lolie/TypeAliases.h>
#include <lolie/Stringp.h>
#include <lolie/DynamicArray.h>
#include <lolie/LinkedList.h>
#include "IRCBot_Error.h"
#include "Plugin.h"
struct irc_connection;

#define IRCBOT_NAME    "Flygande Toalett Toabot"
#define IRCBOT_VERSION "1.2.0"

extern const Stringcp IRCBot_signature;

/**
 * Buffer initialization
 */
#define IRC_WRITE_BUFFER_LEN 512
char write_buffer[IRC_WRITE_BUFFER_LEN];

struct IRCBot{
	struct irc_connection* connection;
	Stringp hostname;
	Stringp nickname;
	Stringp username;
	Stringp realname;
	bool connected;
	struct IRCBot_Error error;
	LinkedList/*<const String*>*/* channels;

	Stringp commandPrefix;
	struct DynamicArray/*<LinkedList<struct Command*>*>*/ commands;

	LinkedList/*<struct Plugin*>*/* plugins;

	struct{
		LinkedList* onConnect;
		LinkedList* onDisconnect;
		LinkedList* onCommand;
		LinkedList* onMessage;
		LinkedList* onJoin;
		LinkedList* onPart;
		LinkedList* onUserJoin;
		LinkedList* onUserPart;
	}pluginHooks;

	enum IRCBot_Exit{
		IRCBOT_EXIT_FALSE,
		IRCBOT_EXIT_SHUTDOWN,
		IRCBOT_EXIT_RESTART,
		IRCBOT_EXIT_RELOADPLUGINS
	}exit;
};

/**
 * Returns whether the bot is connected
 *
 * @param  <struct IRCBot*> bot The bot to perform the action on
 * @return <bool>               True if connected
 */
#define IRCBot_isConnected(bot) ((bot)->connected)

 /**
 * Shutdown the bot
 *
 * @param <struct IRCBot*> bot The bot to perform the action on
 */
#define IRCBot_shutdown(bot) ((bot)->exit=IRCBOT_EXIT_SHUTDOWN)

 /**
 * Restart the bot
 *
 * @param <struct IRCBot*> bot The bot to perform the action on
 */
#define IRCBot_restart(bot) ((bot)->exit=IRCBOT_EXIT_RESTART)

 /**
 * Reload the plugins on the bot
 *
 * @param <struct IRCBot*> bot The bot to perform the action on
 */
#define IRCBot_reloadPlugins(bot) ((bot)->exit=IRCBOT_EXIT_RELOADPLUGINS)

//IRCBot must be connected for these functions
void IRCBot_setNickname(struct IRCBot* bot,Stringcp name);
void IRCBot_setUsername(struct IRCBot* bot,Stringcp name);
void IRCBot_setRealname(struct IRCBot* bot,Stringcp name);

void IRCBot_setCommandPrefix(struct IRCBot* bot,Stringcp prefix);
void IRCBot_setCommandPrefixc(struct IRCBot* bot,char prefix);

void IRCBot_joinChannel(struct IRCBot* bot,Stringcp channel);
void IRCBot_partChannel(struct IRCBot* bot,Stringcp channel);

void IRCBot_sendMessage(struct IRCBot* bot,Stringcp target,Stringcp message);
void IRCBot_sendRaw(struct IRCBot* bot,Stringcp str);

 /**
 * Performs a registered command on the bot
 * (The command string should include the command and the arguments if any)
 *
 * @param bot           The bot to perform the action on
 * @param target        The target for sending messages to
 * @param command_begin Pointer to the beginning of the command string
 * @param command_end   Pointer to the end of the command string
 */
void IRCBot_performCommand(struct IRCBot* bot,Stringcp target,const char* command_begin,const char* command_end);

#endif
