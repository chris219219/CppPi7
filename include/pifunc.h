#ifndef PIFUNC_H
#define PIFUNC_H

#include <string>

typedef uint_fast8_t ufast8;
typedef int_fast32_t fast32;
typedef int_fast64_t fast64;

using std::string;

namespace pifunc
{
    char pihex(fast64 n);
    string pigroup(fast64 start, fast32 length, fast32 threads);
}

#endif