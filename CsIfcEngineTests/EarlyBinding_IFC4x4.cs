using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

using RDF;

namespace CsIfcEngineTests
{
    class EarlyBinding_IFC4x4 : CsTests.TestBase 
    {
        public static void Test()
        {
            ENTER_TEST();

            var ifcModel = ifcengine.sdaiCreateModelBN(0, (string)null, "IFC4x4");
            ASSERT(ifcModel!=0);
            ifcengine.SetSPFFHeaderItem(ifcModel, 9, 0, ifcengine.sdaiSTRING, "IFC4x4");
            ifcengine.SetSPFFHeaderItem(ifcModel, 9, 1, ifcengine.sdaiSTRING, (string)null);

            //
            var logicalVoxelData = IFC4x4.IfcLogicalVoxelData.Create(ifcModel);
            IFC4x4.LOGICAL_VALUE[] arrSet = { IFC4x4.LOGICAL_VALUE.False, IFC4x4.LOGICAL_VALUE.Unknown, IFC4x4.LOGICAL_VALUE.True };
            logicalVoxelData.put_ValueData(arrSet);

            IFC4x4.ListOfIfcLogical lstGet = logicalVoxelData.ValueData;
            ASSERT_EQ(lstGet, arrSet);

            //
            var voxelGrid = IFC4x4.IfcVoxelGrid.Create(ifcModel);
            bool[] arrSetB = { false, false, true };
            voxelGrid.put_Voxels(arrSetB);

            var lstGetB = voxelGrid.Voxels;
            ASSERT_EQ(lstGetB, arrSetB);

            /// 
            /// 
            ifcengine.sdaiSaveModelBN(ifcModel, "ebTest4x4cs.ifc");
            ifcengine.sdaiCloseModel(ifcModel);

            ifcModel = ifcengine.sdaiOpenModelBN(0, "ebTest4x4cs.ifc", "IFC4x4");

            var entityLogicalVoxelData = ifcengine.sdaiGetEntity(ifcModel, "IfcLogicalVoxelData");
            var extent = ifcengine.sdaiGetEntityExtent(ifcModel, entityLogicalVoxelData);
            var N = ifcengine.sdaiGetMemberCount(extent);
            ASSERT(N == 1);
            for (int i = 0; i < N; i++)
            {

                Int64 inst = 0;
                ifcengine.sdaiGetAggrByIndex(extent, i, ifcengine.sdaiINSTANCE, out inst);

                lstGet = ((IFC4x4.IfcLogicalVoxelData)(inst)).ValueData;
                ASSERT_EQ(lstGet, arrSet);
            }


            var entityVoxelGrid = ifcengine.sdaiGetEntity(ifcModel, "IfcVoxelGrid");
            extent = ifcengine.sdaiGetEntityExtent(ifcModel, entityVoxelGrid);
            N = ifcengine.sdaiGetMemberCount(extent);
            ASSERT(N == 1);
            for (int i = 0; i < N; i++)
            {

                Int64 inst = 0;
                ifcengine.sdaiGetAggrByIndex(extent, i, ifcengine.sdaiINSTANCE, out inst);

                lstGetB = ((IFC4x4.IfcVoxelGrid)(inst)).Voxels;
                ASSERT_EQ(lstGetB, arrSetB);
            }

            ifcengine.sdaiCloseModel(ifcModel);
        }

    }
}
