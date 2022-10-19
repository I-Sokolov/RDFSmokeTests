#pragma once

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <list>

#include "engine.h"
#include "geom.h"


#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT_AT(c,file,line) { if (!(c)) { printf ("ASSERT failed at line %d file %s\n", line, file); throw (int) 13;}}

#define ASSERT(c) { ASSERT_AT((c),__FILE__,__LINE__); }

#define ENTER_TEST printf ("Running " __FUNCTION__ ".\n");
