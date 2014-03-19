#include <stdio.h> //Input/output
#include <string.h>
#include <argp.h>

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

//TODO: "&&"" to combine commands and maybe `command` to insert a command with output as return value to an argument
//TODO: Help pages for a list of commands and syntax, explanation, etc.
//TODO: Command aliases
//TODO: Commands: "to <channel/nickname> <command>"
//TODO: Move most the stuff from Main.c to IRCBot.c and rename IRCBot.c to Toabot.c
//TODO: Avoid "static" variables that resides in functions or in the global scope because it makes data races possible.

struct IRCBot bot;
char* defaultChannel;

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
		if(str.length>=delimiter.length && memeq(str.ptr,delimiter.ptr,delimiter.length))
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
					if(connection->initial_channel)
						IRCBot_joinChannel(&bot,STRINGCP(connection->initial_channel,strlen(connection->initial_channel)));
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
				if(message->command.privmsg.text.length>bot.commandPrefix.length && memcmp(message->command.privmsg.text.ptr,bot.commandPrefix.ptr,bot.commandPrefix.length)==0)
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

static struct argp_option program_arg_options[] = {
//  |   Long name   |Key|       Arg       |       Flags       | Description
	{"<hostname/IP>", 0 ,             NULL,         OPTION_DOC,"IP/hostname to connect to" },
	{"nick"         ,'n',         "Toabot",                  0,"Sets the nickname of the bot" },
	{"port"         ,'p',           "6667",                  0,"Port to connect to" },
	{"channel"      ,'c', "<channel name>",                  0,"Initial channel to join" },
	{"verbose"      ,'v',             NULL,                  0,"Produce verbose output" },
	{"quiet"        ,'q',             NULL,                  0,"Don't produce any output" },
	{0}
};

struct program_options{
	char* hostname;
	char* nickname;
	char* channel;
	unsigned int port;
	enum irc_connection_verbosity verbosity;
}options;

/**
 * Parses a single option using the key defined in the `argp_option` structure
 */
static error_t program_arg_parse_option(int key,char* arg,struct argp_state* state){
	struct program_options*const program_options = state->input;

	switch(key){
		case 'n':
			program_options->nickname = arg;
			break;
		case 'p':
			{
				long i = strtol(arg,NULL,10);
				if(i<0 || i>65535)
					return 1;
				program_options->port = (unsigned int)i;
			}
			break;
		case 'c':
			program_options->channel = arg;
			break;
		case 'q':
			program_options->verbosity = IRCINTERFACE_VERBOSITY_SILENT;
			break;
		case 'v':
			program_options->verbosity = IRCINTERFACE_VERBOSITY_VERBOSE;
			break;
		case ARGP_KEY_ARG:
			//If there's too many arguments
			if(state->arg_num>=1)
				argp_usage(state);
	
			switch(state->arg_num){
				case 0:
					program_options->hostname = arg;
					break;
			}
			break;
		case ARGP_KEY_END:
			//If there's not enough arguments
			if(state->arg_num<1)
				argp_usage(state);
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp program_argp = {
	program_arg_options,
	program_arg_parse_option,
	"IP/hostname",
	"Toabot, the extensible IRC bot.\nhttps://github.com/Lolirofle/IRC_Bot"
};

int main(int argc,char **argv){
	int exitCode = 0;
	
	/////////////////////////////////////////////////////////
	// Program arguments
	//

	//Default options
	struct program_options options = {
		.nickname = "Toabot",
		.channel = NULL,
		.port = 6667,
		.verbosity = IRCINTERFACE_VERBOSITY_NORMAL
	};

	argp_parse(&program_argp,argc,argv,0,0,&options);

	/////////////////////////////////////////////////////////
	// Begin main program
	//

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

	Bot:{
		int botExit;
		
		//Initialize bot structure
		IRCBot_initialize(&bot);

		//Load all plugins
		if(!Plugin_loadAll(&bot,"modules"))
			fputs("Warning: Failed to initialize modules\n",stderr);

		//Connect to server
		IRCBot_connect(&bot,Stringcp_from_cstr(options.hostname),options.port);
		bot.connection->verbosity = options.verbosity;
		bot.connection->initial_channel = options.channel;

		Stringcp name=Stringcp_from_cstr(options.nickname);
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
	
	exitCode = EXIT_SUCCESS;

	return exitCode;
}
