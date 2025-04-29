
#include "pch.h"

using namespace IFC2x3;

static void DeleteEntities(SdaiModel model, const char* entity)
{
    auto aggr = xxxxGetEntityAndSubTypesExtentBN(model, entity);
    int i = 0;
    ExpressID prev = 0;
    while (auto inst = sdaiGetAggrByIndex(aggr, i)) {
        auto id = internalGetP21Line(inst);
        if (id == prev){
            i++;
        }
        else {
            prev = id;
            sdaiDeleteInstance(inst);
        }
    }
}

static void ExtractArchiCAD()
{
    auto model = sdaiOpenModelBN(0, "W:\\DevArea\\RDF\\Valler_IFC_ARK_Fail.ifc", "");
    ASSERT(model);

    auto products = xxxxGetEntityAndSubTypesExtentBN(model, "IfcProduct");
    int i = 0;
    while (auto prod = sdaiGetAggrByIndex(products, i)) {
        auto id = internalGetP21Line(prod);
        if (id == 187356) {
            CalculateInstance(prod);
            i++;
        }
        else {
            sdaiDeleteInstance(prod);
        }
    }

    DeleteEntities(model, "IfcPropertySet");
    DeleteEntities(model, "IfcProperty");

    sdaiSaveModelBN(model, "ExtractDeepLeftBranch.ifc");

    sdaiCloseModel(model);
}

static void CalculateExample(const char* filePath)
{
    auto model = sdaiOpenModelBN(0, filePath, "");
    ASSERT(model);

    auto products = xxxxGetEntityAndSubTypesExtentBN(model, "IfcProduct");
    int i = 0;
    while (auto prod = sdaiGetAggrByIndex(products, i++)) {
        auto id = internalGetP21Line(prod);
        CalculateInstance(prod);
    }

    sdaiCloseModel(model);
}

const int step = 100;

static IfcBlock CreateCube(SdaiModel model, double x, double y, double z)
{
    double xyz[3];
    xyz[0] = 0;
    xyz[1] = 0;
    xyz[2] = 0;

    auto pt = IfcCartesianPoint::Create(model);
    pt.put_Coordinates(xyz, 3);

    auto position = IfcAxis2Placement3D::Create(model);
    position.put_Location(pt);

    auto block = IfcBlock::Create(model);
    block.put_XLength(x);
    block.put_YLength(y);
    block.put_ZLength(z);
    block.put_Position(position);

    return block;
}

struct Node
{
    double x;
    double y;
    double z;
    IfcBooleanResult inst;
};

typedef std::list<Node> NodeList;

static IfcBooleanResult CreateNode(SdaiModel model, double x, double y, double z, NodeList& list)
{
    list.push_back(Node());
    list.back().x = x;
    list.back().y = y;
    list.back().z = z;
    list.back().inst = IfcBooleanResult::Create(model);
    list.back().inst.put_Operator(IfcBooleanOperator::UNION);
    return list.back().inst;
}

const int TREE_LEVELS = 5; // 28; // 2800;

static IfcRepresentationItem CreateUnionCubes(SdaiModel model, int mode)
{
    NodeList currentLevel;
    auto outer = CreateNode(model, step, step, step, currentLevel);

    bool left = true;
    bool right = false;

    for (int i = 0; i <= TREE_LEVELS; i++) {

        ASSERT(currentLevel.size());

        NodeList nextLevel;

        for (auto current : currentLevel) {

            if (i < TREE_LEVELS && nextLevel.size() < 5) {
                switch (mode) {
                case -1: left = true; right = false; break;
                case 1: left = false; right = true;  break;
                case 0: left = !left; right = !left; break;
                case 2: left = true; right = true; break;

                case 3:
                    left = rand() % 2;
                    right = rand() % 2;
                    if (!left && !right) {
                        left = currentLevel.size() % 2;
                        right = !left;
                    }
                }
            }
            else {
                left = false;
                right = false;
            }

            if (left) {
                auto operand = CreateNode(model, current.x + step, current.y, current.z + step, nextLevel);
                current.inst.put_FirstOperand().put_IfcBooleanResult(operand);
            }
            else {
                auto operand = CreateCube(model, current.x + step, current.y, current.z);
                current.inst.put_FirstOperand().put_IfcCsgPrimitive3D(operand);
            }

            if (right) {
                auto operand = CreateNode(model, current.x, current.y + step, current.z + step, nextLevel);
                current.inst.put_SecondOperand().put_IfcBooleanResult(operand);
            }
            else {
                auto operand = CreateCube(model, current.x, current.y + step, current.z);
                current.inst.put_SecondOperand().put_IfcCsgPrimitive3D(operand);
            }

        }

        std::swap(currentLevel, nextLevel);        
    }
    
    ASSERT(!currentLevel.size());

    return outer;
}

