// ModelChecker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#define INDENT "\t\t"

struct IssueHandler
{
    virtual void OnIssue(ValidationIssue issue) = NULL;
};

static ValidationIssueLevel CheckModel(const char* filePath, const char* expressSchemaFilePath, IssueHandler* pIssueHandler);
//static ValidationIssueLevel CheckModels(const char* filePath, const char* expressSchemaFilePath);

/// <summary>
/// Issue reporting 
/// </summary>
struct PrintIssue : public IssueHandler
{
    const char* GetEntityName(ValidationIssue issue)
    {
        auto ent = validateGetEntity(issue);
        if (ent) {
            const char* name = 0;
            engiGetEntityName(ent, sdaiSTRING, &name);
            return name;
        }
        else {
            return NULL;
        }
    }

    const char* GetAttrName(ValidationIssue issue)
    {
        auto attr = validateGetAttr(issue);
        if (attr) {
            const char* name = 0;
            engiGetAttrTraits(attr, &name, 0, 0, 0, 0, 0, 0, 0);
            return name;
        }
        else {
            return NULL;
        }
    }

    int_t GetAttrIndex(ValidationIssue issue)
    {
        auto ent = validateGetEntity(issue);
        const char* name = GetAttrName(issue);
        if (ent && name) {
            return engiGetAttrIndexExBN(ent, name, true, false);
        }
        else {
            return -1;
        }
    }

    int64_t GetStepId(ValidationIssue issue)
    {
        auto inst = validateGetInstance(issue);
        if (inst) {
            return internalGetP21Line(inst);
        }
        else {
            return -1;
        }
    }

    virtual void OnIssue(ValidationIssue issue) override
    {
        printf(INDENT "<Issue");

        auto stepId = GetStepId(issue);
        if (stepId > 0) {
            printf(" stepId='#%lld'", stepId);
        }

        auto entity = GetEntityName(issue);
        if (entity) {
            printf(" entity='%s'", entity);
        }

        auto attrName = GetAttrName(issue);
        if (attrName) {
            printf(" attribute='%s'", attrName);
        }

        auto attrIndex = GetAttrIndex(issue);
        if (attrIndex >= 0) {
            printf(" attributeIndex='%lld'", (int64_t) attrIndex);
        }

        auto aggrLevel = validateGetAggrLevel(issue);
        auto aggrIndArray = validateGetAggrIndArray(issue);
        for (int_t i = 0; i < aggrLevel; i++) {
            if (i == 0) {
                printf(" aggregationIndex='%lld'", (int64_t) aggrIndArray[i]);
            }
            else {
                printf(" aggregationIndex_%lld='%lld'", (int64_t) i, (int64_t) aggrIndArray[i]);
            }
        }

        auto issueId = validateGetIssueType(issue);
        auto level = validateGetIssueLevel(issue);
        printf(" issueId='%d' issueLevel='%lld'>\n", (int) issueId, level);

        auto text = validateGetDescription(issue);
        if (text) {
            printf(INDENT INDENT "%s\n", text);
        }

        printf(INDENT "</Issue>\n");
    }
};

static ValidationIssueLevel CheckModel(const char* filePath, const char* expressSchemaFilePath, IssueHandler* pLog, enum_validation_status status)
{
    printf("\t<CheckModel file='%s'", filePath);
    
    if (expressSchemaFilePath)
        printf(" schema='%s'>\n", expressSchemaFilePath);
    else
        printf(" embedded_schema='true'>\n");

    ValidationIssueLevel result = 0;

    SdaiModel model = sdaiOpenModelBN(NULL, filePath, expressSchemaFilePath ? expressSchemaFilePath : "");
    if (model) {
        auto checks = validateModel(model);
        for (auto issue = validateGetFirstIssue(checks); issue; issue = validateGetNextIssue(issue)) {
            pLog->OnIssue(issue);
            result = max(result, validateGetIssueLevel(issue));
        }
        ASSERT(validateGetStatus(checks) == status);
        validateFreeResults(checks);
        sdaiCloseModel(model);
        model = NULL;
    }
    else {
        printf("\t\t<Failure call='sdaiOpenModelBN'>%s</Failure>\n", filePath);
        result = -13;
    }

    printf("\t</CheckModel>\n");
    return result;
}

//
// smoke-test expected issues
//


