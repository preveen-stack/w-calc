#include <string.h>

static char *funcs[] = { "sin", "cos", "tan", "cot",
	"asin", "acos", "atan", "acot",
	"arcsin", "arccos", "arctan", "arccot",
	"sinh", "cosh", "tanh", "coth",
	"asinh", "acosh", "atanh", "acoth",
	"areasinh", "areacosh", "areatanh", "areacoth",
	"log", "logtwo", "ln",
	"round", "abs", "floor", "ceil", "ceiling",
	"sqrt", "cbrt",
	"exp", "fact", "comp",
	"rand", "irand",
	0
};

int isfunc(char *str)
{
	size_t i;

	for (i = 0; funcs[i]; i++) {
		if (!strcmp(funcs[i], str)) {
			return 1;
		}
	}
	return 0;
}
