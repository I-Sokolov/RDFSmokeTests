
#include "pch.h"
#include "ToolBoxEx.h"

static SdaiADB CopyADB(SdaiADB srcADB, SdaiModel dstModel, InstanceMap& instanceMap);
static SdaiInstance CopyInstance(SdaiInstance srcInst, SdaiModel dstModel, InstanceMap& instanceMap);

/// <summary>
/// 
/// </summary>
static void  CopyHeader(SdaiModel srcModel, SdaiModel dstModel)
{
    for (int_t nSection = 0; nSection < 10; nSection++) {
        int_t   nItem = 0;
        char* data = nullptr;
        while (!GetSPFFHeaderItem(srcModel, nSection, nItem, sdaiEXPRESSSTRING, &data)) {
            SetSPFFHeaderItem(dstModel, nSection, nItem, sdaiEXPRESSSTRING, data);
            nItem++;
        }
    }
}

/// <summary>
/// 
/// </summary>
static SdaiNPL CopyAggregate(SdaiAggr srcAggr, SdaiModel dstModel, InstanceMap& instanceMap)
{
    SdaiNPL dstAggr = sdaiCreateNPL();

    SdaiIterator it = sdaiCreateIterator(srcAggr);
    sdaiBeginning(it);
    while (sdaiNext(it)) {

        SdaiADB adbVal = NULL;
        if (sdaiGetAggrByIterator(it, sdaiADB, &adbVal)) {
            if (auto dstVal = CopyADB(adbVal, dstModel, instanceMap)) {
                sdaiAdd(dstAggr, sdaiADB, dstVal);
            }
            else ASSERT(0);
        }
        else ASSERT(!"unset element in aggregation?");

    }
    sdaiDeleteIterator(it);

    return dstAggr;
}

/// <summary>
/// 
/// </summary>
template<typename TValue> static void CopyADBValue(SdaiADB src, SdaiADB dst, SdaiPrimitiveType sdaiType)
{
    TValue value = 0;
    if (sdaiGetADBValue(src, sdaiType, &value)) {
        sdaiPutADBValue(dst, sdaiType, &value);
    }
    else {
        ASSERT(false);
    }
}

/// <summary>
/// 
/// </summary>
static void CopyADBStringValue(SdaiADB src, SdaiADB dst, SdaiPrimitiveType sdaiType)
{
    const char* value = 0;
    if (sdaiGetADBValue(src, sdaiType, &value)) {
        sdaiPutADBValue(dst, sdaiType, value);
    }
    else {
        ASSERT(false);
    }
}

/// <summary>
/// 
/// </summary>
static SdaiADB CopyADB(SdaiADB srcADB, SdaiModel dstModel, InstanceMap& instanceMap)
{
    SdaiADB dstADB = sdaiCreateEmptyADB();

    const char* path = sdaiGetADBTypePath(srcADB, 0);

    auto adbType = sdaiGetADBType(srcADB);
    switch (adbType) {
        case  sdaiBOOLEAN:
            CopyADBValue<bool>(srcADB, dstADB, sdaiBOOLEAN);
            break;
        case  sdaiLOGICAL:
            CopyADBStringValue(srcADB, dstADB, sdaiLOGICAL);
            break;
        case  sdaiENUM:
            CopyADBStringValue(srcADB, dstADB, sdaiENUM);
            break;
        case  sdaiREAL:
            CopyADBValue<SdaiReal>(srcADB, dstADB, sdaiREAL);
            break;
        case  sdaiINTEGER:
            CopyADBValue<SdaiInteger>(srcADB, dstADB, sdaiINTEGER);
            break;
        case  sdaiSTRING:
            CopyADBStringValue(srcADB, dstADB, sdaiEXPRESSSTRING); //request exact express string to avoid conversion
            break;
        case  sdaiINSTANCE:
        {
            SdaiInstance srcValue = NULL;
            sdaiGetADBValue(srcADB, sdaiINSTANCE, &srcValue);
            ASSERT(srcValue);

            auto dstValue = CopyInstance(srcValue, dstModel, instanceMap);
            sdaiPutADBValue(dstADB, sdaiINSTANCE, dstValue);
            break;
        }
        case  sdaiAGGR:
        {
            SdaiAggr srcValue = NULL;
            sdaiGetADBValue(srcADB, sdaiAGGR, &srcValue);
            ASSERT(srcValue);
            
            if (auto dstValue = CopyAggregate(srcValue, dstModel, instanceMap)) {
                sdaiPutADBValue(dstADB, sdaiAGGR, dstValue);
                //TODO memory issue - sdaiDeleteNPL(dstValue);
            }
            break;
        }
        case  sdaiADB:
            ASSERT(!"Not support nested ADB");
            break;
        default:
            ASSERT("Unsupported ADB type");
            break;
    }

    if (path && *path) {
        sdaiPutADBTypePath(dstADB, 1, path);
    }

    return dstADB;
}

