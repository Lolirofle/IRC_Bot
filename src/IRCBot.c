#include <stdio.h> //Input/output
#include <unistd.h>//Unix standard library
#include <string.h>

#include <lolie/Stringp.h>
#include <lolie/LinkedList.h>
#include <lolie/Memory.h>

#include "irc.h"
#include "Locale.h"

/**
 * Buffer initialization
 */
#define IRC_WRITE_BUFFER_LEN 512
char write_buffer[IRC_WRITE_BUFFER_LEN];
char command_prefix = '!',
     command_separator = ' ',
     command_arg_separator = ' ';
enum Languages language = LANG_SWEDISH;

void onMessageFunc(irc_connection_id id,const irc_message* message){
	switch(message->command_type){
		case IRC_MESSAGE_COMMAND_NUMBER:
			if(message->command_type_number == 1){
				irc_join_channel(id,"#bot");
				irc_join_channel(id,"#toa");
			}
			break;
		case IRC_MESSAGE_COMMAND_PRIVMSG:
			//If on a channel with a '#' prefix and the private message has a : prefix
			if(message->command.privmsg.target.ptr[0] == '#' && message->command.privmsg.text.ptr[0] == command_prefix){
				//Initialize read pointers
				char* read_ptr = message->command.privmsg.text.ptr+1,
				    * read_ptr_begin = read_ptr,
				    * read_ptr_end = message->command.privmsg.text.ptr+message->command.privmsg.text.length;

				//Initialize command Stringp
				while(read_ptr<read_ptr_end && *read_ptr!=command_separator)
					++read_ptr;
				Stringp command = STRINGP(read_ptr_begin,read_ptr-read_ptr_begin);
				read_ptr_begin=++read_ptr;

				//Commands
				switch(command.length){
					case 4:
						//Bool
						if(Data_equals(command.ptr,"bool",4)){
							irc_send_message(id,message->command.privmsg.target,rand()%2?STRINGP("true",4):STRINGP("false",5));
							goto SuccessCommand;
						}
						//Dice
						if(Data_equals(command.ptr,"dice",4)){
							write_buffer[0]=rand()%6+'1';
							irc_send_message(id,message->command.privmsg.target,STRINGP(write_buffer,1));
							goto SuccessCommand;
						}
						//Test
						if(Data_equals(command.ptr,"test",4)){
							for(int i=0;i<5;++i)
								irc_send_message(id,message->command.privmsg.target,STRINGP("ABCDEF",6));
							goto SuccessCommand;
						}
						//Echo
						if(Data_equals(command.ptr,"echo",4)){
							irc_send_message(id,message->command.privmsg.target,STRINGP(read_ptr_begin,read_ptr_end-read_ptr_begin));
							goto SuccessCommand;
						}
						//Wikipedia
						if(Data_equals(command.ptr,"wiki",4)){
							int len = Stringp_sput(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),2,
								STRINGP("http://en.wikipedia.org/wiki/",51),
								STRINGP(read_ptr_begin,read_ptr_end-read_ptr_begin)
							);
							irc_send_message(id,message->command.privmsg.target,STRINGP(write_buffer,len));
							goto SuccessCommand;
						}
						//IMDb
						if(Data_equals(command.ptr,"imdb",4)){
							int len = Stringp_sput(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),2,
								STRINGP("http://www.imdb.com/find?s=all&q=",33),
								STRINGP(read_ptr_begin,read_ptr_end-read_ptr_begin)
							);
							irc_send_message(id,message->command.privmsg.target,STRINGP(write_buffer,len));
							goto SuccessCommand;
						}
						goto UnknownCommand;
					case 5:
						//Upper
						if(Data_equals(command.ptr,"upper",5)){
							char* write_ptr = write_buffer;
							while(read_ptr<read_ptr_end){
								*write_ptr++=*read_ptr>='a' && *read_ptr<='z'?(*read_ptr)+('A'-'a'):*read_ptr;
								++read_ptr;
							}

							irc_send_message(id,message->command.privmsg.target,STRINGP(write_buffer,read_ptr_end-read_ptr_begin));
							goto SuccessCommand;
						}
						//Lower
						if(Data_equals(command.ptr,"lower",5)){
							char* write_ptr = write_buffer;
							while(read_ptr<read_ptr_end){
								*write_ptr++=*read_ptr>='A' && *read_ptr<='Z'?(*read_ptr)+('a'-'A'):*read_ptr;
								++read_ptr;
							}

							irc_send_message(id,message->command.privmsg.target,STRINGP(write_buffer,read_ptr_end-read_ptr_begin));
							goto SuccessCommand;
						}
						//ROT13
						if(Data_equals(command.ptr,"rot13",5)){
							char* write_ptr = write_buffer;
							while(read_ptr<read_ptr_end){
								if(*read_ptr>='A' && *read_ptr<='Z')
									*write_ptr++=((*read_ptr)-'A'+13)%26+'A';
								else if(*read_ptr>='a' && *read_ptr<='z')
									*write_ptr++=((*read_ptr)-'a'+13)%26+'a';
								else if(*read_ptr>='0' && *read_ptr<='9')
									*write_ptr++=((*read_ptr)-'0'+5)%10+'0';
								else
									*write_ptr++=*read_ptr;
								++read_ptr;
							}

							irc_send_message(id,message->command.privmsg.target,STRINGP(write_buffer,read_ptr_end-read_ptr_begin));
							goto SuccessCommand;
						}
						//ROT47
						if(Data_equals(command.ptr,"rot47",5)){
							char* write_ptr = write_buffer;
							while(read_ptr<read_ptr_end){
								if(*read_ptr>='!' && *read_ptr<='~')
									*write_ptr++=((*read_ptr)-'!'+47)%94+'!';
								else
									*write_ptr++=*read_ptr;
								++read_ptr;
							}

							irc_send_message(id,message->command.privmsg.target,STRINGP(write_buffer,read_ptr_end-read_ptr_begin));
							goto SuccessCommand;
						}
						goto UnknownCommand;
					case 6:
						//Random
						if(Data_equals(command.ptr,"random",6)){
							int len=snprintf(write_buffer,IRC_WRITE_BUFFER_LEN,"%u (0 to %u)",rand(),RAND_MAX);
							irc_send_message(id,message->command.privmsg.target,STRINGP(write_buffer,len));
							goto SuccessCommand;
						}
						//Choose
						if(Data_equals(command.ptr,"choose",6)){
							irc_send_message(id,message->command.privmsg.target,STRINGP("Not implemented yet",19));
							goto SuccessCommand;
						}
						//Length
						if(Data_equals(command.ptr,"length",6)){
							int len=snprintf(write_buffer,IRC_WRITE_BUFFER_LEN,"%li",read_ptr_end-read_ptr_begin);
							irc_send_message(id,message->command.privmsg.target,STRINGP(write_buffer,len));
							goto SuccessCommand;
						}
						//Google
						if(Data_equals(command.ptr,"google",6)){
							int len = Stringp_sput(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),2,
								STRINGP("https://www.google.com/search?q=",32),
								STRINGP(read_ptr_begin,read_ptr_end-read_ptr_begin)
							);
							irc_send_message(id,message->command.privmsg.target,STRINGP(write_buffer,len));
							goto SuccessCommand;
						}
						goto UnknownCommand;
					case 7:
						//Reverse
						if(Data_equals(command.ptr,"reverse",7)){
							char* write_ptr = write_buffer;
							read_ptr = read_ptr_end;
							while(read_ptr>read_ptr_begin)
								*write_ptr++=*--read_ptr;
							irc_send_message(id,message->command.privmsg.target,STRINGP(write_buffer,read_ptr_end-read_ptr_begin));
							goto SuccessCommand;
						}
						goto UnknownCommand;
					case 8:
						//Language
						if(Data_equals(command.ptr,"language",8)){
							for(unsigned int i=0;i<LANG_COUNT;++i)
								if(Data_equals(locale[i].lang_name.ptr,read_ptr_begin,locale[i].lang_name.length)){
									language=i;
									irc_send_message(id,message->command.privmsg.target,locale[language].language.set);		
									goto SuccessCommand;
								}
							irc_send_message(id,message->command.privmsg.target,locale[language].language.unknown);
							goto SuccessCommand;
						}
						goto UnknownCommand;
					case 9:
						//Word count
						if(Data_equals(command.ptr,"wordcount",9)){
							unsigned int count=0;
							while(read_ptr<read_ptr_end)
								if(*read_ptr++==' ')
									++count;

							int len=snprintf(write_buffer,IRC_WRITE_BUFFER_LEN,"%u words",count);
							irc_send_message(id,message->command.privmsg.target,STRINGP(write_buffer,len));
							goto SuccessCommand;
						}
						goto UnknownCommand;
					case 10:
						//Magic 8-ball
						if(Data_equals(command.ptr,"magic8ball",10)){
							unsigned short len=0;
							if(read_ptr_end-read_ptr_begin<2 || *(read_ptr_end-1)!='?'){
								len=locale[language].magic8ball.ask.length;  memcpy(write_buffer,locale[language].magic8ball.ask.ptr,len);
							}else{
								switch(rand()%20){
									case  0:len=locale[language].magic8ball.yes[0].length;  memcpy(write_buffer,locale[language].magic8ball.yes[0].ptr,len);break;
									case  1:len=locale[language].magic8ball.yes[1].length;  memcpy(write_buffer,locale[language].magic8ball.yes[1].ptr,len);break;
									case  2:len=locale[language].magic8ball.yes[2].length;  memcpy(write_buffer,locale[language].magic8ball.yes[2].ptr,len);break;
									case  3:len=locale[language].magic8ball.yes[3].length;  memcpy(write_buffer,locale[language].magic8ball.yes[3].ptr,len);break;
									case  4:len=locale[language].magic8ball.yes[4].length;  memcpy(write_buffer,locale[language].magic8ball.yes[4].ptr,len);break;
									case  5:len=locale[language].magic8ball.yes[5].length;  memcpy(write_buffer,locale[language].magic8ball.yes[5].ptr,len);break;
									case  6:len=locale[language].magic8ball.yes[6].length;  memcpy(write_buffer,locale[language].magic8ball.yes[6].ptr,len);break;
									case  7:len=locale[language].magic8ball.yes[7].length;  memcpy(write_buffer,locale[language].magic8ball.yes[7].ptr,len);break;
									case  8:len=locale[language].magic8ball.yes[8].length;  memcpy(write_buffer,locale[language].magic8ball.yes[8].ptr,len);break;
									case  9:len=locale[language].magic8ball.yes[9].length;  memcpy(write_buffer,locale[language].magic8ball.yes[9].ptr,len);break;
									case 10:len=locale[language].magic8ball.maybe[0].length;memcpy(write_buffer,locale[language].magic8ball.maybe[0].ptr,len);break;
									case 11:len=locale[language].magic8ball.maybe[1].length;memcpy(write_buffer,locale[language].magic8ball.maybe[1].ptr,len);break;
									case 12:len=locale[language].magic8ball.maybe[2].length;memcpy(write_buffer,locale[language].magic8ball.maybe[2].ptr,len);break;
									case 13:len=locale[language].magic8ball.maybe[3].length;memcpy(write_buffer,locale[language].magic8ball.maybe[3].ptr,len);break;
									case 14:len=locale[language].magic8ball.maybe[4].length;memcpy(write_buffer,locale[language].magic8ball.maybe[4].ptr,len);break;
									case 15:len=locale[language].magic8ball.no[0].length;   memcpy(write_buffer,locale[language].magic8ball.no[0].ptr,len);break;
									case 16:len=locale[language].magic8ball.no[1].length;   memcpy(write_buffer,locale[language].magic8ball.no[1].ptr,len);break;
									case 17:len=locale[language].magic8ball.no[2].length;   memcpy(write_buffer,locale[language].magic8ball.no[2].ptr,len);break;
									case 18:len=locale[language].magic8ball.no[3].length;   memcpy(write_buffer,locale[language].magic8ball.no[3].ptr,len);break;
									case 19:len=locale[language].magic8ball.no[4].length;   memcpy(write_buffer,locale[language].magic8ball.no[4].ptr,len);break;
								}
								if(len==0){
									len=locale[language].magic8ball.failure.length;  memcpy(write_buffer,locale[language].magic8ball.failure.ptr,len);
								}
							}
							irc_send_message(id,message->command.privmsg.target,STRINGP(write_buffer,len));
							goto SuccessCommand;
						}
						goto UnknownCommand;
				}

				UnknownCommand:{
					int len = Stringp_sput(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),2,
						locale[language].unknown_command,
						command
					);
					irc_send_message(id,message->command.privmsg.target,STRINGP(write_buffer,len));
				}
			}
			SuccessCommand:
			break;
	}
}

int main(){
	irc_connection_id connection_id=irc_connect("flygande-toalett.tk",1568);
	irc_set_nickname(connection_id,"Toabot");
	irc_set_username(connection_id,"Bot");

	char read_buffer[IRC_BUFFER_LENGTH+1];
	int read_len;

	//While a message is sent from the server
	while((read_len = read(connection_id,read_buffer,IRC_BUFFER_LENGTH))){
		if(read_len<0){//Error checking
			fprintf(stderr,"Error: read() returned negative value: %i\n",read_len);
			return EXIT_FAILURE;
		}

		//Print the raw message that was received
		Stringp_put(STRINGP(read_buffer,read_len),stdout);

		irc_parse_message(connection_id,STRINGP(read_buffer,read_len),&onMessageFunc);
	}	
	return EXIT_SUCCESS;
}
