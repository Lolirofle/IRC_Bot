#include "irc.h"

#include <stdio.h> //Input/output
#include <stdarg.h>//Variable length arguments
#include <stdlib.h>//Standard definitions
#include <string.h>

#include <lolie/Stringp.h>//Null terminatd Stringp operations
#include <lolie/Memory.h>//memeq
#include <lolie/LinkedList.h>
#include <lolie/ControlStructures.h>
#include <lolie/Math.h>

#include <unistd.h>//Unix standard library
#include <netdb.h> //Networking

void irc_send_rawf(const irc_connection* connection,const char* format,...){
	static char format_buffer[IRC_FORMAT_BUFFER_LENGTH];

	va_list ap;
	va_start(ap,format);
	vsnprintf(format_buffer,IRC_FORMAT_BUFFER_LENGTH,format,ap);
	va_end(ap);
	irc_send_rawnt(connection,format_buffer);
}

bool irc_connect(const char* host,unsigned short port,irc_connection* out_connection){
	struct addrinfo hints,
	               *result;
	char portStr[6];portStr[intToDecStr((long)port,STRINGP(portStr,6))]='\0';

	printf("Connecting to `%s:%s`\n",host,portStr);

	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(host,portStr,&hints,&result);
	out_connection->id = socket(result->ai_family,result->ai_socktype,result->ai_protocol);

	int connection = connect(out_connection->id,result->ai_addr,result->ai_addrlen);
	if(connection<0){
		printf("Error: connect() returned %i\n",connection);
		return false;
	}

	freeaddrinfo(result);

	//Initiate read buffer
	if(!(out_connection->read_buffer = malloc(IRC_BUFFER_LENGTH+1)))
		return false;//TODO: May leak memory on failure

	return true;
}

bool irc_disconnect(const irc_connection* connection){
	free(connection->read_buffer);
	return close(connection->id)>=0;
}

void irc_send_raw(const irc_connection* connection,const char* str,size_t len){
	//stdout verbose output
	Stringp_put(STRINGP(">> ",3),stdout);
	Stringp_put(STRINGP(str,len),stdout);

	//Write to server
	write(connection->id,str,len);
}

void irc_send_message(const irc_connection* connection,Stringcp target,Stringcp message){
	char write_buffer[message.length+target.length+12];

	int len = Stringp_vcopy(STRINGP(write_buffer,IRC_BUFFER_LENGTH),5,
		STRINGCP("PRIVMSG ",8),
		target,
		STRINGCP(" :",2),
		message,
		STRINGCP("\r\n",2)
	);
	irc_send_raw(connection,write_buffer,len);
}

//////////////////////////
// Parsing

static bool irc_parse_param_privmsg(struct irc_message* out,byte param_count,const char* begin,const char* after_end){
	switch(param_count){
		case 1:
			out->command.privmsg.target = STRINGCP(begin,after_end-begin);
			break;
		case 2:
			out->command.privmsg.text = STRINGCP(begin,after_end-begin);
			break;
		default:
			return false;
	}
	return true;
}

static bool irc_parse_param_join(struct irc_message* out,byte param_count,const char* begin,const char* after_end){
	switch(param_count){
		case 1:
			//out->command.join.channels = STRINGCP(begin,after_end-begin);
			break;
		case 2:
			//out->command.join.channels = STRINGCP(begin,after_end-begin);
			break;
		default:
			return false;
	}
	return true;
}

static bool irc_parse_param_part(struct irc_message* out,byte param_count,const char* begin,const char* after_end){
	switch(param_count){
		case 1:
			//out->command.join.channels = STRINGCP(begin,after_end-begin);
			break;
		case 2:
			out->command.part.message = STRINGCP(begin,after_end-begin);
			break;
		default:
			return false;
	}
	return true;
}

static bool irc_parse_param_topic(struct irc_message* out,byte param_count,const char* begin,const char* after_end){
	switch(param_count){
		case 1:
			out->command.topic.channel = STRINGCP(begin,after_end-begin);
			break;
		case 2:
			out->command.topic.text = STRINGCP(begin,after_end-begin);
			break;
		default:
			return false;
	}
	return true;
}

