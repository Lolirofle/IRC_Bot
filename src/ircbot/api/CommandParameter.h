#ifndef __LOLIROFLE_IRCBOT_API_COMMANDPARAMETER_H_INCLUDED__
#define __LOLIROFLE_IRCBOT_API_COMMANDPARAMETER_H_INCLUDED__

#include <lolie/Stringp.h>
#include <lolie/TypeAliases.h>

/**
 * Represents the type of requirement this parameter has
 */
enum CommandParameterRequirement{
	//The parameter is required
	COMMAND_PARAMETER_REQUIRED,

	//The parameter is optional. This option must be after all the required ones.
	COMMAND_PARAMETER_OPTIONAL,

	//The parameter is a variable length argument. It means that it is separated and can handle the rest of the arguments. This option must be the last.
	COMMAND_PARAMETER_VARARG
};

struct CommandParameterType{
	Stringp name;
	Stringp description;
	enum CommandParameterParserReturn{
		COMMANDPARAMETERPARSER_SUCCESS,
		COMMANDPARAMETERPARSER_PARSEERROR,
	}(*parser)(char* start,char** out_end);
};

struct CommandParameter{
	Stringp name;
	Stringp description;
	enum CommandParameterRequirement requirement;

	struct CommandParameterType type;
	byte typeData[];
};

const struct CommandParameterType* CommandParameterType_int();
const struct CommandParameterType* CommandParameterType_float();
const struct CommandParameterType* CommandParameterType_bool();
const struct CommandParameterType* CommandParameterType_values();
const struct CommandParameterType* CommandParameterType_free();//TODO: Separation? How to implement?

#endif
