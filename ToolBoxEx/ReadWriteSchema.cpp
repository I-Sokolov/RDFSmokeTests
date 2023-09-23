
#include "pch.h"


enum class enum_embedded_schema : unsigned char
{
    NONE,
    IFC2X3_TC1,
    IFC4_ADD2_TC1,
    IFC4X1_FINAL,
    IFC4X2_DRAFT,
    IFC4X3_ADD1,
    IFC4X4,
    AP242,
    AP214,
    AP203,
    CIS2
};


extern __declspec(dllimport) const char* parsingReadSchema_SetGeneratedSchemaFile(
    enum_embedded_schema	schema,
    const char* directory = nullptr
);

extern __declspec(dllimport) bool ParseFunctionsSmokeTest(int_t model);


static bool FileEquals(std::string& file1, std::string& file2)
{
    FILE* fp1 = NULL;
    fopen_s(&fp1, file1.c_str(), "rt");
    ASSERT(fp1);

    FILE* fp2 = NULL;
    fopen_s(&fp2, file2.c_str(), "rt");
    ASSERT(fp2);

    int ch1;
    while ((ch1 = fgetc(fp1)) != EOF) {
        auto ch2 = fgetc(fp2);
        if (ch2 != ch1) {
            return false;
        }
    }

    if (fgetc(fp2) != EOF) //file2 bigger?
    {
        return false;
    }

    fclose(fp1);
    fclose(fp2);

    return true;
}


static void ReadWriteSchema(const char* expFileName, const char* embeddedName, enum_embedded_schema generate)
{
    ENTER_TEST_NAME(expFileName);

    //
    // read and write schema from file
    //
    std::filesystem::path readFilePath("..\\TestData\\schemas");
    readFilePath.append(expFileName);

    auto generatedFile = parsingReadSchema_SetGeneratedSchemaFile(generate, ".");
    auto model = sdaiCreateModelBN(1, "", readFilePath.string().c_str());
    ASSERT(model != 0);
    ASSERT(ParseFunctionsSmokeTest(model));

    std::string writeFile = "WriteSchema_";
    writeFile.append(expFileName);

    bool ok = engiSaveSchemaBN(model, writeFile.c_str());
    ASSERT(ok);

    sdaiCloseModel(model);

    //
    // check re-read
    //
    parsingReadSchema_SetGeneratedSchemaFile(enum_embedded_schema::NONE);
    model = sdaiCreateModelBN(2, "", writeFile.c_str());
    ASSERT(model); //writer issue?
    ASSERT(ParseFunctionsSmokeTest(model));

    std::string rewriteFile = "ReWriteSchema_";
    rewriteFile.append(expFileName);

    ok = engiSaveSchemaBN(model, rewriteFile.c_str());
    ASSERT(ok); //write-read is different from read

    sdaiCloseModel(model);

    bool fileEquals = FileEquals(writeFile, rewriteFile);
    ASSERT(fileEquals); //write issue?
    
    printf(" ... compared to rewrite\n");
    
    if (generatedFile) {
        printf("GENERATION MODE - DO NOT DO COMPARISION TESTS FOR EMBEDDED SCHEMA\n");
        return; //>>>>>>>>>>>>>>
    }

    //
    // write embedded schema to file
    //
    model = sdaiCreateModelBN(1, "", embeddedName);
    ASSERT(model != 0);
    ASSERT(ParseFunctionsSmokeTest(model));

    std::string writeEmbedded = "WriteEmbedded_";
    writeEmbedded.append(embeddedName);
    writeEmbedded.append(".exp");

    ok = engiSaveSchemaBN(model, writeEmbedded.c_str());

    //
    //
    fileEquals = FileEquals(writeFile, writeEmbedded);
    ASSERT(fileEquals); //embedded schema mismatch express file?
    printf(" ... compared to embedded\n");
}


extern void ReadWriteSchemaTest()
{
    ReadWriteSchema("IFC4_ADD2_TC1.exp", "IFC4", enum_embedded_schema::IFC4_ADD2_TC1);
    ReadWriteSchema("IFC2X3_TC1.exp", "IFC2x3", enum_embedded_schema::IFC2X3_TC1);
    ReadWriteSchema("IFC4x1.exp", "IFC4x1", enum_embedded_schema::IFC4X1_FINAL);
    ReadWriteSchema("IFC4x2.exp", "IFC4x2", enum_embedded_schema::IFC4X2_DRAFT);
    ReadWriteSchema("IFC4X3_ADD2.exp", "IFC4x3", enum_embedded_schema::IFC4X3_ADD1);
    ReadWriteSchema("IFC4X4.exp", "IFC4x4", enum_embedded_schema::IFC4X4);
    ReadWriteSchema("structural_frame_schema.exp", "CIS2", enum_embedded_schema::CIS2);
    ReadWriteSchema("ap242ed3_mim_lf_v1.152.exp", "AP242", enum_embedded_schema::AP242);
    ReadWriteSchema("AP214E3_2010.exp", "AP214", enum_embedded_schema::AP214);
    ReadWriteSchema("part403ts_wg3n2635mim_lf.exp", "AP203", enum_embedded_schema::AP203);
}