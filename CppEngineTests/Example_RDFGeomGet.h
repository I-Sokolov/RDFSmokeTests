#pragma once

/// <summary>
/// This example gets B-Rep of an instance, print it and returns some statistic information.
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

extern void Example_RDFGeomGet(OwlInstance inst, SomeStatistic& stat);
