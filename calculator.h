#ifndef WCALC_H
#define WCALC_H

#define VERSION "1.1.1"

#define W_EXTRA 5

#define WPI 3.14159265358979323846264338327950288419716939937510582097
#define WE 2.718281828459045235360287471352662497757247093699959574966

enum functions {wnot,wsin,wcos,wtan,wasin,wacos,watan,wsinh,wcosh,wtanh,wasinh,wacosh,watanh,wlog,wln,wround,wneg};
enum operations {wplus,wminus,wmult,wdiv,wmod,weql,wexp,wor,wand,wequal,wnequal,wgt,wlt,wgeq,wleq,wfact,wnone};

double parseme (char *);
void print_results (void);
void push_value (double);
void report_error (char *);
void print_result (void);
void print_this_result (double result);

long fact (int);

extern double last_answer;
extern char pretty_answer[200];
extern short standard_output;

extern int precision;
extern short engineering;
extern short picky_variables;
extern short use_radians;

#endif
