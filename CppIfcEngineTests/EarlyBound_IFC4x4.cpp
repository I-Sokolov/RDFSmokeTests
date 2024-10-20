//
#include "pch.h"


extern void EarlyBound_IFC4x4_test()
{
    ENTER_TEST

    int_t  ifcModel = sdaiCreateModelBN(0, NULL, "IFC4x4");
    ASSERT(ifcModel);
    SetSPFFHeaderItem(ifcModel, 9, 1, sdaiSTRING, (const char*)0);
    SetSPFFHeaderItem(ifcModel, 9, 0, sdaiSTRING, "IFC4x4");

    const char* val = NULL;
    GetSPFFHeaderItem(ifcModel, 9, 1, sdaiSTRING, &val);
    ASSERT(!val);
    GetSPFFHeaderItem(ifcModel, 9, 0, sdaiSTRING, &val);
    ASSERT(0==strcmp(val, "IFC4x4"));

    //
    auto logicalVoxelData = IFC4x4::IfcLogicalVoxelData::Create(ifcModel);
    IFC4x4::LOGICAL_VALUE arrSet[] = { IFC4x4::LOGICAL_VALUE::False, IFC4x4::LOGICAL_VALUE::Unknown, IFC4x4::LOGICAL_VALUE::True };
    logicalVoxelData.put_ValueData(arrSet, 3);

    IFC4x4::ListOfIfcLogical lstGet;
    logicalVoxelData.get_ValueData(lstGet);
    ASSERT(lstGet.size() == 3 && lstGet.front()==arrSet[0] && lstGet.back()==arrSet[2]);

    //
    auto voxelGrid = IFC4x4::IfcVoxelGrid::Create(ifcModel);
    bool arrSetB[] = { false,false,true };
    voxelGrid.put_Voxels(arrSetB, 3);

    std::vector<bool> lstGetB;
    voxelGrid.get_Voxels(lstGetB);
    ASSERT(lstGetB.size() == 3 && !lstGetB[0] && !lstGetB[1] && lstGetB[2]);

    /// 
    /// 
    sdaiSaveModelBN(ifcModel, "ebTest4x4.ifc");
    sdaiCloseModel(ifcModel);

    ifcModel = sdaiOpenModelBN(NULL, "ebTest4x4.ifc", "IFC4x4");

    auto entityLogicalVoxelData = sdaiGetEntity(ifcModel, "IfcLogicalVoxelData");
    int_t* extent = sdaiGetEntityExtent(ifcModel, entityLogicalVoxelData);
    auto N = sdaiGetMemberCount(extent);
    ASSERT(N == 1);
    for (int i = 0; i < N; i++) {

        int_t inst = 0;
        sdaiGetAggrByIndex(extent, i, sdaiINSTANCE, &inst);

        lstGet.clear();
        IFC4x4::IfcLogicalVoxelData(inst).get_ValueData(lstGet);
        ASSERT(lstGet.size() == 3 && lstGet.front() == arrSet[0] && lstGet.back() == arrSet[2]);
    }


    auto entityVoxelGrid = sdaiGetEntity(ifcModel, "IfcVoxelGrid");
    extent = sdaiGetEntityExtent(ifcModel, entityVoxelGrid);
    N = sdaiGetMemberCount(extent);
    ASSERT(N == 1);
    for (int i = 0; i < N; i++) {

        int_t inst = 0;
        sdaiGetAggrByIndex(extent, i, sdaiINSTANCE, &inst);

        lstGetB.clear();
        IFC4x4::IfcVoxelGrid(inst).get_Voxels(lstGetB);
        ASSERT(lstGetB.size() == 3 && !lstGetB[0] && !lstGetB[1] && lstGetB[2]);
    }

    sdaiCloseModel(ifcModel);
}
