#include "pch.h"
#include "ToolBoxEx.h"

#define TEST_DIR "..\\TestData\\DataFiles"

struct DataFileContent
{
    std::map<int, std::string> instances;
};

static void SkipSpaces(const char*& pch)
{
    while (*pch && isspace(*pch)) {
        pch++;
    }
}

static double ExtractNumber(const char*& pch)
{
    char* endPtr = nullptr;
    double num = strtod(pch, &endPtr);
    if (pch == endPtr) {
        ASSERT(!"Expected a number but not found");
        return 0;
    }
    pch = endPtr;
    return num;
}

static void CompareInstanceString(const std::string& inst1, const std::string& inst2)
{
    auto pch1 = inst1.c_str();
    auto pch2 = inst2.c_str();

    bool literal = false;
   
    while (*pch1 && *pch2) {

        if (!literal) {
            SkipSpaces(pch1);
            SkipSpaces(pch2);
        }

        if (isdigit(*pch1)) {
            auto num1 = ExtractNumber(pch1);
            auto num2 = ExtractNumber(pch2);
            ASSERT (fabs(num1 - num2) < 1e-11);
        }
        else {
            ASSERT(*pch1 == *pch2);

            if (*pch1 == '\'') {
                if (!literal) {
                    literal = true;
                }
                else if (*(pch1 + 1) == '\'') { 
                    // double-quote '' in literal
                    ASSERT(*(pch2 + 1) == '\'');
                    pch1++;
                    pch2++;
                }
                else {
                    literal = false;
                }
            }

            pch1++;
            pch2++;
        }
    }

    ASSERT(!*pch1 && !*pch2);
}

static void CompareFileContent(const DataFileContent& data1, const DataFileContent& data2)
{
    auto it1 = data1.instances.begin();
    auto it2 = data2.instances.begin();

    while (it1 != data1.instances.end() && it2 != data2.instances.end()) {
        
        ASSERT (it1->first == it2->first);

        CompareInstanceString(it1->second, it2->second);

        ++it1;
        ++it2;
    }

    ASSERT(it1 == data1.instances.end() && it2 == data2.instances.end());
}

static bool IsInstance(const std::string& line)
{
    auto pch = line.c_str();
    SkipSpaces(pch);
    return (*pch == '#');
}

static bool InstanceFinished(const std::string& str)
{
    //is literal finished
    bool literal = false;
    for (auto pch = str.c_str(); *pch; pch++) {
        if (*pch == '\'') {
            if (!literal) {
                literal = true;
            }
            else if (*(pch+1) == '\'') {
                pch++; // double-quote '' in literal
            }
            else {
                literal = false;
            }
        }
    }

    if (literal) {
        return false; //>>>
    }

    auto last = str.length() - 1;
    
    while (last > 0 && isspace(str[last]))
        last--;

    if (str[last] != ';') {
        return false; //>>>
    }

    last--;
    while (last > 0 && isspace(str[last]))
        last--;

    if (str[last] != ')') {
        return false; //>>>
    }

    return true;
}

static bool NextLine(std::ifstream& file, std::string& line)
{
    if (!std::getline(file, line)) {
        return false;
    }

    if (IsInstance(line)) {
        //read multi-line instance
        while (!InstanceFinished(line)) {
            std::string piece;
            if (!std::getline(file, piece)) {
                ASSERT(!"Unexpected end of file while reading instance");
                return false;
            }
            line += piece;
        }
    }

    return true;
}

static void ReadDataFile(const char* filePath, DataFileContent& content)
{
    std::ifstream file(filePath);
    ASSERT(file.is_open());

    std::string line;
    while (NextLine(file, line)) {

        if (IsInstance(line)) {
            auto pos = line.find('=');
            ASSERT(pos != std::string::npos);

            auto idStr = line.substr(1, pos-1);
            int id = std::stoi(idStr);

            auto contentStr = line.substr(pos + 1);

            content.instances[id] = contentStr;
        }
    }
}

static void CompareFiles(const char* file1, const char* file2)
{
    DataFileContent content1;
    ReadDataFile(file1, content1);

    DataFileContent content2;
    ReadDataFile(file2, content2);

    CompareFileContent(content1, content2);
}   

static void TestDataFile(std::filesystem::path readPath)
{
    ENTER_TEST_NAME(readPath.string().c_str());


    auto model = sdaiOpenModelBN(1, readPath.string().c_str(), "");
    ASSERT(model);

    auto savePath = readPath.filename();

    sdaiSaveModelBN(model, savePath.string().c_str());

    auto apiSavePath = savePath;
    auto ext = ".api" + savePath.extension().string();
    apiSavePath.replace_extension(ext);

    SaveModel(model, apiSavePath.string().c_str());

    sdaiCloseModel(model);

    CompareFiles(savePath.string().c_str(), readPath.string().c_str());
    CompareFiles(apiSavePath.string().c_str(), readPath.string().c_str());

}

static void TestDataFiles(std::string dir)
{
    WIN32_FIND_DATAA ffd;
    auto hFind = FindFirstFileA((dir + "\\*").c_str(), &ffd);
    ASSERT(hFind != INVALID_HANDLE_VALUE);

    do {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0) {
                TestDataFiles(dir + "\\" + ffd.cFileName);
            }
        }
        else {
            TestDataFile(dir + "\\" + ffd.cFileName);
        }
    } while (FindNextFileA(hFind, &ffd) != 0);

    FindClose(hFind);

}

extern void ReadWriteDataFileTest()
{
    //TestDataFile(R"(..\TestData\DataFiles\ComplexInstance2.step)");
    TestDataFiles(TEST_DIR);
}