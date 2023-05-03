#include "pch.h"

template <typename T> bool IsEqual(T a, T b) { return a == b; }
static bool IsEqual(const char* a, const char* b) { return 0==strcmp(a,b); }

template <typename T> void SetGetDatatypeProp(OwlClass cls, const char* propName, T v)
{
    auto model = GetModel(cls);

    auto prop = GetPropertyByName(model, propName);
    ASSERT(prop);

    int64_t card = 0;
    T* val;
    int64_t res = GetDataTypeProperty(cls, prop, (void**)&val, &card);
    ASSERT(card == 0 && res == 0);

    res = SetDatatypeProperty(cls, prop, v);
    ASSERT(res == 0);

    res = GetDataTypeProperty(cls, prop, (void**)&val, &card);
    ASSERT(card == 1 && IsEqual (val[0],v) && res == 0);
}

extern void MetaPropertiesTest()
{
    ENTER_TEST;

    OwlModel model = OpenModel(NULL);
    ASSERT(model);

    auto propEnanced = CreateProperty(model, DATATYPEPROPERTY_TYPE_CHAR, "MyPropery");

    auto internalClass = GetClassByName(model, "Box");
    ASSERT(internalClass);

    SetGetDatatypeProp(internalClass, "length", 2.4);
    SetGetDatatypeProp(internalClass, "MyPropery", "my valye");

    auto customClass = CreateClass(model, "ClassA");
    SetClassParent(customClass, internalClass, 1);

    SetGetDatatypeProp(customClass, "length", 2.4);
    SetGetDatatypeProp(customClass, "MyPropery", "my valye");

    CloseModel(model);

}