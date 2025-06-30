#include "pch.h"

#include "rdfgeom.h"

/// <summary>
/// Example: get B-Rep of an instance
/// The example gets shell of instance, print it and returns some statistic information
/// </summary>
struct SomeStatistic
{
    int_t   numOfPoints;
    int_t   numOfConceptualFaces;
    int_t   numOfFaces;
    int_t   numOfVertices;

    VECTOR3 sumOfCoordinates;

    void AddPoint(const VECTOR3& pt) { sumOfCoordinates.x += pt.x; sumOfCoordinates.y += pt.y; sumOfCoordinates.z += pt.z; }
    void AddPoint(const VECTOR2& pt) { sumOfCoordinates.x += pt.u; sumOfCoordinates.y += pt.v; }
};

static void ExamplePrintVertex(CONCEPTUAL_FACE* cface, STRUCT_VERTEX* vertex, SomeStatistic& stat, int_t level, int_t i)
{
    std::string offset(level * 2, ' ');
    printf("%s[%d]: ", offset.c_str(), i);
    stat.numOfVertices++;

   int_t index = rdfgeom_vertex_GetPointIndex(vertex);
   printf("index %d ", index);
   
   //this is index of point in the shell of conceptual face owner
   if (OwlInstance owner = rdfgeom_cface_GetInstance(cface)) {
       if (SHELL* ownerShell = rdfgeom_GetBRep(owner)) {

           if (VECTOR3* points = rdfgeom_GetPoints(ownerShell)) {
               VECTOR3 pt = points[index];
               printf("non-transformed coordinates %g, %g, %g ", pt.x, pt.y, pt.z);
           }

           if (VECTOR3* normals = rdfgeom_GetNormals(ownerShell)) {
               VECTOR3 n = normals[index];
               printf("normal %g, %g, %g ", n.x, n.y, n.z);
           }

           if (VECTOR2* textures = rdfgeom_GetTextureCoordinates(ownerShell)) {
               VECTOR2 t = textures[index];
               printf("texture %g, %g ", t.u, t.v);
           }
       }
   }

   printf("\n");
}

static void ExamplePrintFace(CONCEPTUAL_FACE* cface, STRUCT_FACE* face, SomeStatistic& stat, int_t level)
{
    std::string offset(level * 2, ' ');
    printf("%sFace:\n", offset.c_str());
    stat.numOfFaces++;

    printf("%s Boundary:\n", offset.c_str());
    STRUCT_VERTEX** vertexPP = rdfgeom_face_GetBoundary(face);
    int_t i = 0;
    while (!rdfgeom_vertex_EndOfList(vertexPP)) {
        ExamplePrintVertex(cface, *vertexPP, stat, level + 1, i++);
        vertexPP = rdfgeom_vertex_GetNext(*vertexPP);
    }

    printf("%s Openings:\n", offset.c_str());
    STRUCT_FACE** openingPP = rdfgeom_face_GetOpenings(face);
    while (!rdfgeom_face_EndOfList(openingPP)) {
        ExamplePrintFace(cface, *openingPP, stat, level + 1);
        openingPP = rdfgeom_face_GetNext(*openingPP);
    }
}

