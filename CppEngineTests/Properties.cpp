#include "pch.h"

const char* test[] = { "Test string 1", "Test string 2" };
const wchar_t* wtest[] = { L"Test string 1", L"Test string 2" };
const bool testBool[] = { true, false };
const int64_t testInt[] = { 1234567890, -9876543210 };
const double testDouble[] = { 3.14159, 2.71828 };
const unsigned char testByte[] = { 0x12, 0x34};


static void CheckDatatypeProps(int64_t model)
{
    auto inst = GetInstancesByIterator(model, NULL);

    auto propString = GetPropertyByName(model, "String");
    auto propChar   = GetPropertyByName(model, "Char");
    auto propWChar  = GetPropertyByName(model, "WChar");
    auto propBool = GetPropertyByName(model, "Bool");
    auto propInt = GetPropertyByName(model, "Int");
    auto propDouble = GetPropertyByName(model, "Double");
    auto propByte = GetPropertyByName(model, "Byte");

    int64_t saveEncoding = 0;
    bool saveAscii = false;
    int64_t saveCharSize = GetCharacterSerialization(model, &saveEncoding, &saveAscii);


    int64_t cnt = 0;
    const char** chValues = nullptr;
    const wchar_t** wchValues = nullptr;
    const bool* boolValues = nullptr;
    const int64_t* intValues = nullptr;
    const double* doubleValues = nullptr;
    const unsigned char* byteValues = nullptr;

    //
    SetCharacterSerialization(model, 32, 0, true);
    
    GetDataTypeProperty(inst, propString, (const void**)&chValues, &cnt);
    ASSERT(cnt == 2 && !strcmp(chValues[0], test[0]) && !strcmp(chValues[1], test[1]));

    GetDataTypeProperty(inst, propChar, (const void**)&chValues, &cnt);
    ASSERT(cnt == 2 && !strcmp(chValues[0], test[0]) && !strcmp(chValues[1], test[1]));

    GetDataTypeProperty(inst, propWChar, (const void**)&wchValues, &cnt);
    ASSERT(cnt == 2 && !wcscmp(wchValues[0], wtest[0]) && !wcscmp(wchValues[1], wtest[1]));

    GetDataTypeProperty(inst, propBool, (const void**)&boolValues, &cnt);
    ASSERT_ARR_EQ(boolValues, testBool, 2);

    GetDataTypeProperty(inst, propInt, (const void**)&intValues, &cnt);
    ASSERT_ARR_EQ(intValues, testInt, 2);

    GetDataTypeProperty(inst, propDouble, (const void**)&doubleValues, &cnt);
    ASSERT_ARR_EQ(doubleValues, testDouble, 2);

    GetDataTypeProperty(inst, propByte, (const void**)&byteValues, &cnt);
    ASSERT_ARR_EQ(byteValues, testByte, 2);

    //
    SetCharacterSerialization(model, 32, 0, false);

    GetDataTypeProperty(inst, propString, (const void**)&wchValues, &cnt);
    ASSERT(cnt == 2 && !wcscmp(wchValues[0], wtest[0]) && !wcscmp(wchValues[1], wtest[1]));

    GetDataTypeProperty(inst, propChar, (const void**)&chValues, &cnt);
    ASSERT(cnt == 2 && !strcmp(chValues[0], test[0]) && !strcmp(chValues[1], test[1]));

    GetDataTypeProperty(inst, propWChar, (const void**)&wchValues, &cnt);
    ASSERT(cnt == 2 && !wcscmp(wchValues[0], wtest[0]) && !wcscmp(wchValues[1], wtest[1]));

    //
    SetCharacterSerialization(model, saveEncoding, 0, saveAscii);
}

static void TestDatatypeProps(int startAscii)
{
    int64_t model = CreateModel();

    auto propString = CreateProperty(model, DATATYPEPROPERTY_TYPE_STRING, "String");
    auto propChar = CreateProperty(model, DATATYPEPROPERTY_TYPE_CHAR_ARRAY, "Char");
    auto propWChar = CreateProperty(model, DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY, "WChar");
    auto propBool = CreateProperty(model, DATATYPEPROPERTY_TYPE_BOOLEAN, "Bool");
    auto propInt = CreateProperty(model, DATATYPEPROPERTY_TYPE_INTEGER, "Int");
    auto propDouble = CreateProperty(model, DATATYPEPROPERTY_TYPE_DOUBLE, "Double");
    auto propByte = CreateProperty(model, DATATYPEPROPERTY_TYPE_BYTE, "Byte");

    if (startAscii > 0) {
        SetCharacterSerialization(model, 32, 0, true);
    }
    else if (startAscii < 0) {
        SetCharacterSerialization(model, 32, 0, false);
    }

    bool ascii = false;
    GetCharacterSerialization(model, NULL, &ascii);

    auto inst = GEOM::Sphere::Create(model);

    if (ascii) {
        SetDatatypeProperty(inst, propString, test, 2);
    }
    else {
        SetDataTypeProperty(inst, propString, wtest, 2);
    }

    SetDatatypeProperty(inst, propChar, test, 2);
    SetDatatypeProperty(inst, propWChar, wtest, 2);

    SetDataTypeProperty(inst, propBool, testBool, 2);
    SetDataTypeProperty(inst, propInt, testInt, 2);
    SetDataTypeProperty(inst, propDouble, testDouble, 2);
    SetDataTypeProperty(inst, propByte, testByte, 2);

    CheckDatatypeProps(model);

    const char* files[] = { "TestTextProp.bin" }; //.ttl, .rdf, .bct not implements strings 
    
    for (auto file : files) {
        SaveModel(model, file);
    }

    CloseModel(model);

    for (auto file : files) {
        model = OpenModel(file);
        CheckDatatypeProps(model);
        CloseModel(model);
    }
}

