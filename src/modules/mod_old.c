#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ircbot/api/IRCBot.h>
#include <ircbot/api/Command.h>
#include <ircbot/api/Commands.h>
#include <ircbot/Locale.h>
#include <lolie/TypeAliases.h>
#include <lolie/Memory.h>
#include <lolie/Stringp.h>
#include <lolie/url.h>
#include <lolie/Math.h>

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
		Stringcp_from_cstr("test"),
		Stringcp_from_cstr("Test command"),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			#define TEST_STRING "Test command has been executed"
			IRCBot_sendMessage(bot,target,STRINGCP(TEST_STRING,sizeof(TEST_STRING)));
			return true;
		})
	};

	c[1]=(struct Command){
		Stringcp_from_cstr("bool"),
		Stringcp_from_cstr("Outputs true or false randomly"),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			IRCBot_sendMessage(bot,target,locale[language].boolean[rand()%2]);
			return true;
		})
	};

	c[2]=(struct Command){
		Stringcp_from_cstr("dice"),
		Stringcp_from_cstr("Roll a dice"),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			write_buffer[0]=rand()%6+'1';
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,1));
			return true;
		})
	};

	c[3]=(struct Command){
		Stringcp_from_cstr("wiki"),
		Stringcp_from_cstr("Creates a wikipedia link"),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			int len = Stringp_vcopy(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),1,
				Stringcp_from_cstr("http://en.wikipedia.org/wiki/")
			);
			len+=url_encode(STRINGCP(arg->free.begin,arg->free.end-arg->free.begin),STRINGP(write_buffer+len,IRC_WRITE_BUFFER_LEN-len));
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,len));
			return true;
		})
	};

	c[4]=(struct Command){
		Stringcp_from_cstr("imdb"),
		Stringcp_from_cstr("Creates a IMDb search link"),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			int len = Stringp_vcopy(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),1,
				Stringp_from_cstr("http://www.imdb.com/find?s=all&q=")
			);
			len+=url_encode(STRINGCP(arg->free.begin,arg->free.end-arg->free.begin),STRINGP(write_buffer+len,IRC_WRITE_BUFFER_LEN-len));
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,len));
			return true;
		})
	};

	c[5]=(struct Command){
		Stringcp_from_cstr("date"),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			time_t t;time(&t);
			struct tm* time_data = localtime(&t);

			int len = strftime(write_buffer,IRC_WRITE_BUFFER_LEN,"%F %X %Z, %A v.%V, Day %j of the year",time_data);
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,len));
			return true;
		})
	};

	//Upper
	c[6]=(struct Command){
		Stringcp_from_cstr("upper"),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			char* write_ptr = write_buffer;
			size_t argLen=arg->free.end-arg->free.begin;

			while(arg->free.begin<arg->free.end){
				*write_ptr++=*arg->free.begin>='a' && *arg->free.begin<='z'?(*arg->free.begin)+('A'-'a'):*arg->free.begin;
				++arg->free.begin;
			}

			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,argLen));
			return true;
		})
	};
	//Lower
	c[7]=(struct Command){
		Stringcp_from_cstr("lower"),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			char* write_ptr = write_buffer;
			size_t argLen=arg->free.end-arg->free.begin;

			while(arg->free.begin<arg->free.end){
				*write_ptr++=*arg->free.begin>='A' && *arg->free.begin<='Z'?(*arg->free.begin)+('a'-'A'):*arg->free.begin;
				++arg->free.begin;
			}

			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,argLen));
			return true;
		})
	};
	//ROT13
	c[8]=(struct Command){
		Stringcp_from_cstr("rot13"),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			char* write_ptr = write_buffer;
			size_t argLen=arg->free.end-arg->free.begin;

			while(arg->free.begin<arg->free.end){
				if(*arg->free.begin>='A' && *arg->free.begin<='Z')
					*write_ptr++=((*arg->free.begin)-'A'+13)%26+'A';
				else if(*arg->free.begin>='a' && *arg->free.begin<='z')
					*write_ptr++=((*arg->free.begin)-'a'+13)%26+'a';
				else if(*arg->free.begin>='0' && *arg->free.begin<='9')
					*write_ptr++=((*arg->free.begin)-'0'+5)%10+'0';
				else
					*write_ptr++=*arg->free.begin;
				++arg->free.begin;
			}

			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,argLen));
			return true;
		})
	};
	//ROT47
	c[9]=(struct Command){
		Stringcp_from_cstr("rot47"),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			char* write_ptr = write_buffer;
			size_t argLen=arg->free.end-arg->free.begin;

			while(arg->free.begin<arg->free.end){
				if(*arg->free.begin>='!' && *arg->free.begin<='~')
					*write_ptr++=((*arg->free.begin)-'!'+47)%94+'!';
				else
					*write_ptr++=*arg->free.begin;
				++arg->free.begin;
			}

			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,argLen));
			return true;
		})
	};

	//Random
	c[10]=(struct Command){
		Stringcp_from_cstr("random"),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			unsigned int value=rand();
			unsigned int min=1;
			unsigned int max=RAND_MAX;
			const char* read_ptr=arg->free.begin;

			if(arg->free.end-arg->free.begin>0)//If argument 1 exists (random <max>)
				while(true)
					if(read_ptr>arg->free.begin && (read_ptr>=arg->free.end || read_ptr[0]==' ')){
						max=decStrToInt(arg->free.begin,MIN(read_ptr-arg->free.begin,9));
						arg->free.begin=++read_ptr;

						if(arg->free.end-arg->free.begin>0)//If argument 2 exists (random <min> <max>)
							while(true)
								if(read_ptr>arg->free.begin && (read_ptr>=arg->free.end || read_ptr[0]==' ')){
									min=max;
									max=decStrToInt(arg->free.begin,MIN(read_ptr-arg->free.begin,9));
									arg->free.begin=++read_ptr;
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
		Stringcp_from_cstr("choose"),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			if(arg->free.begin>=arg->free.end){
				IRCBot_sendMessage(bot,target,locale[language].missing_argument);
				return false;
			}

			LinkedList* list = LinkedList_init;
			size_t list_length = 0;

			string_splitted(STRINGP(arg->free.begin,arg->free.end-arg->free.begin),function(size_t,(Stringp str){
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

				Stringcp* arg = smalloc(sizeof(Stringcp));
				*arg = STRINGCP(begin,end-begin);
				LinkedList_push(&list,arg);

				++list_length;

				return true;
			}));

			if(list_length>0)
				IRCBot_sendMessage(bot,target,*(Stringcp*)LinkedList_get(list,rand()%list_length));

			LinkedList_clean(&list,function(bool,(void* elem){
				free(elem);
				return true;
			}));

			return true;
		})
	};
	//Length
	c[12]=(struct Command){
		Stringcp_from_cstr("length"),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			int len=snprintf(write_buffer,IRC_WRITE_BUFFER_LEN,"%li",arg->free.end-arg->free.begin);
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,len));
			return true;
		})
	};
	//Google
	c[13]=(struct Command){
		Stringcp_from_cstr("google"),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			int len = Stringp_vcopy(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),1,
				Stringp_from_cstr("https://www.google.com/search?q=")
			);
			len+=url_encode(STRINGCP(arg->free.begin,arg->free.end-arg->free.begin),STRINGP(write_buffer+len,IRC_WRITE_BUFFER_LEN-len));
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,len));
			return true;
		})
	};
	//Prefix
	c[14]=(struct Command){
		Stringcp_from_cstr("prefix"),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			int len = arg->free.end-arg->free.begin;
			int write_len;

			if(len>0){
				free(bot->commandPrefix.ptr);
				bot->commandPrefix = STRINGP(smalloc(len),len);
				memcpy(bot->commandPrefix.ptr,arg->free.begin,len);
				write_len = Stringp_vcopy(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),4,
					locale[language].prefix.set,
					STRINGCP(" \"",2),
					STRINGCP(arg->free.begin,len),
					STRINGCP("\"",1)
				);
			}else{
				write_len = Stringp_vcopy(STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN),4,
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
		Stringcp_from_cstr("reverse"),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			char* write_ptr = write_buffer;
			const char* read_ptr = arg->free.end;

			while(read_ptr>arg->free.begin)
				*write_ptr++=*--read_ptr;
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,arg->free.end-arg->free.begin));
			return true;
		})
	};
	//Language
	c[16]=(struct Command){
		Stringcp_from_cstr("language"),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			for(unsigned int i=0;i<LANG_COUNT;++i)
				if(memcmp(locale[i].lang_name.ptr,arg->free.begin,locale[i].lang_name.length)==0){
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
		Stringcp_from_cstr("wordcount"),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			unsigned int count=0;

			if(arg->free.begin<arg->free.end){
				count=1;
				while(arg->free.begin<arg->free.end)
					if(*arg->free.begin++==' ')//TODO: The ability to count complex sentences
						++count;
			}

			int len=snprintf(write_buffer,IRC_WRITE_BUFFER_LEN,"%u words",count);
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,len));
			return true;
		})
	};
	//URL encode
	c[18]=(struct Command){
		Stringcp_from_cstr("urlencode"),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			IRCBot_sendMessage(bot,target,STRINGCP(write_buffer,url_encode(STRINGCP(arg->free.begin,arg->free.end-arg->free.begin),STRINGP(write_buffer,IRC_WRITE_BUFFER_LEN))));
			return true;
		})
	};
	//Magic 8-ball
	c[19]=(struct Command){//TODO: magic8ball isn't in the help listing
		Stringcp_from_cstr("magic8ball"),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
			unsigned short len=0;
			if(arg->free.end-arg->free.begin<2 || *(arg->free.end-1)!='?'){
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
		Stringcp_from_cstr(""),
		Stringcp_from_cstr(""),
		function(bool,(struct IRCBot* bot,Stringcp target,union CommandArgument* arg){
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
