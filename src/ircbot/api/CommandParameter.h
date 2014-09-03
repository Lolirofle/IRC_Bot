#ifndef __LOLIROFLE_IRCBOT_API_COMMANDPARAMETER_H_INCLUDED__
#define __LOLIROFLE_IRCBOT_API_COMMANDPARAMETER_H_INCLUDED__

#include <lolien/seq/StringP.h>
#include <lolien/types.h>

/**
 * Represents the type of requirement this parameter has
 */
enum CommandParameterRequirement{
	/**
	 * The parameter is required
	 */
	COMMAND_PARAMETER_REQUIRED,

	/**
	 * The parameter is optional. This option must be after all the required ones.
	 */
	COMMAND_PARAMETER_OPTIONAL,

	/**
	 * The parameter is a variable length argument.
	 * It means that it is separated and can handle the rest of the arguments.
	 * This option can be anywhere, if it makes contact with an invalid value, it will jump to the next argument.
	 */
	COMMAND_PARAMETER_VARARG
};

struct CommandParameterType{
	StringP name;
	StringP description;
	enum CommandParameterParserReturn{
		COMMANDPARAMETERPARSER_SUCCESS,
		COMMANDPARAMETERPARSER_PARSEERROR,
	}(*parser)(char* start,char** out_end);
};

struct CommandParameter{
	StringP name;
	StringP description;
	enum CommandParameterRequirement requirement;

	const struct CommandParameterType* type;
	byte typeData[];
};

typedef const struct CommandParameterType*(*CommandParameterTypeParser)(const struct CommandParameterType* type,void* typeData,const char** parserPos);

extern const struct CommandParameterType CommandParameterType_int;
extern const struct CommandParameterType CommandParameterType_float;
extern const struct CommandParameterType CommandParameterType_bool;
extern const struct CommandParameterType CommandParameterType_values;
extern const struct CommandParameterType CommandParameterType_free;

#endif
