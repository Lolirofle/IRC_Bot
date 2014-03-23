#include "IRCBot.h"

#include <stdlib.h>
#include <string.h>
#include "ircinterface/irc.h"
#include <ircinterface/irc_messagenumbers.h>
#include "Commands.h"
#include "Locale.h"
#include "api/Command.h"
#include "api/CommandArgument.h"
#include <lolie/Stringp.h>
#include <lolie/String.h>
#include <lolie/ControlStructures.h>

//If TIMESTAMP is defined, use it in the version signature
#ifdef TIMESTAMP
#	define IRCBOT_VERSIONSIG IRCBOT_VERSION"-"TIMESTAMP
#else
#	define IRCBOT_VERSIONSIG IRCBOT_VERSION
#endif

const Stringcp IRCBot_signature={IRCBOT_NAME " v" IRCBOT_VERSIONSIG,sizeof(IRCBOT_NAME " v" IRCBOT_VERSIONSIG)-1};

/**
 * Allocates a string wrapped in a Stringp, copying the string from `src` and null-terminates it.
 * The length of the new Stringp will not include the null-termination character.
 *
 * @param src The string we copies from
 * @return    The copied string
 */
static Stringp Stringp_from_malloc_copy_nt(Stringcp src){
	Stringp out;

	//Mallocate string
	out=(Stringp){malloc(src.length+1),src.length};
	//malloc error check
	if(!out.ptr)
		return Stringp_init;
	//Copy string
	memcpy(out.ptr,src.ptr,src.length);
	//Set null termination for compatibility
	out.ptr[src.length]='\0';

	return out;
}

/**
 * Frees a allocated string wrapped in Stringp, setting the pointer to NULL and length to zero.
 *
 * @param str Stringp to free
 */
static void Stringp_free_malloc(Stringp* str){
	if(str && str->ptr){
		free(str->ptr);
		str->ptr=NULL;
		str->length=0;
	}
}

bool IRCBot_initialize(struct IRCBot* bot){
	if(!bot)
		return false;

	bot->hostname=Stringp_init;
	bot->nickname=Stringp_init;
	bot->username=Stringp_init;
	bot->realname=Stringp_init;
	bot->connected=false;

	bot->error=IRCBot_Error_init;
	
	bot->commandPrefix=Stringp_init;
	initCommands(&bot->commands);

	bot->plugins=LinkedList_init;

	bot->exit=IRCBOT_EXIT_FALSE;

	return true;
}

bool IRCBot_free(struct IRCBot* bot){
	if(!bot)
		return false;

	//Disconnect connection if connected
	IRCBot_disconnect(bot);

	//Free mallocated strings
	Stringp_free_malloc(&bot->hostname);
	Stringp_free_malloc(&bot->nickname);
	Stringp_free_malloc(&bot->username);
	Stringp_free_malloc(&bot->realname);
	Stringp_free_malloc(&bot->error.message);
	Stringp_free_malloc(&bot->commandPrefix);
	freeCommands(&bot->commands);

	//Free channel list
	for(LinkedList** list=&bot->channels;*list!=NULL;)
		free(LinkedList_pop(list));

	return true;
}

bool IRCBot_connect(struct IRCBot* bot,Stringcp host,unsigned short port){
	if(!bot || bot->connected)
		return false;

	//Check with plugin hooks
	LinkedList_forEach(bot->pluginHooks.onConnect,node){
		if(!((typeof(((struct Plugin*)0)->functions.onConnect))(node->ptr))(bot,host,port));
			return false;
	}

	//Mallocate and copy string
	bot->hostname=Stringp_from_malloc_copy_nt(host);

	//malloc error check
	if(!bot->hostname.ptr){
		bot->error.code=IRCBOT_ERROR_MEMORY;
		return false;
	}

	//Connect using arguments
	if(!(bot->connection=malloc(sizeof(struct irc_connection)))){
		bot->error.code=IRCBOT_ERROR_MEMORY;
		return false;
	}

	bot->connection->initial_channel = NULL;
	bot->connection->verbosity = IRCINTERFACE_VERBOSITY_NORMAL;

	//Error checking
	if(!irc_connect(bot->hostname.ptr,port,bot->connection) || bot->connection->id<0){
		bot->error.code=IRCBOT_ERROR_CONNECT;
		bot->error.message.length=46*sizeof(char)+8;
		char* str=malloc(bot->error.message.length);
		if(!str){
			bot->error.code=IRCBOT_ERROR_MEMORY;
			return false;
		}
		bot->error.message.length=snprintf(str,bot->error.message.length,"Error: IRC connection id is a negative value: %i",bot->connection->id);
		bot->error.message.ptr=str;
		return false;
	}

	//Successful connection
	bot->connected=true;

	return true;
}

