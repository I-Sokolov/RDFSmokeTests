
#include "pch.h"
#include "ToolBoxEx.h"

static void    SaveFileAggregation(
    FILE* fp,
    SdaiAggr        aggregation
);

/// <summary>
/// 
/// </summary>
static void    SaveFileHeader(
    FILE* fp,
    int_t   model
)
{
    fprintf(fp, "ISO-10303-21;\n");
    fprintf(fp, "HEADER;\n");
    fprintf(fp, "FILE_DESCRIPTION (");

    {
        //  Description
        int_t   i = 0;
        char* description = nullptr;
        fprintf(fp, "(");
        if (!GetSPFFHeaderItem(model, 0, i++, sdaiEXPRESSSTRING, (char**)&description)) {
            fprintf(fp, "'%s'", description);
            while (!GetSPFFHeaderItem(model, 0, i++, sdaiEXPRESSSTRING, (char**)&description))
                fprintf(fp, ", '%s'", description);
        }
        fprintf(fp, ")");
    }

    fprintf(fp, ", ");

    {
        //  ImplementationLevel
        char* implementationLevel = nullptr;
        GetSPFFHeaderItem(model, 1, 0, sdaiEXPRESSSTRING, (char**)&implementationLevel);
        if (implementationLevel)
            fprintf(fp, "'%s'", implementationLevel);
        else
            fprintf(fp, "$");
    }

    fprintf(fp, ");\n");
    fprintf(fp, "FILE_NAME (");

    {
        //  Name
        char* name = nullptr;
        GetSPFFHeaderItem(model, 2, 0, sdaiEXPRESSSTRING, (char**)&name);
        if (name)
            fprintf(fp, "'%s'", name);
        else
            fprintf(fp, "$");
    }

    fprintf(fp, ", ");

    {
        //  TimeStamp
        char* timeStamp = nullptr;
        GetSPFFHeaderItem(model, 3, 0, sdaiEXPRESSSTRING, (char**)&timeStamp);
        if (timeStamp)
            fprintf(fp, "'%s'", timeStamp);
        else
            fprintf(fp, "$");
    }

    fprintf(fp, ", ");

    {
        //  Author
        int_t   i = 0;
        char* author = nullptr;
        fprintf(fp, "(");
        if (!GetSPFFHeaderItem(model, 4, i++, sdaiEXPRESSSTRING, (char**)&author)) {
            fprintf(fp, "'%s'", author);
            while (!GetSPFFHeaderItem(model, 4, i++, sdaiEXPRESSSTRING, (char**)&author))
                fprintf(fp, ", '%s'", author);
        }
        fprintf(fp, ")");
    }

    fprintf(fp, ", ");

    {
        //  Organization
        int_t   i = 0;
        char* organization = nullptr;
        fprintf(fp, "(");
        if (!GetSPFFHeaderItem(model, 5, i++, sdaiEXPRESSSTRING, (char**)&organization)) {
            fprintf(fp, "'%s'", organization);
            while (!GetSPFFHeaderItem(model, 5, i++, sdaiEXPRESSSTRING, (char**)&organization))
                fprintf(fp, ", '%s'", organization);
        }
        fprintf(fp, ")");
    }

    fprintf(fp, ", ");

    {
        //  PreprocessorVersion
        char* preprocessorVersion = nullptr;
        GetSPFFHeaderItem(model, 6, 0, sdaiEXPRESSSTRING, (char**)&preprocessorVersion);
        if (preprocessorVersion)
            fprintf(fp, "'%s'", preprocessorVersion);
        else
            fprintf(fp, "$");
    }

    fprintf(fp, ", ");

    {
        //  OriginatingSystem
        char* originatingSystem = nullptr;
        GetSPFFHeaderItem(model, 7, 0, sdaiEXPRESSSTRING, (char**)&originatingSystem);
        if (originatingSystem)
            fprintf(fp, "'%s'", originatingSystem);
        else
            fprintf(fp, "$");
    }

    fprintf(fp, ", ");

    {
        //  Authorization
        char* authorization = nullptr;
        GetSPFFHeaderItem(model, 8, 0, sdaiEXPRESSSTRING, (char**)&authorization);
        if (authorization)
            fprintf(fp, "'%s'", authorization);
        else
            fprintf(fp, "$");
    }

    fprintf(fp, ");\n");
    fprintf(fp, "FILE_SCHEMA (");

    {
        //  FileSchema
        int_t   i = 0;
        char* fileSchema = nullptr;
        fprintf(fp, "(");
        if (!GetSPFFHeaderItem(model, 9, i++, sdaiEXPRESSSTRING, (char**)&fileSchema)) {
            fprintf(fp, "'%s'", fileSchema);
            while (!GetSPFFHeaderItem(model, 9, i++, sdaiEXPRESSSTRING, (char**)&fileSchema))
                fprintf(fp, ", '%s'", fileSchema);
        }
        fprintf(fp, ")");
    }

    fprintf(fp, ");\n");
    fprintf(fp, "ENDSEC;\n");
}


