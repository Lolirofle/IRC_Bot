#include <stdio.h> //Input/output
#include <string.h>

#include <lolie/Stringp.h>
#include <lolie/LinkedList.h>
#include <lolie/Memory.h>
#include <lolie/Math.h>
#include <lolie/ControlStructures.h>

#include <ircinterface/irc.h>
#include <ircinterface/irc_messagenumbers.h>
#include "Commands.h"
#include "IRCBot.h"
#include "api/Plugin.h"

#include "argtable2.h"

//TODO: "&&"" to combine commands and maybe `command` to insert a command with output as return value to an argument
//TODO: Help pages for a list of commands and syntax, explanation, etc.
//TODO: Command aliases

struct IRCBot bot;
char* default_channel;

Stringp string_splitted(Stringp str,size_t(*delimiterFunc)(Stringp str),bool(*onSplitFunc)(const char* begin,const char* end)){
	const char* arg_begin=str.ptr;

	loop{
		if(str.length<1){
			onSplitFunc(arg_begin,str.ptr);
			break;
		}else{
			size_t delim = delimiterFunc(str);
			if(delim>0){
				str.length-=delim;
				if(!onSplitFunc(arg_begin,str.ptr))
					return STRINGP(str.ptr+delim,str.length);
				str.ptr+=delim;
				arg_begin=str.ptr;
				continue;
			}
		}

		++str.ptr;
		--str.length;
	}

	return str;
}

Stringp string_splitted_delim(Stringp str,Stringp delimiter,bool(*onSplitFunc)(const char* begin,const char* end)){
	return string_splitted(str,function(size_t,(Stringp str){
		if(str.length>=delimiter.length && Memory_equals(str.ptr,delimiter.ptr,delimiter.length))
			return delimiter.length;
		else
			return 0;
	}),onSplitFunc);
}

Stringp Stringp_find_substr(Stringp str,bool(*findFunc)(Stringp str));

void onMessageFunc(const irc_connection* connection,const irc_message* message){
	//Check with plugin hooks
	LinkedList_forEach(bot.pluginHooks.onMessage,node){
		if(!((typeof(((struct Plugin*)0)->functions.onMessage))(node->ptr))(&bot,message));
			return;
	}

	switch(message->command_type){
		case IRC_MESSAGE_TYPE_NUMBER:
			switch(message->command_type_number){
				case 1:
					IRCBot_joinChannel(&bot,STRINGCP(default_channel,strlen(default_channel)));
					//IRCBot_joinChannel(&bot,STRINGCP("#toa",4));
					break;
				case IRC_MESSAGE_TYPENO_ERR_NONICKNAMEGIVEN:
				case IRC_MESSAGE_TYPENO_ERR_ERRONEUSNICKNAME:
				case IRC_MESSAGE_TYPENO_ERR_NICKNAMEINUSE:
				case IRC_MESSAGE_TYPENO_ERR_NICKCOLLISION:
					//In case of infinite loop
					if(bot.nickname.length>32)
						break;

					//Reallocate and copy string
					bot.nickname.ptr=realloc(bot.nickname.ptr,++bot.nickname.length);
					bot.nickname.ptr[bot.nickname.length-1]='_';

					//realloc error check
					if(!bot.nickname.ptr){
						bot.error.code=IRCBOT_ERROR_MEMORY;
						return;
					}

					//Send to server
					irc_set_nickname(bot.connection,bot.nickname.ptr);
					break;

				default:
					break;
			}

			break;
		case IRC_MESSAGE_TYPE_PRIVMSG:
			//If on a channel with a '#' prefix and the private message has the correct prefix
			if(message->command.privmsg.target.ptr[0] == '#'){
				if(message->command.privmsg.text.length==bot.commandPrefix.length || memcmp(message->command.privmsg.text.ptr,bot.commandPrefix.ptr,bot.commandPrefix.length)==0)
					IRCBot_performCommand(
						&bot,
						//Target is to the channel that the command was requested in
						STRINGP_CONST(message->command.privmsg.target),
						//Command begins after the command prefix
						message->command.privmsg.text.ptr + bot.commandPrefix.length,
						//Command ends at the same position (end of the message)
						message->command.privmsg.text.ptr + message->command.privmsg.text.length
					);
			}

			//If private message
			else if(message->command.privmsg.target.length == bot.nickname.length && memcmp(message->command.privmsg.target.ptr,bot.nickname.ptr,bot.nickname.length)==0)
				IRCBot_performCommand(
					&bot,
					//Target is the nickname that sent the command
					STRINGP_CONST(message->prefix.user.nickname),
					//Command begins at the beginning of the message
					message->command.privmsg.text.ptr,
					//Command ends at the end of the message
					message->command.privmsg.text.ptr + message->command.privmsg.text.length
				);
			break;
	}
}

