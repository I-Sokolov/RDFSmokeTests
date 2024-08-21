#include "pch.h"

extern void ADBcreate()
{
    ENTER_TEST;
    
    //setStringUnicode(1);

    int_t model = sdaiCreateModelBN(0, nullptr, "IFC4x1");

    for (int_t i = 0; i < 100000; i++) {
        void* value = sdaiCreateADB(sdaiSTRING, (void*)"bar");
        sdaiPutADBTypePath(value, 1, "IFCTEXT");
        int_t prop = sdaiCreateInstanceBN(model, "IFCPROPERTYSINGLEVALUE");
        sdaiPutAttrBN(prop, "Name", sdaiUNICODE, (void*)L"foo");
        sdaiPutAttrBN(prop, "NominalValue", sdaiADB, value);
    }

   // std::string filename = "e:\\Downloads\\foo.ifc";
   // sdaiSaveModelBN(model, filename.c_str());

    sdaiCloseModel(model);

}