
#include "pch.h"

#define IFCENGINE_SCHEMA_WRITER
#include "..\ExpressSchemaWriterAPI.h"

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


static void ReadWriteSchema(const char* expFileName, const char* embeddedName)
{
    ENTER_TEST_NAME(expFileName);

    //
    // read and write schema from file
    //
    std::filesystem::path readFilePath("..\\TestData\\schemas");
    readFilePath.append(expFileName);

    auto model = sdaiCreateModelBN(1, "", readFilePath.string().c_str());
    ASSERT(model != 0);

    std::string writeFile = "WriteSchema_";
    writeFile.append(expFileName);

    bool ok = RDF::ExpressSchemaWriter::WriteSchema(model, writeFile.c_str());
    ASSERT(ok);

    sdaiCloseModel(model);

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
    bool fileEquals = FileEquals(writeFile, writeEmbedded);
    ASSERT(fileEquals); //embedded schema mismatch express file?
}


extern void ReadWriteSchemaTest()
{
    ReadWriteSchema("IFC4_ADD2_TC1.exp", "IFC4");
    ReadWriteSchema("IFC2X3_TC1.exp", "IFC2x3");
    ReadWriteSchema("IFC4x1.exp", "IFC4x1");
    ReadWriteSchema("IFC4x2.exp", "IFC4x2");
    ReadWriteSchema("IFC4X3_ADD1.exp", "IFC4x3");
    ReadWriteSchema("structural_frame_schema.exp", "CIS2");
    ReadWriteSchema("ap242ed2_mim_lf_v1.101.exp", "AP242");
}