struct IssueInfo
{
    int64_t                     stepId;
    const char*                 entity;
    const char*                 attrName;
    int_t                       attrInd;
    int_t                       aggrLevel;
    int_t*                      aggrIndArray;
    enum_validation_type         issueType;
};


//
// smoke test run and check
//

struct CheckExpectedIssuses : public PrintIssue
{
    CheckExpectedIssuses(IssueInfo* rExpectedIssues, int nExpectedIssues) : m_rExpectedIssues(rExpectedIssues), m_nExpectedIssues(nExpectedIssues) {}

    virtual void OnIssue(ValidationIssue issue) override;

private:
    IssueInfo* m_rExpectedIssues;
    int m_nExpectedIssues;
};


void CheckExpectedIssuses::OnIssue(ValidationIssue issue)
{
    //base report
    __super::OnIssue(issue);

    //check issue expected
    auto stepId = GetStepId(issue);
    auto vi = validateGetIssueType(issue);
    auto attrInd = GetAttrIndex(issue);
    auto aggrLevel = validateGetAggrLevel(issue);
    auto aggrIndArray = validateGetAggrIndArray(issue);

    bool found = false;
    for (int i = 0; i < m_nExpectedIssues; i++) {
        auto& expected = m_rExpectedIssues[i];
        if (expected.stepId == stepId 
            && expected.attrInd ==  attrInd
            && expected.aggrLevel == aggrLevel) {

            ASSERT(expected.issueType == vi);
            
            for (int i = 0; i < aggrLevel; i++) {
                ASSERT(expected.aggrIndArray[i] == aggrIndArray[i]);
            }
            
            found = true;
            expected.stepId = -2; //mark as reported
            break;
        }
    }
    ASSERT(found);
}

static int_t r0[] = { 0 };
static int_t r1[] = { 1 };
static int_t r2[] = { 2 };
static int_t r3[] = { 3 };
static int_t r4[] = { 4 };
static int_t r5[] = { 5 };
static int_t r6[] = { 6 };
static int_t r7[] = { 7 };
static int_t r9[] = { 9 };
static int_t r32[] = { 3,2 };

