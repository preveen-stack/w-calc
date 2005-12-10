#include <string.h>

int isconst(char *str)
{
	if (strcmp(str, "e") && strcmp(str, "pi") && strcmp(str, "PI") &&
		strcmp(str, "pI") && strcmp(str, "Pi") && strcmp(str, "random") &&
		strcmp(str, "irandom") && strcmp(str, "Na")
		&& strcmp(str, "NA") && strcmp(str, "k") && strcmp(str, "Cc") &&
		strcmp(str, "ec") && strcmp(str, "R") && strcmp(str, "G") &&
		strcmp(str, "g") && strcmp(str, "Me") && strcmp(str, "Mp") &&
		strcmp(str, "Mn") && strcmp(str, "Md") && strcmp(str, "u") &&
		strcmp(str, "amu") && strcmp(str, "c") && strcmp(str, "h") &&
		strcmp(str, "mu0") && strcmp(str, "muzero") && strcmp(str, "muZERO")
		&& strcmp(str, "epsilonzero") && strcmp(str, "epsilonZERO") &&
		strcmp(str, "epsilon0") && strcmp(str, "EPSILONzero")
		&& strcmp(str, "EPSILONZERO") && strcmp(str, "EPSILON0") &&
		strcmp(str, "muB") && strcmp(str, "b") && strcmp(str, "ao") &&
		strcmp(str, "F") && strcmp(str, "Vm") && strcmp(str, "NAk") &&
		strcmp(str, "eV") && strcmp(str, "sigma") && strcmp(str, "alpha") &&
		strcmp(str, "gamma") && strcmp(str, "re") && strcmp(str, "Kj") &&
		strcmp(str, "Rk") && strcmp(str, "Rinf") && strcmp(str, "Eh") &&
		strcmp(str, "Gf") && strcmp(str, "Mmu") && strcmp(str, "Mt") &&
		strcmp(str, "Mtau") && strcmp(str, "Mh") && strcmp(str, "Malpha") &&
		strcmp(str, "n0") && strcmp(str, "nzero") && strcmp(str, "c1") &&
		strcmp(str, "c2") && strcmp(str, "G0") && strcmp(str, "Gzero") &&
		strcmp(str, "Z0") && strcmp(str, "Zzero") && strcmp(str, "Phi0") &&
		strcmp(str, "Phizero") && strcmp(str, "PhiZERO")) {
		return 0;
	} else {
		return 1;
	}
}
