
#include "pch.h"

#include "..\ExpressSchemaWriterAPI.h"

enum class enum_embedded_schema : unsigned char
{
    NONE,
    IFC2X3_TC1,
    IFC4_ADD2_TC1,
    IFC4X1_FINAL,
    IFC4X2_DRAFT,
    IFC4X3_ADD1,
    AP242,
    CIS2
};

extern __declspec(dllimport) const char* parsingRead_SetGeneratedSchemaFile(
    enum_embedded_schema	schema,
    const char* directory = nullptr
);

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

    parsingRead_SetGeneratedSchemaFile(generate, ".");
    auto model = sdaiCreateModelBN(1, "", readFilePath.string().c_str());
    ASSERT(model != 0);

    std::string writeFile = "WriteSchema_";
    writeFile.append(expFileName);

    bool ok = RDF::ExpressSchemaWriter::WriteSchema(model, writeFile.c_str());
    ASSERT(ok);

    sdaiCloseModel(model);

    //
    // check re-read
    //
    parsingRead_SetGeneratedSchemaFile(enum_embedded_schema::NONE);
    model = sdaiCreateModelBN(2, "", writeFile.c_str());
    ASSERT(model); //writer issue?

    std::string rewriteFile = "ReWriteSchema_";
    rewriteFile.append(expFileName);

    ok = RDF::ExpressSchemaWriter::WriteSchema(model, rewriteFile.c_str());
    ASSERT(ok); //write-read is different from read

    sdaiCloseModel(model);

    bool fileEquals = FileEquals(writeFile, rewriteFile);
    ASSERT(fileEquals); //write issue?

    //printf("..Do not test embedded\n");
    //return; 
    
    //
    // write embedded schema to file
    //
    model = sdaiCreateModelBN(1, "", embeddedName);
    ASSERT(model != 0);

    std::string writeEmbedded = "WriteEmbedded_";
    writeEmbedded.append(embeddedName);
    writeEmbedded.append(".exp");

    ok = RDF::ExpressSchemaWriter::WriteSchema(model, writeEmbedded.c_str());

    //
    //
    fileEquals = FileEquals(writeFile, writeEmbedded);
    ASSERT(fileEquals); //embedded schema mismatch express file?
}


extern void ReadWriteSchemaTest()
{
    ReadWriteSchema("IFC4_ADD2_TC1.exp", "IFC4", enum_embedded_schema::IFC4_ADD2_TC1);
    ReadWriteSchema("IFC2X3_TC1.exp", "IFC2x3", enum_embedded_schema::IFC2X3_TC1);
    ReadWriteSchema("IFC4x1.exp", "IFC4x1", enum_embedded_schema::IFC4X1_FINAL);
    ReadWriteSchema("IFC4x2.exp", "IFC4x2", enum_embedded_schema::IFC4X2_DRAFT);
    ReadWriteSchema("IFC4X3_ADD1.exp", "IFC4x3", enum_embedded_schema::IFC4X3_ADD1);
    ReadWriteSchema("structural_frame_schema.exp", "CIS2", enum_embedded_schema::CIS2);
    ReadWriteSchema("ap242ed2_mim_lf_v1.101.exp", "AP242", enum_embedded_schema::AP242);
}