static bool irc_parse_param_notice(struct irc_message* out,byte param_count,const char* begin,const char* after_end){
	switch(param_count){
		case 1:
			out->command.notice.target = STRINGCP(begin,after_end-begin);
			break;
		case 2:
			out->command.notice.text = STRINGCP(begin,after_end-begin);
			break;
		default:
			return false;
	}
	return true;
}

static bool irc_parse_param_kick(struct irc_message* out,byte param_count,const char* begin,const char* after_end){
	switch(param_count){
		case 1:
			//out->command.kick.channels = STRINGCP(begin,after_end-begin);
			break;
		case 2:
			//out->command.kick.channels = STRINGCP(begin,after_end-begin);
			break;
		case 3:
			out->command.kick.comment = STRINGCP(begin,after_end-begin);
			break;
		default:
			return false;
	}
	return true;
}

static bool irc_parse_param_nick(struct irc_message* out,byte param_count,const char* begin,const char* after_end){
	switch(param_count){
		case 1:
			out->command.nick.name = STRINGCP(begin,after_end-begin);
			break;
		default:
			return false;
	}
	return true;
}

static bool irc_parse_param_quit(struct irc_message* out,byte param_count,const char* begin,const char* after_end){
	switch(param_count){
		case 1:
			out->command.quit.message = STRINGCP(begin,after_end-begin);
			break;
		default:
			return false;
	}
	return true;
}

static bool irc_parse_param_squit(struct irc_message* out,byte param_count,const char* begin,const char* after_end){
	switch(param_count){
		case 1:
			out->command.squit.server = STRINGCP(begin,after_end-begin);
			break;
		case 2:
			out->command.squit.comment = STRINGCP(begin,after_end-begin);
			break;
		default:
			return false;
	}
	return true;
}

static bool irc_parse_param_mode(struct irc_message* out,byte param_count,const char* begin,const char* after_end){
	switch(param_count){
		case 1:
			out->command.mode.nickname = STRINGCP(begin,after_end-begin);
			break;
		case 2:
			if(after_end-begin<2)
				return false;
			switch(begin[0]){
				case '+':
					out->command.mode.operation = true;
					break;
				case '-':
					out->command.mode.operation = false;
					break;
				default:
					return false;
			}
			out->command.mode.mode = begin[1];
			break;
		default:
			return false;
	}
	return true;
}

static bool irc_parse_param_kill(struct irc_message* out,byte param_count,const char* begin,const char* after_end){
	switch(param_count){
		case 1:
			out->command.kill.nickname = STRINGCP(begin,after_end-begin);
			break;
		case 2:
			out->command.kill.comment = STRINGCP(begin,after_end-begin);
			break;
		default:
			return false;
	}
	return true;
}

static bool irc_parse_param_away(struct irc_message* out,byte param_count,const char* begin,const char* after_end){
	switch(param_count){
		case 1:
			out->command.away.text = STRINGCP(begin,after_end-begin);
			break;
		default:
			return false;
	}
	return true;
}

static bool irc_parse_param_error(struct irc_message* out,byte param_count,const char* begin,const char* after_end){
	switch(param_count){
		case 1:
			out->command.error.message = STRINGCP(begin,after_end-begin);
			break;
		default:
			return false;
	}
	return true;
}

