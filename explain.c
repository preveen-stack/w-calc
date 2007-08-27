#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>
#include <ctype.h> /* for isspace() */
#include "number.h"
#include "uint32_max.h"
#include "variables.h"
#include "calculator.h"
#include "list.h"
#include "extract_vars.h"
#include "evalvar.h"
#include "isconst.h"
#include "isfunc.h"
#include "explain.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif

static void explain_command(const char *);
static void explain_variable(const char *);
static void explain_constant(const char *);
static void explain_function(const char *);

void explain(const char *str)
{				       /*{{{ */
    size_t curs;
    char * mystr;

    if (str == NULL || str[0] == 0) {
	printf("Nothing to explain.\n");
	return;
    }
    mystr = strdup(str);
    /* for sanity's sake, remove any trailing whitespace */
    curs = strlen(mystr) - 1;
    while (isspace(mystr[curs]) && curs > 0) {
	mystr[curs] = 0;
	curs--;
    }
    if (!strcmp(mystr, "q")) {
	printf("Quits the program.");
    } else if (*mystr == '\\') {	       // it's a command
	explain_command(mystr);
    } else if (isconst(mystr)) {	       // it's a constant
	explain_constant(mystr);
    } else if (isfunc(mystr)) {	       // it's a function
	explain_function(mystr);
    } else if (varexists(mystr)) {       // it's a variable
	explain_variable(mystr);
    } else {			       // it's a call for help
	printf("%s is neither a command, constant, function, or variable.\n",
	       mystr);
    }
    free(mystr);
}				       /*}}} */