static void TestDatatypeProps()
{
    ENTER_TEST;
    
    TestDatatypeProps(0);
    TestDatatypeProps(-1);
    TestDatatypeProps(1);
}

static void TestDerivedProps()
{
    ENTER_TEST;
    int64_t model = CreateModel();

    auto sphere = GEOM::Sphere::Create(model);
    auto poly = GEOM::PolyLine3D::Create(model);
    auto mat = GEOM::Material::Create(model);
    poly.set_material(mat);

    //
    // Test set-unset derived
    //
    auto propRadius = CreateProperty(model, DATATYPEPROPERTY_TYPE_INTEGER, "My_radius");
    auto propMat = CreateProperty(model, OBJECTPROPERTY_TYPE, "My_material");
    auto propLength = GetPropertyByName(model, "length"); 
    auto propObject = GetPropertyByName(model, "object");
    auto propMaterial = GetPropertyByName(model, "material");

    ASSERT(!GetPropertyDerived(sphere, propRadius));
    ASSERT(!GetPropertyDerived(sphere, propMat));
    ASSERT(!GetPropertyDerived(sphere, propLength));
    ASSERT(!GetPropertyDerived(sphere, propObject));
    ASSERT(!GetPropertyDerived(poly, propMaterial));

    int_t rad = 1;
    RdfsResource matRes = mat;

    SetDatatypeProperty(sphere, propRadius, &rad, 1);
    SetObjectProperty(sphere, propMat, &matRes, 1);

    SetPropertyDerived(sphere, propRadius, true);
    SetPropertyDerived(sphere, propMat,    true);
    SetPropertyDerived(sphere, propLength, true);
    SetPropertyDerived(sphere, propObject, true);

    ASSERT(GetPropertyDerived(sphere, propRadius));
    ASSERT(GetPropertyDerived(sphere, propMat));
    ASSERT(GetPropertyDerived(sphere, propLength));
    ASSERT(GetPropertyDerived(sphere, propObject));

    auto sphereCls = GetInstanceClass(sphere);
    auto myCls = CreateClass(model, "MyClass");
    SetClassParent(sphereCls, myCls);

    ASSERT(GetPropertyDerived(sphere, propRadius));
    ASSERT(GetPropertyDerived(sphere, propMat));
    ASSERT(GetPropertyDerived(sphere, propLength));
    ASSERT(GetPropertyDerived(sphere, propObject));

    SetPropertyDerived(sphere, propRadius, false);
    SetPropertyDerived(sphere, propMat,    false);
    SetPropertyDerived(sphere, propLength, false);
    SetPropertyDerived(sphere, propObject, false);

    UnsetClassParent(sphereCls, myCls);

    ASSERT(!GetPropertyDerived(sphere, propRadius));
    ASSERT(!GetPropertyDerived(sphere, propMat));
    ASSERT(!GetPropertyDerived(sphere, propLength));
    ASSERT(!GetPropertyDerived(sphere, propObject));

    SetPropertyDerived(sphere, propRadius, true);
    SetPropertyDerived(sphere, propMat,    true);
    SetPropertyDerived(sphere, propLength, true);
    SetPropertyDerived(sphere, propObject, true);

    ASSERT(GetPropertyDerived(sphere, propRadius));
    ASSERT(GetPropertyDerived(sphere, propMat));
    ASSERT(GetPropertyDerived(sphere, propLength));
    ASSERT(GetPropertyDerived(sphere, propObject));

    //
    // Test calculate derived
    //
    double points[] = { 0,0,0,1,0,0,1,1,0 };
    poly.set_points(points, 9);

    //
    CalculateInstance(poly);
    
    //calculated by default
    auto len = poly.get_length();

    ASSERT(len && *len == 2);
    ASSERT(GetPropertyDerived(poly, propLength));

    //restoring when set
    poly.set_length(7);
    poly.set_points(points, 9);

    CalculateInstance(poly);

    len = poly.get_length();

    ASSERT(len && *len == 2);
    ASSERT(GetPropertyDerived(poly, propLength));

    //
    SetPropertyDerived(poly, propLength, false);
    poly.set_length(7);
 
    CalculateInstance(poly);

    len = poly.get_length();

    ASSERT(len && *len == 7);
    ASSERT(!GetPropertyDerived(poly, propLength));

    //set derived

    //reset
    SetDatatypeProperty(poly, propLength, NULL, 0);
    SetPropertyDerived(poly, propLength, false);

    CalculateInstance(poly);

    len = poly.get_length();
    ASSERT(!len);
    ASSERT(!GetPropertyDerived(poly, propLength));

    SaveModel(model, "TestDerivedProps.bin");

    CloseModel(model);

    //
    // read and check state
    //
    model = OpenModel("TestDerivedProps.bin");

    OwlInstance inst = NULL;
    while (inst = GetInstancesByIterator(model, inst)) {
        auto cls = GetInstanceClass(inst);
        auto clsName = GetNameOfClass(cls);
        if (0 == strcmp(clsName, "Sphere")) {
            sphere = inst;
        }
        else if (0 == strcmp(clsName, "PolyLine3D")) {
            poly = inst;
        }
        else if (0 == strcmp(clsName, "Material")) {
            mat = inst;
        }
    }
    ASSERT(sphere && poly && mat);

    propRadius = GetPropertyByName(model, "My_radius");
    propMat = GetPropertyByName(model, "My_material");
    propLength = GetPropertyByName(model, "length");
    propObject = GetPropertyByName(model, "object");
    propMaterial = GetPropertyByName(model, "material");

    ASSERT(GetPropertyDerived(sphere, propRadius));
    ASSERT(GetPropertyDerived(sphere, propMat));
    ASSERT(GetPropertyDerived(sphere, propLength));
    ASSERT(GetPropertyDerived(sphere, propObject));

    ASSERT(GetPropertyDerived(poly, propLength));

    ASSERT(poly.get_material() && *poly.get_material() == mat);
    ASSERT(propMaterial);
    ASSERT(!GetPropertyDerived(poly, propMaterial));

    CloseModel(model);
}