static void ExamplePrintConceptualFace(CONCEPTUAL_FACE* cface, SomeStatistic& stat, int_t level)
{
    std::string offset(level * 2, ' ');
    printf("%sConceptual face:\n", offset.c_str()); 
    stat.numOfConceptualFaces++;

    MATRIX* T = rdfgeom_cface_GetLocalTranformation(cface);
    if (T) {
        printf("%s Local transform\n", offset.c_str());
        printf("%s  %g  %g  %g  %g\n", offset.c_str(), T->_11, T->_21, T->_31, T->_41);
        printf("%s  %g  %g  %g  %g\n", offset.c_str(), T->_12, T->_22, T->_32, T->_42);
        printf("%s  %g  %g  %g  %g\n", offset.c_str(), T->_13, T->_23, T->_33, T->_43);
    }

    //
    printf("%s Faces:\n", offset.c_str());
    STRUCT_FACE** facePP = rdfgeom_cface_GetFaces(cface);
    while (!rdfgeom_face_EndOfList(facePP)) {
        ExamplePrintFace(cface, *facePP, stat, level + 1);
        facePP = rdfgeom_face_GetNext(*facePP);
    }

    //
    printf("%s Vertices:\n", offset.c_str());
    STRUCT_VERTEX** vertexPP = rdfgeom_cface_GetVertices(cface);
    int_t i = 0;
    while (!rdfgeom_vertex_EndOfList(vertexPP)) {
        ExamplePrintVertex(cface, *vertexPP, stat, level + 1, i++);
        vertexPP = rdfgeom_vertex_GetNext(*vertexPP);
    }

    //
    printf("%s Children conceptual faces:\n", offset.c_str());
    CONCEPTUAL_FACE** childPP = rdfgeom_cface_GetChildren(cface);
    while (!rdfgeom_cface_EndOfList(childPP)) {
        ExamplePrintConceptualFace(*childPP, stat, level + 1);
        childPP = rdfgeom_cface_GetNext(*childPP);
    }
}

extern void ExamplePrintShell(OwlInstance inst, SomeStatistic& stat)
{
    //This example prints B-Rep of the inst

    //instance must be calculated to prepare shell 
    CalculateInstance(inst);

    if (SHELL* shell = rdfgeom_GetBRep(inst)) {

        //
        // Points of the shell
        //
        int_t numOfPoints = rdfgeom_GetNumOfPoints(shell);
        
        stat.numOfPoints += numOfPoints;

        //
        printf("Points coordinates:\n");
        if (VECTOR3* points = rdfgeom_GetPoints(shell)) {
            for (int_t i = 0; i < numOfPoints; i++) {

                VECTOR3& pt = points[i];
                printf(" P[%d]: %g, %g, %g\n", i, pt.x, pt.y, pt.z);
                stat.AddPoint(pt);
            }
        }

        //
        if (VECTOR3* normals = rdfgeom_GetNormals(shell)) {
            printf("Normals:\n");
            for (int_t i = 0; i < numOfPoints; i++) {

                VECTOR3& n = normals[i];
                printf(" N[%d]: %g, %g, %g\n", i, n.x, n.y, n.z);
                stat.AddPoint(n);
            }
        }
        else {
            printf("Normals are not provided\n");
        }

        //
        if (VECTOR2* textures = rdfgeom_GetTextureCoordinates(shell)) {
            printf("Texture coordinates:\n");
            for (int_t i = 0; i < numOfPoints; i++) {

                VECTOR2& uv = textures[i];
                printf(" t[%d]: %g, %g\n", i, uv.u, uv.v);
                stat.AddPoint(uv);
            }
        }
        else {
            printf("Texture is not provided\n");
        }

        //
        // Faces
        //
        CONCEPTUAL_FACE** cfacePP = rdfgeom_GetConceptualFaces(shell);
        while (!rdfgeom_cface_EndOfList(cfacePP)) {
            ExamplePrintConceptualFace(*cfacePP, stat, 0);
            cfacePP = rdfgeom_cface_GetNext(*cfacePP);
        }
    }
}

/// <summary>
/// 
/// </summary>
extern void Test_Example_RDFGeomGet()
{
    ENTER_TEST;

    OwlModel model = OpenModel("..\\TestData\\Cube.bin");
    ASSERT(model);

    OwlClass clsCube = GetClassByName(model, "Cube");
    ASSERT(clsCube);

    OwlInstance inst = NULL;
    while (inst = GetInstancesByIterator(model, inst)) {

        if (IsInstanceOfClass(inst, clsCube)) {

            SomeStatistic stat;
            memset(&stat, 0, sizeof(SomeStatistic));

            ExamplePrintShell(inst, stat);

            ASSERT(stat.numOfPoints == 8 && stat.numOfConceptualFaces == 6 && stat.numOfFaces == 6 && stat.numOfVertices == 30);
            ASSERT(stat.sumOfCoordinates.x == 28 && stat.sumOfCoordinates.y == 28 && stat.sumOfCoordinates.z == 28);
        }
    }

    CloseModel(model);
}