bool IRCBot_disconnect(struct IRCBot* bot){
	if(!bot || !bot->connected)
		return false;

	//Call plugin hooks
	LinkedList_forEach(bot->pluginHooks.onDisconnect,node){
		((typeof(((struct Plugin*)0)->functions.onDisconnect))(node->ptr))(bot);
	}

	//Disconnect connection
	bot->connected=false;
	if(!irc_disconnect(bot->connection)){
		static const Stringcp disconnectError={"IRC disconnect failed",21};

		bot->error.code=IRCBOT_ERROR_DISCONNECT;
		bot->error.message.length=disconnectError.length;
		memcpy(bot->error.message.ptr,disconnectError.ptr,disconnectError.length);
		return false;
	}

	free(bot->connection);

	return true;
}

void IRCBot_setNickname(struct IRCBot* bot,Stringcp name){
	//Free existing string if any
	if(!Stringp_isEmpty(bot->nickname))
		Stringp_free_malloc(&bot->nickname);

	//Mallocate and copy string
	bot->nickname=Stringp_from_malloc_copy_nt(name);

	//malloc error check
	if(!bot->nickname.ptr){
		bot->error.code=IRCBOT_ERROR_MEMORY;
		return;
	}

	//Send to server
	irc_set_nickname(bot->connection,bot->nickname.ptr);
}

void IRCBot_setUsername(struct IRCBot* bot,Stringcp name){
	//Free existing string if any
	if(!Stringp_isEmpty(bot->username))
		Stringp_free_malloc(&bot->username);

	//Mallocate and copy string
	bot->username=Stringp_from_malloc_copy_nt(name);

	//malloc error check
	if(!bot->username.ptr){
		bot->error.code=IRCBOT_ERROR_MEMORY;
		return;
	}

	//Send to server
	irc_set_username(bot->connection,bot->username.ptr,bot->realname.ptr);
}

void IRCBot_setRealname(struct IRCBot* bot,Stringcp name){
	//Free existing string if any
	if(!Stringp_isEmpty(bot->realname))
		Stringp_free_malloc(&bot->realname);

	//Mallocate and copy string
	bot->realname=Stringp_from_malloc_copy_nt(name);

	//malloc error check
	if(!bot->realname.ptr){
		bot->error.code=IRCBOT_ERROR_MEMORY;
		return;
	}

	//Send to server
	irc_set_username(bot->connection,bot->username.ptr,bot->realname.ptr);
}

void IRCBot_setCommandPrefix(struct IRCBot* bot,Stringcp prefix){
	//Free existing string if any
	if(!Stringp_isEmpty(bot->commandPrefix))
		Stringp_free_malloc(&bot->commandPrefix);

	//Mallocate and copy string
	bot->commandPrefix=Stringp_from_malloc_copy_nt(prefix);

	//malloc error check
	if(!bot->commandPrefix.ptr){
		bot->error.code=IRCBOT_ERROR_MEMORY;
		return;
	}
}

