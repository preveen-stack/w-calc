#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* System Headers */
#include <string.h>

/* Internal Headers */
#include "isconst.h"

#warning this is all in utf8... should probably use iconv

static const struct name_with_exp consts[] = {
    { { "e", NULL },
      "The base of the natural logarithm function. Arbitrary precision. Occasionally called Euler's Number (after Leonhard Eulter) or Napier's Constant (after John Napier)." },
    { { "pi", "pI", "Pi", "PI", "\317\200", NULL },
      "The ratio of a circle's circumference to its diameter. Arbitrary precision." },
    { { "random", NULL },
      "A random number between 0 and 2^32." },
    { { "irandom", NULL },
      "A random integer between 0 and 2^32." },
    { { "Na", "NA", NULL },
      "Avogadro's Constant. The number of molecules in one mole." },
    { { "k", NULL },
      "Boltzmann's Constant. The physical constant relating temperature to energy." },
    { { "Cc", NULL },
      "Coulomb force constant. Equal to 1/(4*pi*epsilonzero)." /* W_Cc */ },
    { { "ec", NULL },
      "Elementary charge. The electric charge carried by a single proton." /* W_ec */ },
    { { "R", NULL },
      "Molar Gas Constant. Used in equations of state to relate various groups of state functions to one another." /* W_R */ },
    { { "G", NULL },
      "The universal gravitational constant. Appears in Einstein's theory of general relativity." /* W_G */ },
    { { "g", NULL },
      "The gravitational acceleration of Earth at sea-level." /* W_g */ },
    { { "Me", NULL },
      "The mass of an electron in kilograms." /* W_Me */ },
    { { "Mp", NULL },
      "The mass of a proton in kilograms." /* W_Mp */ },
    { { "Mn", NULL },
      "The mass of a neutron in kilograms." /* W_Mn */ },
    { { "Md", NULL },
      "The mass of a deuteron in kilograms." /* W_Md */ },
    { { "u", "amu", NULL },
      "Atomic mass unit. One twelfth of the mean mass of an atop of carbon 12 in its nuclear and electronic ground state." /* W_u */ },
    { { "c", NULL },
      "The speed of light." /* W_c */ },
    { { "h", NULL },
      "Planck Constant. Used to describe the sizes of quanta, central to the theory of quantum mechanics." /* W_h */ },
    { { "mu0", "muzero", "muZERO", "\302\2650", "\302\265zero", "\302\265ZERO", "\316\2740", "\316\274zero", "\316\274ZERO", NULL },
      "Permeability of free space. Also known as the magnetic constant." },
    { { "epsilonzero", "epsilonZERO", "epsilon0", "EPSILONzero", "EPSILONZERO", "EPSILON0", "\316\265zero", "\316\265ZERO", "\316\2650", NULL },
      "Permittivity of free space. The ratio of D/E in vacuum." /* W_epsilon0 */ },
    { { "\302\265B", "\316\274B", "muB", NULL },
      "Bohr magneton. A physical constant of magnetic moment." /* W_muB */ },
    { { "\302\265N", "\316\274N", "muN", NULL },
      "Nuclear magneton. A physical constant of magnetic moment." /* W_muN */ },
    { { "b", NULL },
      "Wein displacement constant. Defines the relationship between the thermodynamic temperature of a black body and the wavelength at which the intensity of the radiation is the greatest." /* W_b */ },
    { { "a0", NULL },
      "Bohr radius. A single electron orbiting at the lowest possible energy in a hydrogen atom orbits at this distance." /* W_a0 */ },
    { { "F", NULL },
      "Faraday constant. The amount of electric charge in one mole of electrons." /* W_F */ },
    { { "Vm", "NAk", NULL },
      "Molar volume of ideal gas." /* W_Vm */ },
    { { "eV", NULL },
      "Electron volt. The amount of kinetic energy gained by a single unbound electron when it passes through an electrostatic potential difference of one volt, in vacuum." /* W_eV */ },
    { { "sigma", "\317\203", NULL },
      "Stefan-Boltzmann constant. The constant of proportionality between the total energy radiated per unit surface area of a black body in unit time and the fourth power of the thermodynamic temperature, as per the Stefan-Boltzmann law." /* W_sigma */ },
    { { "alpha", "\316\261", NULL },
      "Fine-structure constant. The fundamental physical constant characterizing the strength of the electromagnetic interaction." /* W_alpha */ },
    { { "gamma", "GAMMA", "\316\263", NULL },
      "Euler-Mascheroni constant. Used mainly in number theory, defined as the limiting difference between the harmonic series and the natural logarithm." /* W_gamma */ },
    { { "re", NULL },
      "Electron radius, also known as the Compton radius or the Thomson scattering length based on a classical relativistic model of the electron." /* W_re */ },
    { { "Kj", NULL },
      "Josephson Constant. The inverse of the magnetic flux quantum." /* W_Kj */ },
    { { "Rk", NULL },
      "von Klitzing constant. Named after Klaus von Klitzing, the basic resistance unit." /* W_Rk */ },
    { { "Rinf", "R\342\210\236", NULL },
      "The \"infinity\" Rydberg constant, named after physicist Janne Rydberg. Used to calculate the Rydberg constant of any other chemical element." /* W_Rinf */ },
    { { "Eh", NULL },
      "Hartree energy. The atomic unit of energy, named after physicist Douglas Hartree." /* W_Eh */ },
    { { "Gf", NULL },
      "Fermi Coupling Constant." /* W_Gf */ },
    { { "Mmu", "M\302\265", "M\316\274", NULL },
      "The mass of a muon in kilograms." /* W_Mmu */ },
    { { "Mt", "Mtau", "M\317\204", NULL },
      "The tau mass in kilograms." /* W_Mt */ },
    { { "Mh", NULL },
      "The mass of a helion in kilograms." /* W_Mh */ },
    { { "Malpha", "M\316\261", NULL },
      "The mass of an alpha particle in kilograms." /* W_Malpha */ },
    { { "n0", "nzero", "nZERO", NULL },
      "Loschmidt constant. The number of molecules in a cubic meter of ideal gas. Equal to the Avogadro constant divided by molar volume." /* W_n0 */ },
    { { "c1", NULL },
      "First radiation constant." /* W_c1 */ },
    { { "c2", NULL },
      "Second radiation constant." /* W_c2 */ },
    { { "G0", "Gzero", "GZERO", NULL },
      "Conductance quantum." /* W_G0 */ },
    { { "Z0", "Zzero", "ZZERO", NULL },
      "Characteristic impedance of vacuum, the characteristic impedance of electromagnetic radiation in vacuum. Equal to mu0*c." /* W_Z0 */ },
    { { "Phi0", "Phizero", "PhiZERO", "\316\2460", "\316\246zero", "\316\246ZERO", NULL },
      "Magnetic flux quantum. The quantum of magnetic flux passing through a superconductor." /* W_Phi0 */ },
    { { "\302\274", NULL },
      "A special symbol for 0.25." /* W_quarter */ },
    { { "\302\275", NULL },
      "A special symbol for 0.5." /* W_half */ },
    { { "\302\276", NULL },
      "A special symbol for 0.75." /* W_threequarters */ },
    { { "K", NULL },
      "Catalan's constant commonly appears in estimates of combinatorial functions and in certain classes of sums and definite integrals." /* W_K */ },
    { { NAN_STRING, NULL },
      "Not-a-number. This is a \"special\" number that has specific (unusual) mathematical consequences." /* W_NaN */ },
    { { INF_STRING, NULL },
      "This represents infinity. This is a \"special\" number that has specific (unusual) mathematical consequences." /* W_Inf */ },
    { { 0 }, NULL }
};

const struct name_with_exp *getConsts()
{
    return consts;
}


int
isconst(const char *str)
{   /*{{{*/
    unsigned i, j;

    for (i = 0; consts[i].explanation; i++) {
        for (j = 0; consts[i].names[j]; j++) {
            if (strcmp(consts[i].names[j], str) == 0) {
                return 1;
            }
        }
    }
    return 0;
} /*}}}*/

/* vim:set expandtab: */
