// ModelChecker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#define INDENT "\t\t"

struct IssueHandler
{
    virtual void OnIssue(ValidationIssue* issue) = NULL;
};

static ValidationIssueLevel CheckModel(const char* filePath, const char* expressSchemaFilePath, IssueHandler* pIssueHandler);
//static ValidationIssueLevel CheckModels(const char* filePath, const char* expressSchemaFilePath);

/// <summary>
/// Issue reporting 
/// </summary>
struct PrintIssue : public IssueHandler
{
    virtual void OnIssue(ValidationIssue* issue) override
    {
        printf(INDENT "<Issue");

        auto stepId = validateGetStepId(issue);
        if (stepId > 0) {
            printf(" stepId='#%lld'", stepId);
        }

        auto entity = validateGetEntityName(issue);
        if (entity) {
            printf(" entity='%s'", entity);
        }

        auto attrName = validateGetAttrName(issue);
        if (attrName) {
            printf(" attribute='%s'", attrName);
        }

        auto attrIndex = validateGetAttrIndex(issue);
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

#if 0
static ValidationIssueLevel CheckModels(const char* filePathWC, const char* expressSchemaFilePath)
{
    ValidationIssueLevel res = 0;

    const auto directory = std::filesystem::path {filePathWC}.parent_path();

    WIN32_FIND_DATAA ffd;
    auto hFind = FindFirstFileA(filePathWC, &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("\t\t<Failure call='FindFirstFile'>%s</Failure>\n", filePathWC);
        return -13;
    }

    do {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            //_tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
        }
        else {
            std::filesystem::path filePath(directory);
            filePath.append(ffd.cFileName);
            res += CheckModel (filePath.string().c_str(), expressSchemaFilePath, NULL);
        }
    } while (FindNextFileA(hFind, &ffd) != 0);

    FindClose(hFind);

    return res;
}
#endif

static ValidationIssueLevel CheckModel(const char* filePath, const char* expressSchemaFilePath, IssueHandler* pLog, bool expectedMore)
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
        ASSERT(validateIsComplete(checks) != expectedMore);
        validateFreeResults(checks);
        //sdaiCloseModel(model);
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
    ValidationIssueType         issueType;
};


//
// smoke test run and check
//

struct CheckExpectedIssuses : public PrintIssue
{
    CheckExpectedIssuses(IssueInfo* rExpectedIssuesIFC2x3, int nExpectedIssues) : m_rExpectedIssues(rExpectedIssuesIFC2x3), m_nExpectedIssues(nExpectedIssues) {}

    virtual void OnIssue(ValidationIssue* issue) override;

private:
    IssueInfo* m_rExpectedIssues;
    int m_nExpectedIssues;
};


void CheckExpectedIssuses::OnIssue(ValidationIssue* issue)
{
    //base report
    __super::OnIssue(issue);

    //check issue expected
    bool found = false;
    for (int i = 0; i < m_nExpectedIssues; i++) {
        auto& expected = m_rExpectedIssues[i];
        if (expected.stepId == validateGetStepId (issue) 
            && expected.attrInd == validateGetAttrIndex (issue) 
            && expected.aggrLevel == validateGetAggrLevel (issue)) {

            auto vi = validateGetIssueType(issue);
            ASSERT(expected.issueType == vi);
            
            auto aggrLevel = validateGetAggrLevel(issue);
            auto aggrIndArray = validateGetAggrIndArray(issue);
            for (int i = 0; i < aggrLevel; i++) {
                ASSERT(expected.aggrIndArray[i] == aggrIndArray[i]);
            }
            
            found = true;
            expected.stepId = -1; //mark as reported
            break;
        }
    }
    ASSERT(found);
}

static int_t r1[] = { 1 };
static int_t r2[] = { 2 };
static int_t r3[] = { 3 };
static int_t r4[] = { 4 };
static int_t r6[] = { 6 };
static int_t r7[] = { 7 };
static int_t r9[] = { 9 };
static int_t r32[] = { 3,2 };

