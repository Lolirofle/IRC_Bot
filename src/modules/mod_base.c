//gcc -o mod_testcommands.o -c -fPIC mod_testcommands.c
//gcc -shared -Wl,-soname,mod_testcommands.so -o mod_testcommands.so mod_testcommands.o
// OR
//gcc -fPIC -shared -Wl,-soname,mod_testcommands.so -o mod_testcommands.so mod_testcommands.c

#include <stdio.h>
#include <string.h>
#include <ircbot/api/IRCBot.h>
#include <ircbot/api/Command.h>
#include <ircbot/api/Commands.h>
#include <lolien/types.h>
#include <lolie/DynamicArray.h>
#include <lolie/String.h>
#include <lolie/Stringp.h>

const char plugin_version[] = "1.0";
const char plugin_author[]  = "Lolirofle";

static struct Command* c=NULL;

bool plugin_onLoad(struct IRCBot* bot){
	if(!(c=malloc(sizeof(struct Command)*11))){
		fputs("Error: Cannot allocate memory for commands",stderr);
		return false;
	}

	c[0]=(struct Command){
		Stringcp_from_cstr("test"),
		Stringcp_from_cstr("Test command"),
		function(bool,(struct IRCBot* bot,Stringcp target,struct CommandArgument* arg){
			#define TEST_STRING "Test command has been executed"
			IRCBot_sendMessage(bot,target,STRINGCP(TEST_STRING,sizeof(TEST_STRING)));
			return true;
		})
	};
	c[1]=(struct Command){
		Stringcp_from_cstr("version"),
		Stringcp_from_cstr("Outputs version of bot"),
		function(bool,(struct IRCBot* bot,Stringcp target,struct CommandArgument* arg){
			IRCBot_sendMessage(bot,target,IRCBot_signature);
			return true;
		})
	};
	c[2]=(struct Command){
		Stringcp_from_cstr("bot"),
		Stringcp_from_cstr("Call me"),
		function(bool,(struct IRCBot* bot,Stringcp target,struct CommandArgument* arg){
			IRCBot_sendMessage(bot,target,STRINGCP("IT'S A ME, TOABOTU",18));
			return true;
		})
	};
	c[3]=(struct Command){
		Stringcp_from_cstr("echo"),
		Stringcp_from_cstr("Echoes the arguments"),
		function(bool,(struct IRCBot* bot,Stringcp target,struct CommandArgument* arg){
			IRCBot_sendMessage(bot,target,STRINGCP(arg->begin,arg->end-arg->begin));
			return true;
		})
	};
	c[4]=(struct Command){
		Stringcp_from_cstr("help"),
		Stringcp_from_cstr("Outputs a list of commands and their usage"),
		function(bool,(struct IRCBot* bot,Stringcp target,struct CommandArgument* arg){
			char* writePtr=write_buffer;
			char* writeBufferEnd=write_buffer+IRC_WRITE_BUFFER_LEN;

			writePtr+=snprintf(writePtr,writeBufferEnd-writePtr,"Commands: ");

			//For each command list and existing commands
			DynamicArray_forEach(bot->commands,commandLengthList){
				LinkedList_forEach(*(LinkedList**)commandLengthList,node){
					writePtr+=snprintf(writePtr,writeBufferEnd-writePtr,"%.*s, ",(int)((struct Command*)node->ptr)->name.length,((struct Command*)node->ptr)->name.ptr);
				}
			}

			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,writePtr-write_buffer));

			return true;
		})
	};
	c[5]=(struct Command){
		Stringcp_from_cstr("shutdown"),
		Stringcp_from_cstr("Shutdown the bot"),
		function(bool,(struct IRCBot* bot,Stringcp target,struct CommandArgument* arg){
			IRCBot_shutdown(bot);
			IRCBot_sendMessage(bot,target,STRINGCP("Shutting down...",16));
			return true;
		})
	};
	c[6]=(struct Command){
		Stringcp_from_cstr("restart"),
		Stringcp_from_cstr("Restart the bot"),
		function(bool,(struct IRCBot* bot,Stringcp target,struct CommandArgument* arg){
			IRCBot_restart(bot);
			IRCBot_sendMessage(bot,target,STRINGCP("Restarting...",13));
			return true;
		})
	};
	c[7]=(struct Command){
		Stringcp_from_cstr("reload"),
		Stringcp_from_cstr("Reload the bot"),
		function(bool,(struct IRCBot* bot,Stringcp target,struct CommandArgument* arg){
			IRCBot_reloadPlugins(bot);
			IRCBot_sendMessage(bot,target,STRINGCP("Modules is reloaded",19));
			return true;
		})
	};
	c[8]=(struct Command){
		Stringcp_from_cstr("join"),
		Stringcp_from_cstr("Makes the bot join specified channel"),
		function(bool,(struct IRCBot* bot,Stringcp target,struct CommandArgument* arg){
			IRCBot_joinChannel(bot,STRINGCP(arg->begin,arg->end-arg->begin));
			return true;
		})
	};
	c[9]=(struct Command){
		Stringcp_from_cstr("part"),
		Stringcp_from_cstr("Makes the bot part from specified channel"),
		function(bool,(struct IRCBot* bot,Stringcp target,struct CommandArgument* arg){
			IRCBot_partChannel(bot,STRINGCP(arg->begin,arg->end-arg->begin));
			return true;
		})
	};
	c[10]=(struct Command){
		Stringcp_from_cstr("channels"),
		Stringcp_from_cstr("List the channels the bot currently resides in"),
		function(bool,(struct IRCBot* bot,Stringcp target,struct CommandArgument* arg){
			char* writePtr=write_buffer;
			char* writeBufferEnd=write_buffer+IRC_WRITE_BUFFER_LEN;

			writePtr+=snprintf(writePtr,writeBufferEnd-writePtr,"Current channels: ");

			//For each channel the bot resides in
			LinkedList_forEach(bot->channels,node){
				//Copy filename to write buffer
				writePtr+=snprintf(writePtr,writeBufferEnd-writePtr,"%.*s, ",(int)((String*)node->ptr)->length,((String*)node->ptr)->data);
			}

			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,writePtr-write_buffer));
			return true;
		})
	};
	return registerCommandsFromArray(&bot->commands,c,11);
}

bool plugin_onUnload(struct IRCBot* bot){
	for(uint i=0;i<11;++i)
		if(!unregisterCommandByName(&bot->commands,c[i].name))
			fprintf(stderr,"Module: mod_base: Warning: Command couldn't be freed: %s\n",c[i].name.ptr);

	if(c){
		free(c);
		c=NULL;
	}
	return true;
}
