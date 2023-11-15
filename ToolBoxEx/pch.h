#pragma once

#include <Windows.h>

#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <filesystem>

#include "ifcengine.h"

#ifdef ASSERT
#undef ASSERT
#endif

static void DoAssert(bool c, int line, const char* file)
{
    if (!c) { 
        printf("ASSERT failed at line %d file %s\n", line, file); 
        throw (int)13; 
    }
}

#define ASSERT(c) {DoAssert ((c), __LINE__, __FILE__);}

#define ENTER_TEST printf ("Running " __FUNCTION__ ".\n");

#define ENTER_TEST_NAME(name) printf ("Running " __FUNCTION__ " %s.\n", (name));