static IssueInfo rExpectedIssuesIFC2x3[] =
{
    //id   class                    attrName                    ind     aggrLev/aggrInd         Issue
    {84,    "IFCCARTESIANPOINTLIST2D",NULL,                     -1,     0,NULL,         ValidationIssueType::WrongNumberOfArguments},
    {51,    "IfcProductDefinitionShape","Representations",      2,      1,r3,           ValidationIssueType::UnresolvedReference},
    {74,    "IfcPolyLoop",          "Polygon",                  0,      1,r1,           ValidationIssueType::UnresolvedReference},
    {110,   "IfcProject",           "GlobalId",                 0,      0,NULL,         ValidationIssueType::MissedNonOptionalArgument},
    {111,   "IfcProject",           "ObjectType",               4,      0,NULL,         ValidationIssueType::UnexpectedStar},
    {112,   "IfcProject",           "OwnerHistory",             1,      0,NULL,         ValidationIssueType::UnexpectedAggregation},
    {113,   "IfcProject",           NULL,                       -1,     0,NULL,         ValidationIssueType::WrongNumberOfArguments},
    {114,   "IfcProject",           NULL,                       -1,     0,NULL,         ValidationIssueType::WrongNumberOfArguments},
    {115,   "IfcProject",           "RepresentationContexts",   7,      0,NULL,         ValidationIssueType::ExpectedAggregation},
    {116,   "IfcProject",           "OwnerHistory",             1,      0,NULL,         ValidationIssueType::WrongArgumentType},
    {120,   "IfcOwnerHistory",      "OwningUser",               0,      0,NULL,         ValidationIssueType::WrongArgumentType},
    {121,   "IfcOwnerHistory",      "OwningUser",               0,      0,NULL,         ValidationIssueType::WrongArgumentType},
    {122,   "IfcOwnerHistory",      "OwningUser",               0,      0,NULL,         ValidationIssueType::WrongArgumentType},
    {123,   "IfcOwnerHistory",      "CreationDate",             7,      0,NULL,         ValidationIssueType::WrongArgumentType},
    {124,   "IfcOwnerHistory",      "OwningUser",               0,      0,NULL,         ValidationIssueType::WrongArgumentType},
    {125,   "IfcOwnerHistory",      "ChangeAction",             3,      0,NULL,         ValidationIssueType::WrongArgumentType},
    {126,   "IfcOwnerHistory",      "OwningUser",               0,      0,NULL,         ValidationIssueType::WrongArgumentType},
    {130,   "IfcMeasureWithUnit",   "ValueComponent",           0,      0,NULL,         ValidationIssueType::WrongArgumentType},
    {170,   "IfcUnitAssignment",    "Units",                    0,      1,r4,           ValidationIssueType::WrongArgumentType},
    {170,   "IfcUnitAssignment",    "Units",                    0,      1,r6,           ValidationIssueType::UnexpectedValueType},
    {171,   "IfcUnitAssignment",    "Units",                    0,      1,r4,           ValidationIssueType::UnexpectedValueType},
    {172,   "IfcUnitAssignment",    "Units",                    0,      1,r4,           ValidationIssueType::UnexpectedAggregation},
    {230,   "IfcSite",              "RefLatitude",              9,      0,NULL,         ValidationIssueType::WrongAggregationSize},
    {231,   "IfcSite",              "RefLatitude",              9,      0,NULL,         ValidationIssueType::WrongAggregationSize},
    {6,     "IfcApplication",       "Version",                  1,      0,NULL,         ValidationIssueType::UniqueRuleViolation},
    {51,    "IfcProdcutDefinitionShape",NULL,                   -1,     0,NULL,         ValidationIssueType::WhereRuleViolation}
};

static IssueInfo rExpectedIssuesIFC2x3_LimitCount[] =
{
    {51,    "IfcProductDefinitionShape",    "Representations",      2,      1,r3,           ValidationIssueType::UnresolvedReference},
    {51,    "IfcProdcutDefinitionShape",    NULL,                   -1,     0,NULL,         ValidationIssueType::WhereRuleViolation},
    {74,    "IfcPolyLoop",                  "Polygon",              0,      1,r1,           ValidationIssueType::UnresolvedReference},
    {84,    "IFCCARTESIANPOINTLIST2D",      NULL,                   -1,     0,NULL,         ValidationIssueType::WrongNumberOfArguments},
    {110,   "IfcProject",                   "GlobalId",             0,      0,NULL,         ValidationIssueType::MissedNonOptionalArgument}
};

static IssueInfo rExpectedIssuesIFC2x3_LimitTime[] =
{
    {-1,    "IfcProductDefinitionShape",    "Representations",      2,      1,r3,           ValidationIssueType::UnresolvedReference}
};