Stringcp irc_parse_message(const irc_connection* connection,Stringcp raw_message,irc_message* out){
	const char* read_ptr       = raw_message.ptr,
	          * read_ptr_begin = read_ptr,
	          *const read_ptr_end = raw_message.ptr + raw_message.length;

	//Initialize initial fields
	out->raw_message = raw_message;
	out->prefix_type = IRC_MESSAGE_PREFIX_UNDETERMINED;
	out->command_type_type = IRC_MESSAGE_COMMAND_TYPE_TYPE_UNDETERMINED;

	//Begin parsing
	//It parses from left to right, convenient for getting what type of message it is but hard for determining the prefix.

	//If at end
	if(read_ptr>=read_ptr_end)
		goto ParseTerminate;

	//If the message has a prefix
	if(raw_message.ptr[0] == ':'){
		read_ptr_begin = ++read_ptr;

		//Prefix
		for(bool continueLoop=true;continueLoop;){
			//If at end of message
			if(read_ptr>=read_ptr_end)
				goto ParseTerminate;
			if(read_ptr[0] == '\r' && read_ptr[1] == '\n')
				goto ParseTerminateCRLF;

			//If the prefix type hasn't been determined to be in any format and a '!' or '@' is found
			switch(read_ptr[0]){
				case ' ':
					continueLoop = false;

					//If the prefix hasn't been determined to be in any format and it is the end
					if(out->prefix_type == IRC_MESSAGE_PREFIX_UNDETERMINED){
						//It is the server name (The string from read_ptr_begin to read_ptr)
						out->prefix_type = IRC_MESSAGE_PREFIX_SERVER;
						out->prefix.server.name = STRINGCP(read_ptr_begin,read_ptr-read_ptr_begin);
						read_ptr_begin = ++read_ptr;
						break;
					}
				case '@':
				case '!':
					if(out->prefix_type == IRC_MESSAGE_PREFIX_UNDETERMINED){
						//It is a nickname parameter (The string from read_ptr_begin to read_ptr)
						out->prefix.user.nickname = STRINGCP(read_ptr_begin,read_ptr-read_ptr_begin);
						out->prefix_type = IRC_MESSAGE_PREFIX_USER;
					}
					//If the prefix type has been determined to be in the user format
					else if(out->prefix_type == IRC_MESSAGE_PREFIX_USER){
						//Lookup the beginning and determine by that
						switch(read_ptr_begin[-1]){
							case '@':
								out->prefix.user.host = STRINGCP(read_ptr_begin,read_ptr-read_ptr_begin);
								break;
							case '!':
								out->prefix.user.username = STRINGCP(read_ptr_begin,read_ptr-read_ptr_begin);
								break;
							default:
								break;
						}
					}
					read_ptr_begin = ++read_ptr;
					break;
				default:
					++read_ptr;
					break;
			}
		}
	}else
		out->prefix_type = IRC_MESSAGE_PREFIX_NONE;

	//Prepare termination variable that should be toggled when at end of message
	enum{
		IRC_PARSE_TERMINATE_NOT,
		IRC_PARSE_TERMINATE_EOF,
		IRC_PARSE_TERMINATE_CRLF,
	}terminate = IRC_PARSE_TERMINATE_NOT;

	//Command
	bool(*irc_parse_param)(struct irc_message* out,byte param_count,const char* begin,const char* after_end) = NULL;

	loop{
		//If at end of message
		if(read_ptr>=read_ptr_end)
			terminate = IRC_PARSE_TERMINATE_EOF;
		else if(read_ptr[0] == '\r' && read_ptr[1] == '\n')
			terminate = IRC_PARSE_TERMINATE_CRLF;

		//If end of command
		if(terminate!=IRC_PARSE_TERMINATE_NOT || read_ptr[0] == ' '){
			out->command_type_type = IRC_MESSAGE_COMMAND_TYPE_TYPE_ENUMERATOR;
			switch(read_ptr-read_ptr_begin){
				case 3:
					if(read_ptr_begin[0]>='0' && read_ptr_begin[0]<='9' &&
					   read_ptr_begin[1]>='0' && read_ptr_begin[1]<='9' &&
					   read_ptr_begin[2]>='0' && read_ptr_begin[2]<='9'){
						out->command_type_type = IRC_MESSAGE_COMMAND_TYPE_TYPE_NUMBER;
						out->command_type.number = (read_ptr_begin[0]-'0')*100 + (read_ptr_begin[1]-'0')*10 + (read_ptr_begin[2]-'0')*1;
						break;
					}
					goto NotFoundMessageType;
				case 4:
					if(memeq(read_ptr_begin,"JOIN",4)){
						out->command_type.enumerator = IRC_MESSAGE_COMMAND_TYPE_JOIN;
						out->command.join.channels = NULL;
						out->command.join.keys     = NULL;
						irc_parse_param = irc_parse_param_join;
						break;
					}
					if(memeq(read_ptr_begin,"PART",4)){
						out->command_type.enumerator = IRC_MESSAGE_COMMAND_TYPE_PART;
						out->command.part.channels = NULL;
						out->command.part.message  = Stringcp_init;
						irc_parse_param = irc_parse_param_part;
						break;
					}
					if(memeq(read_ptr_begin,"PING",4)){
						out->command_type.enumerator = IRC_MESSAGE_COMMAND_TYPE_PING;
						break;
					}
					if(memeq(read_ptr_begin,"PONG",4)){
						out->command_type.enumerator = IRC_MESSAGE_COMMAND_TYPE_PONG;
						break;
					}
					if(memeq(read_ptr_begin,"NICK",4)){
						out->command_type.enumerator = IRC_MESSAGE_COMMAND_TYPE_NICK;
						out->command.nick.name = Stringcp_init;
						irc_parse_param = irc_parse_param_nick;
						break;
					}
					if(memeq(read_ptr_begin,"KICK",4)){
						out->command_type.enumerator = IRC_MESSAGE_COMMAND_TYPE_KICK;
						out->command.kick.channels = NULL;
						out->command.kick.users    = NULL;
						irc_parse_param = irc_parse_param_kick;
						break;
					}
					if(memeq(read_ptr_begin,"QUIT",4)){
						out->command_type.enumerator = IRC_MESSAGE_COMMAND_TYPE_QUIT;
						out->command.quit.message = Stringcp_init;
						irc_parse_param = irc_parse_param_quit;
						break;
					}
					if(memeq(read_ptr_begin,"MODE",4)){
						out->command_type.enumerator = IRC_MESSAGE_COMMAND_TYPE_MODE;
						out->command.mode.nickname  = Stringcp_init;
						out->command.mode.operation = false;
						out->command.mode.mode      = '\0';
						irc_parse_param = irc_parse_param_mode;
						break;
					}
					if(memeq(read_ptr_begin,"KILL",4)){
						out->command_type.enumerator = IRC_MESSAGE_COMMAND_TYPE_KILL;
						out->command.kill.nickname = Stringcp_init;
						out->command.kill.comment  = Stringcp_init;
						irc_parse_param = irc_parse_param_kill;
						break;
					}
					if(memeq(read_ptr_begin,"AWAY",4)){
						out->command_type.enumerator = IRC_MESSAGE_COMMAND_TYPE_AWAY;
						out->command.away.text = Stringcp_init;
						irc_parse_param = irc_parse_param_away;
						break;
					}
					goto NotFoundMessageType;
				case 5:
					if(memeq(read_ptr_begin,"TOPIC",5)){
						out->command_type.enumerator = IRC_MESSAGE_COMMAND_TYPE_TOPIC;
						out->command.topic.channel = Stringcp_init;
						out->command.topic.text    = Stringcp_init;
						irc_parse_param = irc_parse_param_topic;
						break;
					}
					if(memeq(read_ptr_begin,"SQUIT",5)){
						out->command_type.enumerator = IRC_MESSAGE_COMMAND_TYPE_SQUIT;
						out->command.squit.server  = Stringcp_init;
						out->command.squit.comment = Stringcp_init;
						irc_parse_param = irc_parse_param_squit;
						break;
					}
					if(memeq(read_ptr_begin,"ERROR",5)){
						out->command_type.enumerator = IRC_MESSAGE_COMMAND_TYPE_ERROR;
						out->command.error.message = Stringcp_init;
						irc_parse_param = irc_parse_param_error;
						break;
					}
					goto NotFoundMessageType;
				case 6:
					if(memeq(read_ptr_begin,"NOTICE",6)){
						out->command_type.enumerator = IRC_MESSAGE_COMMAND_TYPE_NOTICE;
						out->command.notice.target = Stringcp_init;
						out->command.notice.text   = Stringcp_init;
						irc_parse_param = irc_parse_param_notice;
						break;
					}
					goto NotFoundMessageType;
				case 7:
					if(memeq(read_ptr_begin,"PRIVMSG",7)){
						out->command_type.enumerator = IRC_MESSAGE_COMMAND_TYPE_PRIVMSG;
						out->command.privmsg.target = Stringcp_init;
						out->command.privmsg.text   = Stringcp_init;
						irc_parse_param = irc_parse_param_privmsg;
						break;
					}
					goto NotFoundMessageType;
				default:
				NotFoundMessageType:
					out->command_type_type = IRC_MESSAGE_COMMAND_TYPE_TYPE_UNKNOWN;
					out->command_type.unknown = STRINGCP(read_ptr_begin,read_ptr-read_ptr_begin);
					break;
			}

			read_ptr_begin = ++read_ptr;

			switch(terminate){
				case IRC_PARSE_TERMINATE_NOT:
					break;
				case IRC_PARSE_TERMINATE_EOF:
					goto ParseTerminate;
				case IRC_PARSE_TERMINATE_CRLF:
					goto ParseTerminateCRLF;
			}
			break;
		}

		++read_ptr;
	}

	//Params
	if(irc_parse_param){
		byte param_count = 0;
		bool trailing = false;
		loop{
			//If at end of message
			if(read_ptr>=read_ptr_end)
				terminate = IRC_PARSE_TERMINATE_EOF;
			else{
				if(read_ptr[0] == '\r' && read_ptr[1] == '\n')
					terminate = IRC_PARSE_TERMINATE_CRLF;
				//Param trailing
				else if(read_ptr[0] == ':'){
					read_ptr_begin = ++read_ptr;
					trailing = true;
				}
			}

			//If end of param
			if(terminate!=IRC_PARSE_TERMINATE_NOT || (!trailing && read_ptr[0] == ' ')){
				//TODO: Standards says 14 is the max amount of parameters
				if(!irc_parse_param(out,++param_count,read_ptr_begin,read_ptr))
					printf("Error: Parse error on message command parameter %u: \"%.*s\"\n",param_count,(int)(read_ptr-read_ptr_begin),read_ptr_begin);

				switch(terminate){
					case IRC_PARSE_TERMINATE_NOT:
						break;
					case IRC_PARSE_TERMINATE_EOF:
						goto ParseTerminate;
					case IRC_PARSE_TERMINATE_CRLF:
						goto ParseTerminateCRLF;
				}

				read_ptr_begin = ++read_ptr;
			}else
				++read_ptr;
		};
	}
	//Ignore params until end
	else{
		loop{
			if(read_ptr>=read_ptr_end)
				terminate = IRC_PARSE_TERMINATE_EOF;
			else if(read_ptr[0] == '\r' && read_ptr[1] == '\n')
				terminate = IRC_PARSE_TERMINATE_CRLF;

			//If end of param
			if(terminate!=IRC_PARSE_TERMINATE_NOT){
				out->command.unknown.params = STRINGCP(read_ptr_begin,read_ptr-read_ptr_begin);

				switch(terminate){
					case IRC_PARSE_TERMINATE_EOF:
						goto ParseTerminate;
					case IRC_PARSE_TERMINATE_CRLF:
						goto ParseTerminateCRLF;
					case IRC_PARSE_TERMINATE_NOT:
						break;//Should not happen because of check some lines up
				}
			}

			++read_ptr;
		}
	}

	ParseTerminate:
		return STRINGCP(read_ptr,raw_message.length-(read_ptr-raw_message.ptr));

	ParseTerminateCRLF:
		read_ptr+=2;
		goto ParseTerminate;
}

