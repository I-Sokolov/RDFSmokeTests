#include "pch.h"

using namespace IFC4;

static void TestADBBoolean(SdaiInstance inst, const char* attr)
{
    //get $
    SdaiADB adb = nullptr;
    auto ret = sdaiGetAttrBN(inst, attr, sdaiADB, &adb);
    ASSERT(!ret && !adb);

    //variants
    adb = sdaiCreateEmptyADB();
    ret = sdaiGetAttrBN(inst, attr, sdaiADB, adb);
    ASSERT(!ret && !sdaiGetADBType(adb));
    sdaiDeleteADB(adb);

    adb = sdaiCreateEmptyADB();
    ret = sdaiGetAttrBN(inst, attr, sdaiADB, &adb);
    ASSERT(!ret && !sdaiGetADBType(adb));
    sdaiDeleteADB(adb);

    adb = sdaiCreateADB(sdaiSTRING, "2");
    ret = sdaiGetAttrBN(inst, attr, sdaiADB, adb);
    ASSERT(!ret && !sdaiGetADBType(adb));
    sdaiDeleteADB(adb);

    adb = sdaiCreateADB(sdaiSTRING, "2");
    ret = sdaiGetAttrBN(inst, attr, sdaiADB, &adb);
    ASSERT(!ret && !sdaiGetADBType(adb));
    sdaiDeleteADB(adb);


    //put
    bool bVal = true;
    adb = sdaiCreateADB(sdaiBOOLEAN, &bVal);
    sdaiPutADBTypePath(adb, 1, "IFCBOOLEAN");
    sdaiPutAttrBN(inst, attr, sdaiADB, adb);
    sdaiDeleteADB(adb);
    adb = NULL;

    ret = sdaiGetAttrBN(inst, attr, sdaiADB, &adb);
    ASSERT(ret==adb && adb);

    ret = sdaiGetADBValue(adb, sdaiBOOLEAN, &bVal);
    ASSERT(ret==&bVal && bVal);

    //you can also get it as logical
    const char* logVal = NULL;
    ret = sdaiGetADBValue(adb, sdaiLOGICAL, &logVal);
    ASSERT(ret==&logVal && 0 == strcmp(logVal, "T"));

    sdaiDeleteADB(adb);

    //variants
    adb = sdaiCreateEmptyADB();
    ret = sdaiGetAttrBN(inst, attr, sdaiADB, adb);
    ASSERT(ret == adb && sdaiGetADBType(adb) == sdaiBOOLEAN);
    sdaiDeleteADB(adb);

    adb = sdaiCreateEmptyADB();
    ret = sdaiGetAttrBN(inst, attr, sdaiADB, &adb);
    ASSERT(ret == adb && sdaiGetADBType(adb) == sdaiBOOLEAN);
    sdaiDeleteADB(adb);

    adb = sdaiCreateADB(sdaiSTRING, "2");
    ret = sdaiGetAttrBN(inst, attr, sdaiADB, adb);
    ASSERT(ret == adb && sdaiGetADBType(adb) == sdaiBOOLEAN);
    sdaiDeleteADB(adb);

    adb = sdaiCreateADB(sdaiSTRING, "2");
    ret = sdaiGetAttrBN(inst, attr, sdaiADB, &adb);
    ASSERT(ret == adb && sdaiGetADBType(adb) == sdaiBOOLEAN);
    auto typePath = sdaiGetADBTypePath(adb, sdaiSTRING);
    ASSERT(!strcmp(typePath, "IFCBOOLEAN"));
    sdaiDeleteADB(adb);

    //
    adb = NULL;

    //replace select value with primitive value
    // 
    //put double
    double dVal = 5.4;
    sdaiPutAttrBN(inst, attr, sdaiREAL, &dVal);
    
    ret = sdaiGetAttrBN(inst, attr, sdaiADB, &adb);
    ASSERT(ret == adb && adb);

    //value is REAL now
    ret = sdaiGetADBValue(adb, sdaiREAL, &dVal);
    ASSERT(ret == &dVal && dVal == 5.4);
    ret = sdaiGetADBValue(adb, sdaiBOOLEAN, &bVal);
    ASSERT(ret == NULL); 

    //and typePath is ""
    typePath = sdaiGetADBTypePath(adb, sdaiSTRING);
    ASSERT(!strcmp(typePath, ""));

    sdaiDeleteADB(adb);
    adb = NULL;
}

static void TestADBLogical(SdaiInstance inst, const char* attr)
{
    void* adb = sdaiCreateADB(sdaiLOGICAL, "U");
    sdaiPutADBTypePath(adb, 1, "IFCLOGICAL");
    sdaiPutAttrBN(inst, attr, sdaiADB, adb);
    sdaiDeleteADB(adb);
    
    auto ret = sdaiGetAttrBN(inst, attr, sdaiADB, &adb);
    ASSERT(ret && adb);

    const char* logVal = NULL;
    ret = sdaiGetADBValue(adb, sdaiLOGICAL, &logVal);
    ASSERT(ret && 0 == strcmp(logVal, "U"));

    sdaiDeleteADB(adb);
}