int main(int argc, char **argv){
	
	int exitcode = 0;
	//argtable
	
	struct arg_str*	nick 	= arg_str1("n", "nick", "<string>",	"nick of the bot");
	struct arg_str*	server 	= arg_str1(NULL, NULL,  "<string>",	"ip/domain name");
	struct arg_str* channel = arg_str1("c", "channel", "<string>", "channel to join initially");
	struct arg_int*	port	= arg_int0("p", "port", "<int>", 	"da port");
	struct arg_lit*	help 	= arg_lit0("h", "help", "this exakt help");
	struct arg_end* 	end 	= arg_end(20);
	void* argtable[] = {nick, server, port, channel, help, end};
	int nerrors;
	
	if (arg_nullcheck(argtable) != 0){
		/* NULL entries were detected, some allocations must have failed */
		printf("%s: insufficient memory\n", "toabot");
		exitcode = 1;
		goto exit;
	}
	
	
	port->ival[0] = 6697;
	
	nerrors = arg_parse(argc, argv, argtable); 
	
	if(help->count > 0){
		printf("Toabot: The extensible irc bot.\n");
		printf("https://github.com/Lolirofle/IRC_Bot\n");
		arg_print_glossary(stdout,argtable,"  %-25s %s\n");
		exitcode = 0;
		goto exit;
	}
	if(nerrors > 0){
		arg_print_errors(stdout,end,"toabot");
		printf("Use --help, you fool");
		exitcode = 1;
		goto exit;
	}
	//The right arguments are supplied, on to the normal main.
	default_channel = channel->sval[0];
	
	//Top border
	for(size_t i=IRCBot_signature.length;i>0;--i)
		putchar('=');
	putchar('\n');

	//Text
	printf("%s\nCopyright (C) 2014, Lolirofle\n",IRCBot_signature.ptr);

	//Bottom border
	for(size_t i=IRCBot_signature.length;i>0;--i)
		putchar('=');
	putchar('\n');
	putchar('\n');

	int botExit;
	Bot:{
		//Initialize bot structure
		IRCBot_initialize(&bot);

		//Load all plugins
		if(!Plugin_loadAll(&bot,"modules"))
			fputs("Warning: Failed to initialize modules\n",stderr);

		//Connect to server
		IRCBot_connect(&bot,Stringcp_from_cstr(server->sval[0]),port->ival[0]);

		Stringcp name=Stringcp_from_cstr(nick->sval[0]);
		IRCBot_setNickname(&bot,name);
		IRCBot_setUsername(&bot,name);
		IRCBot_setCommandPrefixc(&bot,'!');

		//While a message is sent from the server
		ReadLoop: while(bot.exit==IRCBOT_EXIT_FALSE && irc_read_message(bot.connection,&onMessageFunc));

		if(botExit==IRCBOT_EXIT_RELOADPLUGINS){
			bot.exit=IRCBOT_EXIT_FALSE;
			Plugin_unloadAll(&bot);
			if(!Plugin_loadAll(&bot,"modules"))
				fputs("Warning: Failed to initialize modules\n",stderr);
			goto ReadLoop;
		}

		//Disconnect connection
		IRCBot_disconnect(&bot);

		//Unload all plugins
		Plugin_unloadAll(&bot);

		botExit=bot.exit;

		//Free resources
		IRCBot_free(&bot);

		if(botExit==IRCBOT_EXIT_RESTART)
			goto Bot;
	}
	
	exitcode = EXIT_SUCCESS;
	
	exit:
		arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

	return exitcode;
}