static IfcHalfSpaceSolid CreateHalfSpaceSolid(SdaiModel model, double angle)
{
    double components[3] = { sin(angle), cos(angle), 0 };

    auto normal = IfcDirection::Create(model);
    normal.put_DirectionRatios(components, 3);

    components[0] *= 50;
    components[1] *= 50;

    auto location = IfcCartesianPoint::Create(model);
    location.put_Coordinates(components, 3);

    auto position = IfcAxis2Placement3D::Create(model);
    position.put_Axis(normal);
    position.put_Location(location);

    auto plane = IfcPlane::Create(model);
    plane.put_Position(position);

    auto solid = IfcHalfSpaceSolid::Create(model);
    solid.put_BaseSurface(plane);
    solid.put_AgreementFlag(false);

    return solid;
}

static IfcRepresentationItem CreateDeepClip(SdaiModel model, int mode)
{
    auto block = CreateCube(model, 100, 100, 100);

    auto clip = IfcBooleanClippingResult::Create(model);
    clip.put_FirstOperand().put_IfcCsgPrimitive3D(block);;

    auto plane = CreateHalfSpaceSolid(model, 0);
    clip.put_SecondOperand().put_IfcHalfSpaceSolid(plane);

    double PI2 = 3.1415 / 2;
    double step = PI2 / TREE_LEVELS;
    for (double angle = step; angle < PI2; angle += step) {

        auto next = IfcBooleanClippingResult::Create(model);
        next.put_FirstOperand().put_IfcBooleanResult(clip);

        auto plane = CreateHalfSpaceSolid(model, angle);
        next.put_SecondOperand().put_IfcHalfSpaceSolid(plane);

        clip = next;
    }

    return clip;
}

static IfcRepresentationItem CreateSolid(SdaiModel model, int mode)
{
    if (mode < 4)
        return CreateUnionCubes(model, mode);
    else
        return CreateDeepClip(model, mode);
}

static void CreateGeometry(int_t model, IfcWall wall, int mode)
{
    auto solid = CreateSolid(model, mode);

    SetOfIfcRepresentationItem lstReprItems;
    lstReprItems.push_back(solid);

    auto shapeRepr = IfcShapeRepresentation::Create(model);
    shapeRepr.put_RepresentationIdentifier("Body");
    shapeRepr.put_RepresentationType("CSG");
    shapeRepr.put_Items(lstReprItems);

    ListOfIfcRepresentation lstRepr;
    lstRepr.push_back(shapeRepr);

    auto prodShape = IfcProductDefinitionShape::Create(model);
    prodShape.put_Representations(lstRepr);

    wall.put_Representation(prodShape);
}

static void SetupAggregation(int_t model, IfcObjectDefinition aggregator, IfcObjectDefinition part)
{
    auto aggregate = IfcRelAggregates::Create(model);
    aggregate.put_RelatingObject(aggregator);

    SetOfIfcObjectDefinition aggregated;
    aggregated.push_back(part);
    aggregate.put_RelatedObjects(aggregated);
}

