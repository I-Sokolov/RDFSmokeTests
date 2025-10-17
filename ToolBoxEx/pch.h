#pragma once

#ifdef _DEBUG
//#define VLD_ON
#endif

#ifdef VLD_ON
#pragma message ("---- Enable Visual Leak Detector -----")
#include "vld.h"
#endif

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

#include "SmokeTests.h"

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


static long HeapUsed()
{
    _HEAPINFO info = { 0, 0, 0 };
    long used = 0;
    int rc;

    while ((rc = _heapwalk(&info)) == _HEAPOK) {
        if (info._useflag == _USEDENTRY)
            used += (long)info._size;
    }
    if (rc != _HEAPEND && rc != _HEAPEMPTY)
        used = (used ? -used : -1);

    return used;
}
