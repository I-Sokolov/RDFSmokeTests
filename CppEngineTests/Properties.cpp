#include "pch.h"

const char* test[] = { "Test string 1", "Test string 2" };
const wchar_t* wtest[] = { L"Test string 1", L"Test string 2" };

static void CheckTextProps(int64_t model)
{
    auto inst = GetInstancesByIterator(model, NULL);

    auto propString = GetPropertyByName(model, "String");
    auto propChar   = GetPropertyByName(model, "Char");
    auto propWChar  = GetPropertyByName(model, "WChar");

    int64_t saveEncoding = 0;
    bool saveAscii = false;
    int64_t saveCharSize = GetCharacterSerialization(model, &saveEncoding, &saveAscii);


    int64_t cnt = 0;
    const char** chValues = nullptr;
    const wchar_t** wchValues = nullptr;

    //
    SetCharacterSerialization(model, 32, 0, true);
    
    GetDataTypeProperty(inst, propString, (const void**)&chValues, &cnt);
    ASSERT(cnt == 2 && !strcmp(chValues[0], test[0]) && !strcmp(chValues[1], test[1]));

    GetDataTypeProperty(inst, propChar, (const void**)&chValues, &cnt);
    ASSERT(cnt == 2 && !strcmp(chValues[0], test[0]) && !strcmp(chValues[1], test[1]));

    GetDataTypeProperty(inst, propWChar, (const void**)&wchValues, &cnt);
    ASSERT(cnt == 2 && !wcscmp(wchValues[0], wtest[0]) && !wcscmp(wchValues[1], wtest[1]));

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

static void TestTextProps(int startAscii)
{
    int64_t model = CreateModel();

    auto propString = CreateProperty(model, DATATYPEPROPERTY_TYPE_STRING, "String");
    auto propChar = CreateProperty(model, DATATYPEPROPERTY_TYPE_CHAR_ARRAY, "Char");
    auto propWChar = CreateProperty(model, DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY, "WChar");

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

    CheckTextProps(model);

    const char* files[] = { "TestTextProp.bin" }; //.ttl, .rdf, .bct not implements strings 
    
    for (auto file : files) {
        SaveModel(model, file);
    }

    CloseModel(model);

    for (auto file : files) {
        model = OpenModel(file);
        CheckTextProps(model);
        CloseModel(model);
    }
}

static void TestTextProps()
{
    ENTER_TEST;
    
    TestTextProps(0);
    TestTextProps(-1);
    TestTextProps(1);
}

extern void Test_Properties()
{
    TestTextProps();
}
