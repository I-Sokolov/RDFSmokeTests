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

static void Rotation90about111(GEOM::Matrix T)
{
    double inv_sqrt3 = 1.0 / sqrt(3.0);
    double c = 0.0;  // cos(90°)
    double s = 1.0;  // sin(90°)
    double x = inv_sqrt3, y = inv_sqrt3, z = inv_sqrt3;

    T.set__11( x * x * (1 - c) + c);
    T.set__12( x * y * (1 - c) - z * s);
    T.set__13( x * z * (1 - c) + y * s);

    T.set__21(y * x * (1 - c) + z * s);
    T.set__22(y * y * (1 - c) + c);
    T.set__23(y * z * (1 - c) - x * s);

    T.set__31(z * x * (1 - c) - y * s);
    T.set__32(z * y * (1 - c) + x * s);
    T.set__33(z * z * (1 - c) + c);
}

static void RotationZ30(GEOM::Matrix T)
{
    double angle = 3.1415 / 6.0;  // 30 degrees
    double c = cos(angle);
    double s = sin(angle);

    double t[12] = {
         c, -s, 0 ,
         s,  c, 0 ,
         0,  0, 1 ,
         0,  0, 3
    };

    T.set_coordinates(t, 12);
}

static void RepetitionBBox()
{
    ENTER_TEST;

    auto model = CreateModel();

    auto cone = GEOM::Cone::Create(model);
    cone.set_radius(3);
    cone.set_height(5);

    auto Tcone = GEOM::Matrix::Create(model);
    Rotation90about111(Tcone);
    Tcone.set__11(0.5 * *Tcone.get__11());
    Tcone.set__12(0.5 * *Tcone.get__12());
    Tcone.set__13(0.5 * *Tcone.get__13());
    Tcone.set__41(10);

    auto transform = GEOM::Transformation::Create(model);
    transform.set_object(cone);
    transform.set_matrix(Tcone);
    
    auto Trep = GEOM::Matrix::Create(model);
    RotationZ30(Trep);

    auto repitition = GEOM::Repetition::Create(model);
    repitition.set_object(transform);
    repitition.set_matrix(Trep);
    repitition.set_count(16);

    CalculateInstance(repitition);
    
    double box[6] = { 0,0,0,0,0,0 };
    GetBoundingBox(repitition, box, box + 3);

    double boxe[6] = { -13.231992010750275, -13.232021423358674, -2.0980762113533160, 13.232050807568879, 13.231962569743747, 48.764742878019980 };
    ASSERT_ARR_EQ(box, boxe, 6);

    //SaveModel(model, "..\\output\\Repit.bin");
    CloseModel(model);
}

static void CreateExternalRefs(OwlModel& model1, OwlModel& model2, OwlInstance& referenced)
{
    model1 = CreateModel();
    model2 = CreateModel();

    auto cube = GEOM::Cube::Create(model1);
    referenced = cube;
    cube.set_length(5);

    auto matrix = GEOM::Matrix::Create(model1);
    matrix.set__43(6);

    auto transform = GEOM::Transformation::Create(model2);
    transform.set_object(cube);
    transform.set_matrix(matrix);

    CalculateInstance(transform);

    double boxe[6] = { 0,0,6,5,5,11 };
    double box[6] = { 0,0,0,0,0,0 };
    GetBoundingBox(transform, box, box + 3);
    ASSERT_ARR_EQ(box, boxe, 6);
}

static void CrossModelReferences()
{
    ENTER_TEST;

    OwlModel model1, model2;
    OwlInstance referenced;
    CreateExternalRefs(model1, model2, referenced);

    auto removed = RemoveInstance(referenced);
    ASSERT(removed == 99);

    CloseModel(model2);

    removed = RemoveInstance(referenced);
    ASSERT(removed == 0);

    CloseModel(model1);

    //close reverse order
    CreateExternalRefs(model1, model2, referenced);
    CloseModel(model1);
    CloseModel(model2);
}


extern void VariousTests()
{
    CrossModelReferences();
    ThingIsParent();
    SetParentUpdatesBackLinks();
    RemovePropertyTest();
    RepetitionBBox();
}