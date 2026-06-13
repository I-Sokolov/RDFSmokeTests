#include "pch.h"

static void AddEntities(OwlModel model)
{
    auto cls = CreateClass(model, "MyTestClass");

    auto prop = CreateProperty(model, DATATYPEPROPERTY_TYPE_STRING, "TestListOfString");    
    const char* str[] = { "my string 1", "my \"string\" 2", "line 3" };
    SetDatatypeProperty(cls, prop, str, _countof(str));

    prop = CreateProperty(model, DATATYPEPROPERTY_TYPE_INTEGER, "TestListOfInt");
    int_t vals[] = { 1, 2, 3, 4, 6, 7, 8, 9, 10, 22, -4, -5 };
    SetDatatypeProperty(cls, prop, vals, _countof(vals));
}

extern void SaveAsTTL()
{
    ENTER_TEST;
    auto model = OpenModel("..\\TestData\\test.bin");

    AddEntities(model);

    auto res = SaveModel(model, "test.ttl");
    ASSERT(res == 0);

    //
    SetOverrideFileIO(model, FLAGBIT(5), FLAGBIT(5));

    res = SaveModelW(model, L"test2.ttl");
    ASSERT(res == 0);


    CloseModel(model);

    //
    model = OpenModel("test.ttl");
    ASSERT(model);

    CloseModel(model);

    //
    model = OpenModelW(L"test2.ttl");
    ASSERT(model);

    CloseModel(model);
}
