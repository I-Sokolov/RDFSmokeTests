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

extern void Test_Properties()
{
    TestDatatypeProps();
}
