#include "api/CommandParameter.h"

const struct CommandParameterType CommandParameterType_int = {
	.name        = {"int",3},
	.description = {"Integer. Whole number including natural numbers and negative.",61},
	.parser      = NULL
};

const struct CommandParameterType CommandParameterType_float = {
	.name        = {"real",4},
	.description = {"Real number.",12},
	.parser      = NULL
};

const struct CommandParameterType CommandParameterType_bool = {
	.name        = {"bool",4},
	.description = {"Boolean. True/false, 1/0, yes/no.",33},
	.parser      = NULL
};

const struct CommandParameterType CommandParameterType_values = {
	.name        = {"values",6},
	.description = {"Choose between a list of values",31},
	.parser      = NULL
};

const struct CommandParameterType CommandParameterType_free = {
	.name        = {"string",6},
	.description = {"Non-terminated string.",22},
	.parser      = NULL
};