static void TestBinaries(SdaiModel ifcModel)
{
    ENTER_TEST;
    const int NC = 4;
    char rasterCode[NC * 1024 * 4 + 2];
    rasterCode[0] = '1';
    rasterCode[NC * 1024 * 4] = '8';
    rasterCode[NC * 1024 * 4 + 1] = 0;
    for (int i = 1; i < NC * 1024 * 4; i++) {
        rasterCode[i] = 'A' + i % 3;
    }

    //
    //scalar attribute
    auto blobTexture = IfcBlobTexture::Create(ifcModel);
    blobTexture.put_RepeatS(true);
    blobTexture.put_RepeatT(true);
    blobTexture.put_RasterFormat("PNG");
    blobTexture.put_Mode("MODULATE");

    ASSERT(blobTexture.get_RasterCode() == NULL);
    blobTexture.put_RasterCode(rasterCode);
    auto gotCode = blobTexture.get_RasterCode();
    ASSERT(0 == strcmp(gotCode, rasterCode));

    //put/get with SDAI
    sdaiPutAttrBN(blobTexture, "RasterCode", sdaiBINARY, rasterCode);
    IfcBinary gotData = NULL;
    auto res = sdaiGetAttrBN(blobTexture, "RasterCode", sdaiBINARY, &gotData);
    ASSERT(res == &gotData && !strcmp(gotData, rasterCode));

    //can also get as string
    sdaiGetAttrBN(blobTexture, "RasterCode", sdaiSTRING, &gotData);
    ASSERT(!strcmp(gotData, rasterCode));

    //
    //aggregation
    auto pixelTexture = IfcPixelTexture::Create(ifcModel);

    ListOfIfcBinary lstBin;
    pixelTexture.get_Pixel(lstBin);
    ASSERT(lstBin.size() == 0);

    lstBin.push_back(rasterCode);
    lstBin.push_back(rasterCode);

    pixelTexture.put_Pixel(lstBin);
    
    lstBin.clear();
    pixelTexture.get_Pixel(lstBin);
    ASSERT(lstBin.size() == 2 && !strcmp(lstBin.front(), rasterCode) && !strcmp(lstBin.back(), rasterCode));

    //
    //select
    auto value = IfcAppliedValue::Create(ifcModel);

    auto bin = value.get_AppliedValue().get_IfcValue().get_IfcSimpleValue().get_IfcBinary();
    ASSERT(bin == 0);

    value.put_AppliedValue().put_IfcValue().put_IfcSimpleValue().put_IfcBinary(rasterCode);
    bin = value.get_AppliedValue().get_IfcValue().get_IfcSimpleValue().get_IfcBinary();
    ASSERT(!strcmp(bin, rasterCode));

    //simplified form
    bin = value.get_AppliedValue().as_text();
    ASSERT(!strcmp(bin, rasterCode));

    //
    //save and read
    const char* FILE_NAME = "TestBinaries.ifc";

    sdaiSaveModelBN(ifcModel, FILE_NAME);

    //
    // Re-read
    //
    SdaiModel readModel = sdaiOpenModelBN(NULL, FILE_NAME, "IFC4");

    auto entityBlobTexture = sdaiGetEntity(readModel, "IfcBlobTexture");
    auto blobTextureAggr = xxxxGetEntityAndSubTypesExtent(readModel, entityBlobTexture);
    auto N = sdaiGetMemberCount(blobTextureAggr);
    ASSERT(N == 1);
    for (int_t i = 0; i < N; i++) {
        int_t inst = 0;
        auto ret = sdaiGetAggrByIndex(blobTextureAggr, i, sdaiINSTANCE, &inst);
        ASSERT(ret && (SdaiInstance)ret == inst);
        auto code = IfcBlobTexture(inst).get_RasterCode();
        ASSERT(0 == strcmp(code, rasterCode));
    }
    int_t inst = 0;
    auto ret = sdaiGetAggrByIndex(blobTextureAggr, N+1, sdaiINSTANCE, &inst);
    ASSERT(!ret && !inst);

    const char* argName = NULL;
    engiGetAttrNameByIndex(entityBlobTexture, 1, sdaiSTRING, &argName);
    ASSERT(0 == strcmp(argName, "RepeatT"));

    int_t argType;
    engiGetAttrTypeByIndex(entityBlobTexture, 1, &argType);
    ASSERT(argType == sdaiBOOLEAN);

    auto entityPixelTexture = sdaiGetEntity(readModel, "IfcPixelTexture");
    auto pixelTextureAggr = sdaiGetEntityExtent(readModel, entityPixelTexture);
    N = sdaiGetMemberCount(pixelTextureAggr);
    ASSERT(N == 1);
    for (int_t i = 0; i < N; i++) {
        int_t inst = 0;
        sdaiGetAggrByIndex(pixelTextureAggr, i, sdaiINSTANCE, &inst);
        ListOfIfcBinary lstBin2;
        IfcPixelTexture(inst).get_Pixel(lstBin2);
        ASSERT(lstBin2.size() == 2 && !strcmp(lstBin2.front(), rasterCode) && !strcmp(lstBin2.back(), rasterCode));
    }

    auto entityValue = sdaiGetEntity(readModel, "IfcAppliedValue");
    auto valueAggr = sdaiGetEntityExtent(readModel, entityValue);
    N = sdaiGetMemberCount(pixelTextureAggr);
    ASSERT(N == 1);
    for (int_t i = 0; i < N; i++) {
        int_t inst = 0;
        sdaiGetAggrByIndex(valueAggr, i, sdaiINSTANCE, &inst);
        auto v = IfcAppliedValue(inst).get_AppliedValue().get_IfcValue().get_IfcSimpleValue().get_IfcBinary();
        ASSERT(!strcmp(v, rasterCode));
    }

    sdaiCloseModel(readModel);
}

