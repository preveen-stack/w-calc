/*
 * stdin
 * Copyright (c) 2013 Micron Technology, Inc.
 *
 */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* System Headers */
#include <string.h>
#include <ctype.h>                     /* for isspace() */

/* Internal Headers */
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
#include "output.h"

static int  explain_command(const char *,
                            int);
static void explain_variable(const char *);
static void explain_constant(const char *);
static void explain_function(const char *);

void explain(const char *str)
{                                      /*{{{ */
    size_t curs;
    char  *mystr;

    if ((str == NULL) || (str[0] == 0)) {
        str = "\\explain";
    }
    mystr = strdup(str);
    /* for sanity's sake, remove any trailing whitespace */
    curs  = strlen(mystr) - 1;
    while (isspace(mystr[curs]) && curs > 0) {
        mystr[curs] = 0;
        curs--;
    }
    if (!strcmp(mystr, "q")) {
        display_explanation("Quits the program.");
    } else if (!strcmp(mystr, "constants")) {
        display_explanation("Wcalc understands many standard physical constants.\n"
                            "Here is the list of the ones currently understood;\n"
                            "each can be explained in detail individually (some\n"
                            "mean the same thing).\n");
        printconsts();
    } else if (*mystr == '\\') {       // it's a command
        explain_command(mystr, 0);
    } else if (isconst(mystr) != W_notaconstant) {      // it's a constant
        explain_constant(mystr);
    } else if (isfunc(mystr)) {        // it's a function
        explain_function(mystr);
    } else if (varexists(mystr)) {     // it's a variable
        explain_variable(mystr);
    } else {                           // it's a call for help
        int   len         = strlen(mystr) + 2;
        char *add_a_slash = calloc(len, sizeof(char));

        snprintf(add_a_slash, len, "\\%s", mystr);
        if (explain_command(add_a_slash, 1) == -1) {
            report_error("%s is neither a command, constant, function, or variable.\n", mystr);
        }
        free(add_a_slash);
    }
    free(mystr);
}                                      /*}}} */

