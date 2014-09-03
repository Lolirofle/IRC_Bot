#include <stdio.h> //Input/output
#include <string.h>
#include <argp.h>
#include <libconfig.h>

#include <lolien/seq/StringP.h>
#include <lolien/controlstructures.h>

#include "Commands.h"
#include "IRCBot.h"
#include "api/Plugin.h"

//TODO: "&&"" to combine commands and maybe `command` to insert a command with output as return value to an argument
//TODO: Help pages for a list of commands and syntax, explanation, etc.
//TODO: Command aliases
//TODO: Command: "to <channel/nickname> <command>"
//TODO: Rename IRCBot.c to Toabot.c
//TODO: Multithreading in the bot for command processing. Also check if it is needed for mod_externalscripts
//TODO: Split message struct to server_message and client_message where server message is messages from the server

//TODO: Remove when the parameter API is correctly implemented
StringP string_splitted(StringP str,size_t(*delimiterFunc)(StringP str),bool(*onSplitFunc)(const char* begin,const char* end)){
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
	const char* configPath;
	const char* hostname;
	const char* nickname;
	const char* username;
	const char* realname;
	const char* channel;
	const char* joinmessage;
	const char* partmessage;
	const char* quitmessage;
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

struct IRCBot bot;

int main(int argc,char **argv){	
	struct program_options options;

	/////////////////////////////////////////////////////////
	// Program config
	//
	config_t configuration;{
		//Initialize config
		config_init(&config.configuration);

		//Read from file
		if(!config_read_file(&configuration,"toabot.conf")){
			fprintf(stderr,"Error: Configuration file: %s:%d: %s\n",config_error_file(&configuration),config_error_line(&configuration),config_error_text(&configuration));
			config_destroy(&configuration);
			return EXIT_FAILURE;
		}

		config_setting_t* settings;
		settings = config_root_setting(&configuration);

		//Read settings
		config_setting_t* setting;
		
		if(setting = config_setting_get_member(settings,"nickname"))
			options.nickname = config_setting_get_string(setting);

		if(setting = config_setting_get_member(settings,"username"))
			options.username = config_setting_get_string(setting);

		if(setting = config_setting_get_member(settings,"realname"))
			options.realname = config_setting_get_string(setting);

		if(setting = config_setting_get_member(settings,"channel"))
			options.channel = config_setting_get_string(setting);

		if(setting = config_setting_get_member(settings,"hostname"))
			options.hostname = config_setting_get_string(setting);

		if(setting = config_setting_get_member(settings,"port"))
			options.port = config_setting_get_int(setting);

		if(setting = config_setting_get_member(settings,"joinmessage"))
			options.joinmessage = config_setting_get_string(setting);

		if(setting = config_setting_get_member(settings,"partmessage"))
			options.partmessage = config_setting_get_string(setting);

		if(setting = config_setting_get_member(settings,"quitmessage"))
			options.quitmessage = config_setting_get_string(setting);
	}

	/////////////////////////////////////////////////////////
	// Program arguments
	//
	argp_parse(&program_argp,argc,argv,0,0,&options);

	/////////////////////////////////////////////////////////
	// Program default options
	//
	if(!options.nickname)options.nickname = "Toabot";
	if(!options.username)options.username = options.nickname;
	if(!options.realname)options.realname = options.nickname;
	if(!options.channel) options.channel  = NULL;
	if(options.port==0)  options.port     = 6667;

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
		//Initialize bot structure
		IRCBot_initialize(&bot);

		//Load all plugins
		if(!Plugin_loadAll(&bot,"modules"))
			fputs("Warning: Failed to initialize modules\n",stderr);

		//Connect to server
		IRCBot_connect(&bot,StringCP_fromCStr(options.hostname),options.port);
		bot.connection->verbosity = options.verbosity;
		bot.connection->initial_channel = options.channel;

		IRCBot_setNickname(&bot,StringCP_fromCStr(options.nickname));
		IRCBot_setUsername(&bot,StringCP_fromCStr(options.username));
		IRCBot_setCommandPrefixc(&bot,'!');

		//While a message is sent from the server
		int botExit;
		ReadLoop: while((botExit=IRCBot_waitEvents(&bot))==IRCBOT_EXIT_FALSE);

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

		config_destroy(&configuration);
	}

	return EXIT_SUCCESS;
}