static void TestPutAttr(SdaiModel model)
{
    ENTER_TEST

    auto window = IfcWindow::Create(model);

    ASSERT(window.get_PredefinedType().IsNull());
    window.put_PredefinedType(IfcWindowTypeEnum::SKYLIGHT);
    ASSERT(window.get_PredefinedType().Value()==IfcWindowTypeEnum::SKYLIGHT);
    sdaiPutAttrBN(window, "PredefinedType", sdaiENUM, nullptr);
    ASSERT(window.get_PredefinedType().IsNull());

    ASSERT(window.get_OverallWidth().IsNull());
    window.put_OverallWidth(50);
    ASSERT(window.get_OverallWidth().Value() == 50);
    sdaiPutAttrBN(window, "OverallWidth", sdaiREAL, nullptr);
    ASSERT(window.get_OverallWidth().IsNull());

    //*/$
    auto unit = IfcSIUnit::Create(model);

    //
    IfcMeasureWithUnit measureWithUnit = IfcMeasureWithUnit::Create(model);
    
    //make complex with logical
    measureWithUnit.put_ValueComponent().put_IfcSimpleValue().put_IfcLogical(IfcLogical::True);
    ASSERT(measureWithUnit.get_ValueComponent().get_IfcSimpleValue().get_IfcLogical().Value() == IfcLogical::True);

    auto type = engiGetInstanceAttrTypeBN(measureWithUnit, "ValueComponent");
    ASSERT(type == sdaiADB);

    auto ifcMeasureWithUnit = sdaiGetEntity(model, "IfcMeasureWithUnit");
    type = engiGetAttrTypeBN(ifcMeasureWithUnit, "ValueComponent");
    ASSERT(type == sdaiADB);

    type = engiGetAttrTypeBN(measureWithUnit, "ValueComponent");
    ASSERT(type == sdaiADB);

    auto adb = sdaiCreateEmptyADB();        
    ASSERT(sdaiGetAttrBN(measureWithUnit, "ValueComponent", sdaiADB, &adb));

    type = sdaiGetADBType(adb);
    ASSERT(type == sdaiBOOLEAN); //.T. is recognized as boolean but you can get as logical also

    type = engiGetInstanceAttrTypeBN(window, "FillsVoids");
    ASSERT(type == 0);

    measureWithUnit = IfcMeasureWithUnit::Create(model);
    TestADBBoolean (measureWithUnit, "ValueComponent");
    TestADBLogical (measureWithUnit, "ValueComponent");
}

static void TestGetAttrType(SdaiModel ifcModel, const char* entityName, const char* attrName, int_t expected)
{
    SdaiEntity entity = sdaiGetEntity(ifcModel, entityName);
    auto attr = sdaiGetAttrDefinition(entity, attrName);
    auto type = engiGetAttrType(attr);
    ASSERT(type == expected);

    type = engiGetAttrTypeBN(entity, attrName);
    ASSERT(type == expected);
}

static void TestGetAttrType(SdaiModel ifcModel)
{
    ENTER_TEST

    TestGetAttrType(ifcModel, "IfcWall", "GlobalId", sdaiSTRING);
    TestGetAttrType(ifcModel, "IfcWall", "OwnerHistory", sdaiINSTANCE);
    TestGetAttrType(ifcModel, "IfcWall", "IsDecomposedBy", sdaiINSTANCE | engiTypeFlagAggr);
    TestGetAttrType(ifcModel, "IfcWall", "PredefinedType", sdaiENUM);
    TestGetAttrType(ifcModel, "IfcPolyLoop", "Polygon", sdaiINSTANCE | engiTypeFlagAggr);
    TestGetAttrType(ifcModel, "IfcPolygonalFaceSet", "PnIndex", sdaiINTEGER | engiTypeFlagAggr);
    TestGetAttrType(ifcModel, "IfcRelDefinesByProperties", "RelatingPropertyDefinition", sdaiINSTANCE | engiTypeFlagAggrOption);
    TestGetAttrType(ifcModel, "IfcMeasureWithUnit", "ValueComponent", sdaiADB);
    TestGetAttrType(ifcModel, "IfcFeatureElementAddition", "ProjectsElements", sdaiINSTANCE);
    TestGetAttrType(ifcModel, "IfcSite", "RefLatitude", sdaiINTEGER | engiTypeFlagAggr);
    TestGetAttrType(ifcModel, "IfcRationalBSplineCurveWithKnots", "WeightsData", sdaiREAL | engiTypeFlagAggr);
    TestGetAttrType(ifcModel, "IfcWindow", "FillsVoids", sdaiINSTANCE | engiTypeFlagAggr);
}