static IssueInfo rExpectedIssuesIFC4[] =
{
    {14,    "IfcShapeRepresentation",   "ContextOfItems",       0,      0,NULL,         ValidationIssueType::MissedNonOptionalArgument},
    {14,    "IfcShapeRepresentation",   NULL,                   -1,     0,NULL,         ValidationIssueType::WhereRuleViolation},
    {29,    "IfcIndexedPolyCurve",      "Segments",             1,      2,r32,          ValidationIssueType::WrongArgumentType},
    {29,    "IfcIndexedPolyCurve",      "Segments",             1,      1,r2,           ValidationIssueType::WrongAggregationSize},
    {7,     "IfcRelAggregates",         "RelatedObjects",       5,      1,r2,           ValidationIssueType::UnresolvedReference},
    {3,     "IfcRelAggregates",         "RelatedObjects",       5,      1,r1,           ValidationIssueType::AggrElementValueNotUnique},
    {19,    "IfcSpatialStructureElement",NULL,                  -1,     0,NULL,         ValidationIssueType::AbstractEntity},
    {1,     "IfcRoot",                   "GlobalId",            0,      0,NULL,         ValidationIssueType::UniqueRuleViolation},
    {21,    "IfcPropertyListValue",      NULL,                  -1,     0,NULL,         ValidationIssueType::WhereRuleViolation},
    {21,    "IfcPropertyListValue",      "ListValues",          2,      1,r7,           ValidationIssueType::WrongArgumentType},
    {21,    "IfcPropertyListValue",      "ListValues",          2,      1,r9,           ValidationIssueType::WrongArgumentType},
    {17,    "IfcBlobTexture",            NULL,                  -1,     0,NULL,         ValidationIssueType::WhereRuleViolation}
};

static IssueInfo rExpectedIssuesIFC4x3[] =
{
    {1158,    "IfcPointByDistanceExpression",   "DistanceAlong",       0,       0,NULL,         ValidationIssueType::WhereRuleViolation},
    {19,      "IfcRelContainedInSpatialStructure",NULL,                 -1,     0,NULL,         ValidationIssueType::WhereRuleViolation},
    {17,      "IfcGeometricRepresentationContext",NULL,                 -1,     0,NULL,         ValidationIssueType::WhereRuleViolation}
};

static void TestInvalidParameters()
{
    printf("\t<TestInvalidParameters>\n");

    ValidationResults* checks = validateModel((int_t)&checks);
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

static void CheckModelTest(const char* file, IssueInfo* rExpectedIssues, int nExpectedIssues, bool expectedMore)
{
    //ENTER_TEST_NAME(file);

    std::string modelPath("..\\TestData\\");
    modelPath += file;

    CheckExpectedIssuses log(rExpectedIssues, nExpectedIssues);
    auto result = CheckModel(modelPath.c_str(), NULL, &log, expectedMore);
    
    //all expected issues are reported
    for (int i = 0; i < nExpectedIssues; i++) {
        ASSERT(rExpectedIssues[i].stepId == -1);
    }

    printf("\t<Finished errorLevel='%d' />\n", (int)result);
}


extern void ModelCheckerTests()
{
    ENTER_TEST;

    printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    printf("<RDFExpressModelChecker>\n");


    TestInvalidParameters();

    CheckModelTest("ModelCheckerIFC2x3.ifc", rExpectedIssuesIFC2x3, _countof(rExpectedIssuesIFC2x3), false);
    CheckModelTest("ModelCheckerIFC4.ifc", rExpectedIssuesIFC4, _countof(rExpectedIssuesIFC4), false);
    CheckModelTest("ModelCheckerTESTSWE_UT_LP_4.ifc", rExpectedIssuesIFC4x3, _countof(rExpectedIssuesIFC4x3), false);
    
    validateSetLimits(-1, 4);
    CheckModelTest("ModelCheckerIFC2x3.ifc", rExpectedIssuesIFC2x3_LimitCount, _countof(rExpectedIssuesIFC2x3_LimitCount), true);

    validateSetLimits(0, -1);
    CheckModelTest("ModelCheckerIFC2x3.ifc", rExpectedIssuesIFC2x3_LimitTime, _countof(rExpectedIssuesIFC2x3_LimitTime), true);

    printf("</RDFExpressModelChecker>\n");
}