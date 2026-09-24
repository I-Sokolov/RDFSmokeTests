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

            var ifcModel = IFCEngine.sdaiCreateModelBN(0, (string)null, "IFC4x4");
            ASSERT(ifcModel!=0);
            IFCEngine.SetSPFFHeaderItem(ifcModel, 9, 0, IFCEngine.sdaiSTRING, "IFC4x4");
            IFCEngine.SetSPFFHeaderItem(ifcModel, 9, 1, IFCEngine.sdaiSTRING, (string)null);

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
            IFCEngine.sdaiSaveModelBN(ifcModel, "ebTest4x4cs.ifc");
            IFCEngine.sdaiCloseModel(ifcModel);

            ifcModel = IFCEngine.sdaiOpenModelBN(0, "ebTest4x4cs.ifc", "IFC4x4");

            var entityLogicalVoxelData = IFCEngine.sdaiGetEntity(ifcModel, "IfcLogicalVoxelData");
            var extent = IFCEngine.sdaiGetEntityExtent(ifcModel, entityLogicalVoxelData);
            var N = IFCEngine.sdaiGetMemberCount(extent);
            ASSERT(N == 1);
            for (int i = 0; i < N; i++)
            {

                Int64 inst = 0;
                IFCEngine.sdaiGetAggrByIndex(extent, i, IFCEngine.sdaiINSTANCE, out inst);

                lstGet = ((IFC4x4.IfcLogicalVoxelData)(inst)).ValueData;
                ASSERT_EQ(lstGet, arrSet);
            }


            var entityVoxelGrid = IFCEngine.sdaiGetEntity(ifcModel, "IfcVoxelGrid");
            extent = IFCEngine.sdaiGetEntityExtent(ifcModel, entityVoxelGrid);
            N = IFCEngine.sdaiGetMemberCount(extent);
            ASSERT(N == 1);
            for (int i = 0; i < N; i++)
            {

                Int64 inst = 0;
                IFCEngine.sdaiGetAggrByIndex(extent, i, IFCEngine.sdaiINSTANCE, out inst);

                lstGetB = ((IFC4x4.IfcVoxelGrid)(inst)).Voxels;
                ASSERT_EQ(lstGetB, arrSetB);
            }

            IFCEngine.sdaiCloseModel(ifcModel);
        }

    }
}
