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


extern void BackLinksTests()
{
    SetParentUpdatesBackLinks();
}