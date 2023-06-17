#include "pch.h"

template <typename T> bool IsEqual(T a, T b) { return a == b; }
static bool IsEqual(const char* a, const char* b) { return 0==strcmp(a,b); }

template <typename T> void SetGetDatatypeProp(OwlClass cls, const char* propName, T v)
{
    auto model = GetModel(cls);

    auto prop = GetPropertyByName(model, propName);
    ASSERT(prop);
    ASSERT(IsProperty(prop));
    ASSERT(model == GetModel(prop));

    int64_t card = 0;
    T* val;
    int64_t res = GetDataTypeProperty(cls, prop, (const void**)&val, &card);
    ASSERT(card == 0 && res == 0);

    res = SetDatatypeProperty(cls, prop, v);
    ASSERT(res == 0);

    res = GetDataTypeProperty(cls, prop, (const void**)&val, &card);
    ASSERT(card == 1 && IsEqual (val[0],v) && res == 0);
}

static void SetGetObjecttypeProp(OwlClass cls, const char* propName, OwlInstance value)
{
    auto model = GetModel(cls);
    auto prop = GetPropertyByName(model, propName);
    ASSERT(prop);
    ASSERT(IsProperty(prop));
    ASSERT(model == GetModel(prop));

    int64_t card = 0;
    const OwlInstance* val = NULL;
    int64_t res = GetObjectTypeProperty(cls, prop, &val, &card);
    ASSERT(card == 0 && res == 0);

    res = SetObjectTypeProperty(cls, prop, &value, 1);
    ASSERT(res == 0);

    res = GetObjectTypeProperty(cls, prop, &val, &card);
    ASSERT(card == 1 && IsEqual(val[0], value) && res == 0);
}


extern void MetaPropertiesTest()
{
    ENTER_TEST;

    OwlModel model = OpenModel(NULL);
    ASSERT(model);
    ASSERT(IsModel(model));
    ASSERT(!IsClass(model));
    ASSERT(!IsInstance(model));
    ASSERT(!IsProperty(model));
    ASSERT(model == GetModel(model));

    CreateProperty(model, DATATYPEPROPERTY_TYPE_CHAR, "MyPropery");
    CreateProperty(model, OBJECTTYPEPROPERTY_TYPE, "MyObjectPropery");

    auto internalClass = GetClassByName(model, "Box");
    ASSERT(internalClass);
    ASSERT(IsClass(internalClass));
    ASSERT(!IsModel(internalClass));
    ASSERT(model == GetModel(internalClass));

    auto inst = GEOM::Material::Create(model);
    ASSERT(IsInstance(inst));
    ASSERT(model == GetModel(inst));

    SetGetDatatypeProp(internalClass, "length", 2.4);
    SetGetDatatypeProp(internalClass, "MyPropery", "my valye");

    SetGetObjecttypeProp(internalClass, "object", inst);
    SetGetObjecttypeProp(internalClass, "MyObjectPropery", inst);

    auto customClass = CreateClass(model, "ClassA");
    SetClassParent(customClass, internalClass, 1);

    SetGetDatatypeProp(customClass, "length", 2.4);
    SetGetDatatypeProp(customClass, "MyPropery", "my valye");

    SetGetObjecttypeProp(customClass, "object", inst);
    SetGetObjecttypeProp(customClass, "MyObjectPropery", inst);

    CloseModel(model);

}