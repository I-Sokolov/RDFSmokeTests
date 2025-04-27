
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

static IfcRepresentationItem CreateSolid(SdaiModel model, int mode)
{
    NodeList currentLevel;
    auto outer = CreateNode(model, step, step, step, currentLevel);

    bool left = true;
    bool right = false;

    int N = 28; // 00;
    for (int i = 0; i <= N; i++) {

        ASSERT(currentLevel.size());

        NodeList nextLevel;

        for (auto current : currentLevel) {

            if (i < N && nextLevel.size() < 5) {
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


static void CreateGeometry(int_t model, IfcWall wall, int mode)
{
    auto solid = CreateSolid(model, mode);

    SetOfIfcRepresentationItem lstReprItems;
    lstReprItems.push_back(solid);

    auto shapeRepr = IfcShapeRepresentation::Create(model);
    shapeRepr.put_RepresentationIdentifier("Body");
    shapeRepr.put_RepresentationType("SweptSolid");
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

static void CreateTreeModel(const char* fileName, int mode)
{
    SdaiModel model = sdaiCreateModelBN("IFC2x3");
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

    sdaiSaveModelBN(model, fileName);
    sdaiCloseModel(model);
}

static void CreateTreeModels()
{
    CreateTreeModel("SmallLeftTree.ifc", -1);
    CreateTreeModel("SmallRightTree.ifc", 1);
    CreateTreeModel("SmallMiddleTree.ifc", 0);
    CreateTreeModel("SmallFullTree.ifc", 2);
    CreateTreeModel("SmallRandomTree.ifc", 3);
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

    CalculateModel("SmallLeftTree.ifc", "SmallLeftTree.bin");
    CalculateModel("SmallRightTree.ifc", "SmallRightTree.bin");
    CalculateModel("SmallMiddleTree.ifc", "SmallMiddleTree.bin");
    CalculateModel("SmallFullTree.ifc", "SmallFullTree.bin");
    CalculateModel("SmallRandomTree.ifc", "SmallRandomTree.bin");
}

extern void DeepBoolean()
{
    //ExtractArchiCAD();
    //CreateTreeModels();
    CalculateModels();

}