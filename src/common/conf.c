#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "conf.h"

struct _conf conf;

struct _conf * getConf(void) {
    return &conf;
}
