
#include "pch.h"

using namespace IFC2x3;

const int TREE_LEVELS = 4; // 2800;


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

static IfcBlock CreateCuboid(SdaiModel model, double x, double y, double z, double posx = 0)
{
    double xyz[3];
    xyz[0] = posx;
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
                auto operand = CreateCuboid(model, current.x + step, current.y, current.z);
                current.inst.put_FirstOperand().put_IfcCsgPrimitive3D(operand);
            }

            if (right) {
                auto operand = CreateNode(model, current.x, current.y + step, current.z + step, nextLevel);
                current.inst.put_SecondOperand().put_IfcBooleanResult(operand);
            }
            else {
                auto operand = CreateCuboid(model, current.x, current.y + step, current.z);
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
    auto block = CreateCuboid(model, 100, 100, 100);

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

static IfcRepresentationItem CreateBooleanSolid(SdaiModel model, int mode)
{
    if (mode < 4)
        return CreateUnionCubes(model, mode);
    else
        return CreateDeepClip(model, mode);
}

static void CreateRepresentation(SdaiModel model, IfcWall wall, IfcRepresentationItem body)
{
    SetOfIfcRepresentationItem lstReprItems;
    lstReprItems.push_back(body);

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

static IfcBuildingStorey CreateSpatialStructure(SdaiModel model)
{
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

    return ifcStory;
}

static void CreateWall(SdaiModel model, IfcBuildingStorey storey, IfcRepresentationItem body)
{
    auto wall = IfcWall::Create(model);
    wall.put_GlobalId("2o1ykWxGT4ZxPjHNe4gayR");
    wall.put_Name("My wall");
    wall.put_Description("My wall description");

    CreateRepresentation(model, wall, body);

    SetupContainment(model, storey, wall);
}

static void CreateTreeModel(const char* fileName, int mode, const char* schema)
{
    SdaiModel model = sdaiCreateModelBN(schema);
    ASSERT(model);

    auto story = CreateSpatialStructure(model);
    auto solid = CreateBooleanSolid(model, mode);

    CreateWall(model, story, solid);

    char saveName[256];
    sprintf_s(saveName, "%s_%s_%s", TREE_LEVELS < 10 ? "Short" : "Deep", schema, fileName);

    sdaiSaveModelBN(model, saveName);
    sdaiCloseModel(model);
}

static void CreateTreeModels(const char* schema)
{
    CreateTreeModel("LeftTree.ifc", -1, schema);
    CreateTreeModel("RightTree.ifc", 1, schema);
    CreateTreeModel("MiddleTree.ifc", 0, schema);
    CreateTreeModel("FullTree.ifc", 2, schema);
    CreateTreeModel("RandomTree.ifc", 3, schema);
    CreateTreeModel("MultiClip.ifc", 4, schema);
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
        //SaveInstanceTree(prod, binPath);
    }

    sdaiCloseModel(model);
}

static void CalculateModels()
{
    if (TREE_LEVELS < 10) {
        CalculateModel("Short_IFC2x3_MultiClip.ifc", "IFC2x3_SmallMultiClip.bin");
        CalculateModel("Short_IFC2x3_LeftTree.ifc", "IFC2x3_SmallLeftTree.bin");
        CalculateModel("Short_IFC2x3_RightTree.ifc", "IFC2x3_SmallRightTree.bin");
        CalculateModel("Short_IFC2x3_MiddleTree.ifc", "IFC2x3_SmallMiddleTree.bin");
        CalculateModel("Short_IFC2x3_FullTree.ifc", "IFC2x3_SmallFullTree.bin");
        CalculateModel("Short_IFC2x3_RandomTree.ifc", "IFC2x3_SmallRandomTree.bin");

        CalculateModel("Short_IFC4_MultiClip.ifc", "IFC4_SmallMultiClip.bin");
        CalculateModel("Short_IFC4_LeftTree.ifc", "IFC4_SmallLeftTree.bin");
        CalculateModel("Short_IFC4_RightTree.ifc", "IFC4_SmallRightTree.bin");
        CalculateModel("Short_IFC4_MiddleTree.ifc", "IFC4_SmallMiddleTree.bin");
        CalculateModel("Short_IFC4_FullTree.ifc", "IFC4_SmallFullTree.bin");
        CalculateModel("Short_IFC4_RandomTree.ifc", "IFC4_SmallRandomTree.bin");
    }
    else {
        CalculateModel("IFC2x3_DeepMultiClip.ifc", "IFC2x3_DeepMultiClip.bin");
        CalculateModel("IFC2x3_DeepLeftTree.ifc", "IFC2x3_DeepLeftTree.bin");
        CalculateModel("IFC2x3_DeepRightTree.ifc", "IFC2x3_DeepRightTree.bin");
        CalculateModel("IFC2x3_DeepMiddleTree.ifc", "IFC2x3_DeepMiddleTree.bin");
        CalculateModel("IFC2x3_DeepFullTree.ifc", "IFC2x3_DeepFullTree.bin");
        CalculateModel("IFC2x3_DeepRandomTree.ifc", "IFC2x3_DeepRandomTree.bin");

        CalculateModel("IFC4_DeepMultiClip.ifc", "IFC4_DeepMultiClip.bin");
        CalculateModel("IFC4_DeepLeftTree.ifc", "IFC4_DeepLeftTree.bin");
        CalculateModel("IFC4_DeepRightTree.ifc", "IFC4_DeepRightTree.bin");
        CalculateModel("IFC4_DeepMiddleTree.ifc", "IFC4_DeepMiddleTree.bin");
        CalculateModel("IFC4_DeepFullTree.ifc", "IFC4_DeepFullTree.bin");
        CalculateModel("IFC4_DeepRandomTree.ifc", "IFC4_DeepRandomTree.bin");
    }
}

static void CreateCyclic(SdaiModel model, IfcBuildingStorey storey, IfcBooleanOperator op2, double pos)
{
    auto bool1 = IfcBooleanResult::Create(model);
    auto bool2 = IfcBooleanResult::Create(model);
    auto block1 = CreateCuboid(model, 100, 100, 100, pos);
    auto block2 = CreateCuboid(model, 110, 110, 110, pos);

    bool1.put_FirstOperand().put_IfcBooleanResult(bool2);
    bool1.put_SecondOperand().put_IfcCsgPrimitive3D(block1);
    bool1.put_Operator(IfcBooleanOperator::INTERSECTION);

    bool2.put_FirstOperand().put_IfcCsgPrimitive3D(block2);
    bool2.put_SecondOperand().put_IfcBooleanResult(bool1);
    bool2.put_Operator(op2);

    CreateWall(model, storey, bool1);
}

static void CreateNull1(SdaiModel model, IfcBuildingStorey storey, IfcBooleanOperator oper, double pos)
{
    auto bool1 = IfcBooleanResult::Create(model);
    auto block1 = CreateCuboid(model, 100, 100, 100, pos);

    bool1.put_SecondOperand().put_IfcCsgPrimitive3D(block1);
    bool1.put_Operator(oper);

    CreateWall(model, storey, bool1);
}

static void CreateNullDeep(SdaiModel model, IfcBuildingStorey storey, IfcBooleanOperator oper, double pos, bool left = true)
{
    auto bool2 = IfcBooleanResult::Create(model);
    bool2.put_Operator(oper);

    if (pos > 0) {
        auto block1 = CreateCuboid(model, 100, 100, 100, pos);
        auto block2 = CreateCuboid(model, 110, 110, 110, pos);
        bool2.put_FirstOperand().put_IfcCsgPrimitive3D(block1);
        bool2.put_SecondOperand().put_IfcCsgPrimitive3D(block2);
    }

    auto bool1 = IfcBooleanResult::Create(model);
    bool1.put_Operator(oper);
    if (left) {
        bool1.put_SecondOperand().put_IfcBooleanResult(bool2);
    }
    else {
        bool1.put_FirstOperand().put_IfcBooleanResult(bool2);
    }

    CreateWall(model, storey, bool1);
}


static void CreateInvalidBooleans()
{
    SdaiModel model = sdaiCreateModelBN("IFC2x3");
    ASSERT(model);

    auto storey = CreateSpatialStructure(model);

    //cyclic
    CreateCyclic(model, storey, IfcBooleanOperator::DIFFERENCE, 0);
    CreateCyclic(model, storey, IfcBooleanOperator::INTERSECTION, 200);

    //null operand 1
    CreateNull1(model, storey, IfcBooleanOperator::UNION, 300);
    CreateNull1(model, storey, IfcBooleanOperator::INTERSECTION, 400);

    //
    auto b = IfcBooleanClippingResult::Create(model);
    b.put_Operator(IfcBooleanOperator::UNION);
    CreateWall(model, storey, b);

    //
    CreateNullDeep(model, storey, IfcBooleanOperator::UNION, 500);
    CreateNullDeep(model, storey, IfcBooleanOperator::INTERSECTION, 600);
    CreateNullDeep(model, storey, IfcBooleanOperator::UNION, -1);
    CreateNullDeep(model, storey, IfcBooleanOperator::INTERSECTION, -1);

    //
    CreateNullDeep(model, storey, IfcBooleanOperator::UNION, 700, false);
    CreateNullDeep(model, storey, IfcBooleanOperator::INTERSECTION, 800, false);

    //
    sdaiSaveModelBN(model, "InvalidBooleans.ifc");
    sdaiCloseModel(model);
}


static void CaluclateInvalidBooleans()
{
    auto model = sdaiOpenModelBN(0, "InvalidBooleans.ifc", "");
    ASSERT(model);

    auto walls = sdaiGetEntityExtentBN(model, "IfcWall");

    //
    auto wall = sdaiGetAggrByIndex(walls, 0);
    ASSERT(wall);
    CalculateInstance(wall);
    double boxm[3];
    double boxM[3];
    auto res = GetBoundingBox(wall, boxm, boxM);
    ASSERT(res);

    wall = sdaiGetAggrByIndex(walls, 1);
    ASSERT(wall);
    CalculateInstance(wall);
    res = GetBoundingBox(wall, boxm, boxM);
    ASSERT(!res);

    wall = sdaiGetAggrByIndex(walls, 2);
    ASSERT(wall);
    CalculateInstance(wall);
    res = GetBoundingBox(wall, boxm, boxM);
    ASSERT(res);

    wall = sdaiGetAggrByIndex(walls, 3);
    ASSERT(wall);
    CalculateInstance(wall);
    res = GetBoundingBox(wall, boxm, boxM);
    ASSERT(!res);

    wall = sdaiGetAggrByIndex(walls, 4);
    ASSERT(wall);
    CalculateInstance(wall);
    res = GetBoundingBox(wall, boxm, boxM);
    ASSERT(!res);

    wall = sdaiGetAggrByIndex(walls, 5);
    ASSERT(wall);
    CalculateInstance(wall);
    res = GetBoundingBox(wall, boxm, boxM);
    ASSERT(res);

    wall = sdaiGetAggrByIndex(walls, 6);
    ASSERT(wall);
    CalculateInstance(wall);
    res = GetBoundingBox(wall, boxm, boxM);
    ASSERT(!res);

    wall = sdaiGetAggrByIndex(walls, 7);
    ASSERT(wall);
    CalculateInstance(wall);
    res = GetBoundingBox(wall, boxm, boxM);
    ASSERT(!res);

    wall = sdaiGetAggrByIndex(walls, 8);
    ASSERT(wall);
    CalculateInstance(wall);
    res = GetBoundingBox(wall, boxm, boxM);
    ASSERT(!res);

    wall = sdaiGetAggrByIndex(walls, 9);
    ASSERT(wall);
    CalculateInstance(wall);
    res = GetBoundingBox(wall, boxm, boxM);
    ASSERT(res);

    wall = sdaiGetAggrByIndex(walls, 10);
    ASSERT(wall);
    CalculateInstance(wall);
    res = GetBoundingBox(wall, boxm, boxM);
    ASSERT(!res);

    sdaiCloseModel(model);
}


extern void BooleanTest()
{
    ENTER_TEST;
 /*
    //ExtractArchiCAD();
    CreateTreeModels("IFC2x3");
    CreateTreeModels("IFC4");

    CalculateModels();
*/
    CreateInvalidBooleans();
    CaluclateInvalidBooleans();
}