static int explain_command(const char *str,
                           int         test)
{                                      /*{{{ */
    str++;
    if (!strcmp(str, "b") || !strcmp(str, "bin") || !strcmp(str, "binary")) {
        display_explanation("This displays the output in binary.");
    } else if (!strcmp(str, "store")) {
        display_explanation("Saves the variable specified in the preload file, ~/.wcalc_preload. Use like so: \\store variablename");
    } else if (!strcmp(str, "q")) {
        display_explanation("Exits the program.");
    } else if (!strcmp(str, "d") || !strcmp(str, "dec") ||
               !strcmp(str, "decimal")) {
        display_explanation("This displays the output in decimal.");
    } else if (!strcmp(str, "delim")) {
        display_explanation("This toggles the formatting of output such that delimiters are inserted in integers.");
    } else if (!strcmp(str, "dsep")) {
        display_explanation("Used like so: \\dsepX This sets the decimal separator to X. By default, it is a period (.).");
    } else if (!strcmp(str, "e") || !strcmp(str, "eng") ||
               !strcmp(str, "engineering")) {
        display_explanation("This toggles the formatting of output between decimal and scientific notation.");
    } else if (!strcmp(str, "cons") || !strcmp(str, "conservative")) {
        display_explanation("Toggles precision guards.");
    } else if (!strcmp(str, "h") || !strcmp(str, "hex") ||
               !strcmp(str, "hexadecimal") || !strcmp(str, "x")) {
        display_explanation("This displays the output in hexadecimal.");
    } else if (!strcmp(str, "help")) {
        display_explanation("This displays a generic help message.");
    } else if (!strcmp(str, "hlimit")) {
        display_explanation("This places (or removes) a limit on the number of things stored in the history. Use like this: \\hlimitX where X is a number. 0 removes the limit.");
    } else if (!strcmp(str, "ints")) {
        display_explanation("Toggles whether long integers will be abbreviated or not (conflicts with engineering notation for large numbers, but not for decimals).");
    } else if (!strcmp(str, "li") || !strcmp(str, "list") ||
               !strcmp(str, "listvars")) {
        display_explanation("Prints out the currently defined variables.");
    } else if (!strcmp(str, "o") || !strcmp(str, "oct") ||
               !strcmp(str, "octal")) {
        display_explanation("This displays the output in octal.");
    } else if (!strcmp(str, "open")) {
        display_explanation("Loads a saved file. Used like this: \\openXXXXX where XXXXX is the name of the file to load.");
    } else if (!strcmp(str, "p")) {
        display_explanation("Sets the precision. Use it like so: \\pX where X is the desired precision. Precision here is in digits. This setting only affects display. -1 means \"auto\".");
    } else if (!strcmp(str, "pre") || !strcmp(str, "prefix") ||
               !strcmp(str, "prefixes")) {
        display_explanation("Toggles whether prefixes are shown with the output.");
    } else if (!strcmp(str, "pref") || !strcmp(str, "prefs") ||
               !strcmp(str, "preferences")) {
        display_explanation("Prints out the current preference settings.");
    } else if (!strcmp(str, "r") || !strcmp(str, "rad") ||
               !strcmp(str, "radians")) {
        display_explanation("Toggles radian mode for trigonometric functions.");
    } else if (!strcmp(str, "rou") || !strcmp(str, "round") ||
               !strcmp(str, "rounding")) {
        display_explanation("Sets the rounding indication. The possible arguments to this preference are \"none\", \"simple\", and \"sig_fig\". Use like this: \\round none");
    } else if (!strcmp(str, "re") || !strcmp(str, "remember") ||
               !strcmp(str, "remember_errors")) {
        display_explanation("Toggles whether errors are remembered in history.");
    } else if (!strcmp(str, "bits")) {
        display_explanation("Sets the number of bits used internally to represent numbers. Used like this: \\bitsX where X is a number that must be above %li and below %li.",
                            (long int)NUM_PREC_MIN, (long int)NUM_PREC_MAX);
    } else if (!strcmp(str, "save")) {
        display_explanation("Saves the history and variable list to a file. Used like this: \\saveXXXXX where XXXXX is the name of the file to save.");
    } else if (!strcmp(str, "tsep")) {
        display_explanation("Used like this: \\tsepX Sets the thousands-place separator character to X. The default is a comma (,).");
    } else if (!strcmp(str, "c") || !strcmp(str, "conv") ||
               !strcmp(str, "convert")) {
        display_explanation("Used either like this: \"\\convert unit1 to unit2\" or like this: \"\\convert unit1 unit2\". Converts the previous answer from the first unit to the second.");
    } else if (!strcmp(str, "base")) {
        display_explanation("Prints out the previous answer in any base from 2 to 36.");
    } else if (!strcmp(str, "verbose")) {
        display_explanation("Prints out the lines to be evaluated before evaluating them.");
    } else if (!strcmp(str, "explain")) {
        display_explanation("Gives you information about commands, variables, constants and functions.");
    } else if (!strcmp(str, "cmod")) {
        display_explanation("Changes how the modulus operator (%%) behaves with negative numbers. The default is to behave like the C programming language modulus, the other is slightly more flexible. For example, with the default setting:\t-340 %% 60 == -40; 340 %% -60 == 40; -340 %% -60 == -40When this setting is toggled, it behaves like this:\t-340 %% 60 == -40; 340 %% -60 == -20; -340 %% -60 == 20");
    } else {
        if (test == 0) {
            report_error("Undefined command.");
        }
        return -1;
    }
    return 0;
}                                      /*}}} */

static void explain_variable(const char *str)
{                                      /*{{{ */
    struct answer var;

    var = getvar_full(str);
    if (var.err) {
        report_error("An error was encountered!\n");
        return;
    }
    if (var.exp) {                     // it's an expression (i.e. a function)
        List strings;

        strings = extract_vars(var.exp);
        display_expvar_explanation(str, var.exp, strings, var.desc);
        freeList(&strings);
    } else {
        display_valvar_explanation(str, &var.val, var.desc);
    }
}                                      /*}}} */

