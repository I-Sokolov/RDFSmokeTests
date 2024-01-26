#pragma once

#ifdef _DEBUG
//#define VLD_ON
#endif

#ifdef VLD_ON
#pragma message ("---- Enable Visual Leak Detector -----")
#include "vld.h"
#endif

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

static void Assert(bool c, int line, const char* file)
{
    if (!(c)) { 
        printf("ASSERT failed at line %d file %s\n", line, file);
        throw (int)13; 
    }
}

#define ASSERT(c) Assert(c, __LINE__, __FILE__);

#define ENTER_TEST printf ("Running " __FUNCTION__ ".\n");