static void explain_command(const char *str)
{				       /*{{{ */
    str++;
    if (!strcmp(str, "b") || !strcmp(str, "bin") || !strcmp(str, "binary")) {
	printf("This displays the output in binary.\n");
    } else if (!strcmp(str, "store")) {
	printf
	    ("Saves the variable specified in the preload file, ~/.wcalc_preload. Use like so: \\store variablename\n");
    } else if (!strcmp(str, "q")) {
	printf("Exits the program.\n");
    } else if (!strcmp(str, "d") || !strcmp(str, "dec") ||
	       !strcmp(str, "decimal")) {
	printf("This displays the output in decimal.\n");
    } else if (!strcmp(str, "dsep")) {
	printf
	    ("Used like so: \\dsepX This sets the decimal separator to X. By default, it is a period (.).\n");
    } else if (!strcmp(str, "e") || !strcmp(str, "eng") ||
	       !strcmp(str, "engineering")) {
	printf
	    ("This toggles the formatting of output between decimal and scientific notation. This setting only applies when precision is specified.\n");
    } else if (!strcmp(str, "cons") || !strcmp(str, "conservative")) {
	printf("Toggles precision guards.\n");
    } else if (!strcmp(str, "h") || !strcmp(str, "hex") ||
	       !strcmp(str, "hexadecimal") || !strcmp(str, "x")) {
	printf("This displays the output in hexadecimal.\n");
    } else if (!strcmp(str, "help")) {
	printf("This displays a generic help message.\n");
    } else if (!strcmp(str, "hlimit")) {
	printf
	    ("This places (or removes) a limit on the number of things stored in the history. Use like this: \\hlimitX where X is a number. 0 removes the limit.\n");
    } else if (!strcmp(str, "ints")) {
	printf
	    ("Toggles whether long integers will be abbreviated or not (conflicts with engineering notation for large numbers, but not for decimals).\n");
    } else if (!strcmp(str, "li") || !strcmp(str, "list") ||
	       !strcmp(str, "listvars")) {
	printf("Prints out the currently defined variables.\n");
    } else if (!strcmp(str, "o") || !strcmp(str, "oct") ||
	       !strcmp(str, "octal")) {
	printf("This displays the output in octal.\n");
    } else if (!strcmp(str, "open")) {
	printf
	    ("Loads a saved file. Used like this: \\openXXXXX where XXXXX is the name of the file to load.\n");
    } else if (!strcmp(str, "p")) {
	printf
	    ("Sets the precision. Use it like so: \\pX where X is the desired precision. Precision here is in digits. This setting only affects display. -1 means \"auto\".\n");
    } else if (!strcmp(str, "pre") || !strcmp(str, "prefix") ||
	       !strcmp(str, "prefixes")) {
	printf("Toggles whether prefixes are shown with the output.\n");
    } else if (!strcmp(str, "pref") || !strcmp(str, "prefs") ||
	       !strcmp(str, "preferences")) {
	printf("Prints out the current preference settings.\n");
    } else if (!strcmp(str, "r") || !strcmp(str, "rad") ||
	       !strcmp(str, "radians")) {
	printf("Toggles radian mode for trigonometric functions.\n");
    } else if (!strcmp(str, "rou") || !strcmp(str, "round") ||
	       !strcmp(str, "rounding")) {
	printf
	    ("Sets the rounding indication. The possible arguments to this preference are \"none\", \"simple\", and \"sig_fig\". Use like this: \\round none\n");
    } else if (!strcmp(str, "re") || !strcmp(str, "remember") ||
	       !strcmp(str, "remember_errors")) {
	printf("Toggles whether errors are remembered in history.\n");
    } else if (!strcmp(str, "bits")) {
	printf
	    ("Sets the number of bits used internally to represent numbers. Used like this: \\bitsX where X is a number that must be above %li and below %li.\n",
	     (long int)NUM_PREC_MIN, (long int)NUM_PREC_MAX);
    } else if (!strcmp(str, "save")) {
	printf
	    ("Saves the history and variable list to a file. Used like this: \\saveXXXXX where XXXXX is the name of the file to save.\n");
    } else if (!strcmp(str, "tsep")) {
	printf
	    ("Used like this: \\tsepX Sets the thousands-place separator character to X. The default is a comma (,).\n");
    } else if (!strcmp(str, "c") || !strcmp(str, "conv") ||
	       !strcmp(str, "convert")) {
	printf
	    ("Used either like this: \"\\convert unit1 to unit2\" or like this: \"\\convert unit1 unit2\". Converts the previous answer from the first unit to the second.\n");
    } else if (!strcmp(str, "base")) {
	printf("Prints out the previous answer in any base from 2 to 36.\n");
    } else if (!strcmp(str, "verbose")) {
	printf
	    ("Prints out the lines to be evaluated before evaluating them.\n");
    } else if (!strcmp(str, "explain")) {
	printf
	    ("Gives you information about commands, variables, constants and functions.\n");
    } else if (!strcmp(str, "cmod")) {
	printf
	    ("Changes how the modulus operator (%%) behaves with negative numbers.\n The default is to behave like the C programming language modulus, the other is slightly more flexible. For example, with the default setting:\n\n\t-340 %% 60 == -40; 340 %% -60 == 40; -340 %% -60 == -40\n\nWhen this setting is toggled, it behaves like this:\n\n\t-340 %% 60 == -40; 340 %% -60 == -20; -340 %% -60 == 20\n");
    } else {
	printf("Undefined command.\n");
    }
}				       /*}}} */

static void explain_variable(const char *str)
{				       /*{{{ */
    struct answer var;

    var = getvar_full(str);
    if (var.err) {
	printf("This should never happen.\n");
	return;
    }
    if (var.exp) {		       // it's an expression (i.e. a function)
	List strings;

	printf("%s is the expression: %s\n", str, var.exp);
	printf("%s uses the following variables:\n", str);
	strings = extract_vars(var.exp);
	while (listLen(strings) > 0) {
	    char *curstr = (char *)getHeadOfList(strings);
	    char *value = evalvar(curstr);

	    printf("\t%s\t(currently: %s)\n", curstr,
		   value ? value : "undefined");
	    if (curstr) {
		free(curstr);
	    }
	    if (value) {
		free(value);
	    }
	}
	freeList(&strings);
    } else {
	char std_save;

	std_save = standard_output;
	standard_output = 0;
	printf("%s is a variable with the value: %s\n", str,
	       print_this_result(var.val));
	standard_output = std_save;
    }
    if (var.desc != NULL) {
	printf("Description: %s\n", var.desc);
    }
}				       /*}}} */