static void explain_constant(const char *str)
{                                      /*{{{ */
    switch (isconst(str)) {
        case W_e:
            display_explanation("The base of the natural logarithm function. Arbitrary precision. Occasionally called Euler's Number (after Leonhard Eulter) or Napier's Constant (after John Napier).");
            break;
        case W_pi:
            display_explanation("The ratio of a circle's circumference to its diameter. Arbitrary precision.");
            break;
        case W_random:
            display_explanation("A random number between 0 and %u.", UINT32_MAX);
            break;
        case W_irandom:
            display_explanation("A random integer between 0 and %u.", UINT32_MAX);
            break;
        case W_Na:
            display_explanation("Avogadro's Constant. The number of molecules in one mole.");
            break;
        case W_k:
            display_explanation("Boltzmann's Constant. The physical constant relating temperature to energy.");
            break;
        case W_Cc:
            display_explanation("Coulomb force constant. Equal to 1/(4*pi*epsilonzero).");
            break;
        case W_ec:
            display_explanation("Elementary charge. The electric charge carried by a single proton.");
            break;
        case W_R:
            display_explanation("Molar Gas Constant. Used in equations of state to relate various groups of state functions to one another.");
            break;
        case W_G:
            display_explanation("The universal gravitational constant. Appears in Einstein's theory of general relativity.");
            break;
        case W_g:
            display_explanation("The gravitational acceleration of Earth at sea-level.");
            break;
        case W_Me:
            display_explanation("The mass of an electron in kilograms.");
            break;
        case W_Mp:
            display_explanation("The mass of a proton in kilograms.");
            break;
        case W_Mn:
            display_explanation("The mass of a neutron in kilograms.");
            break;
        case W_Md:
            display_explanation("The mass of a deuteron in kilograms.");
            break;
        case W_u:
            display_explanation("Atomic mass unit. One twelfth of the mean mass of an atop of carbon 12 in its nuclear and electronic ground state.");
            break;
        case W_c:
            display_explanation("The speed of light.");
            break;
        case W_h:
            display_explanation("Planck Constant. Used to describe the sizes of quanta, central to the theory of quantum mechanics.");
            break;
        case W_mu0:
            display_explanation("Permeability of free space. Also known as the magnetic constant.");
            break;
        case W_epsilon0:
            display_explanation("Permittivity of free space. The ratio of D/E in vacuum.");
            break;
        case W_muB:
            display_explanation("Bohr magneton. A physical constant of magnetic moment.");
            break;
        case W_muN:
            display_explanation("Nuclear magneton. A physical constant of magnetic moment.");
            break;
        case W_b:
            display_explanation("Wein displacement constant. Defines the relationship between the thermodynamic temperature of a black body and the wavelength at which the intensity of the radiation is the greatest.");
            break;
        case W_a0:
            display_explanation("Bohr radius. A single electron orbiting at the lowest possible energy in a hydrogen atom orbits at this distance.");
            break;
        case W_F:
            display_explanation("Faraday constant. The amount of electric charge in one mole of electrons.");
            break;
        case W_Vm:
            display_explanation("Molar volume of ideal gas.");
            break;
        case W_eV:
            display_explanation("Electron volt. The amount of kinetic energy gained by a single unbound electron when it passes through an electrostatic potential difference of one volt, in vacuum.");
            break;
        case W_sigma:
            display_explanation("Stefan-Boltzmann constant. The constant of proportionality between the total energy radiated per unit surface area of a black body in unit time and the fourth power of the thermodynamic temperature, as per the Stefan-Boltzmann law.");
            break;
        case W_alpha:
            display_explanation("Fine-structure constant. The fundamental physical constant characterizing the strength of the electromagnetic interaction.");
            break;
        case W_gamma:
            display_explanation("Euler-Mascheroni constant. Used mainly in number theory, defined as the limiting difference between the harmonic series and the natural logarithm.");
            break;
        case W_re:
            display_explanation("Electron radius, also known as the Compton radius or the Thomson scattering length based on a classical relativistic model of the electron.");
            break;
        case W_Kj:
            display_explanation("Josephson Constant. The inverse of the magnetic flux quantum.");
            break;
        case W_Rk:
            display_explanation("von Klitzing constant. Named after Klaus von Klitzing, the basic resistance unit.");
            break;
        case W_Rinf:
            display_explanation("The \"infinity\" Rydberg constant, named after physicist Janne Rydberg. Used to calculate the Rydberg constant of any other chemical element.");
            break;
        case W_Eh:
            display_explanation("Hartree energy. The atomic unit of energy, named after physicist Douglas Hartree.");
            break;
        case W_Gf:
            display_explanation("Fermi Coupling Constant.");
            break;
        case W_Mmu:
            display_explanation("The mass of a muon in kilograms.");
            break;
        case W_Mt:
            display_explanation("The tau mass in kilograms.");
            break;
        case W_Mh:
            display_explanation("The mass of a helion in kilograms.");
            break;
        case W_Malpha:
            display_explanation("The mass of an alpha particle in kilograms.");
            break;
        case W_n0:
            display_explanation("Loschmidt constant. The number of molecules in a cubic meter of ideal gas. Equal to the Avogadro constant divided by molar volume.");
            break;
        case W_c1:
            display_explanation("First radiation constant.");
            break;
        case W_c2:
            display_explanation("Second radiation constant.");
            break;
        case W_G0:
            display_explanation("Conductance quantum.");
            break;
        case W_Z0:
            display_explanation("Characteristic impedance of vacuum, the characteristic impedance of electromagnetic radiation in vacuum. Equal to mu0*c.");
            break;
        case W_Phi0:
            display_explanation("Magnetic flux quantum. The quantum of magnetic flux passing through a superconductor.");
            break;
        case W_K:
            display_explanation("Catalan's constant commonly appears in estimates of combinatorial functions and in certain classes of sums and definite integrals.");
            break;
        case W_Inf:
            display_explanation("This represents infinity. This is a \"special\" number that has specific (unusual) mathematical consequences.");
            break;
        case W_NaN:
            display_explanation("Not-a-number. This is a \"special\" number that has specific (unusual) mathematical consequences.");
            break;
        case W_half:
            display_explanation("A special symbol for 0.5.");
            break;
        case W_quarter:
            display_explanation("A special symbol for 0.25.");
            break;
        case W_threequarters:
            display_explanation("A special symbol for 0.75.");
            break;
        case W_notaconstant:
            display_explanation("This is not a constant... you should not be seeing this message (report the bug!)");
            break;
    }
}                                      /*}}} */