static IssueInfo rExpectedIssuesIFC2x3[] =
{
    //id   class                    attrName                    ind     aggrLev/aggrInd         Issue
    {24,    "IfcLocalPlacement",    "PlacesObject",             -1,     0,NULL,         enum_validation_type::__AGGREGATION_SIZE},
    {84,    "IFCCARTESIANPOINTLIST2D",NULL,                     -1,     0,NULL,         enum_validation_type::__KNOWN_ENTITY},
    {51,    "IfcProductDefinitionShape","Representations",      2,      1,r3,           enum_validation_type::__REFERENCE_EXISTS},
    {52,    "IfcShapeRepresentation","OfProductRepresentation", -1,     0,NULL,         enum_validation_type::__AGGREGATION_SIZE},
    {52,    "IfcShapeRepresentation","OfProductRepresentation", -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {81,    "IfcShapeRepresentation","OfProductRepresentation", -1,     0,NULL,         enum_validation_type::__AGGREGATION_SIZE},
    {81,    "IfcShapeRepresentation","OfProductRepresentation", -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {151,   "IfcProductDefinitionShape","Representations",      2,      1,r3,           enum_validation_type::__REFERENCE_EXISTS},
    {74,    "IfcPolyLoop",          "Polygon",                  0,      1,r1,           enum_validation_type::__REFERENCE_EXISTS},
    {74,    "IfcPolyLoop",          NULL,                       -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {110,   "IfcProject",           "GlobalId",                 0,      0,NULL,         enum_validation_type::__REQUIRED_ARGUMENTS},
    {111,   "IfcProject",           "ObjectType",               4,      0,NULL,         enum_validation_type::__STAR_USAGE},
    {112,   "IfcProject",           "OwnerHistory",             1,      0,NULL,         enum_validation_type::__AGGREGATION_NOT_EXPECTED},
//    {113,   "IfcProject",           "OwnerHistory",             1,      0,NULL,         enum_validation_type::__REQUIRED_ARGUMENTS},
//    {113,   "IfcProject",           "RepresentationContexts",   7,      0,NULL,         enum_validation_type::__REQUIRED_ARGUMENTS},
//    {113,   "IfcProject",           "UnitsInContext",           8,      0,NULL,         enum_validation_type::__REQUIRED_ARGUMENTS},
    {113,   "IfcProject",           NULL,                       -1,     0,NULL,         enum_validation_type::__NO_OF_ARGUMENTS},
//    {113,   "IfcProject",           NULL,                       -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
//    {113,   "IfcProject",           NULL,                       -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {114,   "IfcProject",           NULL,                       -1,     0,NULL,         enum_validation_type::__NO_OF_ARGUMENTS},
    {115,   "IfcProject",           "RepresentationContexts",   7,      0,NULL,         enum_validation_type::__ARRGEGATION_EXPECTED},
    {115,   "IfcProject",           NULL,                       -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {116,   "IfcProject",           "OwnerHistory",             1,      0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {120,   "IfcOwnerHistory",      "OwningUser",               0,      0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {121,   "IfcOwnerHistory",      "OwningUser",               0,      0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {122,   "IfcOwnerHistory",      "OwningUser",               0,      0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {123,   "IfcOwnerHistory",      "CreationDate",             7,      0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {124,   "IfcOwnerHistory",      "OwningUser",               0,      0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {125,   "IfcOwnerHistory",      "ChangeAction",             3,      0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {126,   "IfcOwnerHistory",      "OwningUser",               0,      0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {130,   "IfcMeasureWithUnit",   "ValueComponent",           0,      0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {170,   "IfcUnitAssignment",    "Units",                    0,      1,r4,           enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {170,   "IfcUnitAssignment",    "Units",                    0,      1,r6,           enum_validation_type::__ARGUMENT_PRIM_TYPE},
    {171,   "IfcUnitAssignment",    "Units",                    0,      1,r4,           enum_validation_type::__ARGUMENT_PRIM_TYPE},
    {172,   "IfcUnitAssignment",    "Units",                    0,      1,r4,           enum_validation_type::__AGGREGATION_NOT_EXPECTED},
    {230,   "IfcSite",              "RefLatitude",              9,      0,NULL,         enum_validation_type::__AGGREGATION_SIZE},
    {230,   "IfcSite",              NULL,                       -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {231,   "IfcSite",              "RefLatitude",              9,      0,NULL,         enum_validation_type::__AGGREGATION_SIZE},
    {231,   "IfcSite",              NULL,                       -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {162,   "IfcApplication",       NULL,                       -1,     0,NULL,         enum_validation_type::__UNIQUE_RULE},
    {163,   "IfcApplication",       NULL,                       -1,     0,NULL,         enum_validation_type::__UNIQUE_RULE},
    {51,    "IfcProdcutDefinitionShape",NULL,                   -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {151,   "IfcProdcutDefinitionShape","ShapeOfProduct",       -1,     0,NULL,         enum_validation_type::__AGGREGATION_SIZE},
    {151,   "IfcProdcutDefinitionShape",NULL,                   -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {85,    "IfcPropertyEnumeration","EnumerationValues",        1,     1,r1,           enum_validation_type::__AGGREGATION_NOT_EXPECTED},
    {-1,    NULL,                       NULL,                   -1,     0,NULL,         enum_validation_type::__WHERE_RULE}
};

static IssueInfo rExpectedIssuesIFC2x3_LimitCount[] =
{
    {24,    "IfcLocalPlacement",            "PlacesObject",         -1,     0,NULL,         enum_validation_type::__AGGREGATION_SIZE},
    {51,    "IfcProductDefinitionShape",    "Representations",      2,      1,r3,           enum_validation_type::__REFERENCE_EXISTS},
    {51,    "IfcProdcutDefinitionShape",    NULL,                   -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {52,    "IfcShapeRepresentation",       "OfProductRepresentation", -1,  0,NULL,         enum_validation_type::__AGGREGATION_SIZE},
    {52,    "IfcShapeRepresentation",       NULL,                   -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
//    {74,    "IfcPolyLoop",                  "Polygon",              0,      1,r1,           enum_validation_type::__REFERENCE_EXISTS}
};

static IssueInfo rExpectedIssuesIFC2x3_LimitTime[] =
{
    {-2, NULL,NULL,0,NULL,0,enum_validation_type::__NONE} //cannot allocate an array of constant size 0
};

static IssueInfo rExpectedIssuesIFC2x3_once[] =
{
    //id   class                    attrName                    ind     aggrLev/aggrInd         Issue
    {24,    "IfcLocalPlacement",    "PlacesObject",             -1,     0,NULL,         enum_validation_type::__AGGREGATION_SIZE},
    {84,    "IFCCARTESIANPOINTLIST2D",NULL,                     -1,     0,NULL,         enum_validation_type::__KNOWN_ENTITY},
    {51,    "IfcProductDefinitionShape","Representations",      2,      1,r3,           enum_validation_type::__REFERENCE_EXISTS},
    {52,    "IfcShapeRepresentation",       NULL,                   -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {110,   "IfcProject",           "GlobalId",                 0,      0,NULL,         enum_validation_type::__REQUIRED_ARGUMENTS},
    {111,   "IfcProject",           "ObjectType",               4,      0,NULL,         enum_validation_type::__STAR_USAGE},
//    {112,   "IfcProject",           "OwnerHistory",             1,      0,NULL,         enum_validation_type::__AGGREGATION_NOT_EXPECTED},
    {115,   "IfcProject",           "RepresentationContexts",   7,      0,NULL,         enum_validation_type::__ARRGEGATION_EXPECTED},
    {115,   "IfcProject",           NULL,                       -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {116,   "IfcProject",           "OwnerHistory",             1,      0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {170,   "IfcUnitAssignment",    "Units",                    0,      1,r6,           enum_validation_type::__ARGUMENT_PRIM_TYPE},
    {162,   "IfcApplication",       NULL,                       -1,     0,NULL,         enum_validation_type::__UNIQUE_RULE},
    {74,    "IfcPolyLoop",          NULL,                       -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {113,   "IfcProject",           NULL,                       -1,     0,NULL,         enum_validation_type::__NO_OF_ARGUMENTS},
//    {113,   "IfcProject",           NULL,                       -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {230,   "IfcSite",              NULL,                       -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {51,    "IfcProdcutDefinitionShape",NULL,                   -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {85,    "IfcPropertyEnumeration","EnumerationValues",        1,     1,r1,           enum_validation_type::__AGGREGATION_NOT_EXPECTED},
    {-1,    NULL,                       NULL,                   -1,     0,NULL,         enum_validation_type::__WHERE_RULE}
};

static IssueInfo rExpectedIssuesIFC4[] =
{
    {2,     "IfcSite",                  "Decomposes",           -1,     1,r1,           enum_validation_type::__AGGREGATION_UNIQUE},
    {2,     "IfcSite",                  "Decomposes",           -1,     0,NULL,         enum_validation_type::__AGGREGATION_SIZE},
    {2,     "IfcSite",                  NULL,                   -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {8,     "IfcWall",                  NULL,                   -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {14,    "IfcShapeRepresentation",   "ContextOfItems",       0,      0,NULL,         enum_validation_type::__REQUIRED_ARGUMENTS},
    {14,    "IfcShapeRepresentation",   NULL,                   -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {29,    "IfcIndexedPolyCurve",      "Segments",             1,      2,r32,          enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {29,    "IfcIndexedPolyCurve",      "Segments",             1,      1,r2,           enum_validation_type::__AGGREGATION_SIZE},
    {29,    "IfcIndexedPolyCurve",      NULL,                   -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {7,     "IfcRelAggregates",         "RelatedObjects",       5,      1,r2,           enum_validation_type::__REFERENCE_EXISTS},
    {3,     "IfcRelAggregates",         "RelatedObjects",       5,      1,r2,           enum_validation_type::__AGGREGATION_UNIQUE},
    {19,    "IfcSpatialStructureElement",NULL,                  -1,     0,NULL,         enum_validation_type::__ABSTRACT_ENTITY},
    {19,    "IfcSpatialStructureElement",NULL,                  -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {2,     "IfcRoot",                   NULL,                  -1,     0,NULL,         enum_validation_type::__UNIQUE_RULE},
    {21,    "IfcPropertyListValue",      NULL,                  -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {21,    "IfcPropertyListValue",      "ListValues",          2,      1,r7,           enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {21,    "IfcPropertyListValue",      "ListValues",          2,      1,r9,           enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {17,    "IfcBlobTexture",            NULL,                  -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {22,    "IfcIndexedColourMap",       "MappedTo",            0,      0,NULL,         enum_validation_type::__REQUIRED_ARGUMENTS},
    {22,    "IfcIndexedColourMap",       "Colours",             2,      0,NULL,         enum_validation_type::__REQUIRED_ARGUMENTS},
    {22,    "IfcIndexedColourMap",       "ColourIndex",         3,      0,NULL,         enum_validation_type::__WHERE_RULE},
    {24,    "IfcMeasureWithUnit",        "ValueComponent",      0,      0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {25,    "IfcMeasureWithUnit",        "ValueComponent",      0,      0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {39,    "IfcIndexedPolyCurve",       "Segments",            1,      1,r2,           enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {39,    "IfcIndexedPolyCurve",       "Segments",            -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {27,    "IfcPropertyEnumeration",    "EnumerationValues",   1,      1,r5,           enum_validation_type::__AGGREGATION_UNIQUE}
};

static IssueInfo rExpectedIssuesIFC4_ExcludeRules[] =
{
    {2,     "IfcSite",                  "Decomposes",           -1,     1,r1,           enum_validation_type::__AGGREGATION_UNIQUE},
    {2,     "IfcSite",                  "Decomposes",           -1,     0,NULL,         enum_validation_type::__AGGREGATION_SIZE},
    {14,    "IfcShapeRepresentation",   "ContextOfItems",       0,      0,NULL,         enum_validation_type::__REQUIRED_ARGUMENTS},
    {29,    "IfcIndexedPolyCurve",      "Segments",             1,      2,r32,          enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {29,    "IfcIndexedPolyCurve",      "Segments",             1,      1,r2,           enum_validation_type::__AGGREGATION_SIZE},
    {7,     "IfcRelAggregates",         "RelatedObjects",       5,      1,r2,           enum_validation_type::__REFERENCE_EXISTS},
    {3,     "IfcRelAggregates",         "RelatedObjects",       5,      1,r2,           enum_validation_type::__AGGREGATION_UNIQUE},
    {19,    "IfcSpatialStructureElement",NULL,                  -1,     0,NULL,         enum_validation_type::__ABSTRACT_ENTITY},
    {21,    "IfcPropertyListValue",      "ListValues",          2,      1,r7,           enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {21,    "IfcPropertyListValue",      "ListValues",          2,      1,r9,           enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {22,    "IfcIndexedColourMap",       "MappedTo",            0,      0,NULL,         enum_validation_type::__REQUIRED_ARGUMENTS},
    {22,    "IfcIndexedColourMap",       "Colours",             2,      0,NULL,         enum_validation_type::__REQUIRED_ARGUMENTS},
    {24,    "IfcMeasureWithUnit",        "ValueComponent",      0,      0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {25,    "IfcMeasureWithUnit",        "ValueComponent",      0,      0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {39,    "IfcIndexedPolyCurve",       "Segments",            1,      1,r2,           enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {27,    "IfcPropertyEnumeration",    "EnumerationValues",   1,      1,r5,           enum_validation_type::__AGGREGATION_UNIQUE}
};

static IssueInfo rExpectedIssuesIFC4x3[] =
{
    {19,      "IfcRelContainedInSpatialStructure",  NULL,                 -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {21,      "IfcSIUnit",                          NULL,                 -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {23,      "IfcSIUnit",                          NULL,                 -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {17,      "IfcGeometricRepresentationContext",  NULL,                 -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {138,     "IfcCurveSegment",                    "SegmentLength",       3,     0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {143,     "IfcShapeRepresentation",             NULL,                 -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {1173,    "IfcRailwayPart",                     "PredefinedType",     10,     0,NULL,         enum_validation_type::__ARGUMENT_EXPRESS_TYPE},
    {1173,    "IfcRailwayPart",                     NULL,                 -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {1173,    "IfcRailwayPart",                     NULL,                 -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {1176,    "IfcShapeRepresentation",             NULL,                 -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {1181,    "IfcRailwayPart",                     NULL,                 -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {1181,    "IfcRailwayPart",                     NULL,                 -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {1184,    "IfcShapeRepresentation",             NULL,                 -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {1189,    "IfcRailwayPart",                     NULL,                 -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {1189,    "IfcRailwayPart",                     NULL,                 -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {1192,    "IfcShapeRepresentation",             NULL,                 -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
    {1560,    "IfcPresentationLayerWithStyle",      NULL,                 -1,     0,NULL,         enum_validation_type::__WHERE_RULE},
};

static void TestInvalidParameters()
{
    printf("\t<TestInvalidParameters>\n");

    ValidationResults checks = validateModel((int_t)&checks);
    auto issue = validateGetFirstIssue(checks);
    auto level = validateGetIssueLevel(issue);
    ASSERT(level == 100000);
    ASSERT(!validateGetNextIssue(issue));
    printf("\t\t<Finished errorLevel='%lld' />\n", level);
    validateFreeResults(checks);

    checks = validateInstance((int_t)checks);
    issue = validateGetFirstIssue(checks);
    level = validateGetIssueLevel(issue);
    ASSERT(level == 100000);
    ASSERT(!validateGetNextIssue(issue));
    printf("\t\t<Finished errorLevel='%lld' />\n", level);
    validateFreeResults(checks);

    printf("\t</TestInvalidParameters>\n");
}

static void CheckModelTest(const char* file, IssueInfo* rExpectedIssues, int nExpectedIssues, enum_validation_status status)
{
    //ENTER_TEST_NAME(file);

    std::string modelPath("..\\TestData\\");
    modelPath += file;

    CheckExpectedIssuses log(rExpectedIssues, nExpectedIssues);
    auto result = CheckModel(modelPath.c_str(), NULL, &log, status);
    
    //all expected issues are reported
    for (int i = 0; i < nExpectedIssues; i++) {
        ASSERT(rExpectedIssues[i].stepId == -2);
    }
}

static void CompositeTests()
{
    printf(" <CompositeTests>\n");

    TestInvalidParameters();

    CheckModelTest("ModelCheckerIFC2x3.ifc", rExpectedIssuesIFC2x3, _countof(rExpectedIssuesIFC2x3), enum_validation_status::__COMPLETE_ALL);
    CheckModelTest("ModelCheckerIFC4.ifc", rExpectedIssuesIFC4, _countof(rExpectedIssuesIFC4), enum_validation_status::__COMPLETE_ALL);
    CheckModelTest("ModelCheckerIFC4x3.ifc", rExpectedIssuesIFC4x3, _countof(rExpectedIssuesIFC4x3), enum_validation_status::__COMPLETE_ALL);
 
    uint64_t issueTypesAll = validateGetOptions(NULL, NULL, NULL, 0);
    ASSERT(issueTypesAll == 0xFFFF);

    validateSetOptions(0, 4, false, 0, 0);
    CheckModelTest("ModelCheckerIFC2x3.ifc", rExpectedIssuesIFC2x3_LimitCount, _countof(rExpectedIssuesIFC2x3_LimitCount), enum_validation_status::__COUNT_EXCEED);

    int_t sec;
    int_t cnt;
    bool  showOnce;
    auto issueTypes = validateGetOptions(&sec, &cnt, &showOnce, 0);
    ASSERT(sec == 0 && cnt == 4 && !showOnce && issueTypes == issueTypesAll);

    validateSetOptions(1, -1, false, 0, 0);
    CheckModelTest("ModelCheckerIFC2x3.ifc", rExpectedIssuesIFC2x3_LimitTime, _countof(rExpectedIssuesIFC2x3_LimitTime), enum_validation_status::__TIME_EXCEED);

    issueTypes = validateGetOptions(&sec, &cnt, &showOnce, ~(uint64_t(0)));
    ASSERT(sec == 1 && cnt == -1 && !showOnce && issueTypes == issueTypesAll);

    validateSetOptions(-1, -1, false, 0, uint64_t(enum_validation_type::__UNIQUE_RULE) | uint64_t(enum_validation_type::__WHERE_RULE));
    CheckModelTest("ModelCheckerIFC4.ifc", rExpectedIssuesIFC4_ExcludeRules , _countof(rExpectedIssuesIFC4_ExcludeRules), enum_validation_status::__COMPLETE_NOT_ALL);

    issueTypes = validateGetOptions(&sec, &cnt, &showOnce, uint64_t(enum_validation_type::__UNIQUE_RULE) | uint64_t(enum_validation_type::__ABSTRACT_ENTITY));
    ASSERT(sec == -1 && cnt == -1 && issueTypes == uint64_t(enum_validation_type::__ABSTRACT_ENTITY));

    validateSetOptions(-1, -1, true, ~0, ~0);
    issueTypes = validateGetOptions(&sec, &cnt, &showOnce, ~(uint64_t(0)));
    ASSERT(sec == -1 && cnt == -1 && showOnce && issueTypes == issueTypesAll);
    CheckModelTest("ModelCheckerIFC2x3.ifc", rExpectedIssuesIFC2x3_once, _countof(rExpectedIssuesIFC2x3_once), enum_validation_status::__COMPLETE_NOT_ALL);

    validateSetOptions(-1, -1, false, ~0, ~0);
    printf(" </CompositeTests>\n");
}


static std::string GetSchemaPath(const char* schemaName)
{
    std::map<std::string, std::string> map;

    if (map.empty()) {
        map["IFC2X3"] = "";
        map["IFC4"] = "";
        map["IFC4X3_RC4"] = R"(..\TestData\schemas\IFC4x3_RC4.exp)";
        map["IFC4X3_ADD1"] = R"(..\TestData\schemas\IFC4x3_ADD1.exp)";
        map["IFC4X3_ADD2"] = "";
    }
    
    auto it = map.find(schemaName);

    ASSERT(it != map.end());

    return it->second;
}

static void PassOrFailCheck(const char* filePath)
{
    printf("\t<PassOrFailCheck file='%s'>\n", filePath);

    bool expectFail = false;
    if (strstr(filePath, "fail")) {
        expectFail = true;
        ASSERT(!strstr(filePath, "pass"));
    }
    else {
        ASSERT(strstr(filePath, "pass"));
    }

    //read header and schema
    SdaiModel model = sdaiOpenModelBN(NULL, filePath, NULL);
    ASSERT(model);

    const char* schemaName = nullptr;        
    GetSPFFHeaderItem(model, 9, 0, sdaiSTRING, &schemaName);
    ASSERT(schemaName);
    
    std::string schemaPath = GetSchemaPath(schemaName);

    printf("\t\t<Schema name='%s' path='%s'/>\n", schemaName, schemaPath.c_str());
    
    sdaiCloseModel(model);
   
    model = sdaiOpenModelBN(NULL, filePath, schemaPath.c_str());
    ASSERT(model);

    auto checks = validateModel(model);
    ASSERT(validateGetStatus(checks) == enum_validation_status::__COMPLETE_ALL);

    int_t result = 0;
    PrintIssue log;
    for (auto issue = validateGetFirstIssue(checks); issue; issue = validateGetNextIssue(issue)) {
        log.OnIssue(issue);
        result = max(result, validateGetIssueLevel(issue));
    }
    validateFreeResults(checks);

    if (expectFail == (result > 0)) {
        printf("\t\t<CheckResult>OK</CheckResult>\n");
    }
    else {
        printf("\t\t<CheckResult>TEST NOT PASSED</CheckResult>\n");
        ASSERT(0);
    }
    
    sdaiCloseModel(model);

    printf("\t</PassOrFailCheck>\n");
}

static void PassOrFailTests(const char* subfolder)
{
    ValidationIssueLevel res = 0;

    auto directory = std::filesystem::path{ "..\\TestData" };
    directory.append(subfolder);
    
    auto modelPathWC = directory;
    modelPathWC.append("*.ifc");

    WIN32_FIND_DATA ffd;
    auto hFind = FindFirstFile(modelPathWC.c_str(), &ffd);
    ASSERT(hFind != INVALID_HANDLE_VALUE);

    do {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            //_tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
        }
        else {
            std::filesystem::path filePath(directory);
            filePath.append(ffd.cFileName);
            PassOrFailCheck(filePath.string().c_str());
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);
}

static void PassOrFailTests()
{
    printf(" <PassOrFailTests>\n");

    PassOrFailTests("IfcOpenShell\\validate");
    PassOrFailTests("IfcOpenShell\\rules");    
    PassOrFailTests("IFC4x3");

    printf(" <PassOrFailTests>\n");
}

extern void ModelCheckerTests()
{
    ENTER_TEST;

    printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    printf("<RDFExpressModelChecker>\n");

    CompositeTests();
    PassOrFailTests();

    printf("</RDFExpressModelChecker>\n");
}