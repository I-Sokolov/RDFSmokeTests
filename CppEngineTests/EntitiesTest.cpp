#include "pch.h"
using namespace GEOM;

static void TriangulationTest()
{
    ENTER_TEST;

    auto model = OpenModel("..\\TestData\\Triangulation.bin");
    ASSERT(model && GetInstancesByIterator(model, NULL));

    int64_t settings = 0b111111000000001111000001110001ull;
    int64_t mask = 0b11111111111111111111011101110111ull;
    SetFormat(model, settings, mask);

    int cnt = 0;
    OwlInstance inst = NULL;
    while (inst = GetInstancesByIterator(model, inst)) {
        auto cls = GetInstanceClass(inst);
        auto clsName = GetNameOfClass(cls);
        if (0 == strcmp(clsName, "Triangulation")) {
            cnt++;

            ClearedInstanceExternalBuffers(inst);

            auto settings = GetFormat(GetModel(inst), GetFormat(0, 0));
            auto vertexSize = GetVertexDataOffset(0, settings);

            int64_t NVertex = 0, NIndex = 0;
            CalculateInstance(inst, &NVertex, &NIndex);
            ASSERT(NVertex == 96 && NIndex == 480);

            std::vector<double> vertex(NVertex * vertexSize);
            std::vector<int64_t> index(NIndex);

            UpdateInstanceVertexBuffer(inst, vertex.data());
            UpdateInstanceIndexBuffer(inst, index.data());

            double box[6];
            GetBoundingBox(inst, box, box + 3);
            //printf("    Box (%g, %g, %g) - (%g, %g, %g)\n", box[0], box[1], box[2], box[3], box[4], box[5]);
            double ebox[6] = { 25, 0, 0, 125, 100, 100 };
            ASSERT_ARR_EQ(box, ebox, 6);
        }
    }

    ASSERT(cnt == 1);

    CloseModel(model);
}

extern void EntitiesTests()
{
    TriangulationTest();
}