static void SetupContainment(int_t model, IfcSpatialStructureElement spatialElement, IfcProduct product)
{
    auto contain = IfcRelContainedInSpatialStructure::Create(model);
    contain.put_RelatingStructure(spatialElement);

    SetOfIfcProduct products;
    products.push_back(product);
    contain.put_RelatedElements(products);
}

static void CreateTreeModel(const char* fileName, int mode, const char* schema)
{
    SdaiModel model = sdaiCreateModelBN(schema);
    ASSERT(model);

    //spatial structure
    //
    auto project = IfcProject::Create(model);
    project.put_GlobalId("1o1ykWxGT4ZxPjHNe4gayR");
    project.put_Name("HelloWall project");
    project.put_Description("Example to demonstract early-binding abilities");

    auto ifcSite = IfcSite::Create(model);
    ifcSite.put_Name("HelloWall site");
    SetupAggregation(model, project, ifcSite);

    auto ifcBuilding = IfcBuilding::Create(model);
    ifcBuilding.put_Name("HelloWall building");
    SetupAggregation(model, ifcSite, ifcBuilding);

    auto ifcStory = IfcBuildingStorey::Create(model);
    ifcStory.put_Name("My first storey");
    SetupAggregation(model, ifcBuilding, ifcStory);

    //wall
    //
    auto wall = IfcWall::Create(model);
    wall.put_GlobalId("2o1ykWxGT4ZxPjHNe4gayR");
    wall.put_Name("My wall");
    wall.put_Description("My wall description");

    CreateGeometry(model, wall, mode);

    SetupContainment(model, ifcStory, wall);

    char saveName[256];
    sprintf_s(saveName, "%s_%s", schema, fileName);

    sdaiSaveModelBN(model, saveName);
    sdaiCloseModel(model);
}

static void CreateTreeModels(const char* schema)
{
    CreateTreeModel("SmallLeftTree.ifc", -1, schema);
    CreateTreeModel("SmallRightTree.ifc", 1, schema);
    CreateTreeModel("SmallMiddleTree.ifc", 0, schema);
    CreateTreeModel("SmallFullTree.ifc", 2, schema);
    CreateTreeModel("SmallRandomTree.ifc", 3, schema);
    CreateTreeModel("SmallMultiClip.ifc", 4, schema);
}

static void CalculateModel(const char* ifcPath, const char* binPath)
{
    auto model = sdaiOpenModelBN(0, ifcPath, "");
    ASSERT(model);

    auto products = xxxxGetEntityAndSubTypesExtentBN(model, "IfcWall");
    int i = 0;
    while (auto prod = sdaiGetAggrByIndex(products, i++)) {
        auto id = internalGetP21Line(prod);
        CalculateInstance(prod);
        SaveInstanceTree(prod, binPath);

    }

    sdaiCloseModel(model);
}

static void CalculateModels()
{
    //CalculateModel("W:\\DevArea\\RDF\\RightTree.ifc", "W:\\DevArea\\RDF\\Save_RightTree.bin");
    //CalculateModel("W:\\DevArea\\RDF\\LeftTree.ifc", "W:\\DevArea\\RDF\\Save_LeftTree.bin");

    CalculateModel("IFC4_SmallMultiClip.ifc", "SmallMultiClip.bin");
    CalculateModel("IFC4_SmallLeftTree.ifc", "SmallLeftTree.bin");
    CalculateModel("IFC4_SmallRightTree.ifc", "SmallRightTree.bin");
    CalculateModel("IFC4_SmallMiddleTree.ifc", "SmallMiddleTree.bin");
    CalculateModel("IFC4_SmallFullTree.ifc", "SmallFullTree.bin");
    CalculateModel("IFC4_SmallRandomTree.ifc", "SmallRandomTree.bin");
}

extern void DeepBoolean()
{
    //ExtractArchiCAD();
    //CreateTreeModels("IFC2x3");
    //CreateTreeModels("IFC4");

    CalculateModels();

}