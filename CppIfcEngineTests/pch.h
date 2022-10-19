#pragma once

#include <stdio.h>

#include "ifcengine.h"
#include "IFC4.h"
#include "IFC2x3.h"
#include "IFC4x3.h"
#include "CIS2.h"


#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT(c) {if (!(c)) { printf ("ASSERT failed at LINE %d FILE %s\n", __LINE__, __FILE__); throw (int) 13;}}

