#pragma once

#include <stdio.h>
#include <vector>
#include <iostream>

#include "ifcengine.h"
#include "..\ModelCheckerAPI.h"

#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT(c) {if (!(c)) { printf ("ASSERT failed at line %d file %s\n", __LINE__, __FILE__); throw (int) 13;}}

#define ENTER_TEST printf ("Running " __FUNCTION__ ".\n");