/// <summary>
/// 
/// </summary>
static char* UpperCase(
    char* in
)
{
    static char    BUFF[512];

    assert(strlen(in) < 256);
    size_t  len = strlen(in);
    for (size_t i = 0; i <= len; i++) {
        if (in[i] >= 'a' && in[i] <= 'z')
            BUFF[i] = in[i] - 'a' + 'A';
        else
            BUFF[i] = in[i];
    }
    return  BUFF;
}

/// <summary>
/// 
/// </summary>
static void    CreateOID(
    FILE* fp,
    SdaiInstance    instance
)
{
    assert(instance);

    fprintf(fp, "#%i", (int)internalGetP21Line(instance));
}

/// <summary>
/// 
/// </summary>
static void    CreateBoolean(
    FILE* fp,
    bool            value
)
{
    if (value)
        fprintf(fp, ".T.");
    else
        fprintf(fp, ".F.");
}

/// <summary>
/// 
/// </summary>
static void    CreateLogical(
    FILE* fp,
    char* value
)
{
    fprintf(fp, ".%s.", value);
}

/// <summary>
/// 
/// </summary>
static void    CreateEnumeration(
    FILE* fp,
    char* value
)
{
    fprintf(fp, ".%s.", value);
}

/// <summary>
/// 
/// </summary>
static void    CreateReal(
    FILE* fp,
    double          value
)
{
    fprintf(fp, "%f", value);
}

/// <summary>
/// 
/// </summary>
static void    CreateInteger(
    FILE* fp,
    int_t           value
)
{
    fprintf(fp, "%i", (int)value);
}

/// <summary>
/// 
/// </summary>
static void    CreateString(
    FILE* fp,
    char* value
)
{
    if (value)
        fprintf(fp, "'%s'", value);
    else
        fprintf(fp, "''");
}