static void explain_constant(const char *str)
{				       /*{{{ */
    if (!strcmp(str, "e")) {
	printf
	    ("The base of the natural logarithm function. Arbitrary precision. Occasionally called Euler's Number (after Leonhard Eulter) or Napier's Constant (after John Napier).");
    } else if (!strcmp(str, "pi") || !strcmp(str, "PI") || !strcmp(str, "pI")
	       || !strcmp(str, "Pi")) {
	printf
	    ("The ratio of a circle's circumference to its diameter. Arbitrary precision.");
    } else if (!strcmp(str, "random")) {
	printf("A random number between 0 and %u.", UINT32_MAX);
    } else if (!strcmp(str, "irandom")) {
	printf("A random integer between 0 and %u.", UINT32_MAX);
    } else if (!strcmp(str, "Na") || !strcmp(str, "NA")) {
	printf("Avogadro's Constant. The number of molecules in one mole.");
    } else if (!strcmp(str, "k")) {
	printf
	    ("Boltzmann's Constant. The physical constant relating temperature to energy.");
    } else if (!strcmp(str, "Cc")) {
	printf("Coulomb force constant. Equal to 1/(4*pi*epsilonzero).");
    } else if (!strcmp(str, "ec")) {
	printf
	    ("Elementary charge. The electric charge carried by a single proton.");
    } else if (!strcmp(str, "R")) {
	printf
	    ("Molar Gas Constant. Used in equations of state to relate various groups of state functions to one another.");
    } else if (!strcmp(str, "G")) {
	printf
	    ("The universal gravitational constant. Appears in Einstein's theory of general relativity.");
    } else if (!strcmp(str, "g")) {
	printf("The gravitational accelleration of Earth at sea-level.");
    } else if (!strcmp(str, "Me")) {
	printf("The mass of an electron in kilograms.");
    } else if (!strcmp(str, "Mp")) {
	printf("The mass of a proton in kilograms.");
    } else if (!strcmp(str, "Mn")) {
	printf("The mass of a neutron in kilograms.");
    } else if (!strcmp(str, "Md")) {
	printf("The mass of a deuteron in kilograms.");
    } else if (!strcmp(str, "u") || !strcmp(str, "amu")) {
	printf
	    ("Atomic mass unit. One twelfth of the mean mass of an atop of carbon 12 in its nuclear and electronic ground state.");
    } else if (!strcmp(str, "c")) {
	printf("The speed of light.");
    } else if (!strcmp(str, "h")) {
	printf
	    ("Planck Constant. Used to describe the sizes of quanta, central to the theory of quantum mechanics.");
    } else if (!strcmp(str, "mu0") || !strcmp(str, "muzero") ||
	       !strcmp(str, "muZERO")) {
	printf
	    ("Permeability of free space. Also known as the magnetic constant.");
    } else if (!strcmp(str, "epsilonzero") || !strcmp(str, "epsilonZERO") ||
	       !strcmp(str, "epsilon0") || !strcmp(str, "EPSILONzero") ||
	       !strcmp(str, "EPSILONZERO") || !strcmp(str, "EPSILON0")) {
	printf("Permittivity of free space. The ratio of D/E in vacuum.");
    } else if (!strcmp(str, "muB")) {
	printf("Bohr magneton. A physical constant of magnetic moment.");
    } else if (!strcmp(str, "b")) {
	printf
	    ("Wein displacement constant. Defines the relationship between the thermodynamic temperature of a black body and the wavelength at which the intensity of the radiation is the greatest.");
    } else if (!strcmp(str, "a0")) {
	printf
	    ("Bohr radius. A single electron orbiting at the lowest possible energy in a hydrogen atom orbits at this distance.");
    } else if (!strcmp(str, "F")) {
	printf
	    ("Faraday constant. The amount of electric charge in one mole of electrons.");
    } else if (!strcmp(str, "Vm") || !strcmp(str, "NAk")) {
	printf("Molar volume of ideal gas.");
    } else if (!strcmp(str, "eV")) {
	printf
	    ("Electron volt. The amount of kinetic energy gained by a single unbound electron when it passes through an electrostatic potential difference of one volt, in vacuum.");
    } else if (!strcmp(str, "sigma")) {
	printf
	    ("Stefan-Boltzmann constant. The constant of proportionality between the total energy radiated per unit surface area of a black body in unit time and the fourth power of the thermodynamic temperature, as per the Stefan-Boltzmann law.");
    } else if (!strcmp(str, "alpha")) {
	printf
	    ("Fine-structure constant. The fundamental physical constant characterizing the strength of the electromagnetic interaction.");
    } else if (!strcmp(str, "gamma")) {
	printf
	    ("Euler-Mascheroni constant. Used mainly in number theory, defined as the limiting difference between the harmonic series and the natural logarithm.");
    } else if (!strcmp(str, "re")) {
	printf
	    ("Electron radius, also known as the Compton radius or the Thomson scattering length based on a classical relativistic model of the electron.");
    } else if (!strcmp(str, "Kj")) {
	printf
	    ("Josephson Constant. The inverse of the magnetic flux quantum.");
    } else if (!strcmp(str, "Rk")) {
	printf
	    ("von Klitzing constant. Named after Klaus von Klitzing, the basic resistance unit.");
    } else if (!strcmp(str, "Rinf")) {
	printf
	    ("The \"infinity\" Rydberg constant, named after physicist Janne Rydberg. Used to calculate the Rydberg constant of any other chemical element.");
    } else if (!strcmp(str, "Eh")) {
	printf
	    ("Hartree energy. The atomic unit of energy, named after physicist Douglas Hartree.");
    } else if (!strcmp(str, "Gf")) {
	printf("Fermi Coupling Constant.");
    } else if (!strcmp(str, "Mmu")) {
	printf("The mass of a muon in kilograms.");
    } else if (!strcmp(str, "Mt") || !strcmp(str, "Mtau")) {
	printf("The tau mass in kilograms.");
    } else if (!strcmp(str, "Mh")) {
	printf("The mass of a helion in kilograms.");
    } else if (!strcmp(str, "Malpha")) {
	printf("The mass of an alpha particle in kilograms.");
    } else if (!strcmp(str, "n0") || !strcmp(str, "nzero")) {
	printf
	    ("Loschmidt constant. The number of molecules in a cubic meter of ideal gas. Equal to the Avogadro constant divided by molar volume.");
    } else if (!strcmp(str, "c1")) {
	printf("First radiation constant.");
    } else if (!strcmp(str, "c2")) {
	printf("Second radiation constant.");
    } else if (!strcmp(str, "G0") || !strcmp(str, "Gzero")) {
	printf("Conductance quantum.");
    } else if (!strcmp(str, "Z0") || !strcmp(str, "Zzero")) {
	printf
	    ("Characteristic impedance of vacuum, the characteristic impedance of electromagnetic radiation in vacuum. Equal to mu0*c.");
    } else if (!strcmp(str, "Phi0") || !strcmp(str, "Phizero") ||
	       !strcmp(str, "PhiZERO")) {
	printf
	    ("Magnetic flux quantum. The quantum of magnetic flux passing through a superconductor.");
    } else if (!strcmp(str, "K")) {
	printf
	    ("Catalan's constant commonly appears in estimates of combinatorial functions and in certain classes of sums and definite integrals.");
    } else if (!strcmp(str, "sinc")) {
	printf
	    ("Sinus cardinalis, also known as the interpolation function, filtering function, or the first spherical Bessel function, is the product of a sine function and a monotonically decreasing function.");
    }
    printf("\n");
}				       /*}}} */

