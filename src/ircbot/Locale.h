#ifndef __LOLIROFLE_IRCBOT_LOCALE_H_INCLUDED__
#define __LOLIROFLE_IRCBOT_LOCALE_H_INCLUDED__

#include <lolie/Stringp.h>

extern enum Languages language;

struct Locale{
	Stringcp lang_name;

	Stringcp boolean[2];

	struct{
		Stringcp set;
		Stringcp unknown;
	}language;

	struct{
		Stringcp ask;
		Stringcp yes[10];
		Stringcp maybe[5];
		Stringcp no[5];
		Stringcp failure;
	}magic8ball;

	struct{
		Stringcp set;
		Stringcp get;
	}prefix;

	Stringcp missing_argument;
	Stringcp unknown_command;
	Stringcp command_error;
};

enum Languages{
	LANG_SWEDISH,
	LANG_ENGLISH,
	
	LANG_COUNT
};

extern const struct Locale locale[2];

#endif
