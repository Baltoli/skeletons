#ifndef SKEL_MACROS_H
#define SKEL_MACROS_H

#define MAYBE_PAR_FOR(x) for(x,0)
#define NO_PAR_FOR(x) for(x,0)
#define PAR_FOR(x) _Pragma("omp parallel for") for(x,0)

#endif
