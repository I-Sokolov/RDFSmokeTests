using System;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

using RDF;
using IFC4;

namespace CsIfcEngineTests
{
    class EarlyBinding_IFC4 : CsTests.TestBase 
    {
        public static void Test()
        {
            ENTER_TEST();
            
            var ifcModel = ifcengine.sdaiCreateModelBN(0, (string)null, "IFC4");
            ASSERT(ifcModel!=0);
            ifcengine.SetSPFFHeaderItem(ifcModel, 9, 0, ifcengine.sdaiSTRING, "IFC4");
            ifcengine.SetSPFFHeaderItem(ifcModel, 9, 1, ifcengine.sdaiSTRING, (string)null);

            var ownerHistory = IfcOwnerHistory.Create(ifcModel);

            var wall = IfcWall.Create(ifcModel);

            var guid = wall.GlobalId;
            var name = wall.Name;
            var descr = wall.Description;
            IfcOwnerHistory oh = wall.OwnerHistory;
            var predType = wall.PredefinedType;
            ASSERT(descr==null && name==null && guid==null && oh==0 && predType==null);

            wall.GlobalId = "7-7-7";
            wall.Name = "Wall name";
            wall.Description = "My wall description";
            wall.OwnerHistory = ownerHistory;
            wall.PredefinedType = IfcWallTypeEnum.POLYGONAL;

            guid = wall.GlobalId;
            name = wall.Name;
            descr = wall.Description;
            oh = wall.OwnerHistory;
            predType = wall.PredefinedType;
            ASSERT(descr == "My wall description"
                   && name == "Wall name"
                   && guid == "7-7-7"
                   && oh == ownerHistory
                   && predType.Value == IfcWallTypeEnum.POLYGONAL
            );

            var profile = IfcRectangleProfileDef.Create(ifcModel);
            var xdim = profile.XDim;
            var ydim = profile.YDim;
            ASSERT(xdim == null && ydim == null);
            profile.XDim = 10000;
            profile.YDim = 80;
            xdim = profile.XDim;
            ydim = profile.YDim;
            ASSERT(xdim.Value == 10000 && ydim.Value == 80);


            IfcTriangulatedFaceSet triangFaceSet = IfcTriangulatedFaceSet.Create(ifcModel);
            var closed = triangFaceSet.Closed;
            ASSERT(closed==null);
            triangFaceSet.Closed = (false);
            closed = triangFaceSet.Closed;
            ASSERT(!closed.Value);

            IfcBSplineCurve curve = IfcBSplineCurveWithKnots.Create(ifcModel);
            ASSERT(!curve.Degree.HasValue);
            curve.Degree = (5);
            ASSERT(curve.Degree.Value == 5);

            //type casting check
            var product = new IfcProduct (wall);
            ASSERT(product != 0);
            name = product.Name;
            ASSERT(name == "Wall name");

            IfcBuilding building = new IfcBuilding(wall);
            ASSERT(building == 0);

            //
            // SELECT tests
            //

            //
            // various data type and nested selects
            //
            IfcMeasureWithUnit measureWithUnit = IfcMeasureWithUnit.Create(ifcModel);

            //numeric value (sequence notation)
            double? dval = measureWithUnit.ValueComponent.IfcMeasureValue.IfcRatioMeasure;
            //Nullable<double> works also good
            ASSERT(!dval.HasValue);

            //shortcuts methods
            var as_double = measureWithUnit.ValueComponent.as_double;
            var as_text = measureWithUnit.ValueComponent.as_text;
            var as_int = measureWithUnit.ValueComponent.as_int;
            var as_bool = measureWithUnit.ValueComponent.as_bool;
            ASSERT(!as_double.HasValue && as_text == null && as_int == null && as_bool == null);

            //numeric value (alteranative notation)
            var getMeasureValue = new IfcMeasureValue_get(measureWithUnit, "ValueComponent");
            dval = getMeasureValue.IfcRatioMeasure;
            ASSERT(dval == null);

            // see below detached select test
            var measureValue_detachedSelect = new IfcMeasureValue(measureWithUnit, "ValueComponent");
            dval = measureValue_detachedSelect.IfcRatioMeasure;
            ASSERT(!dval.HasValue);

            //text based value
            string sval = getMeasureValue.IfcDescriptiveMeasure;
            ASSERT(sval == null);

            string txt = measureWithUnit.ValueComponent.IfcSimpleValue.IfcText;
            ASSERT(txt == null);

            //set to numeric
            measureWithUnit.ValueComponent.IfcMeasureValue.IfcRatioMeasure = (0.5);

            dval = measureWithUnit.ValueComponent.IfcMeasureValue.IfcRatioMeasure;
            ASSERT(dval.Value == 0.5);

            sval = measureWithUnit.ValueComponent.IfcMeasureValue.IfcDescriptiveMeasure;
            ASSERT(sval == null);

            txt = measureWithUnit.ValueComponent.IfcSimpleValue.IfcText;
            ASSERT(txt == null);

            //check type methodt
            if (measureWithUnit.ValueComponent.IfcMeasureValue.is_IfcAreaMeasure)
            {
                ASSERT(false);
            }
            else if (!measureWithUnit.ValueComponent.IfcMeasureValue.is_IfcRatioMeasure)
            {
                ASSERT(false);
            }
            else if (measureWithUnit.ValueComponent.IfcSimpleValue.is_IfcText)
            {
                ASSERT(false);
            }
            else if (measureWithUnit.ValueComponent.IfcMeasureValue.is_IfcComplexNumber)
            {
                ASSERT(false);
            }

            //shortcuts methods
            as_double = measureWithUnit.ValueComponent.as_double;
            as_text = measureWithUnit.ValueComponent.as_text;
            as_int = measureWithUnit.ValueComponent.as_int;
            as_bool = measureWithUnit.ValueComponent.as_bool;
            ASSERT(as_double.Value == 0.5 && as_text == "0.500000" && as_int.Value == 0 && !as_bool.HasValue);

            //detached select behaviour
            //detached select is also changed when instance changed
            dval = measureValue_detachedSelect.IfcRatioMeasure;
            ASSERT(dval! == 0.5);
            //but changing the detached select will change host instance
            measureValue_detachedSelect.IfcAreaMeasure = (2.7);
            dval = measureValue_detachedSelect.IfcAreaMeasure;
            ASSERT(dval.Value == 2.7);
            //instance was changed
            dval = measureWithUnit.ValueComponent.IfcMeasureValue.IfcRatioMeasure;
            ASSERT(dval == null);
            dval = measureWithUnit.ValueComponent.IfcMeasureValue.IfcAreaMeasure;
            ASSERT(dval! == 2.7);

            //set DescriptiveMeasure
            measureWithUnit.ValueComponent.IfcMeasureValue.IfcDescriptiveMeasure = ("my descreptive measure");

            dval = measureWithUnit.ValueComponent.IfcMeasureValue.IfcRatioMeasure;
            ASSERT(dval==null);

            sval = measureWithUnit.ValueComponent.IfcMeasureValue.IfcDescriptiveMeasure;
            ASSERT(sval == "my descreptive measure");

            txt = measureWithUnit.ValueComponent.IfcSimpleValue.IfcText;
            ASSERT(txt == null);

            as_double = measureWithUnit.ValueComponent.as_double;
            as_text = measureWithUnit.ValueComponent.as_text;
            as_int = measureWithUnit.ValueComponent.as_int;
            as_bool = measureWithUnit.ValueComponent.as_bool;
            ASSERT(as_double == null && (as_text == "my descreptive measure") && as_int == null && as_bool==null) ;

            //set text
            measureWithUnit.ValueComponent.IfcSimpleValue.IfcText = ("my text");

            ASSERT(measureWithUnit.ValueComponent.IfcSimpleValue.is_IfcText);

            dval = measureWithUnit.ValueComponent.IfcMeasureValue.IfcRatioMeasure;
            ASSERT(dval==null);

            sval = measureWithUnit.ValueComponent.IfcMeasureValue.IfcDescriptiveMeasure;
            ASSERT(sval == null);

            txt = measureWithUnit.ValueComponent.IfcSimpleValue.IfcText;
            ASSERT(txt== "my text");

            IfcComplexNumber complexVal = measureWithUnit.ValueComponent.IfcMeasureValue.IfcComplexNumber;
            ASSERT(complexVal.Count==0);

            as_double = measureWithUnit.ValueComponent.as_double;
            as_text = measureWithUnit.ValueComponent.as_text;
            as_int = measureWithUnit.ValueComponent.as_int;
            as_bool = measureWithUnit.ValueComponent.as_bool;
            ASSERT(as_double==null && (as_text == "my text") && as_int==null && as_bool==null);

            //
            // simple aggrgations in select
            //
            double[] arrDouble = { 2, 5 };
            measureWithUnit.ValueComponent.IfcMeasureValue.put_IfcComplexNumber (arrDouble);
            complexVal = measureWithUnit.ValueComponent.IfcMeasureValue.IfcComplexNumber;
            ASSERT(complexVal.Count==2 && complexVal[0]==2 && complexVal[1]==5);

            //
            // entities select
            //
            var actor = IfcActor.Create(ifcModel);

            var person = actor.TheActor.IfcPerson;
            var organization = actor.TheActor.IfcOrganization;
            ASSERT(person == 0 && organization == 0);

            Int64 instance = actor.TheActor.as_instance;
            ASSERT(instance == 0);

            var setPerson = IfcPerson.Create(ifcModel);
            setPerson.Identification = ("justApeson");

            actor.TheActor.IfcPerson = (setPerson);
            person = actor.TheActor.IfcPerson;
            ASSERT(setPerson == person);

            organization = actor.TheActor.IfcOrganization;
            ASSERT(organization == 0);

            instance = actor.TheActor.as_instance;
            ASSERT(instance == person);

            ASSERT(person.Identification == "justApeson");

            ASSERT(actor.TheActor.is_IfcPerson);
            ASSERT(!actor.TheActor.is_IfcOrganization);

            //
            // LOGICAL VALUES
            //
            ASSERT(curve.ClosedCurve==null);
            curve.ClosedCurve = (LOGICAL_VALUE.Unknown);
            ASSERT(curve.ClosedCurve.Value == LOGICAL_VALUE.Unknown);

            var ifcLogical = measureWithUnit.ValueComponent.IfcSimpleValue.IfcLogical;
            ASSERT(ifcLogical==null);
            measureWithUnit.ValueComponent.IfcSimpleValue.IfcLogical = (LOGICAL_VALUE.True);
            ifcLogical = measureWithUnit.ValueComponent.IfcSimpleValue.IfcLogical;
            ASSERT(ifcLogical.Value == LOGICAL_VALUE.True);

            var relIntersect = IfcRelInterferesElements.Create(ifcModel);
            ifcLogical = relIntersect.ImpliedOrder;
            ASSERT(ifcLogical==null);
            relIntersect.ImpliedOrder = (LOGICAL_VALUE.False);
            ifcLogical = relIntersect.ImpliedOrder;
            ASSERT(ifcLogical.Value == LOGICAL_VALUE.False);

            //
            // Aggregations
            //

            //as defined type
            var site = IfcSite.Create(ifcModel);

            IfcCompoundPlaneAngleMeasure longitude;
            longitude = site.RefLongitude;
            ASSERT(longitude.Count == 0);

            longitude = new IfcCompoundPlaneAngleMeasure();
            longitude.Add(54);
            site.put_RefLongitude(longitude);

            longitude = site.RefLongitude;
            ASSERT(longitude.Count == 1 && longitude[0] == 54);


            Int64[] rint = { 3, 4 };
            site.put_RefLongitude(rint);

            longitude = site.RefLongitude;
            ASSERT(longitude.Count == 2 && longitude[0] == 3 && longitude[1] == 4);

            //double unnamed
            var point = IfcCartesianPoint.Create(ifcModel);

            ListOfIfcLengthMeasure coords = point.Coordinates;
            ASSERT(coords.Count==0);

            double[] my2DPoint = { 1.0, 2.0 }; //can use array to set
            point.put_Coordinates(my2DPoint);

            coords = point.Coordinates;
            ASSERT(coords.Count == 2 && coords[0] == 1 && coords[1] == 2);

            coords.Add(3);
            point.put_Coordinates(coords); //can use sdt.list to set
            ASSERT(coords.Count == 3 && coords[0] == 1 && coords[2] == 3);

            //string
            ListOfIfcLabel middleNames = person.MiddleNames;
            ASSERT(middleNames.Count==0);

            string[] DaliMiddleNames = { "Domingo", "Felipe", "Jacinto" };
            person.put_MiddleNames(DaliMiddleNames);

            middleNames = person.MiddleNames;
            ASSERT(middleNames.Count == 3);
            int i = 0;
            foreach (var m in middleNames)
            {
                ASSERT(m == DaliMiddleNames[i++]);
            }

            //
            // LIST of LIST
            //
            var pointList = IfcCartesianPointList3D.Create(ifcModel);

            ListOfListOfIfcLengthMeasure coordList = pointList.CoordList;
            ASSERT(coordList.Count==0);

            //point (1,0.1)
            coordList.Add(new ListOfIfcLengthMeasure());
            coordList.Last().Add(1);
            coordList.Last().Add(0);
            coordList.Last().Add(1);

            //point (0,1,0)
            coordList.Add(new ListOfIfcLengthMeasure());
            coordList.Last().Add(0);
            coordList.Last().Add(1);
            coordList.Last().Add(0);

            pointList.put_CoordList(coordList);

            ListOfListOfIfcLengthMeasure coordListCheck = pointList.CoordList;
            ASSERT_EQ(coordList, coordListCheck);

            //
            // Aggregation in select 
            // 
            var prop = IfcPropertySingleValue.Create(ifcModel);

            IfcComplexNumber cplxNum = prop.NominalValue.IfcMeasureValue.IfcComplexNumber;
            ASSERT(cplxNum.Count == 0);

            double[] cplx = { 2.1, 1.5 };
            prop.NominalValue.IfcMeasureValue.put_IfcComplexNumber(cplx);

            cplxNum=prop.NominalValue.IfcMeasureValue.IfcComplexNumber;
            ASSERT(cplxNum.Count == 2 && cplxNum[0] == 2.1 && cplxNum[1] == 1.5);

            //
            //IndexedPolyCurve
            //
            var poly = IfcIndexedPolyCurve.Create(ifcModel);

            ASSERT(poly.Points == 0);

            ListOfIfcSegmentIndexSelect gotSegments = poly.Segments;
            ASSERT(gotSegments.Count==0);

            //2D points
            double[] rpt ={
                        0,0,
                        1,0,
                        1,1,
                        0,1
            };

            //indexes of line and arc;
            Int64[] line = { 0, 1 };
            Int64[] arc = { 1, 2, 3 };

            //create points list
            //
            var points = IfcCartesianPointList2D.Create(ifcModel);

            var lstCoords = new ListOfListOfIfcLengthMeasure(); 
            for (i = 0; i < 4; i++)
            {
                lstCoords.Add(new ListOfIfcLengthMeasure());
                for (int j = 0; j < 2; j++)
                {
                    lstCoords.Last().Add(rpt[2 * i + j]);
                }
            }

            points.put_CoordList(lstCoords);

            //create segments list =
            //
            IfcSegmentIndexSelect[] putSegments = { new IfcSegmentIndexSelect(poly), new IfcSegmentIndexSelect(poly) };
            putSegments[0].put_IfcLineIndex(line);
            putSegments[1].put_IfcArcIndex(arc);

            //
            //
            poly.put_Segments(putSegments);
            poly.Points = (points);
            poly.SelfIntersect = (false);

            //
            // get and check
            //
            points = 0;

            var pts = poly.Points;
            points = new IfcCartesianPointList2D(pts); 
            ASSERT(points != 0);

            building = new IfcBuilding(pts);
            ASSERT(building == 0);

            coordList = points.CoordList;
            ASSERT_EQ(coordList, lstCoords);

            gotSegments = poly.Segments;
            ASSERT(gotSegments.Count == 2);

            IfcArcIndex arcInd = gotSegments[0].IfcArcIndex;
            IfcLineIndex lineInd = gotSegments[0].IfcLineIndex;
            ASSERT(arcInd.Count == 0);
            ASSERT_EQ(lineInd, line);

            arcInd = gotSegments[1].IfcArcIndex;
            lineInd = gotSegments[1].IfcLineIndex;
            ASSERT_EQ(arcInd, arc);
            ASSERT(lineInd.Count==0);

            //append line
            var line2 = new IfcLineIndex();
            line2.Add(3);
            line2.Add(1);
            var segment3 = new IfcSegmentIndexSelect(poly);
            segment3.put_IfcLineIndex(line2);

            var putLstSegments = putSegments.ToList();
            putLstSegments.Add(segment3);

            poly.put_Segments(putLstSegments);

            //check now
            gotSegments = poly.Segments;
            ASSERT(gotSegments.Count == 3);

            arcInd = gotSegments[0].IfcArcIndex;
            lineInd = gotSegments[0].IfcLineIndex;
            ASSERT(arcInd.Count == 0);
            ASSERT_EQ(lineInd, line);

            arcInd = gotSegments[1].IfcArcIndex;
            lineInd = gotSegments[1].IfcLineIndex;
            ASSERT_EQ(arcInd, arc);
            ASSERT(lineInd.Count == 0);

            arcInd = gotSegments[2].IfcArcIndex;
            lineInd = gotSegments[2].IfcLineIndex;
            ASSERT(arcInd.Count==0);
            ASSERT_EQ(lineInd, line2);


            ///
            /// Aggregation of instances
            /// 
            var prodRepr = wall.Representation;
            ASSERT(prodRepr == 0);

            prodRepr = IfcProductDefinitionShape.Create(ifcModel);
            wall.Representation = (prodRepr);
            ASSERT(wall.Representation == prodRepr);

            ListOfIfcRepresentation lstRep = prodRepr.Representations;
            ASSERT(lstRep.Count==0);

            var repr = IfcShapeRepresentation.Create(ifcModel);
            lstRep.Add(repr);
            prodRepr.put_Representations(lstRep);

            lstRep = prodRepr.Representations;
            ASSERT(lstRep.Count == 1 && lstRep.First() == repr);

            SetOfIfcRepresentationItem lstItems = repr.Items;
            ASSERT(lstItems.Count == 0);

            lstItems.Add(poly);
            lstItems.Add(triangFaceSet);
            lstItems.Add(curve);

            repr.put_Items(lstItems);

            var lstGotItems = repr.Items;
            ASSERT_EQ(lstGotItems, lstItems);

            ///
            /// Defined type aggregation of instance
            var relProps = IfcRelDefinesByProperties.Create(ifcModel);

            SetOfIfcObjectDefinition relObj = relProps.RelatedObjects;
            ASSERT(relObj.Count==0);

            relObj.Add(wall);
            relProps.put_RelatedObjects(relObj);

            var relObjGot = relProps.RelatedObjects;
            ASSERT_EQ(relObj, relObjGot);

            IfcPropertySetDefinitionSet psSet = relProps.RelatingPropertyDefinition.IfcPropertySetDefinitionSet;
            ASSERT(psSet.Count == 0);
            ASSERT(relProps.RelatingPropertyDefinition.IfcPropertySetDefinition == 0);

            var emptyPset = IfcPropertySet.Create(ifcModel);
            emptyPset.Name = ("Empty property set");

            relProps.RelatingPropertyDefinition.IfcPropertySetDefinition = (emptyPset);
            ASSERT(relProps.RelatingPropertyDefinition.IfcPropertySetDefinition == emptyPset);

            psSet = relProps.RelatingPropertyDefinition.IfcPropertySetDefinitionSet;
            ASSERT(psSet.Count == 0);

            psSet.Add(emptyPset);
            relProps.RelatingPropertyDefinition.put_IfcPropertySetDefinitionSet (psSet);
            ASSERT(relProps.RelatingPropertyDefinition.IfcPropertySetDefinition == 0);
            
            var psSetGot = relProps.RelatingPropertyDefinition.IfcPropertySetDefinitionSet;
            ASSERT_EQ(psSet, psSetGot);

            /// 
            /// 
            ifcengine.sdaiSaveModelBN(ifcModel, "Test.ifc");
            ifcengine.sdaiCloseModel(ifcModel);

            ifcModel = ifcengine.sdaiOpenModelBN(0, "Test.ifc", "IFC4");

            var entityIfcRelDefinesByProperties = ifcengine.sdaiGetEntity(ifcModel, "IfcRelDefinesByProperties");
            ASSERT(entityIfcRelDefinesByProperties!=0);

            var rels = ifcengine.sdaiGetEntityExtent(ifcModel, entityIfcRelDefinesByProperties);
            var N_rels = ifcengine.sdaiGetMemberCount(rels);
            ASSERT(N_rels == 1);
            for (i = 0; i < N_rels; i++)
            {

                Int64 rel = 0;
                ifcengine.sdaiGetAggrByIndex(rels, i, ifcengine.sdaiINSTANCE, out rel);

                var get = ((IfcRelDefinesByProperties)(rel)).RelatingPropertyDefinition;
                ASSERT(get.IfcPropertySetDefinition == 0);
                psSet = get.IfcPropertySetDefinitionSet;
                ASSERT(psSet.Count == 1);
                name = psSet[0].Name;
                ASSERT(name == "Empty property set");
            }

            ifcengine.sdaiCloseModel(ifcModel);
        }
    }
}
