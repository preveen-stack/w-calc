#ifndef WCALC_ISCONST
#define WCALC_ISCONST

typedef enum constnames
{
    W_notaconstant,
    W_e, W_pi, W_random, W_irandom, W_Na, W_k, W_Cc, W_ec, W_R, W_G, W_g,
    W_Me, W_Mp, W_Mn, W_Md, W_u, W_c, W_h, W_mu0, W_epsilon0, W_muB,
    W_muN, W_b, W_a0, W_F, W_Vm, W_eV, W_sigma, W_alpha, W_gamma, W_re,
    W_Kj, W_Rk, W_Rinf, W_Eh, W_Gf, W_Mmu, W_Mt, W_Mh, W_Malpha, W_n0,
    W_c1, W_c2, W_G0, W_Z0, W_Phi0, W_quarter, W_half, W_threequarters,
    W_K, W_NaN, W_Inf,
} consttype;

typedef struct
{
    consttype type;
    char *label;
} constdef;

extern const constdef consts[];

consttype isconst(const char *str);
void printconsts(void);

#endif