static void TestGetADBValue(SdaiModel ifcModel)
{
    ENTER_TEST
    ///
    auto instance = internalGetInstanceFromP21Line(ifcModel, 319);

    SdaiAggr listValues = 0;
    auto res = sdaiGetAttrBN(instance, "ListValues", sdaiAGGR, &listValues);
    ASSERT(res == listValues);

    auto count = sdaiGetMemberCount(listValues);
    ASSERT(count == 9);

    void* adbValue = NULL;
    int_t intV = 0;
    double doubleV = 0;
    const char* textV = 0;
    const wchar_t* wcV = nullptr;
    bool boolV = false;

    /// <summary>
    /// 0-string
    /// </summary>
    sdaiGetAggrByIndex(listValues, 0, sdaiADB, &adbValue);
    auto type = sdaiGetADBType(adbValue);
    ASSERT(type == sdaiSTRING);

    ASSERT(!sdaiGetADBValue(adbValue, sdaiAGGR, &intV));
    ASSERT(intV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiINSTANCE, &intV));
    ASSERT(intV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiLOGICAL, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBOOLEAN, &boolV));
    ASSERT(!boolV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiENUM, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBINARY, &textV));
    ASSERT(sdaiGetADBValue(adbValue, sdaiSTRING, &textV));
    ASSERT(!strcmp(textV, "List value 2"));
    ASSERT(sdaiGetADBValue(adbValue, sdaiEXPRESSSTRING, &textV));
    ASSERT(!strcmp(textV, "List value 2"));
    ASSERT(sdaiGetADBValue(adbValue, sdaiUNICODE, &wcV));
    ASSERT(!wcscmp(wcV, L"List value 2"));
    ASSERT(!sdaiGetADBValue(adbValue, sdaiREAL, &doubleV));
    ASSERT(doubleV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiINTEGER, &intV));
    ASSERT(intV == 0);

    /// <summary>
    /// 1-integer
    /// </summary>
    sdaiGetAggrByIndex(listValues, 1, sdaiADB, &adbValue);
    type = sdaiGetADBType(adbValue);
    ASSERT(type == sdaiINTEGER);

    ASSERT(!sdaiGetADBValue(adbValue, sdaiAGGR, &intV));
    ASSERT(intV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiINSTANCE, &intV));
    ASSERT(intV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiLOGICAL, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBOOLEAN, &boolV));
    ASSERT(!boolV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiENUM, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBINARY, &textV));
    ASSERT(!textV);
    ASSERT(sdaiGetADBValue(adbValue, sdaiSTRING, &textV));
    ASSERT(!strcmp(textV, "13"));
    ASSERT(sdaiGetADBValue(adbValue, sdaiEXPRESSSTRING, &textV));
    ASSERT(!strcmp(textV, "13"));
    ASSERT(sdaiGetADBValue(adbValue, sdaiUNICODE, &wcV));
    ASSERT(!wcscmp(wcV,L"13"));
    ASSERT(sdaiGetADBValue(adbValue, sdaiREAL, &doubleV));
    ASSERT(doubleV == 13);
    ASSERT(sdaiGetADBValue(adbValue, sdaiINTEGER, &intV));
    ASSERT(intV == 13);

    /// <summary>
    /// 2-real
    /// </summary>
    sdaiGetAggrByIndex(listValues, 2, sdaiADB, &adbValue);
    type = sdaiGetADBType(adbValue);
    ASSERT(type == sdaiREAL);

    ASSERT(!sdaiGetADBValue(adbValue, sdaiAGGR, &intV));
    ASSERT(intV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiINSTANCE, &intV));
    ASSERT(intV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiLOGICAL, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBOOLEAN, &boolV));
    ASSERT(!boolV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiENUM, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBINARY, &textV));
    ASSERT(!textV);
    ASSERT(sdaiGetADBValue(adbValue, sdaiSTRING, &textV));
    ASSERT(!strcmp(textV, "8.500000"));
    ASSERT(sdaiGetADBValue(adbValue, sdaiEXPRESSSTRING, &textV));
    ASSERT(!strcmp(textV, "8.500000"));
    ASSERT(sdaiGetADBValue(adbValue, sdaiUNICODE, &wcV));
    ASSERT(!wcscmp(wcV,L"8.500000"));
    ASSERT(sdaiGetADBValue(adbValue, sdaiREAL, &doubleV));
    ASSERT(doubleV == 8.5);
    ASSERT(sdaiGetADBValue(adbValue, sdaiINTEGER, &intV));
    ASSERT(intV == 8);

    /// <summary>
    /// 3-boolean
    /// </summary>
    sdaiGetAggrByIndex(listValues, 3, sdaiADB, &adbValue);
    type = sdaiGetADBType(adbValue);
    ASSERT(type == sdaiBOOLEAN);

    ASSERT(!sdaiGetADBValue(adbValue, sdaiAGGR, &intV));
    ASSERT(intV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiINSTANCE, &intV));
    ASSERT(intV == 0);
    ASSERT(sdaiGetADBValue(adbValue, sdaiLOGICAL, &textV));
    ASSERT(!strcmp(textV, "T"));
    ASSERT(sdaiGetADBValue(adbValue, sdaiBOOLEAN, &boolV));
    ASSERT(boolV);
    ASSERT(sdaiGetADBValue(adbValue, sdaiENUM, &textV));
    ASSERT(!strcmp(textV, "T"));
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBINARY, &textV));
    ASSERT(!textV);
    ASSERT(sdaiGetADBValue(adbValue, sdaiSTRING, &textV));
    ASSERT(!strcmp(textV, ".T."));
    ASSERT(sdaiGetADBValue(adbValue, sdaiEXPRESSSTRING, &textV));
    ASSERT(!strcmp(textV, ".T."));
    ASSERT(sdaiGetADBValue(adbValue, sdaiUNICODE, &wcV));
    ASSERT(!wcscmp(wcV, L".T."));
    ASSERT(!sdaiGetADBValue(adbValue, sdaiREAL, &doubleV));
    ASSERT(doubleV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiINTEGER, &intV));
    ASSERT(intV == 0);

    /// <summary>
    /// 4-logical
    /// </summary>
    sdaiGetAggrByIndex(listValues, 4, sdaiADB, &adbValue);
    type = sdaiGetADBType(adbValue);
    ASSERT(type == sdaiLOGICAL);

    ASSERT(!sdaiGetADBValue(adbValue, sdaiAGGR, &intV));
    ASSERT(intV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiINSTANCE, &intV));
    ASSERT(intV == 0);
    ASSERT(sdaiGetADBValue(adbValue, sdaiLOGICAL, &textV));
    ASSERT(!strcmp(textV, "U"));
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBOOLEAN, &boolV));
    ASSERT(!boolV);
    //
    ASSERT(sdaiGetADBValue(adbValue, sdaiENUM, &textV));
    ASSERT(!strcmp(textV, "U"));
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBINARY, &textV));
    ASSERT(!textV);
    ASSERT(sdaiGetADBValue(adbValue, sdaiSTRING, &textV));
    ASSERT(!strcmp(textV, ".U."));
    ASSERT(sdaiGetADBValue(adbValue, sdaiEXPRESSSTRING, &textV));
    ASSERT(!strcmp(textV, ".U."));
    ASSERT(sdaiGetADBValue(adbValue, sdaiUNICODE, &wcV));
    ASSERT(!wcscmp(wcV, L".U."));
    ASSERT(!sdaiGetADBValue(adbValue, sdaiREAL, &doubleV));
    ASSERT(doubleV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiINTEGER, &intV));
    ASSERT(intV == 0);

    /// <summary>
    /// 5-binary
    /// </summary>
    sdaiGetAggrByIndex(listValues, 5, sdaiADB, &adbValue);
    type = sdaiGetADBType(adbValue);
    ASSERT(type == sdaiBINARY);

    ASSERT(!sdaiGetADBValue(adbValue, sdaiAGGR, &intV));
    ASSERT(intV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiINSTANCE, &intV));
    ASSERT(intV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiLOGICAL, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBOOLEAN, &boolV));
    ASSERT(!boolV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiENUM, &textV));
    ASSERT(!textV);
    ASSERT(sdaiGetADBValue(adbValue, sdaiBINARY, &textV));
    ASSERT(!strcmp(textV, "02F"));
    ASSERT(sdaiGetADBValue(adbValue, sdaiSTRING, &textV));
    ASSERT(!strcmp(textV, "02F"));
    ASSERT(sdaiGetADBValue(adbValue, sdaiEXPRESSSTRING, &textV));
    ASSERT(!strcmp(textV, "02F"));
    ASSERT(sdaiGetADBValue(adbValue, sdaiUNICODE, &wcV));
    ASSERT(!wcscmp(wcV, L"02F"));
    ASSERT(!sdaiGetADBValue(adbValue, sdaiREAL, &doubleV));
    ASSERT(doubleV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiINTEGER, &intV));
    ASSERT(intV == 0);

    /// <summary>
    /// 6-instance
    /// </summary>
    sdaiGetAggrByIndex(listValues, 6, sdaiADB, &adbValue);
    type = sdaiGetADBType(adbValue);
    ASSERT(type == sdaiINSTANCE);

    ASSERT(!sdaiGetADBValue(adbValue, sdaiAGGR, &intV));
    ASSERT(intV == 0);

    ASSERT(sdaiGetADBValue(adbValue, sdaiINSTANCE, &intV));
    intV = sdaiGetInstanceType(intV);
    engiGetEntityName(intV, sdaiSTRING, &textV);
    ASSERT(!strcmp(textV, "IfcPropertyListValue"));
    auto typePath = sdaiGetADBTypePath(adbValue, 0);
    ASSERT(!strcmp(typePath, ""));

    ASSERT(!sdaiGetADBValue(adbValue, sdaiLOGICAL, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBOOLEAN, &boolV));
    ASSERT(!boolV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiENUM, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBINARY, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiSTRING, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiEXPRESSSTRING, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiUNICODE, &wcV));
    ASSERT(!wcV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiREAL, &doubleV));
    ASSERT(doubleV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiINTEGER, &intV));
    ASSERT(intV == 0);


    /// <summary>
    /// 7-aggregation
    /// </summary>
    sdaiGetAggrByIndex(listValues, 7, sdaiADB, &adbValue);
    type = sdaiGetADBType(adbValue);
    ASSERT(type == sdaiAGGR);

    ASSERT(sdaiGetADBValue(adbValue, sdaiAGGR, &intV));
    ASSERT(intV != 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiINSTANCE, &intV));
    ASSERT(intV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiLOGICAL, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBOOLEAN, &boolV));
    ASSERT(!boolV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiENUM, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBINARY, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiSTRING, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiEXPRESSSTRING, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiUNICODE, &wcV));
    ASSERT(!wcV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiREAL, &doubleV));
    ASSERT(doubleV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiINTEGER, &intV));
    ASSERT(intV == 0);

    /// <summary>
    /// 8-enum
    /// </summary>
    sdaiGetAggrByIndex(listValues, 8, sdaiADB, &adbValue);
    type = sdaiGetADBType(adbValue);
    ASSERT(type == sdaiENUM);

    ASSERT(!sdaiGetADBValue(adbValue, sdaiAGGR, &intV));
    ASSERT(intV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiINSTANCE, &intV));
    ASSERT(intV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiLOGICAL, &textV));
    ASSERT(!textV);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBOOLEAN, &boolV));
    ASSERT(!boolV);
    //
    ASSERT(sdaiGetADBValue(adbValue, sdaiENUM, &textV));
    ASSERT(!strcmp(textV, "BELL"));
    ASSERT(!sdaiGetADBValue(adbValue, sdaiBINARY, &textV));
    ASSERT(!textV);
    ASSERT(sdaiGetADBValue(adbValue, sdaiSTRING, &textV));
    ASSERT(!strcmp(textV, ".BELL."));
    ASSERT(sdaiGetADBValue(adbValue, sdaiEXPRESSSTRING, &textV));
    ASSERT(!strcmp(textV, ".BELL."));
    ASSERT(sdaiGetADBValue(adbValue, sdaiUNICODE, &wcV));
    ASSERT(!wcscmp(wcV, L".BELL."));
    ASSERT(!sdaiGetADBValue(adbValue, sdaiREAL, &doubleV));
    ASSERT(doubleV == 0);
    ASSERT(!sdaiGetADBValue(adbValue, sdaiINTEGER, &intV));
    ASSERT(intV == 0);

    /// <summary>
    /// 
    /// </summary>
    IFC4::IfcPropertyListValue propListValue(instance);
    ASSERT(propListValue);

    IFC4::ListOfIfcValue lstIfcValue;
    propListValue.get_ListValues(lstIfcValue);
    ASSERT(lstIfcValue.size() == 9);

    auto it = lstIfcValue.begin();
    auto label = it->_IfcSimpleValue().get_IfcLabel();
    ASSERT(!strcmp(label, "List value 2"));

    it++;
    auto in = it->_IfcSimpleValue().get_IfcInteger();
    ASSERT(in.Value() == 13);

    it++;
    auto id = it->_IfcMeasureValue().get_IfcLengthMeasure();
    ASSERT(id.Value() == 8.5);

    it++;
    auto ib = it->_IfcSimpleValue().get_IfcBoolean();
    ASSERT(ib.Value() == true);

    it++;
    auto il = it->_IfcSimpleValue().get_IfcLogical();
    ASSERT(il.Value() == LOGICAL_VALUE::Unknown);

    it++;
    auto ibin = it->_IfcSimpleValue().get_IfcBinary();
    ASSERT(!strcmp(ibin, "02F"));

    it++;//instance but no access functions because it violates schema
    ASSERT(it->_IfcMeasureValue().get_IfcAreaMeasure().IsNull());
    adbValue = it->ADB();
    type = sdaiGetADBType(adbValue);
    ASSERT(type == sdaiINSTANCE);
    ASSERT(sdaiGetADBValue(adbValue, sdaiINSTANCE, &intV));
    intV = sdaiGetInstanceType(intV);
    engiGetEntityName(intV, sdaiSTRING, &textV);
    ASSERT(!strcmp(textV, "IfcPropertyListValue"));

    it++;
    std::list<double> ic;
    it->_IfcMeasureValue().get_IfcComplexNumber(ic);
    ASSERT(ic.back() == 1 && ic.front() == 1 && ic.size() == 2);

    //
    auto entityCtx = sdaiGetEntity(ifcModel, "IfcContext");
    auto ind = engiGetAttrIndexBN(entityCtx, "UnitsInContext");
    ASSERT(ind == 15);
    ind = engiGetAttrIndexExBN(entityCtx, "UnitsInContext", true, false);
    ASSERT(ind == 8);
    ind = engiGetAttrIndexExBN(entityCtx, "UnitsInContext", false, false);
    ASSERT(ind == 4);
}