static void explain_function(const char *str)
{                                      /*{{{ */
    if (!strcmp(str, "sin") || !strcmp(str, "cos") || !strcmp(str, "tan") ||
        !strcmp(str, "cot")) {
        display_explanation("A trigonometric function.");
    } else if (!strcmp(str, "asin") || !strcmp(str, "acos") ||
               !strcmp(str, "atan") || !strcmp(str, "acot") ||
               !strcmp(str, "arcsin") || !strcmp(str, "arccos") ||
               !strcmp(str, "arctan") || !strcmp(str, "arccot")) {
        display_explanation("The inverse of the standard trigonometric function.");
    } else if (!strcmp(str, "sinh") || !strcmp(str, "cosh") ||
               !strcmp(str, "tanh") || !strcmp(str, "coth")) {
        display_explanation("A standard hyperbolic trigonometric function.");
    } else if (!strcmp(str, "asinh") || !strcmp(str, "acosh") ||
               !strcmp(str, "atanh") || !strcmp(str, "acoth") ||
               !strcmp(str, "areasinh") || !strcmp(str, "areacosh") ||
               !strcmp(str, "areatanh") || !strcmp(str, "areacoth")) {
        display_explanation("The inverse of the standard hyperbolic trigonometric function.");
    } else if (!strcmp(str, "log")) {
        display_explanation("A logarithm (base 10).");
    } else if (!strcmp(str, "logtwo")) {
        display_explanation("A logarithm (base 2).");
    } else if (!strcmp(str, "ln")) {
        display_explanation("A logarithm (base e). Also known as the \"natural\" log.");
    } else if (!strcmp(str, "round")) {
        display_explanation("Returns the closest integer to the input number.");
    } else if (!strcmp(str, "abs")) {
        display_explanation("The absolute value (the distance of the number from zero).");
    } else if (!strcmp(str, "sqrt")) {
        display_explanation("The square root function.");
    } else if (!strcmp(str, "floor")) {
        display_explanation("Returns the biggest integer that is not bigger than the input number.");
    } else if (!strcmp(str, "ceil") || !strcmp(str, "ceiling")) {
        display_explanation("Returns the smallest integer that is not smaller than the input number.");
    } else if (!strcmp(str, "cbrt")) {
        display_explanation("The cube root function.");
    } else if (!strcmp(str, "rand")) {
        display_explanation("Returns a random number between 0 and the input number.");
    } else if (!strcmp(str, "irand")) {
        display_explanation("Returns a random integer between 0 and the input number.");
    } else if (!strcmp(str, "fact")) {
        display_explanation("Returns the factorial of the input number.");
    } else if (!strcmp(str, "comp")) {
        display_explanation("Returns the one's complement of the input number.");
    } else if (!strcmp(str, "zeta")) {
        display_explanation("Returns the Riemann zeta function of the input number. This is used primarily in number theory, but also has applications in physics, probability theory, and applied statistics.");
    } else if (!strcmp(str, "Gamma")) {
        display_explanation("Returns the Gamma function of the input number. The Gamma function extends the factorial function to complex and non-natural numbers.");
    } else if (!strcmp(str, "lnGamma")) {
        display_explanation("Returns the natural log of the Gamma function of the input number.");
    } else if (!strcmp(str, "eint")) {
        display_explanation("The exponential integral function.");
    } else if (!strcmp(str, "sinc")) {
        display_explanation("Sinus cardinalis, also known as the interpolation function, filtering function, or the first spherical Bessel function, is the product of a sine function and a monotonically decreasing function.");
    } else if (!strcmp(str, "exp")) {
        display_explanation("Returns the value of e to the given power. Equivalent to: e^");
    } else {
        display_explanation("Hrm... this function doesn't have any documentation! Pester the developer!");
    }
}                                      /*}}} */

/* vim:set expandtab: */
