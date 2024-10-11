#include "pch.h"
#include "engine.h"
#include <Windows.h>


#define DATASET_IFC "C:\\DevArea\\RDF\\TestSets\\IfcEthalons\\"
#define DATASET_AP242 "C:\\DevArea\\RDF\\TestSets\\STEP\\"

extern __declspec(dllimport) void SmokeTest_AuxDataCount(int_t model, int_t& nInstances, int_t& nComplex, int_t& nRdfReference, int_t& nExternalReference);

struct Statistic
{
    int_t nInstances = 0;
    int_t nComplex = 0;
    int_t nRdfReferences = 0;
    int_t nExtReferences = 0;
};

static bool ExtensionIs(const char* name, const char* ext)
{
    size_t pos = strlen(name) - strlen(ext);

    if (pos < 1) {
        return false;
    }

    if (name[pos - 1] != '.') {
        return false;
    }

    return 0 == _stricmp(name + pos, ext);
}

static bool IsIFC(const char* name)
{
    return ExtensionIs(name, "ifc");
}

static bool IsSTEP(const char* name)
{
    return ExtensionIs(name, "step");
}

static void ScreenFile(const char* pathname, Statistic& stat)
{
    ENTER_TEST2(pathname);

    SdaiModel model = sdaiOpenModelBN(0, pathname, "");
    ASSERT(model);

    const char* product = NULL;
    if (IsIFC(pathname)) {
        product = "IfcProduct";
    }
    else if (IsSTEP(pathname)) {
       // product = "PRODUCT_DEFINITION";
    }

    if (product){
        auto aggr = xxxxGetEntityAndSubTypesExtentBN(model, product);
        auto it = sdaiCreateIterator(aggr);
        while (sdaiNext(it)) {
            SdaiInstance product = NULL;
            sdaiGetAggrByIterator(it, sdaiINSTANCE, &product);

            CalculateInstance(product);

        }
        sdaiDeleteIterator(it);
    }

    SmokeTest_AuxDataCount(model, stat.nInstances, stat.nComplex, stat.nRdfReferences, stat.nExtReferences);

    sdaiCloseModel(model);
}

static void Screening(const char* folder, Statistic& stat)
{
    ENTER_TEST2(folder);

    std::string wildcard(folder);
    wildcard.append("*");

    WIN32_FIND_DATAA ffd;
    auto hFind = FindFirstFileA(wildcard.c_str(), &ffd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (strcmp(ffd.cFileName, ".") && strcmp(ffd.cFileName, "..")) {
                    std::string subfolder(folder);
                    subfolder.append(ffd.cFileName);
                    subfolder.append("\\");
                    Screening(subfolder.c_str(), stat);
                }
            }
            else if (IsIFC(ffd.cFileName) || IsSTEP(ffd.cFileName)) {
                std::string pathname(folder);
                pathname.append(ffd.cFileName);
                ScreenFile(pathname.c_str(), stat);
            }
     
        } while (FindNextFileA(hFind, &ffd) != 0);

        FindClose(hFind);
    }
}

static void PrintStatistic(Statistic& stat, const char* name)
{
    printf("Statistic for %s dataset:\n", name);

    printf("  Total instances: %lld\n", stat.nInstances);
    printf("    Have RDF ref: %lld (%g%%)\n", stat.nRdfReferences, 100.0 * double(stat.nRdfReferences) / double(stat.nInstances));
    printf("    Have ext ref: %lld (%g%%)\n", stat.nExtReferences, 100.0 * double(stat.nExtReferences) / double(stat.nInstances));
    printf("    Have sub:     %lld (%g%%)\n", stat.nComplex, 100.0 * double(stat.nComplex) / double(stat.nInstances));
}

extern void Screening(void)
{
    Statistic statIfc, statStep;

    Screening(DATASET_AP242, statStep);
    Screening(DATASET_IFC, statIfc);

    PrintStatistic(statIfc, "IFC");
    PrintStatistic(statStep, "STEP");
}