static void TestAttrIndex(SdaiModel ifcModel)
{
    ENTER_TEST

    auto wall = internalGetInstanceFromP21Line(ifcModel, 8);

    //direct attribute
    auto attr = engiGetEntityAttributeByIndex(wall, 0, false, false);

    const char* attrName = nullptr;
    SdaiBoolean inverse = false;
    SdaiEntity definingEntity = 0;
    enum_express_attr_type attrType = enum_express_attr_type::__NONE;
    SdaiEntity domainEntity = 0;
    SchemaAggr aggrDescr = 0;
    SdaiBoolean optional = false;
    engiGetAttrTraits(attr, &attrName, &definingEntity, nullptr, &inverse, &attrType, &domainEntity, &aggrDescr, &optional);
    ASSERT(!inverse && !strcmp(attrName, "PredefinedType") && optional);
    ASSERT(attrType == enum_express_attr_type::__NONE);
    auto sz = engiGetEntityName(definingEntity, sdaiSTRING);
    ASSERT(!strcmp(sz, "IfcWall"));
    sz = engiGetEntityName(domainEntity, sdaiSTRING);
    ASSERT(!strcmp(sz, "IfcWallTypeEnum"));

    const char* type = NULL;
    sdaiGetAttr(wall, attr, sdaiENUM, &type);
    ASSERT(!strcmp(type, "SOLIDWALL"));

    auto res = sdaiGetAttrBN(wall, "PredefinedType", sdaiENUM, &type);
    ASSERT(res == &type && !strcmp(type, "SOLIDWALL"));

    auto ind = engiGetAttrIndexExBN(wall, "PredefinedType", true, true);
    ASSERT(ind == 32);
    ind = engiGetAttrIndexExBN(wall, "PredefinedType", true, false);
    ASSERT(ind == 8);
    ind = engiGetAttrIndexExBN(wall, "PredefinedType", false, true);
    ASSERT(ind == 0);

    //inverse attribute
    auto story = internalGetInstanceFromP21Line(ifcModel, 6);
    
    attr = engiGetEntityAttributeByIndex(wall, 9, true, true);

    SdaiInstance building = 0;
    res = sdaiGetAttr(story, attr, sdaiINSTANCE, &building);
    ASSERT((SdaiInstance)res == building);
    auto stepId = internalGetP21Line(building);
    ASSERT(stepId == 7);

    sdaiGetAttrBN(story, "Decomposes", sdaiINSTANCE, &building);
    stepId = internalGetP21Line(building);
    ASSERT(stepId == 7);

    ind = engiGetAttrIndexExBN(wall, "Decomposes", true, true);
    ASSERT(ind == 9);
    ind = engiGetAttrIndexExBN(wall, "Decomposes", true, false);
    ASSERT(ind == -1);
    ind = engiGetAttrIndexExBN(wall, "Decomposes", false, true);
    ASSERT(ind == -1);
}