static void TestDefaultValues()
{
    ENTER_TEST;

    int64_t model = CreateModel();

    auto propLength = GetPropertyByName(model, "length");
    auto propObj = GetPropertyByName(model, "objects");
    auto propDer = CreateProperty(model, DATATYPEPROPERTY_TYPE_DOUBLE, "DerProp");
    auto prop2 = CreateProperty(model, DATATYPEPROPERTY_TYPE_DOUBLE, "Prop2");

    auto clsCube = GetClassByName(model, "Cube");
    auto clsGItem = GetClassByName(model, "GeometricItem");
    auto clsCollection = GetClassByName(model, "Collection");

    //default data values
    SetDatatypeProperty(clsCube, propLength, 7.0);
    SetPropertyDerived(clsGItem, propDer, true);

    auto cube = GEOM::Cube::Create(model);

    auto len = cube.get_length();
    ASSERT(len && *len == 7);
    ASSERT(GetPropertyDerived(cube, propDer));

    //default object value
    SetObjectProperty(clsCollection, propObj, cube);

    auto coll = GEOM::Collection::Create(model);

    int64_t crd = 0;
    auto objs = coll.get_objects(&crd);
    ASSERT(objs && crd == 1 && *objs == cube);

    CalculateInstance(coll);

    double box[6] = { 0,0,0,0,0,0 };
    GetBoundingBox(coll, box, box + 3);

    double boxe[6] = { 0,0,0,7,7,7 };
    ASSERT_ARR_EQ(box, boxe, 6);

    //from set parent
    auto parent = CreateClass(model, "parent");
    SetDatatypeProperty(parent, propLength, 10.0);
    SetDatatypeProperty(parent, prop2, 10.0);
    SetDatatypeProperty(parent, propDer, 10.0);

    SetClassParent(clsGItem, parent);

    double* getVal = NULL;
    GetDatatypeProperty(cube, propLength, (void**) &getVal, &crd);
    ASSERT(crd == 1 && *getVal == 7); //not changed
    ASSERT(!GetPropertyDerived(cube, propLength));

    GetDataTypeProperty(cube, prop2, (void**)&getVal, &crd);
    ASSERT(crd == 1 && *getVal == 10); //set default
    ASSERT(!GetPropertyDerived(cube, prop2));

    GetDataTypeProperty(cube, propDer, (void**)&getVal, &crd);
    ASSERT(crd == 0); //not changed
    ASSERT(GetPropertyDerived(cube, propDer));

    CloseModel(model);
}

extern void Test_Properties()
{
    TestDefaultValues();
    TestDerivedProps();
    TestDatatypeProps();
}
