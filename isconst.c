#include <string.h>
#include <stdio.h>
#include "isconst.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif

const constdef consts[] = {
    {W_e,"e"},
    {W_pi,"pi"},
    {W_pi,"pI"},
    {W_pi,"Pi"},
    {W_pi,"PI"},
    {W_pi,"\317\200"},
    {W_random,"random"},
    {W_Na,"Na"},
    {W_Na,"NA"},
    {W_k,"k"},
    {W_Cc,"Cc"},
    {W_ec,"ec"},
    {W_R,"R"},
    {W_G,"G"},
    {W_g,"g"},
    {W_Me,"Me"},
    {W_Mp,"Mp"},
    {W_Mn,"Mn"},
    {W_Md,"Md"},
    {W_u,"u"},
    {W_u,"amu"},
    {W_c,"c"},
    {W_h,"h"},
    {W_mu0, "mu0"},
    {W_mu0, "muzero"},
    {W_mu0, "muZERO"},
    {W_mu0, "\302\2650"},
    {W_mu0, "\302\265zero"},
    {W_mu0, "\302\265ZERO"},
    {W_mu0, "\316\2740"},
    {W_mu0, "\316\274zero"},
    {W_mu0, "\316\274ZERO"},
    {W_epsilon0, "epsilonzero"},
    {W_epsilon0, "epsilonZERO"},
    {W_epsilon0, "epsilon0"},
    {W_epsilon0, "EPSILONzero"},
    {W_epsilon0, "EPSILONZERO"},
    {W_epsilon0, "EPSILON0"},
    {W_epsilon0, "\316\265zero"},
    {W_epsilon0, "\316\265ZERO"},
    {W_epsilon0, "\316\2650"},
    {W_muB, "\302\265B"},
    {W_muB, "\316\274B"},
    {W_muB, "muB"},
    {W_muN, "\302\265N"},
    {W_muN, "\316\274N"},
    {W_muN, "muN"},
    {W_b, "b"},
    {W_a0, "a0"},
    {W_F, "F"},
    {W_Vm, "Vm"},
    {W_Vm, "NAk"},
    {W_eV, "eV"},
    {W_sigma, "sigma"},
    {W_sigma, "\317\203"},
    {W_alpha, "alpha"},
    {W_alpha, "\316\261"},
    {W_gamma, "gamma"},
    {W_gamma, "GAMMA"},
    {W_gamma, "\316\263"},
    {W_re, "re"},
    {W_Kj, "Kj"},
    {W_Rk, "Rk"},
    {W_Rinf, "Rinf"},
    {W_Rinf, "R\342\210\236"},
    {W_Eh, "Eh"},
    {W_Gf, "Gf"},
    {W_Mmu, "Mmu"},
    {W_Mmu, "M\302\265"},
    {W_Mmu, "M\316\274"},
    {W_Mt, "Mt"},
    {W_Mt, "Mtau"},
    {W_Mt, "M\317\204"},
    {W_Mh, "Mh"},
    {W_Malpha, "Malpha"},
    {W_Malpha, "M\316\261"},
    {W_n0, "n0"},
    {W_n0, "nzero"},
    {W_n0, "nZERO"},
    {W_c1, "c1"},
    {W_c2, "c2"},
    {W_G0, "G0"},
    {W_G0, "Gzero"},
    {W_G0, "GZERO"},
    {W_Z0, "Z0"},
    {W_Z0, "Zzero"},
    {W_Z0, "ZZERO"},
    {W_Phi0, "Phi0"},
    {W_Phi0, "Phizero"},
    {W_Phi0, "PhiZERO"},
    {W_Phi0, "\316\2460"},
    {W_Phi0, "\316\246zero"},
    {W_Phi0, "\316\246ZERO"},
    {W_quarter, "\302\274"},
    {W_half, "\302\275"},
    {W_threequarters, "\302\276"},
    {W_K, "K"},
    {W_NaN, "@NaN@"},
    {W_Inf, "@Inf@"},
    {W_notaconstant,0}
};

consttype isconst(const char *str)
{
    int i;

    for (i=0; consts[i].label; i++) {
	if (strcmp(consts[i].label, str) == 0) {
	    return consts[i].type;
	}
    }
    return W_notaconstant;
}

void printconsts()
{
    size_t i;
    size_t linelen = 0;

    for (i=0; consts[i].label; i++) {
	if (linelen + strlen(consts[i].label) + 2 > 70) {
	    printf(",\n");
	    linelen = 0;
	}
	if (linelen == 0) {
	    printf("%s", consts[i].label);
	    linelen = strlen(consts[i].label);
	} else {
	    printf(", %s", consts[i].label);
	    linelen += strlen(consts[i].label) + 2;
	}
    }
    printf("\n");
}