static void GetAllInstancesTest(SdaiModel ifcModel, int_t expectedNum)
{
    ENTER_TEST

    auto aggrAll = xxxxGetAllInstances(ifcModel);
    ASSERT(aggrAll);
    auto nInst = sdaiGetMemberCount(aggrAll);
    ASSERT(nInst == expectedNum);

    SdaiInstance inst = 0;
    auto ret = sdaiGetAggrByIndex(aggrAll, expectedNum, sdaiINSTANCE, &inst);
    ASSERT(ret == 0 && inst == 0);
    ret = sdaiGetAggrByIndex(aggrAll, -1, sdaiINSTANCE, &inst);
    ASSERT(ret == 0 && inst == 0);

    std::vector<SdaiInstance> rInst;
    for (int i = 0; i < expectedNum; i++) {
        ret = sdaiGetAggrByIndex(aggrAll, i, sdaiINSTANCE, &inst);
        ASSERT(ret && (SdaiInstance)ret == inst);
        rInst.push_back(inst);
    }

    for (int k = 0; k < 2; k++) {
        for (int_t i = expectedNum - 1; i >= 0; i--) {
            ret = sdaiGetAggrByIndex(aggrAll, i, sdaiINSTANCE, &inst);
            ASSERT(ret && (SdaiInstance)ret == inst && inst == rInst[i]);
        }
        for (int_t i = expectedNum - 1; i >= 0; i-=2) {
            ret = sdaiGetAggrByIndex(aggrAll, i, sdaiINSTANCE, &inst);
            ASSERT(ret && (SdaiInstance)ret == inst && inst == rInst[i]);
        }
        for (int_t i = 0; i < expectedNum; i += 3) {
            ret = sdaiGetAggrByIndex(aggrAll, i, sdaiINSTANCE, &inst);
            ASSERT(ret && (SdaiInstance)ret == inst && inst == rInst[i]);
        }
        aggrAll = xxxxGetAllInstances(ifcModel);
    }
}

