#include "pch.h"

static void    CheckData(RdfsResource entity, int64_t dataId1, void* data1, int64_t dataId2, void* data2)
{
    void* data = GetExternalReferenceData(entity, dataId1);
    ASSERT(data==data1);

    data = GetExternalReferenceData(entity, dataId2);
    ASSERT(data == data2);

}

static void    SetGetData(RdfsResource entity, int64_t dataId1, int64_t dataId2)
{
    CheckData(entity, dataId1, 0, dataId2, 0);

    auto res = SetExternalReferenceData(entity, dataId1, (void*)77);
    ASSERT(res);
    CheckData(entity, dataId1, (void*)77, dataId2, 0);

    res = SetExternalReferenceData(entity, dataId2, (void*)9);
    ASSERT(res);
    CheckData(entity, dataId1, (void*)77, dataId2, (void*)9);

    res = SetExternalReferenceData(entity, dataId1, 0);
    ASSERT(res);
    CheckData(entity, dataId1, 0, dataId2, (void*)9);

}

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
    auto dataId3 = GetExternalReferenceDataId(model, "app3");
    ASSERT(dataId3 == 3);
    dataId = GetExternalReferenceDataId(model, "AnotherApp");
    ASSERT(dataId == 2);

    SetGetData(model, dataId, dataId3);

    auto prop = GetPropertyByName(model, "width");
    ASSERT(prop);

    SetGetData(prop, dataId3, dataId);

    auto internalClass = GetClassByName(model, "Box");
    ASSERT(internalClass);

    SetGetData(internalClass, dataId, dataId3);

    CloseModel(model);

}