#ifndef __LOLIROFLE_IRCBOT_API_COMMANDS_H_INCLUDED__
#define __LOLIROFLE_IRCBOT_API_COMMANDS_H_INCLUDED__

#include <lolien/types.h>
#include <lolie/Stringp.h>
struct Command;
struct DynamicArray;

/**
 * Registers a command for use in the bot
 *
 * @param commands The command list
 * @param command  The command that should be registered. It should not be a pointer to a temporary storage (e.g. the stack)
 * @return         Returns whether the registration were successful
 */
bool registerCommand(struct DynamicArray* commands,const struct Command* command);

/**
 * Registers a specified amount of commands for use in the bot
 *
 * @param commands The command list
 * @param command  The commands that should be registered. It should not be a pointer to a temporary storage (e.g. the stack)
 * @param count    Number of commands in the array
 * @return         Returns whether the registration were successful
 */
bool registerCommandsFromArray(struct DynamicArray* commands,const struct Command* cmds,size_t count);

/**
 * Returns a command structure by name
 *
 * @param commands The command list
 * @param name     The name of the command to search for
 * @return         The fetched command, or NULL if not found
 */
const struct Command* getCommand(const struct DynamicArray* commands,Stringcp name);

/**
 * Unregister a registered command by pointer to the structure
 *
 * @param commands The command list
 * @param command  The pointer to the structure to search for
 * @return         Returns whether the command was registered and the unregistration were successful
 */
bool unregisterCommand(struct DynamicArray* commands,const struct Command* command);

/**
 * Unregister a registered command by name
 *
 * @param commands    The command list
 * @param commandName The pointer to the structure to search for
 * @return            Returns whether the command were found and registered and the unregistration were successful
 */
bool unregisterCommandByName(struct DynamicArray* commands,Stringcp commandName);

/**
 * Unregisters all registered commands
 *
 * @param commands    The command list
 */
void unregisterCommands(struct DynamicArray* commands);

#endif
