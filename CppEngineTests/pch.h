#pragma once

#include <stdio.h>


#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT(c) {if (!(c)) { printf ("ASSERT failed at LINE %d FILE %s\n", __LINE__, __FILE__); throw (int) 13;}}
