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

bool    AlphabeticalOrder(size_t i, char chrI, char chrII)
{
    if (chrI == '_')
        return true;

    if (chrI >= '0' && chrI <= '9') {
        if (chrII == '_')
            return false;

        if (chrII >= '0' && chrII <= '9') {
            if (chrI > chrII) return false;
        }

        return true;
    }

    if (chrI >= 'a' && chrI <= 'z') {
        if (chrII == '_')
            return false;

        if (chrII >= '0' && chrII <= '9') {
            return false;
        }

        if (chrII >= 'a' && chrII <= 'z') {
            if (chrI > chrII) return false;
        }

        if (i) {
            if (chrII >= 'A' && chrII <= 'Z') {
                if (chrI > chrII) return false;
            }
        }

        return true;
    }

    if (chrI >= 'A' && chrI <= 'Z') {
        if (chrII >= 'A' && chrII <= 'Z') {
            if (chrI > chrII) return false;
        }

        if (i) {
            if (chrII >= 'a' && chrII <= 'z') {
                return true;
            }
        }

        return true;
    }

    return false;
}

bool    InAlphabeticalOrder(const char * strI, const char * strII)
{
    ASSERT(strI && strII);

    size_t  i = 0;
    while (strI[i] && strII[i]) {
        if (strI[i] != strII[i]) {
            if (AlphabeticalOrder(i, strI[i], strII[i]))
                return true;
            else
                return false;
        }
        i++;
    }

    if (strI[i])
        return false;

    return true;
}

static void CreateInstanceTest()
{
    ENTER_TEST;

    auto model = CreateModel();

    int64_t settings = 0b111111000000001111000001110001ull;
    int64_t mask = 0b11111111111111111111011101110111ull;
    int64_t vertexSize = SetFormat(model, settings, mask);

    OwlClass myClass = GetClassesByIterator(model, 0);
    const char    * prevClassName = nullptr;
    while (myClass) {
        const char    * className = GetNameOfClass(myClass);

        if (prevClassName) {
            ASSERT(InAlphabeticalOrder(prevClassName, className));
        }

        OwlInstance inst = CreateInstance(myClass);

        int64_t NVertex = 0, NIndex = 0;
        CalculateInstance(inst, &NVertex, &NIndex);
        
        if (NVertex && NIndex) {
            std::vector<double> vertex(NVertex * vertexSize);
            std::vector<int64_t> index(NIndex);

            UpdateInstanceVertexBuffer(inst, vertex.data());
            UpdateInstanceIndexBuffer(inst, index.data());
        }
        else {
//            ASSERT(NVertex == 0 && NIndex == 0);
        }

        prevClassName = className;
        myClass = GetClassesByIterator(model, myClass);
    }

    {
        RdfProperty myProperty = GetPropertiesByIterator(model, 0);
        const char  * prevPropertyName = nullptr;
        while (myProperty) {
            const char  * propertyName = GetNameOfProperty(myProperty);

            if (GetPropertyType(myProperty) == OBJECTPROPERTY_TYPE) {
                if (prevPropertyName) {
                    ASSERT(InAlphabeticalOrder(prevPropertyName, propertyName));
                }

                prevPropertyName = propertyName;
            }
            else {
                ASSERT(GetPropertyType(myProperty) == DATATYPEPROPERTY_TYPE_BOOLEAN    ||
                       GetPropertyType(myProperty) == DATATYPEPROPERTY_TYPE_STRING     ||
                       GetPropertyType(myProperty) == DATATYPEPROPERTY_TYPE_INTEGER    ||
                       GetPropertyType(myProperty) == DATATYPEPROPERTY_TYPE_DOUBLE     ||
                       GetPropertyType(myProperty) == DATATYPEPROPERTY_TYPE_BYTE       ||
                       GetPropertyType(myProperty) == DATATYPEPROPERTY_TYPE_CHAR_ARRAY ||
                       GetPropertyType(myProperty) == DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY);
            }

            myProperty = GetPropertiesByIterator(model, myProperty);
        }
    }

    {
        RdfProperty myProperty = GetPropertiesByIterator(model, 0);
        const char  * prevPropertyName = nullptr;
        while (myProperty) {
            const char  * propertyName = GetNameOfProperty(myProperty);

            if (GetPropertyType(myProperty) == DATATYPEPROPERTY_TYPE_BOOLEAN    ||
                GetPropertyType(myProperty) == DATATYPEPROPERTY_TYPE_STRING     ||
                GetPropertyType(myProperty) == DATATYPEPROPERTY_TYPE_INTEGER    ||
                GetPropertyType(myProperty) == DATATYPEPROPERTY_TYPE_DOUBLE     ||
                GetPropertyType(myProperty) == DATATYPEPROPERTY_TYPE_BYTE       ||
                GetPropertyType(myProperty) == DATATYPEPROPERTY_TYPE_CHAR_ARRAY ||
                GetPropertyType(myProperty) == DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY) {
                if (prevPropertyName) {
                    ASSERT(InAlphabeticalOrder(prevPropertyName, propertyName));
                }

                prevPropertyName = propertyName;
            }
            else {
                ASSERT(GetPropertyType(myProperty) == OBJECTPROPERTY_TYPE);
            }

            myProperty = GetPropertiesByIterator(model, myProperty);
        }
    }

    CloseModel(model);
}

extern void EntitiesTests()
{
    TriangulationTest();
    CreateInstanceTest();
}