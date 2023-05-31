/**
 * @file Elveflow64_shim.h
 * A shim header for Elveflow64.h. It undefines a precompiler macro, that
 * may clash with other libraries.
 *
 */

#include <Elveflow64.h>
#undef Offset(type, field)(                                                    \
    (NI_REINTERPRET_CAST(size_t, (&NI_REINTERPRET_CAST(type *, 1)->field))) -  \
    1)