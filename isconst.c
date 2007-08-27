#include <string.h>
#include "isconst.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif

const char *consts[] = {
    "e","pi","PI","pI","Pi","random","irandom","Na","NA",
    "k","Cc","ec","R","G","g","Me","Mp","Mn","Md","u",
    "amu","c","h","mu0","muzero","muZERO","epsilonzero",
    "epsilonZERO","epsilon0","EPSILONzero","EPSILONZERO","EPSILON0",
    "muB","b","a0","F","Vm","NAk","eV","sigma","alpha","gamma","re",
    "Kj","Rk","Rinf","Eh","Gf","Mmu","Mt","Mtau","Mh","Malpha","n0",
    "nzero","c1","c2","G0","Gzero","Z0","Zzero","Phi0","Phizero","PhiZERO",
    0
};

int isconst(const char *str)
{
    size_t i;

    for (i=0; consts[i]; i++) {
	if (strcmp(consts[i], str) == 0) {
	    return 1;
	}
    }
    return 0;
}