static void PrecisionTest(SdaiModel ifcModel)
{
    ENTER_TEST;

    auto window = IfcWindow::Create(ifcModel);

    window.put_OverallWidth(1. / 3);

    const char* str = nullptr;
    sdaiGetAttrBN(window, "OverallWidth", sdaiSTRING, &str);
    ASSERT(!strcmp(str, "0.333333"));

    setPrecisionDoubleExport(ifcModel, 15, 10, true);

    sdaiGetAttrBN(window, "OverallWidth", sdaiSTRING, &str);
    ASSERT(!strcmp(str, "0.333333333333333"));

    setPrecisionDoubleExport(ifcModel, 15, 15, true);

    sdaiGetAttrBN(window, "OverallWidth", sdaiSTRING, &str);
    ASSERT(!strcmp(str, "0.333333333333333"));
}

static ExpressID bigID = 2945825;

static void CheckPsetArray(std::vector<IFC4::IfcPropertySet>& psets, int n, ExpressID id1, ExpressID id2, ExpressID id3)
{
    std::vector<ExpressID> ids;
    for (auto inst : psets) {
        ids.push_back(internalGetP21Line(inst));
    }

    ASSERT(psets.size() == n);
    if (n > 0)
        ASSERT(ids[0] == id1);
    if (n > 1)
        ASSERT(ids[1] == id2);
    if (n > 2)
        ASSERT(ids[2] == id3);
}