static void explain_function(const char *str)
{				       /*{{{ */
    if (!strcmp(str, "sin") || !strcmp(str, "cos") || !strcmp(str, "tan") ||
	!strcmp(str, "cot")) {
	printf("A trigonometric function.");
    } else if (!strcmp(str, "asin") || !strcmp(str, "acos") ||
	       !strcmp(str, "atan") || !strcmp(str, "acot") ||
	       !strcmp(str, "arcsin") || !strcmp(str, "arccos") ||
	       !strcmp(str, "arctan") || !strcmp(str, "arccot")) {
	printf("The inverse of the standard trigonometric function.");
    } else if (!strcmp(str, "sinh") || !strcmp(str, "cosh") ||
	       !strcmp(str, "tanh") || !strcmp(str, "coth")) {
	printf("A standard hyperbolic trigonometric function.");
    } else if (!strcmp(str, "asinh") || !strcmp(str, "acosh") ||
	       !strcmp(str, "atanh") || !strcmp(str, "acoth")
	       || !strcmp(str, "areasinh") || !strcmp(str, "areacosh") ||
	       !strcmp(str, "areatanh") || !strcmp(str, "areacoth")) {
	printf
	    ("The inverse of the standard hyperbolic trigonometric function.");
    } else if (!strcmp(str, "log")) {
	printf("A logarithm (base 10).");
    } else if (!strcmp(str, "logtwo")) {
	printf("A logarithm (base 2).");
    } else if (!strcmp(str, "ln")) {
	printf("A logarithm (base e). Also known as the \"natural\" log.");
    } else if (!strcmp(str, "round")) {
	printf("Returns the closest integer to the input number.");
    } else if (!strcmp(str, "abs")) {
	printf("The absolute value (the distance of the number from zero).");
    } else if (!strcmp(str, "sqrt")) {
	printf("The square root function.");
    } else if (!strcmp(str, "floor")) {
	printf
	    ("Returns the biggest integer that is not bigger than the input number.");
    } else if (!strcmp(str, "ceil") || !strcmp(str, "ceiling")) {
	printf
	    ("Returns the smallest integer that is not smaller than the input number.");
    } else if (!strcmp(str, "cbrt")) {
	printf("The cube root function.");
    } else if (!strcmp(str, "rand")) {
	printf("Returns a random number between 0 and the input number.");
    } else if (!strcmp(str, "irand")) {
	printf("Returns a random integer between 0 and the input number.");
    } else if (!strcmp(str, "fact")) {
	printf("Returns the factorial of the input number.");
    } else if (!strcmp(str, "comp")) {
	printf("Returns the one's complement of the input number.");
    } else if (!strcmp(str, "zeta")) {
	printf
	    ("Returns the Riemann zeta function of the input number. This is used primarily in number theory, but also has applications in physics, probability theory, and applied statistics.");
    } else if (!strcmp(str, "Gamma")) {
	printf
	    ("Returns the Gamma function of the input number. The Gamma function extends the factorial function to complex and non-natural numbers.");
    } else if (!strcmp(str, "lnGamma")) {
	printf
	    ("Returns the natural log of the Gamma function of the input number.");
    } else if (!strcmp(str, "eint")) {
	printf("The exponential integral function.");
    } else if (!strcmp(str, "sinc")) {
	printf("Returns the sampling function, which is 1 if the input is 0, or sin(x)/x otherwise.");
    } else if (!strcmp(str, "exp")) {
	printf("Returns the value of e to the given power. Equivalent to: e^");
    } else {
	printf
	    ("Hrm... this function doesn't have any documentation! Pester the developer!");
    }
    printf("\n");
}				       /*}}} */
