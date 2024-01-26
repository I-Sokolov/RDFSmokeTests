#pragma once

#ifdef _DEBUG
//#define VLD_ON
#endif

#ifdef VLD_ON
#pragma message ("---- Enable Visual Leak Detector -----")
#include "vld.h"
#endif

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include <list>
#include <string>
#include <map>
#include <set>

#include "engine.h"
#include "geom.h"


#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT_AT(c,file,line) { if (!(c)) { printf ("ASSERT failed at line %d file %s\n", line, file); throw (int) 13;}}

#define ASSERT(c) { ASSERT_AT((c),__FILE__,__LINE__); }

#define ENTER_TEST_EX2(name, name2) printf ("Running " __FUNCTION__ " %s %s\n", name, name2);

#define ENTER_TEST_EX(name) ENTER_TEST_EX2(name, "")

#define ENTER_TEST ENTER_TEST_EX("")

#define ASSERT_ARR_EQ(r1,r2,N)  	for (int i=0; i<N; i++) { ASSERT_AT((fabs ((double)r1[i]-(double)r2[i]) < 1e-9), /*#r1 "!=" #r2,*/ __FILE__, __LINE__);}