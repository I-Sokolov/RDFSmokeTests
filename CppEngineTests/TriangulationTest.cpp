#include "pch.h"
using namespace GEOM;

static void ProcessInstance(OwlInstance inst)
{
    auto cls = GetInstanceClass(inst);
    auto clsName = GetNameOfClass(cls);
    printf("Class name %s\n", clsName);

    ClearedInstanceExternalBuffers(inst);

    auto settings = GetFormat(GetModel(inst), GetFormat(0, 0));
    auto vertexSize = GetVertexDataOffset(0, settings);

    int64_t NVertex = 0, NIndex = 0;
    CalculateInstance(inst, &NVertex, &NIndex);

    std::vector<double> vertex (NVertex*vertexSize);
    std::vector<int64_t> index(NIndex);

    UpdateInstanceVertexBuffer(inst, vertex.data());
    UpdateInstanceIndexBuffer(inst, index.data());

    double box[6];
    GetBoundingBox(inst, box, box+3);

    printf("    Box (%g, %g, %g) - (%g, %g, %g)\n", box[0], box[1], box[2], box[3], box[4], box[5]);
}

extern void TriangulationTest()
{
    ENTER_TEST;

    auto model = OpenModel(R"(C:\Users\igor\Downloads\Untitled.bin)");
    ASSERT(model);

    std::string strSettings = "111111000000001111000001110001";

    std::bitset<64> bitSettings(strSettings);
    int64_t iSettings = bitSettings.to_ulong();

    std::string strMask = "11111111111111111111011101110111";
    std::bitset<64> bitMask(strMask);
    int64_t iMask = bitMask.to_ulong();

    iSettings |= FORMAT_EXPORT_TRIANGLES;
    iMask |= FORMAT_EXPORT_TRIANGLES;

    int64_t vertexElementSizeInBytes = SetFormat(model, iSettings, iMask);

    OwlInstance inst = NULL;
    while (inst = GetInstancesByIterator(model, inst)) {

        ProcessInstance(inst);

    }

    CloseModel(model);
}