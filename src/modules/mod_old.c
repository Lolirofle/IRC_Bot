#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ircbot/api/IRCBot.h>
#include <ircbot/api/Command.h>
#include <ircbot/api/Commands.h>
#include <ircbot/Locale.h>
#include <lolien/types.h>
#include <lolien/seq/StringP.h>
#include <lolien/url.h>

const char plugin_version[] = "1.0";
const char plugin_author[]  = "Lolirofle";

static struct Command* c=NULL;

#define MOD_OLD_COMMANDCOUNT 20

bool plugin_onLoad(struct IRCBot* bot){
	if(!(c=malloc(sizeof(struct Command)*MOD_OLD_COMMANDCOUNT))){
		fputs("Error: Cannot allocate memory for commands",stderr);
		return false;
	}

	c[0]=(struct Command){
		StringCP_fromCStr("test"),
		StringCP_fromCStr("Test command"),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			#define TEST_STRING "Test command has been executed"
			IRCBot_sendMessage(bot,target,STRINGCP(TEST_STRING,sizeof(TEST_STRING)));
			return true;
		})
	};

	c[1]=(struct Command){
		StringCP_fromCStr("bool"),
		StringCP_fromCStr("Outputs true or false randomly"),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			IRCBot_sendMessage(bot,target,locale[language].boolean[rand()%2]);
			return true;
		})
	};

	c[2]=(struct Command){
		StringCP_fromCStr("dice"),
		StringCP_fromCStr("Roll a dice"),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			write_buffer[0]=rand()%6+'1';
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,1));
			return true;
		})
	};

	c[3]=(struct Command){
		StringCP_fromCStr("wiki"),
		StringCP_fromCStr("Creates a wikipedia link"),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			int len = StringP_vcopy(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),1,
				StringCP_fromCStr("http://en.wikipedia.org/wiki/")
			);
			len+=url_encode(STRINGCP(arg->begin,arg->end-arg->begin),STRINGP(write_buffer+len,IRC_WRITE_BUFFER_LEN-len));
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,len));
			return true;
		})
	};

	c[4]=(struct Command){
		StringCP_fromCStr("imdb"),
		StringCP_fromCStr("Creates a IMDb search link"),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			int len = StringP_vcopy(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),1,
				StringP_fromCStr("http://www.imdb.com/find?s=all&q=")
			);
			len+=url_encode(STRINGCP(arg->begin,arg->end-arg->begin),STRINGP(write_buffer+len,IRC_WRITE_BUFFER_LEN-len));
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,len));
			return true;
		})
	};

	c[5]=(struct Command){
		StringCP_fromCStr("date"),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			time_t t;time(&t);
			struct tm* time_data = localtime(&t);

			int len = strftime(write_buffer,IRC_WRITE_BUFFER_LEN,"%F %X %Z, %A v.%V, Day %j of the year",time_data);
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,len));
			return true;
		})
	};

	//Upper
	c[6]=(struct Command){
		StringCP_fromCStr("upper"),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			char* write_ptr = write_buffer;
			size_t argLen=arg->end-arg->begin;

			while(arg->begin<arg->end){
				*write_ptr++=*arg->begin>='a' && *arg->begin<='z'?(*arg->begin)+('A'-'a'):*arg->begin;
				++arg->begin;
			}

			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,argLen));
			return true;
		})
	};
	//Lower
	c[7]=(struct Command){
		StringCP_fromCStr("lower"),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			char* write_ptr = write_buffer;
			size_t argLen=arg->end-arg->begin;

			while(arg->begin<arg->end){
				*write_ptr++=*arg->begin>='A' && *arg->begin<='Z'?(*arg->begin)+('a'-'A'):*arg->begin;
				++arg->begin;
			}

			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,argLen));
			return true;
		})
	};
	//ROT13
	c[8]=(struct Command){
		StringCP_fromCStr("rot13"),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			char* write_ptr = write_buffer;
			size_t argLen=arg->end-arg->begin;

			while(arg->begin<arg->end){
				if(*arg->begin>='A' && *arg->begin<='Z')
					*write_ptr++=((*arg->begin)-'A'+13)%26+'A';
				else if(*arg->begin>='a' && *arg->begin<='z')
					*write_ptr++=((*arg->begin)-'a'+13)%26+'a';
				else if(*arg->begin>='0' && *arg->begin<='9')
					*write_ptr++=((*arg->begin)-'0'+5)%10+'0';
				else
					*write_ptr++=*arg->begin;
				++arg->begin;
			}

			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,argLen));
			return true;
		})
	};
	//ROT47
	c[9]=(struct Command){
		StringCP_fromCStr("rot47"),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			char* write_ptr = write_buffer;
			size_t argLen=arg->end-arg->begin;

			while(arg->begin<arg->end){
				if(*arg->begin>='!' && *arg->begin<='~')
					*write_ptr++=((*arg->begin)-'!'+47)%94+'!';
				else
					*write_ptr++=*arg->begin;
				++arg->begin;
			}

			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,argLen));
			return true;
		})
	};

	//Random
	c[10]=(struct Command){
		StringCP_fromCStr("random"),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			unsigned int value=rand();
			unsigned int min=1;
			unsigned int max=RAND_MAX;
			const char* read_ptr=arg->begin;

			if(arg->end-arg->begin>0)//If argument 1 exists (random <max>)
				while(true)
					if(read_ptr>arg->begin && (read_ptr>=arg->end || read_ptr[0]==' ')){
						max=decStrToInt(arg->begin,MIN(read_ptr-arg->begin,9));
						arg->begin=++read_ptr;

						if(arg->end-arg->begin>0)//If argument 2 exists (random <min> <max>)
							while(true)
								if(read_ptr>arg->begin && (read_ptr>=arg->end || read_ptr[0]==' ')){
									min=max;
									max=decStrToInt(arg->begin,MIN(read_ptr-arg->begin,9));
									arg->begin=++read_ptr;
									break;
								}
								else if(read_ptr[0]>='0' && read_ptr[0]<='9')
									++read_ptr;
								else
									break;
						break;
					}
					else if(read_ptr[0]>='0' && read_ptr[0]<='9')
						++read_ptr;
					else
						break;
			int len=snprintf(write_buffer,IRC_WRITE_BUFFER_LEN,"%u (%u to %u)",max>min?(value%(max-min+1))+min:value,min,max);
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,len));
			return true;
		})
	};
	//Choose
	c[11]=(struct Command){
		StringCP_fromCStr("choose"),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			if(arg->begin>=arg->end){
				IRCBot_sendMessage(bot,target,locale[language].missing_argument);
				return false;
			}

			LinkedList* list = LinkedList_init;
			size_t list_length = 0;

			string_splitted(STRINGP(arg->begin,arg->end-arg->begin),function(size_t,(StringP str){
				if(str.length>=1 && (*str.ptr=='|' || *str.ptr==',')){
					unsigned short whitespaces=1;
					while(str.length>0 && *++str.ptr==' ')
						++whitespaces;
					return whitespaces;
				}
				else
					return 0;
			}),function(bool,(const char* begin,const char* end){
				if(begin >= end)
					return true;

				StringCP* arg = smalloc(sizeof(StringCP));
				*arg = STRINGCP(begin,end-begin);
				LinkedList_push(&list,arg);

				++list_length;

				return true;
			}));

			if(list_length>0)
				IRCBot_sendMessage(bot,target,*(StringCP*)LinkedList_get(list,rand()%list_length));

			LinkedList_clean(&list,function(bool,(void* elem){
				free(elem);
				return true;
			}));

			return true;
		})
	};
	//Length
	c[12]=(struct Command){
		StringCP_fromCStr("length"),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			int len=snprintf(write_buffer,IRC_WRITE_BUFFER_LEN,"%li",arg->end-arg->begin);
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,len));
			return true;
		})
	};
	//Google
	c[13]=(struct Command){
		StringCP_fromCStr("google"),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			int len = StringP_vcopy(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),1,
				StringP_fromCStr("https://www.google.com/search?q=")
			);
			len+=url_encode(STRINGCP(arg->begin,arg->end-arg->begin),STRINGP(write_buffer+len,IRC_WRITE_BUFFER_LEN-len));
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,len));
			return true;
		})
	};
	//Prefix
	c[14]=(struct Command){
		StringCP_fromCStr("prefix"),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			int len = arg->end-arg->begin;
			int write_len;

			if(len>0){
				free(bot->commandPrefix.ptr);
				bot->commandPrefix = STRINGP(smalloc(len),len);
				memcpy(bot->commandPrefix.ptr,arg->begin,len);
				write_len = StringP_vcopy(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),4,
					locale[language].prefix.set,
					STRINGCP(" \"",2),
					STRINGCP(arg->begin,len),
					STRINGCP("\"",1)
				);
			}else{
				write_len = StringP_vcopy(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),4,
					locale[language].prefix.get,
					STRINGCP(" \"",2),
					bot->commandPrefix,
					STRINGCP("\"",1)
				);
			}
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,write_len));
			return true;
		})
	};
	//Reverse
	c[15]=(struct Command){
		StringCP_fromCStr("reverse"),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			char* write_ptr = write_buffer;
			const char* read_ptr = arg->end;

			while(read_ptr>arg->begin)
				*write_ptr++=*--read_ptr;
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,arg->end-arg->begin));
			return true;
		})
	};
	//Language
	c[16]=(struct Command){
		StringCP_fromCStr("language"),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			for(unsigned int i=0;i<LANG_COUNT;++i)
				if(memcmp(locale[i].lang_name.ptr,arg->begin,locale[i].lang_name.length)==0){
					language=i;
					IRCBot_sendMessage(bot,target,locale[language].language.set);		
					return true;
				}
			IRCBot_sendMessage(bot,target,locale[language].language.unknown);
			return false;
		})
	};
	//Word count
	c[17]=(struct Command){
		StringCP_fromCStr("wordcount"),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			unsigned int count=0;

			if(arg->begin<arg->end){
				count=1;
				while(arg->begin<arg->end)
					if(*arg->begin++==' ')//TODO: The ability to count complex sentences
						++count;
			}

			int len=snprintf(write_buffer,IRC_WRITE_BUFFER_LEN,"%u words",count);
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,len));
			return true;
		})
	};
	//URL encode
	c[18]=(struct Command){
		StringCP_fromCStr("urlencode"),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,url_encode(STRINGCP(arg->begin,arg->end-arg->begin),STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN))));
			return true;
		})
	};
	//Magic 8-ball
	c[19]=(struct Command){//TODO: magic8ball isn't in the help listing
		StringCP_fromCStr("magic8ball"),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			unsigned short len=0;
			if(arg->end-arg->begin<2 || *(arg->end-1)!='?'){
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
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,len));
			return true;
		})
	};
	/*
	c[]=(struct Command){
		StringCP_fromCStr(""),
		StringCP_fromCStr(""),
		function(bool,(struct IRCBot* bot,StringCP target,struct CommandArgument* arg){
			return true;
		})
	};
	*/

	return registerCommandsFromArray(&bot->commands,c,MOD_OLD_COMMANDCOUNT);
}

bool plugin_onUnload(struct IRCBot* bot){
	for(uint i=0;i<MOD_OLD_COMMANDCOUNT;++i)
		if(!unregisterCommandByName(&bot->commands,c[i].name))
			fprintf(stderr,"Module: mod_old: Warning: Command couldn't be freed: %s\n",c[i].name.ptr);

	if(c){
		free(c);
		c=NULL;
	}
	return true;
}
