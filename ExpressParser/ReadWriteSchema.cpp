
#include "pch.h"

static void ReadWriteSchema(const char* strExpFile)
{
    ENTER_TEST_NAME(strExpFile);

    std::filesystem::path readFilePath("..\\TestData\\schemas");
    readFilePath.append(strExpFile);

    auto model = sdaiCreateModelBN(1, "", readFilePath.string().c_str());
    ASSERT(model != 0);

    const char* writeFile = "TestWriteSchema.exp";

    FILE* fp = NULL;
    fopen_s(&fp, writeFile, "wt");
    ASSERT(fp);
    
    RDF::ModelChecker::WriteSchema(model, fp);
    fclose(fp);

    sdaiCloseModel(model);
}


extern void ReadWriteSchemaTest()
{
    ReadWriteSchema("IFC4_ADD2_TC1.exp");
    ReadWriteSchema("IFC2X3_TC1.exp");
    ReadWriteSchema("IFC4x1.exp");
    ReadWriteSchema("IFC4x2.exp");
    ReadWriteSchema("IFC4X3_ADD1.exp");
    ReadWriteSchema("IFC4_ADD2_TC1.exp");
    ReadWriteSchema("structural_frame_schema.exp");
    ReadWriteSchema("ap242ed2_mim_lf_v1.101.exp");

}