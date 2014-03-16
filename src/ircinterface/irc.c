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
		STRINGP("PRIVMSG ",8),
		target,
		STRINGP(" :",2),
		message,
		STRINGP("\r\n",2)
	);
	irc_send_raw(connection,write_buffer,len);
}

void irc_parse_message(const irc_connection* connection,Stringcp raw_message,void(*onMessageFunc)(const irc_connection* connection,const irc_message* message)){
	//If standard message prefix
	if(raw_message.ptr[0] == ':'){
		irc_message message;
		message.raw_message = raw_message;

		char* read_ptr       = raw_message.ptr+1,
		    * read_ptr_begin = read_ptr,
		    * read_ptr_end   = raw_message.ptr+raw_message.length;

		//Prefix
		message.prefix_type = IRC_MESSAGE_PREFIX_UNKNOWN;
		while(true){
			if(read_ptr>=read_ptr_end)
				return;
			if(read_ptr[0] == '\r' && read_ptr[1] == '\n')
				goto TermNewCommand;

			if(*read_ptr == ' '){//If end of prefix
				if(message.prefix_type == IRC_MESSAGE_PREFIX_USER){//If already determined it is a user message, then it is a hostname
					message.prefix.user.host = STRINGP(read_ptr_begin,read_ptr-read_ptr_begin);
					read_ptr_begin = ++read_ptr;
				}else if(message.prefix_type == IRC_MESSAGE_PREFIX_UNKNOWN){//If not yet determined, then it is a servername
					message.prefix.server.name = STRINGP(read_ptr_begin,read_ptr-read_ptr_begin);
					message.prefix_type = IRC_MESSAGE_PREFIX_SERVER;
					read_ptr_begin = ++read_ptr;
				}
				break;
			}

			if(message.prefix_type == IRC_MESSAGE_PREFIX_USER && *read_ptr == '@'){//If already found '!' and finds '@', then it is a username
				message.prefix.user.username = STRINGP(read_ptr_begin,read_ptr-read_ptr_begin);
				read_ptr_begin = ++read_ptr;
			}
			else if(*read_ptr == '!'){//If separated by '!', then it is a nickname
				message.prefix.user.nickname = STRINGP(read_ptr_begin,read_ptr-read_ptr_begin);
				message.prefix_type = IRC_MESSAGE_PREFIX_USER;
				read_ptr_begin = ++read_ptr;
			}

			++read_ptr;
		}

		//Command
		message.command_type = IRC_MESSAGE_TYPE_UNKNOWN;
		while(true){
			if(read_ptr>=read_ptr_end)
				return;
			if(read_ptr[0] == '\r' && read_ptr[1] == '\n')
				goto TermNewCommand;

			if(*read_ptr == ' '){//If end of command
				switch(read_ptr-read_ptr_begin){
					case 3:
						if(read_ptr_begin[0]>='0' && read_ptr_begin[0]<='9' &&
						   read_ptr_begin[1]>='0' && read_ptr_begin[1]<='9' &&
						   read_ptr_begin[2]>='0' && read_ptr_begin[2]<='9'){
							message.command_type = IRC_MESSAGE_TYPE_NUMBER;
							message.command_type_number = atoi((char[4]){read_ptr_begin[0],read_ptr_begin[1],read_ptr_begin[2],'\0'});
						}
						break;
					case 4:
						if(memeq(read_ptr_begin,"JOIN",4)){
							message.command_type = IRC_MESSAGE_TYPE_JOIN;
							message.command.channels=NULL;
						}else if(memeq(read_ptr_begin,"PART",4)){
							message.command_type = IRC_MESSAGE_TYPE_PART;
							message.command.channels=NULL;
						}else if(memeq(read_ptr_begin,"NICK",4))
							message.command_type = IRC_MESSAGE_TYPE_NICK;
						else if(memeq(read_ptr_begin,"KICK",4))
							message.command_type = IRC_MESSAGE_TYPE_KICK;
						break;
					case 5:
						if(memeq(read_ptr_begin,"TOPIC",5))
							message.command_type = IRC_MESSAGE_TYPE_TOPIC;
						break;
					case 6:
						if(memeq(read_ptr_begin,"NOTICE",6))
							message.command_type = IRC_MESSAGE_TYPE_NOTICE;
						break;
					case 7:
						if(memeq(read_ptr_begin,"PRIVMSG",7))
							message.command_type = IRC_MESSAGE_TYPE_PRIVMSG;
						break;
				}
				read_ptr_begin = ++read_ptr;
				break;
			}

			++read_ptr;
		}

		//Params
		bool repeat=false;
		unsigned char paramCount=0;
		while(true){//TODO: Implement all the other message command types and not only privmsg
			if((repeat=(read_ptr[0] == '\r' && read_ptr[1] == '\n')) || read_ptr>=read_ptr_end){
				if(paramCount==1){
					switch(message.command_type){
						case IRC_MESSAGE_TYPE_PRIVMSG:
							message.command.privmsg.text = STRINGP(read_ptr_begin,read_ptr-read_ptr_begin);
							break;
					}
				}
				break;
			}
			
			if(paramCount==0){
				if(read_ptr[0] == ' '){
					switch(message.command_type){
						case IRC_MESSAGE_TYPE_PRIVMSG:
							message.command.privmsg.target = STRINGP(read_ptr_begin,read_ptr-read_ptr_begin);
							break;
						case IRC_MESSAGE_TYPE_JOIN:{
							Stringp* tmp = smalloc(sizeof(Stringp*));
							*tmp = STRINGP(read_ptr_begin,read_ptr-read_ptr_begin);
							LinkedList_push(&message.command.channels,tmp);
						}	break;
					}
					if(*++read_ptr == ':')
						++read_ptr;
					read_ptr_begin = read_ptr;
					++paramCount;
				}
			}
			++read_ptr;
		}

		if(onMessageFunc!=NULL){
			message.raw_message.length=read_ptr+2-message.raw_message.ptr;
			onMessageFunc(connection,&message);
		}
		if(repeat){
			TermNewCommand:
			return irc_parse_message(connection,STRINGCP(read_ptr+2,raw_message.length-(read_ptr+2-raw_message.ptr)),onMessageFunc);
		}
		else
			return;
	}
	//Else if it is a ping request 
	else if(memeq(raw_message.ptr,"PING",4)){
		char tmp[raw_message.length];
		memcpy(tmp,raw_message.ptr,raw_message.length);
		tmp[1]='O';
		irc_send_raw(connection,tmp,raw_message.length);//Send
	}
}