void IRCBot_setCommandPrefixc(struct IRCBot* bot,char prefix){
	//Free existing string if any
	if(!Stringp_isEmpty(bot->commandPrefix))
		Stringp_free_malloc(&bot->commandPrefix);

	//Mallocate
	bot->commandPrefix.ptr=malloc(2);

	//malloc error check
	if(!bot->commandPrefix.ptr){
		bot->error.code=IRCBOT_ERROR_MEMORY;
		return;
	}

	//Copy string
	bot->commandPrefix.ptr[0]=prefix;
	bot->commandPrefix.ptr[1]='\0';
	bot->commandPrefix.length=1;
}

void IRCBot_joinChannel(struct IRCBot* bot,Stringcp channel){
	//Check with plugin hooks
	LinkedList_forEach(bot->pluginHooks.onJoin,node){
		if(!((typeof(((struct Plugin*)0)->functions.onJoin))(node->ptr))(bot,channel));
			return;
	}

	//Mallocate and copy string for storing in bot structure
	String* channelName=String_malloc_from_stringcp_nt(channel);
	LinkedList_push(&bot->channels,channelName);//TODO: Push to list after receiving reply indicating successful join

	//Send JOIN message
	irc_join_channel(bot->connection,channelName->data);
}

void IRCBot_partChannel(struct IRCBot* bot,Stringcp channel){
	//Check with plugin hooks
	LinkedList_forEach(bot->pluginHooks.onPart,node){
		if(!((typeof(((struct Plugin*)0)->functions.onPart))(node->ptr))(bot,channel));
			return;
	}

	//for(LinkedList** listNode=&bot->channels;*listNode!=NULL;listNode=&(*listNode)->next)
	LinkedList_removeFirst(&bot->channels,function(bool,(void* _channelName){
		String*const channelName=_channelName;

		if(channelName->length==channel.length && memcmp(channelName->data,channel.ptr,channel.length)==0){
			//Send PART message
			irc_part_channel(bot->connection,channelName->data);

			free(channelName);
			
			return true;
		}
		return false;
	}));
}

void IRCBot_sendMessage(struct IRCBot* bot,Stringcp target,Stringcp message){
	irc_send_message(bot->connection,target,message);
}

void IRCBot_performCommand(struct IRCBot* bot,Stringcp target,const char* command_begin,const char* command_end){
	//Initialize command name
	Stringcp commandName = STRINGCP(command_begin,0);

	//Initialize argument list pointers
	const char* arg_begin;

	//Parse the command name ending point
	loop{
		//If there's no arguments
		if(command_begin>=command_end){
			arg_begin=command_end;
			commandName.length=command_begin-commandName.ptr;
			break;
		}

		//If the command argument separator is found
		if(*command_begin==' '){
			commandName.length=command_begin-commandName.ptr;

			//Increment read pointer (command_begin) and check if there's actually any arguments at all
			if(++command_begin==command_end)
				arg_begin=command_end;
			else
				arg_begin=command_begin;
			break;
		}
		++command_begin;
	}

	//Initialize command and argument structures
	//(Because it's time to search for the command and verify the arguments to the command parameters)
	const struct Command* currentCommand;
	struct CommandArgument arg;
	arg.begin = arg_begin;
	arg.end   = command_end;

	//Check with plugin hooks if command is allowed to continue parsing
	LinkedList_forEach(bot->pluginHooks.onCommand,node){
		if(!(((typeof(((struct Plugin*)0)->functions.onCommand))(node->ptr))(bot,target,commandName,&arg)))
			return;
	}

	//Look up command, if command is found
	if((currentCommand=getCommand(&bot->commands,commandName))){
		//Perform command, check for return value that indicates error
		if(!currentCommand->func(bot,target,&arg)){
			//Send error message about command failure
			int len = Stringp_vcopy(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),4,
				locale[language].command_error,
				STRINGCP(": \"",3),
				commandName,
				STRINGCP("\"",1)
			);
			irc_send_message(bot->connection,target,STRINGCP(write_buffer,len));
		}
	}else{
		//Send error message about command not found
		int len = Stringp_vcopy(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),4,
			locale[language].unknown_command,
			STRINGCP(": \"",3),
			commandName,
			STRINGCP("\"",1)
		);
		irc_send_message(bot->connection,target,STRINGCP(write_buffer,len));
	}
}

