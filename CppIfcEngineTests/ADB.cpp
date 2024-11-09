#include "pch.h"

static void TestCreate()
{
    ENTER_TEST;
    
    //setStringUnicode(1);

    int_t model = sdaiCreateModelBN(0, nullptr, "IFC4x1");

    for (int_t i = 0; i < 100000; i++) {
        void* value = sdaiCreateADB(sdaiSTRING, "bar");
        sdaiPutADBTypePath(value, 1, "IFCTEXT");
        int_t prop = sdaiCreateInstanceBN(model, "IFCPROPERTYSINGLEVALUE");
        sdaiPutAttrBN(prop, "Name", sdaiUNICODE, L"foo");
        sdaiPutAttrBN(prop, "NominalValue", sdaiADB, value);
    }

   // std::string filename = "e:\\Downloads\\foo.ifc";
   // sdaiSaveModelBN(model, filename.c_str());

    sdaiCloseModel(model);

}


static void ShareOrCopyABD()
{
    ENTER_TEST;

    auto model = sdaiCreateModelBN(0, nullptr, "IFC2x3");

    //get returns modificable value (we can change type), but put copied it to target
    //I'm not sure it is exact intention, the test is just to pin the behavior and alam if changed


    auto val1 = IFC2x3::IfcPropertySingleValue::Create(model);
    auto val2 = IFC2x3::IfcPropertySingleValue::Create(model);

    val1.put_NominalValue().put_IfcSimpleValue().put_IfcText("Text1");
    
    const char* attrName = "NominalValue";

    SdaiADB adb = NULL;
    sdaiGetAttrBN(val1, attrName, sdaiADB, &adb);

    sdaiPutAttrBN(val2, attrName, sdaiADB, adb);

    sdaiPutADBTypePath(adb, 1, "IFCLABEL");

    //
    sdaiGetAttrBN(val1, attrName, sdaiADB, &adb);
    auto typePath = sdaiGetADBTypePath(adb, 1);
    ASSERT(0 == strcmp(typePath, "IFCLABEL"));

    sdaiGetAttrBN(val2, attrName, sdaiADB, &adb);
    typePath = sdaiGetADBTypePath(adb, 1);
    ASSERT(0 == strcmp(typePath, "IFCTEXT"));

    adb = NULL;
    sdaiGetAttrBN(val1, attrName, sdaiADB, &adb);
    typePath = sdaiGetADBTypePath(adb, 1);
    ASSERT(0 == strcmp(typePath, "IFCLABEL"));

    adb = NULL;
    sdaiGetAttrBN(val2, attrName, sdaiADB, &adb);
    typePath = sdaiGetADBTypePath(adb, 1);
    ASSERT(0 == strcmp(typePath, "IFCTEXT"));

    sdaiCloseModel(model);
}


extern void ADBtests()
{
    TestCreate();
    ShareOrCopyABD();
}
