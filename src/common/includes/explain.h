#ifndef WCALC_EXPLAIN
#define WCALC_EXPLAIN

struct name_with_exp {
    const char *names[5];
    const char  *explanation;
};

void explain(const char *str);
#endif
/* vim:set expandtab: */