static void IRCBot_onMessageFunc(const irc_connection* connection,const irc_message* message,void* user_data){
	struct IRCBot*const bot = user_data;

	//Check with plugin hooks
	LinkedList_forEach(bot->pluginHooks.onMessage,node){
		if(!((typeof(((struct Plugin*)0)->functions.onMessage))(node->ptr))(bot,message));
			return;
	}

	switch(message->command_type_type){
		case IRC_MESSAGE_COMMAND_TYPE_TYPE_NUMBER:
			switch(message->command_type.number){
				case IRC_MESSAGE_TYPENO_RPL_WELCOME:
					if(connection->initial_channel)
						IRCBot_joinChannel(bot,STRINGCP(connection->initial_channel,strlen(connection->initial_channel)));
					break;
				case IRC_MESSAGE_TYPENO_ERR_NONICKNAMEGIVEN:
				case IRC_MESSAGE_TYPENO_ERR_ERRONEUSNICKNAME:
				case IRC_MESSAGE_TYPENO_ERR_NICKNAMEINUSE:
				case IRC_MESSAGE_TYPENO_ERR_NICKCOLLISION:
					//In case of infinite loop
					if(bot->nickname.length>32)
						break;

					//Reallocate and copy string
					bot->nickname.ptr=realloc(bot->nickname.ptr,++bot->nickname.length);
					bot->nickname.ptr[bot->nickname.length-1]='_';

					//realloc error check
					if(!bot->nickname.ptr){
						bot->error.code=IRCBOT_ERROR_MEMORY;
						return;
					}

					//Send to server
					irc_set_nickname(bot->connection,bot->nickname.ptr);
					break;

				default:
					break;
			}

			break;

		case IRC_MESSAGE_COMMAND_TYPE_TYPE_ENUMERATOR:
			switch(message->command_type.enumerator){
				case IRC_MESSAGE_COMMAND_TYPE_PRIVMSG:
					//If on a channel with a '#' prefix and the private message has the correct prefix
					if(message->command.privmsg.target.ptr[0] == '#'){
						if(message->command.privmsg.text.length>bot->commandPrefix.length && memcmp(message->command.privmsg.text.ptr,bot->commandPrefix.ptr,bot->commandPrefix.length)==0)
							IRCBot_performCommand(
								bot,
								//Target is to the channel that the command was requested in
								STRINGP_CONST(message->command.privmsg.target),
								//Command begins after the command prefix
								message->command.privmsg.text.ptr + bot->commandPrefix.length,
								//Command ends at the same position (end of the message)
								message->command.privmsg.text.ptr + message->command.privmsg.text.length
							);
					}

					//If private message
					else if(message->command.privmsg.target.length == bot->nickname.length && memcmp(message->command.privmsg.target.ptr,bot->nickname.ptr,bot->nickname.length)==0)
						IRCBot_performCommand(
							bot,
							//Target is the nickname that sent the command
							STRINGP_CONST(message->prefix.user.nickname),
							//Command begins at the beginning of the message
							message->command.privmsg.text.ptr,
							//Command ends at the end of the message
							message->command.privmsg.text.ptr + message->command.privmsg.text.length
						);
					break;

				case IRC_MESSAGE_COMMAND_TYPE_PING:{
					//TODO: Not exactly following the standards
					char tmp[message->raw_message.length];
					memcpy(tmp,message->raw_message.ptr,message->raw_message.length);
					tmp[1]='O';
					//Send response
					irc_send_raw(connection,tmp,message->raw_message.length);
				}	break;

				default:
					break;
			}
			break;

		default:
			break;
	}
}

enum IRCBot_Exit IRCBot_waitEvents(struct IRCBot* bot){
	irc_read_message(bot->connection,bot,&IRCBot_onMessageFunc);
	return bot->exit;
}
