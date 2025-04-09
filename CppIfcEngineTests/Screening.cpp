#include "pch.h"
#include "engine.h"
#include <Windows.h>
#include <chrono>


#if 0

#define DATASET_IFC "C:\\DevArea\\RDF\\TestSets\\IfcEthalons\\"
#define DATASET_AP242 "C:\\DevArea\\RDF\\TestSets\\STEP\\"

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
       product = "PRODUCT_DEFINITION";
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

#endif

static void CheckPerformance(const char* path, double size)
{
    printf("File size %g GB\t", size);

    auto start = std::chrono::high_resolution_clock::now();
    
    //
    auto model = sdaiOpenModelBN(0, path, "");
    
    auto read = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> readTime = read - start;
    printf("Read time %g sec\t", readTime.count());

    //
    auto product = sdaiGetEntity(model, "IfcProduct");
    ASSERT(product);

    auto isDefinedBy = sdaiGetAttrDefinition(product, "IsDefinedBy");
    ASSERT(product);

    int_t nProducts = 0;
    int_t nPropset = 0;
    auto aggr = xxxxGetEntityAndSubTypesExtent(model, product);
    auto it = sdaiCreateIterator(aggr);
    while (sdaiNext(it)) {
        nProducts++;

        SdaiInstance product = NULL;
        sdaiGetAggrByIterator(it, sdaiINSTANCE, &product);

        CalculateInstance(product);

        SdaiAggr props = 0;
        sdaiGetAttr(product, isDefinedBy, sdaiAGGR, &props);
        nPropset += sdaiGetMemberCount(props);
    }
    sdaiDeleteIterator(it);

    auto proceed = std::chrono::high_resolution_clock::now();

    //
    std::chrono::duration<double> proceedTime = proceed - read;

    printf ("process time %g sec\t%lld products\t%lld propsets\n", proceedTime.count(), nProducts, nPropset);

    sdaiCloseModel(model);

}

extern void Screening(void)
{
    CheckPerformance(R"(C:\Users\igor\OneDrive - Bentley Systems, Inc\IFC4RV_cert\IFC_files\Beam_01A_ABD.ifc)", 0);
    CheckPerformance(R"(C:\Users\igor\OneDrive - Bentley Systems, Inc\IFC big files\BigIFC\Master-HiRiseScheme.ifc)", 0.15);
    CheckPerformance(R"(C:\Users\igor\OneDrive - Bentley Systems, Inc\IFC big files\BigIFC\RailWaySupports.ifc)", 1.78);
    CheckPerformance(R"(C:\Users\igor\OneDrive - Bentley Systems, Inc\IFC big files\05K-CAB01_2x3_20240305022956.ifc)", 2.13);
    CheckPerformance(R"(C:\Users\igor\OneDrive - Bentley Systems, Inc\IFC big files\972305_Huge_IFC_ImportCrahs\0-general-ow-cj.ifc)", 3.87);
}