bool irc_read_message(const irc_connection* connection,void(*onMessageFunc)(const irc_connection* connection,const irc_message* message)){
	ssize_t read_len;

	//If a message is sent from the server
	if((read_len = irc_read(connection,STRINGP(connection->read_buffer,IRC_BUFFER_LENGTH)))){
		if(read_len<0){//Error checking
			fprintf(stderr,"Error: read() returned negative value: %zi\n",read_len);
			return false;
		}

		//Print the raw message that was received
		Stringp_put(STRINGP(connection->read_buffer,read_len),stdout);

		irc_message message;
		Stringcp read_string = STRINGCP(connection->read_buffer,read_len);
		do{
			read_string = irc_parse_message(connection,read_string,&message);
			onMessageFunc(connection,&message);
		}while(read_string.length>0);

		return true;
	}
	return false;
}

ssize_t irc_read(const irc_connection* connection,Stringp out){
	return read(connection->id,out.ptr,out.length);
}

extern inline void irc_set_nickname(const irc_connection* connection,const char* name);
extern inline void irc_set_username(const irc_connection* connection,const char* username,const char* realname);
extern inline void irc_join_channel(const irc_connection* connection,const char* channel);
extern inline void irc_part_channel(const irc_connection* connection,const char* channel);
extern inline void irc_send_rawnt(const irc_connection* connection,const char* str);
