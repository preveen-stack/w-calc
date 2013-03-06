/*
 *  definitions.h
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Thu Feb 07 2002.
 *  Copyright (c) 2001 Kyle Wheeler. All rights reserved.
 *
 */

#ifndef WCALC_DEFINITIONS
#define WCALC_DEFINITIONS

#define W_EXTRA      5
#define RAND_FILE    "/dev/random"
#define DEFAULT_BASE 10

#define W_PI                          "3.14159265358979323846264338327950288419716939937510582097"
#define W_E                           "2.718281828459045235360287471352662497757247093699959574966"
#define W_AVOGADROS_CONSTANT          "6.0221415e23"
#define W_BOLTZMANN_CONSTANT          "1.3806505e-23"
#define W_COULOMB_CONSTANT            "8987551787.37"
#define W_ELEMENTARY_CHARGE           "1.60217653e-19"
#define W_MOLAR_GAS_CONSTANT          "8.314472"
#define W_GRAVITATIONAL_CONSTANT      "6.6742e-11" /* m^3 kg^-1 s^-2 */
#define W_GRAVITATIONAL_ACCELLERATION "9.80665"    /* m s^-2 */
#define W_SPEED_OF_LIGHT              "299792458"
#define W_PERMEABILITY_OF_FREE_SPACE  "1.25663706144e-6" /* 4pi*pow(10,-7) */
#define W_PERMITTIVITY_OF_FREE_SPACE  "8.854187817e-12"  /* F m^-1 */
#define W_PLANCK_CONSTANT             "6.6260693e-34"
#define W_BOHR_MAGNETON               "927.400949e-26"  /* J T^-1 */
#define W_NUCLEAR_MAGNETON            "5.05078343e-27"  /* J T^-1 */
#define W_WIEN_DISPLACEMENT           "2.8977685e-3"    /* m K */
#define W_BOHR_RADIUS                 "5.291772108e-11" /* m */
#define W_FARADAY_CONSTANT            "96485.3383"      /* C mol^-1 */
#define W_MOLAR_VOLUME_OF_IDEAL_GAS   "22.413996e-3"    /* m^3 mol^-1 */
#define W_ELECTRON_VOLT               "1.60217653e-19"  /* J */
#define W_STEFAN_BOLTZMANN            "5.670400e-8"
#define W_FINE_STRUCTURE              "7.297352568e-3"
#define W_EULER                       "0.57721566490153286060651209008240243104215933593992359880576723488486772677766467093694706329174674951463144724980708248096050401448654283622417399764492353625350033374293733773767394279259525824709491600873520394816567" /* http://primes.utm.edu/glossary/page.php?sort=Gamma */
#define W_ELECTRON_RADIUS             "2.817940325e-15"                                                                                                                                                                                                              /* m */
#define W_JOSEPHSON_CONSTANT          "483597.879e9"                                                                                                                                                                                                                 /* Hz V^-1 */
#define W_VON_KLITZING_CONSTANT       "25812.807449"                                                                                                                                                                                                                 /* omega */
#define W_RYDBERG_CONSTANT            "10973731.568525"                                                                                                                                                                                                              /* m^-1 */
#define W_HARTREE_ENERGY              "4.35974417e-18"                                                                                                                                                                                                               /* J */
#define W_FERMI_COUPLING_CONSTANT     "1.16638e-5"                                                                                                                                                                                                                   /* GeV^-2 */
#define W_MUON_MASS                   "1.88353140e-28"                                                                                                                                                                                                               /* kg */
#define W_TAU_MASS                    "3.16777e-27"                                                                                                                                                                                                                  /* kg */
#define W_ELECTRON_MASS               "9.1093826e-31"                                                                                                                                                                                                                /* kg */
#define W_PROTON_MASS                 "1.67262171e-27"                                                                                                                                                                                                               /* kg */
#define W_NEUTRON_MASS                "1.67492728e-27"                                                                                                                                                                                                               /* kg */
#define W_DEUTERON_MASS               "3.34358335e-27"                                                                                                                                                                                                               /* kg */
#define W_ATOMIC_MASS                 "1.66053886e-27"                                                                                                                                                                                                               /* kg */
#define W_HELION_MASS                 "5.00641214e-27"                                                                                                                                                                                                               /* kg */
#define W_ALPHA_PARTICLE_MASS         "6.6446565e-27"                                                                                                                                                                                                                /* kg */
#define W_LOSCHMIDT_CONSTANT          "2.6867773e25"                                                                                                                                                                                                                 /* m^-3 */
#define W_FIRST_RADIATION_CONSTANT    "3.74177138e-16"                                                                                                                                                                                                               /* W m^2 */
#define W_SECOND_RADIATION_CONSTANT   "1.4387752e-2"                                                                                                                                                                                                                 /* m K */
#define W_CONDUCTANCE_QUANTUM         "7.748091733e-5"                                                                                                                                                                                                               /* S */
#define W_IMPEDANCE_OF_VACUUM         "376.730313461"                                                                                                                                                                                                                /* ohms */
#define W_MAGNETIC_FLUX_QUANTUM       "2.06783372e-15"                                                                                                                                                                                                               /* Wb */
#define W_CATALAN                     "0.9159655941772190150546035149323841107741493742816721342664981196217630197762547694793565129261151062485744226191961995790358988033258590594315947374811584069953320287733194605190387274781640878659090247064841521630002287276409423882599577415088163974702524820115607076448838078733704899008647751132259971343385815193110992038743101834993784779390390353488916999900473822093605334185626171594509860056"

#define SUPPORTED_SYMBOLS                                         \
    "+ - () {} [] * %% / ! ^ > >= < <= == != && || << >> | & ~\n" \
    "not ** or and equals eq ne xor\n\n"                          \
    "sin cos tan cot asin acos atan acot\n"                       \
    "sinh cosh tanh coth asinh acosh atanh acoth\n"               \
    "log ln logtwo round abs ceil floor sqrt cbrt comp\n"         \
    "Gamma lnGamma zeta sinc\n"                                   \
    "\n"                                                          \
    "e pi Na k Cc ec R G g Me Mp Mn Md u amu c h mu0 epsilon0\n"  \
    "muB muN b ao F Vm NAk eV sigma alpha gamma re Kj Rk Rinf\n"  \
    "Eh Gf Mmu Mt Mh Malpha n0 c1 c2 G0 Phi0 K\n"                 \
    "random irandom rand irand\n"

#endif // ifndef WCALC_DEFINITIONS
/* vim:set expandtab: */
