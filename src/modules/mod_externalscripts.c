#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>
#include <ircbot/api/IRCBot.h>
#include <ircbot/api/Command.h>
#include <ircbot/api/Commands.h>
#include <ircbot/pipes.h>
#include <lolien/types.h>
#include <lolie/Stringp.h>

const char plugin_version[] ="1.2";
const char plugin_author[]  ="Lolirofle";

#define BUFFER_LENGTH 512
static char buffer[BUFFER_LENGTH];
#define SCRIPT_DIR "external_commands"
#define SCRIPT_PATH SCRIPT_DIR "/"
#define SCRIPT_PATH_LEN 18

bool plugin_onCommand(struct IRCBot* bot,Stringcp target,Stringcp command,struct CommandArgument* arg){
	//Copy path and command string to a buffer
	char cmd[SCRIPT_PATH_LEN+command.length+1];
	memcpy(cmd,SCRIPT_PATH,SCRIPT_PATH_LEN);
	char* cmd_iterator=cmd+SCRIPT_PATH_LEN;
	for(Stringcp commandStr=command;commandStr.length>0;++commandStr.ptr,--commandStr.length){
		if(!isalnum(*commandStr.ptr))
			return true;
		*cmd_iterator++=*commandStr.ptr;
	}
	*cmd_iterator='\0';

	//Check for file existence and execution permissions. //TODO: Is it all right to do it like this? Small possibility of race conditions? Should chnage the way p2open works instead? Does it even matter?
	//If this returns OK and the program continues but the file cannot be executed, it will return no output and that the external command didn't return success.
	if(access(cmd,X_OK)==-1)
		return true;

	//Open program
	char* argv[]={cmd+SCRIPT_PATH_LEN,NULL};
	struct PipedStream stream=p2open(cmd,argv);
	if(!stream.in || !stream.out)
		return true;

	//Input to program,arguments
	fwrite(arg->begin,sizeof(char),arg->end-arg->begin,stream.in);
	p2flushWrite(stream);

	//Output from program,output
	size_t len=fread(buffer,sizeof(char),BUFFER_LENGTH,stream.out);
	p2flushRead(stream);

	//Close program
	int returnCode = p2close(stream);

	//Send output message
	if(len>1)
		IRCBot_sendMessage(bot,target,STRINGCP(buffer,len));
	else{//There's no output
		//Check for error in program execution
		if(returnCode!=0)
			IRCBot_sendMessage(bot,target,STRINGCP("Error: External command did not return EXIT_SUCCESS",51));
		else
			IRCBot_sendMessage(bot,target,STRINGCP("Warning: No output from external command",40));
	}

	return false;
}

static struct Command c;

bool plugin_onLoad(struct IRCBot* bot){
	c=(struct Command){
		Stringcp_from_cstr("extcmds"),
		Stringcp_from_cstr("Lists all external commands"),
		function(bool,(struct IRCBot* bot,Stringcp target,struct CommandArgument* arg){
			DIR* directory;
			struct dirent* dir;
			char* writePtr=write_buffer;
			char* writeBufferEnd=write_buffer+IRC_WRITE_BUFFER_LEN;

			//Open directory and initialize `directory`. If specified path cannot be opened as a directory: return false
			if(!(directory=opendir(SCRIPT_DIR)))
				return false;

			writePtr+=snprintf(writePtr,writeBufferEnd-writePtr,"External commands: ");

			//For each file in the directory
			while((dir=readdir(directory))){
				//Skip hidden files
				if(dir->d_name[0]=='.')
					continue;

				//Copy filename to write buffer
				writePtr+=snprintf(writePtr,writeBufferEnd-writePtr,"%s, ",dir->d_name);
			}

			//Free resources
			closedir(directory);

			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,writePtr-write_buffer));
			return true;
		})
	};
	return registerCommand(&bot->commands,&c);
}

bool plugin_onUnload(struct IRCBot* bot){
	if(!unregisterCommandByName(&bot->commands,c.name))
		fprintf(stderr,"Module: mod_externalscripts: Warning: Command couldn't be freed: %s\n",c.name.ptr);
	return true;
}
