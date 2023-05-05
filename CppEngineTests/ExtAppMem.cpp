#include "pch.h"

extern void ExtAppMemTest()
{
    ENTER_TEST;

    OwlModel model = OpenModel(NULL);
    ASSERT(model);

    auto mem = AllocModelMemory(model, 12);
    *((char*)mem) = 'A';

    auto dataId = GetExternalReferenceDataId(model, "app1");
    ASSERT(dataId == 1);
    dataId = GetExternalReferenceDataId(model, "AnotherApp");
    ASSERT(dataId == 2);
    dataId = GetExternalReferenceDataId(model, "app3");
    ASSERT(dataId == 3);
    dataId = GetExternalReferenceDataId(model, "AnotherApp");
    ASSERT(dataId == 2);

    CloseModel(model);

}