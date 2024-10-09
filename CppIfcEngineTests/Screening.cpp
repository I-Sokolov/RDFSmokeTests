#include "pch.h"
#include "engine.h"
#include <Windows.h>


#define DATASET_IFC "C:\\DevArea\\RDF\\TestSets\\IfcEthalons\\"
#define DATASET_AP242 "C:\\DevArea\\RDF\\TestSets\\STEP\\"

static void ScreenFile(const char* pathname)
{
    ENTER_TEST2(pathname);

    SdaiModel model = sdaiOpenModelBN(0, pathname, "");
    ASSERT(model);

    int nprod = 0;
    auto aggr = xxxxGetEntityAndSubTypesExtentBN(model, "IfcProduct");
    auto it = sdaiCreateIterator(aggr);
    while (sdaiNext(it)) {
        SdaiInstance product = NULL;
        sdaiGetAggrByIterator(it, sdaiINSTANCE, &product);

        CalculateInstance(product);

        nprod++;
    }
    sdaiDeleteIterator(it);

    sdaiCloseModel(model);
}

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

static void Screening(const char* folder, const char* ext)
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
                    Screening(subfolder.c_str(), ext);
                }
            }
            else if (ExtensionIs(ffd.cFileName, ext)) {
                std::string pathname(folder);
                pathname.append(ffd.cFileName);
                ScreenFile(pathname.c_str());
            }
     
        } while (FindNextFileA(hFind, &ffd) != 0);

        FindClose(hFind);
    }
}

extern void Screening(void)
{
    Screening(DATASET_AP242, "step");
    Screening(DATASET_IFC, "ifc");
}