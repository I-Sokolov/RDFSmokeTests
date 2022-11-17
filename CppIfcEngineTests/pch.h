#pragma once

#include <stdio.h>
#include <vector>
#include <iostream>

#include "ifcengine.h"
#include "IFC4.h"
#include "IFC2x3.h"
#include "IFC4x3.h"
#include "IFC4x4.h"
#include "CIS2.h"
#include "AP242.h"


#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT(c) {if (!(c)) { printf ("ASSERT failed at line %d file %s\n", __LINE__, __FILE__); throw (int) 13;}}

#define ENTER_TEST printf ("Running " __FUNCTION__ ".\n");
