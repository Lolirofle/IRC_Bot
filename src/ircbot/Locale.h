#ifndef __LOLIROFLE_IRCBOT_LOCALE_H_INCLUDED__
#define __LOLIROFLE_IRCBOT_LOCALE_H_INCLUDED__

#include <lolien/seq/StringP.h>

extern enum Languages language;

struct Locale{
	StringCP lang_name;

	StringCP boolean[2];

	struct{
		StringCP set;
		StringCP unknown;
	}language;

	struct{
		StringCP ask;
		StringCP yes[10];
		StringCP maybe[5];
		StringCP no[5];
		StringCP failure;
	}magic8ball;

	struct{
		StringCP set;
		StringCP get;
	}prefix;

	StringCP missing_argument;
	StringCP unknown_command;
	StringCP command_error;
};

enum Languages{
	LANG_SWEDISH,
	LANG_ENGLISH,
	
	LANG_COUNT
};

extern const struct Locale locale[2];

#endif
