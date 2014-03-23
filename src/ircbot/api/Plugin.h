#ifndef __LOLIROFLE_IRCBOT_API_PLUGIN_H_INCLUDED__
#define __LOLIROFLE_IRCBOT_API_PLUGIN_H_INCLUDED__

#include <lolie/LinkedList.h>
#include <lolie/TypeAliases.h>
#include <lolie/Stringp.h>
struct IRCBot;
struct CommandArgument;
struct irc_message;

struct Plugin{
	void* lib;
	struct{
		/**
		 * Called when plugin is loaded
		 *
		 * @return Returns whether the plugin successfully loaded
		 */
		bool(*onLoad)(struct IRCBot* bot);

		/**
		 * Called when plugin is unloaded
		 *
		 * @return Returns whether the plugin successfully unloaded
		 */
		bool(*onUnload)(struct IRCBot* bot);

		/**
		 * Called before connecting to address
		 *
		 * @return Returns whether the bot should connect to the specified address
		 */
		bool(*onConnect)(struct IRCBot* bot,Stringcp hostname,unsigned short port);

		/**
		 * Called before disconnecting
		 *
		 * @return
		 */
		void(*onDisconnect)(struct IRCBot* bot);

		/**
		 * Called before checking if command exists and executing it
		 *
		 * @return Returns whether it should continue searching for a command
		 */
		bool(*onCommand)(struct IRCBot* bot,Stringcp target,Stringcp command,struct CommandArgument* arg);

		/**
		 * Called after parsing a message and before parsing commands
		 *
		 * @return Returns whether it should continue with the onMessage hooks
		 */
		bool(*onMessage)(struct IRCBot* bot,const struct irc_message* message);

		/**
		 * Called before sending a message
		 *
		 * @return Returns whether it should proceed to send the message
		 */ //TODO: Implement
		bool(*onMessageSend)(struct IRCBot* bot,Stringcp message);

		/**
		 * Called before bot joins a channel
		 *
		 * @return Returns whether it should join the channel
		 */
		bool(*onJoin)(struct IRCBot* bot,Stringcp channel);

		/**
		 * Called before bot parts from a channel
		 *
		 * @return Returns whether it should part from the channel
		 */
		bool(*onPart)(struct IRCBot* bot,Stringcp channel);
	}functions;

	struct{
		const char* name;
		const char* version;
		const char* author;
	}constants;
};

/**
 * Loads a plugin library by filename
 *
 * @param filename Filename of the dynamic library that contains the plugin
 * @return         NULL if error, the plugin structure if successful
 */
struct Plugin* Plugin_load(struct IRCBot* bot,const char* filename);

/**
 * Unloads a plugin, freeing all the resources allocated
 *
 * @param plugin  The plugin structure to be unloaded
 */
bool Plugin_unload(struct IRCBot* bot,struct Plugin* plugin);

/**
 * Loads plugin libraries from a directory
 *
 * @param directoryPath Path of the directory where the plugins reside
 * @return              True if successful, false if error
 */
bool Plugin_loadAll(struct IRCBot* bot,const char* directoryPath);

/**
 * Unloads all plugins, freeing all the resources allocated
 */
void Plugin_unloadAll(struct IRCBot* bot);

#endif
