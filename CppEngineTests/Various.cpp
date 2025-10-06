#include "pch.h"

static void SetParentUpdatesBackLinks()
{
    ENTER_TEST;

    auto model = CreateModel();

    auto material = GEOM::Material::Create(model);
    auto cube = GEOM::Cube::Create(model);

    cube.set_material(material);

    auto clsCube = GetClassByName(model, "Cube");
    ASSERT(clsCube);

    auto myCls = CreateClass(model, "MyGreatClass");
    
    SetClassParent(clsCube, myCls);

    //
    auto res = RemoveInstance(material);
    ASSERT(res == 99);

    res = RemoveInstance(cube);
    ASSERT(res == 0);

    res = RemoveInstance(material);
    ASSERT(res == 0);

    CloseModel(model);
}

static void RemovePropertyTest()
{
    ENTER_TEST;

    auto model = CreateModel();

    auto length = GetPropertyByName(model, "length");
    auto material = GetPropertyByName(model, "material");
    ASSERT(length && material);

    auto res = RemoveProperty(length);
    ASSERT(res == 1);
    res = RemoveProperty(material);
    ASSERT(res == 1);

    const char* DPROP = "DProp";
    const char* OPROP = "OProp";

    auto dprop = CreateProperty(model, DATATYPEPROPERTY_TYPE_DOUBLE, DPROP);
    auto oprop = CreateProperty(model, OBJECTPROPERTY_TYPE, OPROP);

    ASSERT(GetPropertyByName(model, DPROP) && GetPropertyByName(model, OPROP));

    auto box = GetClassByName(model, "Box");
    auto box1 = CreateInstance(box);
    auto box2 = CreateInstance(box);

    SetObjectProperty(box1, oprop, box2);
    SetDatatypeProperty(box1, dprop, 22.2);

    res = RemoveProperty(dprop);
    ASSERT(res == 3);
    res = RemoveProperty(oprop);
    ASSERT(res == 3);

    SetObjectProperty(box1, oprop, NULL, 0);
    SetDataTypeProperty(box1, dprop, NULL, 0);

    ASSERT(GetPropertyByName(model, DPROP) && GetPropertyByName(model, OPROP));

    res = RemoveProperty(dprop);
    ASSERT(res == 0);
    res = RemoveProperty(oprop);
    ASSERT(res == 0);

    ASSERT(!GetPropertyByName(model, DPROP) && !GetPropertyByName(model, OPROP));

    res = RemoveProperty(box);
    ASSERT(res == 1);

    ASSERT(GetPropertyByName(model, "length") && GetPropertyByName(model, "material"));

    CloseModel(model);
}

static void AssertOneParent(OwlClass cls, const char* name)
{
    auto parent = GetClassParentsByIterator(cls, NULL);
    ASSERT(parent);

    auto n = GetNameOfClass(parent);
    ASSERT(0 == strcmp(n, name));

    parent = GetClassParentsByIterator(cls, parent);
    ASSERT(!parent);
}

static void ThingIsParent(OwlClass cls)
{
    AssertOneParent(cls, "Thing");

    auto box = GetClassByName(GetModel(cls), "Box");
    SetClassParent(cls, box);
    AssertOneParent(cls, "Box");

    UnsetClassParent(cls, box);
    AssertOneParent(cls, "Thing");
}

static void ThingIsParent()
{
    ENTER_TEST;

    auto model = CreateModel();

    auto cls = CreateClass(model);
    ThingIsParent(cls);

    cls = CreateClass(model, "MyClass");
    ThingIsParent(cls);

    CloseModel(model);
}

extern void VariousTests()
{
    ThingIsParent();
    SetParentUpdatesBackLinks();
    RemovePropertyTest();
}