bool irc_read_message(const irc_connection* connection,void(*onMessageFunc)(const irc_connection* connection,const irc_message* message)){
	int read_len;

	//If a message is sent from the server
	if((read_len = read(connection->id,connection->read_buffer,IRC_BUFFER_LENGTH))){
		if(read_len<0){//Error checking
			fprintf(stderr,"Error: read() returned negative value: %i\n",read_len);
			return false;
		}

		//Print the raw message that was received
		Stringp_put(STRINGP(connection->read_buffer,read_len),stdout);

		irc_parse_message(connection,STRINGCP(connection->read_buffer,read_len),onMessageFunc);

		return true;
	}
	return false;
}

size_t irc_read(const irc_connection* connection,Stringp out){
	int read_len=read(connection->id,out.ptr,out.length);

	//If a message is sent from the server
	if(read_len<=0){
		fprintf(stderr,"Error: read() returned errorneous value: %i\n",read_len);
		return 0;
	}
	return read_len;
}

extern inline void irc_set_nickname(const irc_connection* connection,const char* name);
extern inline void irc_set_username(const irc_connection* connection,const char* username,const char* realname);
extern inline void irc_join_channel(const irc_connection* connection,const char* channel);
extern inline void irc_part_channel(const irc_connection* connection,const char* channel);
extern inline void irc_send_rawnt(const irc_connection* connection,const char* str);
