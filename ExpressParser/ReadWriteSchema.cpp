
#include "pch.h"

#define IFCENGINE_SCHEMA_WRITER
#include "..\ExpressSchemaWriterAPI.h"

enum class enum_schemas_spff : unsigned char
{
    UNDEFINED = 0,
    IFC151,
    IFC2x,
    IFC20,
    IFC4,
    IFC4x1,
    IFC4x2,
    IFC4x3,
    CIS2,
    AP242,
    CONFIG_CONTROL_DESIGN,
    CONFIG_CONTROL_DESIGN_LINE,
    UNSUPPORTED
};

const char* parsingRead_SetGeneratedSchemaFile(
    enum_schemas_spff	schema,
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


static void ReadWriteSchema(const char* expFileName, const char* embeddedName, enum_schemas_spff generate)
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
    printf("\tcheck comparision with embedded is disabled\n");
        ASSERT(fileEquals); //embedded schema mismatch express file?
}


extern void ReadWriteSchemaTest()
{
    ReadWriteSchema("IFC4_ADD2_TC1.exp", "IFC4", enum_schemas_spff::IFC4);
    ReadWriteSchema("IFC2X3_TC1.exp", "IFC2x3", enum_schemas_spff::IFC2x);
    ReadWriteSchema("IFC4x1.exp", "IFC4x1", enum_schemas_spff::IFC4x1);
    ReadWriteSchema("IFC4x2.exp", "IFC4x2", enum_schemas_spff::IFC4x2);
    ReadWriteSchema("IFC4X3_ADD1.exp", "IFC4x3", enum_schemas_spff::IFC4x3);
    ReadWriteSchema("structural_frame_schema.exp", "CIS2", enum_schemas_spff::CIS2);
    ReadWriteSchema("ap242ed2_mim_lf_v1.101.exp", "AP242", enum_schemas_spff::AP242);
}