/// <summary>
/// 
/// </summary>
static void    SaveFileADB(
    FILE* fp,
    SdaiADB         ADB
)
{
    const char* path = sdaiGetADBTypePath(ADB, 0);
    fprintf(fp, "%s(", path);

    switch (sdaiGetADBType(ADB)) {
        case  sdaiADB:
        {
            SdaiADB    attributeDataBlock = 0;
            if (sdaiGetADBValue(ADB, sdaiADB, &attributeDataBlock)) {
                SaveFileADB(fp, attributeDataBlock);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiAGGR:
        {
            SdaiAggr        valueAggr = nullptr;
            SdaiInstance    valueInstance = 0;
            if (sdaiGetADBValue(ADB, sdaiAGGR, &valueAggr)) {
                fprintf(fp, "(");
                SaveFileAggregation(fp, valueAggr);
                fprintf(fp, ")");
            }
            else if (sdaiGetADBValue(ADB, sdaiINSTANCE, &valueInstance)) {
                CreateOID(fp, valueInstance);
            }
            else {
                assert(false);
            }
        }
        break;
        case  sdaiINSTANCE:
        {
            SdaiInstance    value = 0;
            if (sdaiGetADBValue(ADB, sdaiINSTANCE, &value)) {
                CreateOID(fp, value);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiBOOLEAN:
        {
            bool    value = false;
            if (sdaiGetADBValue(ADB, sdaiBOOLEAN, &value)) {
                CreateBoolean(fp, value);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiLOGICAL:
        {
            char* value = nullptr;
            if (sdaiGetADBValue(ADB, sdaiLOGICAL, &value)) {
                CreateLogical(fp, value);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiENUM:
        {
            char* value = nullptr;
            if (sdaiGetADBValue(ADB, sdaiENUM, &value)) {
                CreateEnumeration(fp, value);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiREAL:
        {
            double  value = 0.;
            if (sdaiGetADBValue(ADB, sdaiREAL, &value)) {
                CreateReal(fp, value);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiINTEGER:
        {
            int_t   value = 0;
            if (sdaiGetADBValue(ADB, sdaiINTEGER, &value)) {
                CreateInteger(fp, value);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiSTRING:
        {
            char* value = nullptr;
            if (sdaiGetADBValue(ADB, sdaiEXPRESSSTRING, &value)) {
                CreateString(fp, value);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        default:
            assert(false);
            fprintf(fp, "??");
            break;
    }

    fprintf(fp, ")");
}

/// <summary>
/// 
/// </summary>
static void    SaveFileAggregationElement(
    FILE* fp,
    SdaiAggr        aggregation,
    int_t           aggrType,
    SdaiInteger     index
)
{
    switch (aggrType) {
        case  sdaiADB:
        {
            SdaiADB    attributeDataBlock = 0;
            if (engiGetAggrElement(aggregation, index, sdaiADB, &attributeDataBlock)) {
                SaveFileADB(fp, attributeDataBlock);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiAGGR:
        {
            SdaiAggr        valueAggr = nullptr;
            SdaiInstance    valueInstance = 0;
            if (engiGetAggrElement(aggregation, index, sdaiAGGR, &valueAggr)) {
                fprintf(fp, "(");
                SaveFileAggregation(fp, valueAggr);
                fprintf(fp, ")");
            }
            else if (engiGetAggrElement(aggregation, index, sdaiINSTANCE, &valueInstance)) {
                CreateOID(fp, valueInstance);
            }
            else {
                assert(false);
            }
        }
        break;
        case  sdaiINSTANCE:
        {
            SdaiInstance    value = 0;
            if (engiGetAggrElement(aggregation, index, sdaiINSTANCE, &value)) {
                CreateOID(fp, value);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiBOOLEAN:
        {
            bool    value = false;
            if (engiGetAggrElement(aggregation, index, sdaiBOOLEAN, &value)) {
                CreateBoolean(fp, value);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiLOGICAL:
        {
            char* value = nullptr;
            if (engiGetAggrElement(aggregation, index, sdaiLOGICAL, &value)) {
                CreateLogical(fp, value);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiENUM:
        {
            char* value = nullptr;
            if (engiGetAggrElement(aggregation, index, sdaiENUM, &value)) {
                CreateEnumeration(fp, value);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiREAL:
        {
            double  value = 0.;
            if (engiGetAggrElement(aggregation, index, sdaiREAL, &value)) {
                CreateReal(fp, value);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiINTEGER:
        {
            int_t   value = 0;
            if (engiGetAggrElement(aggregation, index, sdaiINTEGER, &value)) {
                CreateInteger(fp, value);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiSTRING:
        {
            char* value = nullptr;
            if (engiGetAggrElement(aggregation, index, sdaiEXPRESSSTRING, &value)) {
                CreateString(fp, value);
            }
            else {
                assert(false);
                fprintf(fp, "$");
            }
        }
        break;
        default:
            assert(false);
            fprintf(fp, "??");
            break;
    }
}

/// <summary>
/// 
/// </summary>
static void    SaveFileAggregation(
    FILE* fp,
    SdaiAggr        aggregation
)
{
    SdaiInteger memberCount = sdaiGetMemberCount(aggregation);
    if (memberCount == 0)
        return;

    int_t   aggrType = 0;
    engiGetAggrType(aggregation, &aggrType);

    SdaiInteger index = 0;
    SaveFileAggregationElement(fp, aggregation, aggrType, index++);
    while (index < memberCount) {
        fprintf(fp, ", ");
        SaveFileAggregationElement(fp, aggregation, aggrType, index++);
    }
}

/// <summary>
/// 
/// </summary>
static void    SaveFileAttribute(
    FILE* fp,
    SdaiInstance    instance,
    SdaiAttr        attribute
)
{
    int_t   attrType = engiGetAttrType(attribute);
    if (attrType & engiTypeFlagAggr ||
        attrType & engiTypeFlagAggrOption)
        attrType = sdaiAGGR;

    switch (attrType) {
        case  sdaiADB:
        {
            SdaiADB    attributeDataBlock = 0;
            if (sdaiGetAttr(instance, attribute, sdaiADB, &attributeDataBlock)) {
                assert(attributeDataBlock);
                SaveFileADB(fp, attributeDataBlock);
            }
            else {
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiAGGR:
        {
            SdaiAggr        valueAggr = nullptr;
            SdaiInstance    valueInstance = 0;
            if (sdaiGetAttr(instance, attribute, sdaiAGGR, &valueAggr)) {
                fprintf(fp, "(");
                SaveFileAggregation(fp, valueAggr);
                fprintf(fp, ")");
            }
            else if (sdaiGetAttr(instance, attribute, sdaiINSTANCE, &valueInstance)) {
                CreateOID(fp, valueInstance);
            }
            else {
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiINSTANCE:
        {
            SdaiInstance    value = 0;
            if (sdaiGetAttr(instance, attribute, sdaiINSTANCE, &value)) {
                CreateOID(fp, value);
            }
            else {
                if (engiGetAttrDerived(sdaiGetInstanceType(instance), attribute)) {
                    fprintf(fp, "*");
                }
                else {
                    fprintf(fp, "$");
                }
            }
        }
        break;
        case  sdaiBOOLEAN:
        {
            bool    value = false;
            if (sdaiGetAttr(instance, attribute, sdaiBOOLEAN, &value)) {
                CreateBoolean(fp, value);
            }
            else {
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiLOGICAL:
        {
            char* value = nullptr;
            if (sdaiGetAttr(instance, attribute, sdaiLOGICAL, &value)) {
                CreateLogical(fp, value);
            }
            else {
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiENUM:
        {
            char* value = nullptr;
            if (sdaiGetAttr(instance, attribute, sdaiENUM, &value)) {
                CreateEnumeration(fp, value);
            }
            else {
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiREAL:
        {
            double  value = 0.;
            if (sdaiGetAttr(instance, attribute, sdaiREAL, &value)) {
                CreateReal(fp, value);
            }
            else {
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiINTEGER:
        {
            int_t   value = 0;
            if (sdaiGetAttr(instance, attribute, sdaiINTEGER, &value)) {
                CreateInteger(fp, value);
            }
            else {
                fprintf(fp, "$");
            }
        }
        break;
        case  sdaiSTRING:
        {
            char* value = nullptr;
            if (sdaiGetAttr(instance, attribute, sdaiEXPRESSSTRING, &value)) {
                CreateString(fp, value);
            }
            else {
                fprintf(fp, "$");
            }
        }
        break;
        default:
            assert(false);
            fprintf(fp, "??");
            break;
    }
}

/// <summary>
/// 
/// </summary>
static void    SaveFileInstance(
    FILE* fp,
    SdaiInstance    instance
)
{
    char* entityName = nullptr;
    engiGetEntityName(sdaiGetInstanceType(instance), sdaiSTRING, (const char**)&entityName);
    fprintf(fp, "#%i = %s(", (int)internalGetP21Line(instance), UpperCase(entityName));

    bool    countedWithParents = true,
        countedWithInverse = false;

    SdaiEntity  entity = sdaiGetInstanceType(instance);
    SdaiInteger index = 0;
    SdaiAttr    attribute = engiGetEntityAttributeByIndex(
        entity,
        index++,
        countedWithParents,
        countedWithInverse
    );
    SaveFileAttribute(fp, instance, attribute);
    while (attribute) {
        attribute = engiGetEntityAttributeByIndex(
            entity,
            index++,
            countedWithParents,
            countedWithInverse
        );
        if (attribute) {
            fprintf(fp, ", ");
            SaveFileAttribute(fp, instance, attribute);
        }
    }

    fprintf(fp, ");\n");
}


/// <summary>
/// 
/// </summary>
static void    SaveFileBody(
    FILE* fp,
    int_t   model
)
{
    fprintf(fp, "DATA;\n");

    int_t* myAggr = xxxxGetAllInstances(model),
        myAggrCount = sdaiGetMemberCount(myAggr);
    for (int_t j = 0; j < myAggrCount; j++) {
        int_t   myInstance = 0;
        engiGetAggrElement(myAggr, j, sdaiINSTANCE, &myInstance);
        SaveFileInstance(fp, myInstance);
    }

    fprintf(fp, "ENDSEC;\n");
}

/// <summary>
/// 
/// </summary>
static void    SaveFileFooter(
    FILE* fp
)
{
    fprintf(fp, "END-ISO-10303-21;\n");
}


/// <summary>
/// 
/// </summary>
static void    SaveModel(int_t   model, FILE* fp)
{
    SaveFileHeader(fp, model);
    SaveFileBody(fp, model);
    SaveFileFooter(fp);
}

/// <summary>
/// 
/// </summary>
extern void SaveModel(SdaiModel model, const char* filePath)
{
    FILE* fp = nullptr;
    fopen_s(&fp, filePath, "wt");
    ASSERT(fp);

    if (fp) {
        SaveModel(model, fp);
        fclose(fp);
    }
}
