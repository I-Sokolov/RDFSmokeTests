#pragma once

#include <Windows.h>

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string>
#include <set>
#include <filesystem>

#include "ifcengine.h"

#define IFCENGINE_MODEL_CHECKER
#include "..\ModelCheckerAPI.h"

#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT(c) {if (!(c)) { printf ("ASSERT failed at line %d file %s\n", __LINE__, __FILE__); throw (int) 13;}}

#define ENTER_TEST printf ("Running " __FUNCTION__ ".\n");