static void CheckTestBigID(SdaiModel ifcModel, bool modified)
{
    auto id1 = bigID + 6;
    auto id2 = bigID + 7;
    auto id3 = bigID + 8;

    auto props = sdaiGetEntityExtentBN(ifcModel, "IfcPropertySingleValue");
    SdaiInteger Nprops = sdaiGetMemberCount(props);
    ASSERT(Nprops == (modified ? 4 : 5));
    for (SdaiInteger i = 0; i < Nprops; i++) {
        IFC4::IfcProperty prop = sdaiGetAggrByIndex(props, i);
        ASSERT(prop);

        auto id = internalGetP21Line(prop);
        ASSERT(id == bigID + i + (modified ? 2 : 1));

        std::vector<IFC4::IfcPropertySet> psets;
        prop.get_PartOfPset(psets);

        if (modified) {
            switch (i+1) {
            case 1: CheckPsetArray(psets, 2, id2, id3, 0); break;
            case 2: CheckPsetArray(psets, 1, id3, 0, 0); break;
            case 3: CheckPsetArray(psets, 0, 0, 0, 0); break;
            case 4: CheckPsetArray(psets, 1, id2, 0, 0); break;
            default: ASSERT(0);
            }
        }
        else {
            if (i < 4) {
                CheckPsetArray(psets, 3, id1, id2, id3);
            }
            else {
                ASSERT(psets.size() == 0);
            }
        }

    }
}

static void TestBigID()
{
    ENTER_TEST;

    SdaiModel  ifcModel = sdaiCreateModelBN(0, NULL, "IFC4");
    ASSERT(ifcModel);

    auto ok = SmokeTest_SetLastUsedExpressID(ifcModel, bigID);
    ASSERT(ok);

    IFC4::IfcProperty prop[5];
    for (int i = 0; i < 5; i++) {
        prop[i] = IFC4::IfcPropertySingleValue::Create(ifcModel);
    }
    
    IFC4::IfcPropertySet pset[3];
    for (int i = 0; i < 3; i++) {
        pset[i] = IFC4::IfcPropertySet::Create(ifcModel);
        pset[i].put_HasProperties(prop, 4);
    }

    //
    CheckTestBigID(ifcModel, false);

    sdaiSaveModelBN(ifcModel, "TestBigId.ifc");

    auto ifcModelRead = sdaiOpenModelBN(0, "TestBigId.ifc", "");
    ASSERT(ifcModelRead);
    CheckTestBigID(ifcModelRead, false);
    sdaiCloseModel(ifcModelRead);

    //
    sdaiDeleteInstance(prop[0]);

    sdaiUnsetAttrBN(pset[0], "HasProperties");
    
    SdaiAggr aggr = 0;
    sdaiGetAttrBN(pset[1], "HasProperties", sdaiAGGR, &aggr);
    sdaiRemoveByIndex(aggr, 1);
    sdaiPutAggrByIndex(aggr, 1, sdaiINSTANCE, (SdaiInstance)prop[4]);

    sdaiGetAttrBN(pset[2], "HasProperties", sdaiAGGR, &aggr);
    sdaiUnsetArrayByIndex(aggr, 2);

    //
    CheckTestBigID(ifcModel, true);

    sdaiSaveModelBN(ifcModel, "TestBigIdMod.ifc");
    sdaiCloseModel(ifcModel);

    ifcModelRead = sdaiOpenModelBN(0, "TestBigIdMod.ifc", "");
    ASSERT(ifcModelRead);
    CheckTestBigID(ifcModelRead, true);
    sdaiCloseModel(ifcModelRead);
}


extern void EngineTests(void)
{
    ENTER_TEST

    SdaiModel  ifcModel = sdaiCreateModelBN(0, NULL, "IFC4");
    ASSERT(ifcModel);
    SetSPFFHeaderItem(ifcModel, 9, 0, sdaiSTRING, "IFC4");
    SetSPFFHeaderItem(ifcModel, 9, 1, sdaiSTRING, (char*)0);

    GetAllInstancesTest(ifcModel, 0);

    TestBinaries(ifcModel);
    TestPutAttr(ifcModel);
    TestGetAttrType(ifcModel);

    GetAllInstancesTest(ifcModel, 7);

    PrecisionTest(ifcModel);

    const char* FILE_NAME = "EngineTests.ifc";

    sdaiSaveModelBN(ifcModel, FILE_NAME);
    sdaiCloseModel(ifcModel);

    //
    ifcModel = sdaiOpenModelBN(NULL, "..\\TestData\\IFC4_test.ifc", "IFC4");
    ASSERT(ifcModel);
    GetAllInstancesTest(ifcModel, 17);

    TestAttrIndex(ifcModel);
    TestGetADBValue(ifcModel);

    sdaiCloseModel(ifcModel);

    TestBigID();
}