/// <summary>
/// 
/// </summary>
static void CopyAttributes(SdaiInstance srcInst, SdaiInstance dstInst, SdaiEntity srcEntity, bool partOfComplex, InstanceMap& instanceMap)
{
    auto dstEntity = sdaiGetInstanceType(dstInst);

    SdaiAttr attr = NULL;
    while (NULL != (attr = engiGetEntityAttributeByIterator(srcEntity, attr))) {

        if (!engiIsAttrExplicit(attr))
            continue;

        auto srcDefiningEntity = engiGetAttrDefiningEntity(attr);
        if (partOfComplex && srcDefiningEntity != srcEntity)
            continue;

        std::string qualifiedName = engiGetEntityName(srcDefiningEntity, sdaiSTRING);
        qualifiedName += ".";
        qualifiedName += engiGetAttrName(attr);

        auto dstAttr = sdaiGetAttrDefinition(dstEntity, qualifiedName.c_str());
        ASSERT(dstAttr);

        SdaiADB adb = NULL;
        if (sdaiGetAttr(srcInst, attr, sdaiADB, &adb)) {
            //
            if (auto dstADB = CopyADB(adb, sdaiGetInstanceModel(dstInst), instanceMap)) {
                sdaiPutAttr(dstInst, dstAttr, sdaiADB, dstADB);
                //TODO memory issue - sdaiDeleteADB(dstADB);
            }
            else ASSERT(0);
        }
        else 
        {
            //assume this is unset value
            SdaiString src = NULL;
            sdaiGetAttr(srcInst, attr, sdaiEXPRESSSTRING, &src);
            ASSERT(src && (*src == '$' || *src == '*'));
            //nothing to do, assume this is default value for new instance
            SdaiString dst = NULL;
            sdaiGetAttr(dstInst, dstAttr, sdaiEXPRESSSTRING, &dst);
            //TODO - invalid default value - ASSERT(dst && src && 0==strcmp(src, dst));
        }
    }
}

/// <summary>
/// 
/// </summary>
static void CopyAttributes(SdaiInstance srcInst, SdaiInstance dstInst, InstanceMap& instanceMap)
{
    SdaiEntity  entity = sdaiGetInstanceType(srcInst);

    if (engiIsComplexEntity(entity))
    {
        SdaiInteger index = 0;
        while (SdaiEntity component = engiGetEntityParentEx(entity, index++))
        {
            CopyAttributes(srcInst, dstInst, component, true, instanceMap);
        }
    }
    else
    {
        CopyAttributes(srcInst, dstInst, entity, false, instanceMap);
    }
}


/// <summary>
/// 
/// </summary>
static SdaiInstance CreateIstance(SdaiEntity entity, SdaiModel dstModel)
{
    if (engiIsComplexEntity(entity)) {
        
        std::vector<const char*> names;
        SdaiInteger i = 0;
        while (SdaiEntity componenet = engiGetEntityParentEx(entity, i++)) {
            auto name = engiGetEntityName(componenet, sdaiSTRING);
            ASSERT(name && *name);
            names.push_back(name);
        }

        return sdaiCreateComplexInstanceBN(dstModel, names.size(), names.data());
    }
    else {
        auto name = engiGetEntityName(entity, sdaiSTRING);
        ASSERT(name && *name);
        return sdaiCreateInstanceBN(dstModel, name);
    }
}

/// <summary>
/// 
/// </summary>
static SdaiInstance CopyInstance(SdaiInstance srcInst, SdaiModel dstModel, InstanceMap& instanceMap)
{
    if (srcInst == NULL)
        return NULL;

    auto srcId = internalGetP21Line(srcInst);

    auto& dstId = instanceMap[srcId];
    if (dstId) {
        return internalGetInstanceFromP21Line(dstModel, dstId)  ; //>>>>
    }

    auto entity = sdaiGetInstanceType(srcInst);
    ASSERT(entity);

    auto dstInst = CreateIstance(entity, dstModel);
    ASSERT(dstInst);

    dstId = internalGetP21Line(dstInst);

    CopyAttributes(srcInst, dstInst, instanceMap);

    return dstInst;
}

/// <summary>
/// 
/// </summary>
static void    CopyInstances(SdaiModel srcModel, SdaiModel dstModel, InstanceMap& instanceMap)
{
    auto all = xxxxGetAllInstances(srcModel);
    auto it = sdaiCreateIterator(all);

    while (sdaiNext(it)) {

        SdaiInstance srcInst = NULL;
        sdaiGetAggrByIterator(it, sdaiINSTANCE, &srcInst);
        ASSERT(srcInst);

        CopyInstance(srcInst, dstModel, instanceMap);
    }
}


/// <summary>
/// 
/// </summary>
extern SdaiModel CopyModelByAPI(SdaiModel source, InstanceMap& instanceMap)
{
    const char* schemaFile = "CopyModelByAPI.exp";

    auto ok = engiSaveSchemaBN(source, schemaFile);
    ASSERT(ok);

    auto target = sdaiCreateModelBN(0, NULL, schemaFile);
    ASSERT(target);

    CopyHeader(source, target);
    
    CopyInstances(source, target, instanceMap);

    return target;
}
