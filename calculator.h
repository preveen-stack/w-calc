#ifndef WCALC_H
#define WCALC_H

#define VERSION "1.3"

#define W_EXTRA 5

#define W_PI 3.14159265358979323846264338327950288419716939937510582097
#define W_E 2.718281828459045235360287471352662497757247093699959574966
#define W_AVOGADROS_CONSTANT 6.0221367e23
#define W_BOLTZMANN_CONSTANT 1.380658e-23
#define W_COULOMB_CONSTANT 8987551787.37
#define W_ELEMENTARY_CHARGE 1.60217616263e-19
#define W_MOLAR_GAS_CONSTANT 8.31451
#define W_GRAVITATIONAL_CONSTANT 6.67259e-11
#define W_GRAVITATIONAL_ACCELLERATION 9.80665
#define W_ELECTRON_MASS 9.1093897e-31
#define W_PROTON_MASS 1.6726231e-27
#define W_NEUTRON_MASS 1.6749286e-27
#define W_DEUTERON_MASS 1875.61276275
#define W_ATOMIC_MASS 1.6605387313e-27
#define W_SPEED_OF_LIGHT 299792458
#define W_PERMEABILITY_OF_FREE_SPACE 1.25663706144e-6
#define W_PERMITTIVITY_OF_FREE_SPACE 8.854187817e-12
#define W_PLANCK_CONSTANT 6.6260755e-34
#define W_BOHR_MAGNETON 5.78838174943e-11
#define W_NUCLEAR_MAGNETON 3.15245123824e-14
#define W_WIEN_DISPLACEMENT 2.897768651e-3
#define W_W_BOSON_MASS 80.41956
#define W_Z_BOSON_MASS 91.188222
#define W_BOHR_RADIUS 0.529177208319e-10
#define W_FARADAY_CONSTANT 96485.31
#define W_MOLAR_VOLUME_OF_IDEAL_GAS 22.4141
#define W_ELECTRON_VOLT 1.602177250e-12
#define W_RADIATION_DENSITY 7.5646e-15
#define W_STEFAN_BOLTZMANN 5.67040040e-8
#define W_FINE_STRUCTURE 7.29735253327e-3
#define W_EULER 0.577215664901532861
#define W_ELECTRON_RADIUS 2.81794028531e-15

enum functions {wnot,wsin,wcos,wtan,wasin,wacos,watan,wsinh,wcosh,wtanh,wasinh,wacosh,watanh,wlog,wln,wround,wneg,wabs};
enum operations {wplus,wminus,wmult,wdiv,wmod,wfact,wexp,wor,wand,wequal,wnequal,wgt,wlt,wgeq,wleq,wnone};

double parseme (char *);
void print_results (void);
void push_value (double);
void report_error (char *);
void print_result (void);
void print_this_result (double result);
double uber_function (enum functions func, double input);
double simple_exp (double first, enum operations op, double second);


long fact (int);

extern double last_answer;
extern char pretty_answer[200];
extern short standard_output;
extern short compute;

extern int precision;
extern short engineering;
extern short picky_variables;
extern short use_radians;

#endif
