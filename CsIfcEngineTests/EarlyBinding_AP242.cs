using System;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using AP242;

namespace CsIfcEngineTests
{
    class EarlyBinding_AP242 : CsTests.TestBase
    {
        public static void Run()
        {
            test_multi_parent();

            test_list3();
        }


        static void test_list3()
        {
            ENTER_TEST();

            /// 
            /// Create list of list of list of double
            /// 
            long model = RDF.ifcengine.sdaiCreateModelBN(0, null as string, "AP242");
            RDF.ifcengine.SetSPFFHeaderItem(model, 9, 0, RDF.ifcengine.sdaiSTRING, "AP242");
            RDF.ifcengine.SetSPFFHeaderItem(model, 9, 1, RDF.ifcengine.sdaiSTRING, null as string);

            ASSERT(model!=0);

            var bspline_volume = rational_b_spline_volume.Create(model);

            list_of_list_of_list_of_double weights = bspline_volume.weights_data;
            ASSERT(weights.Count == 0);

            for (int i = 0; i < 2; i++)
            {

                weights.Add(new list_of_list_of_double());
                var list2 = weights.Last();

                for (int j = 0; j < 3; j++)
                {

                    list2.Add(new list_of_double());
                    var list = list2.Last();

                    for (int k = 0; k < 2; k++)
                    {
                        list.Add(100 * i + 10 * j + k);
                    }
                }
            }

            bspline_volume.put_weights_data(weights);

            //bag
            var segment = composite_curve_segment.Create(model);

            bag_of_composite_curve bag = segment.using_curves;
            ASSERT(bag.Count==0);

            //defined types on selects
            var equiv = equivalence_notable_instance.Create(model);

            list_of_equivalence_detected_difference_select lstCompared = equiv.compared_elements;
            ASSERT(lstCompared.Count == 0);

            var vertexPoint = vertex_point.Create(model);
            vertexPoint.name = "Test vertex point";
            var valCompared1 = new equivalence_detected_difference_select(equiv);
            valCompared1.a3ms_inspected_equivalence_element_select.vertex_point = vertexPoint;
            lstCompared.Add(valCompared1);
            equiv.put_compared_elements(lstCompared);

            lstCompared = equiv.compared_elements;
            ASSERT(lstCompared.Count!=0);
            var test = lstCompared.First().a3ms_inspected_equivalence_element_select.vertex_point.name;
            ASSERT(!strcmp(test, "Test vertex point"));

            //
            var prodDefOccur = product_definition_occurrence.Create(model);
            ASSERT(prodDefOccur.definition.product_definition == 0);

            var prodDef = product_definition.Create(model);
            prodDefOccur.definition.product_definition = prodDef;
            ASSERT(prodDefOccur.definition.product_definition == prodDef);

            //            
            var appliedUsageRights = applied_usage_right.Create(model);
            set_of_ir_usage_item lstUsageItems = appliedUsageRights.items;
            ASSERT(lstUsageItems.Count == 0);


            var usageItem = applied_classification_assignment.Create(model);
            var role = classification_role.Create(model);
            role.name = "Test role";
            usageItem.role = role;

            lstUsageItems.Add(new ir_usage_item(appliedUsageRights));
            lstUsageItems.Last().applied_classification_assignment = usageItem;

            appliedUsageRights.put_items(lstUsageItems);

            lstUsageItems = appliedUsageRights.items;
            ASSERT(lstUsageItems.Count == 1);
            ASSERT(lstUsageItems.Last().action == 0);
            test = lstUsageItems.Last().applied_classification_assignment.role.name;
            ASSERT(!strcmp(test, "Test role"));

            //            
            var listedLogical = listed_logical_data.Create(model);
            ListOfLOGICAL_VALUE lstLogical = listedLogical.values;
            ASSERT(lstLogical.Count == 0);

            lstLogical.Add(LOGICAL_VALUE.True);
            lstLogical.Add(LOGICAL_VALUE.False);
            lstLogical.Add(LOGICAL_VALUE.Unknown);

            listedLogical.put_values(lstLogical);

            lstLogical = listedLogical.values;
            ASSERT(lstLogical.Count == 3 && lstLogical.First() == LOGICAL_VALUE.True && lstLogical.Last() == LOGICAL_VALUE.Unknown);

            //
            var extreme = extreme_instance.Create(model);

            var dir = direction.Create(model);

            set_of_location_of_extreme_value_select setLocations = extreme.locations_of_extreme_value;
            ASSERT(setLocations.Count == 0);

            setLocations.Add(new location_of_extreme_value_select(extreme));
            setLocations.Last().inspected_shape_element_select.direction = dir;

            extreme.put_locations_of_extreme_value(setLocations);

            List<location_of_extreme_value_select> getLocations = extreme.locations_of_extreme_value;
            ASSERT(getLocations.Count == 1 && getLocations[0].inspected_shape_element_select.direction == dir);

            //
            RDF.ifcengine.sdaiSaveModelBN(model, "Test.ap");
            RDF.ifcengine.sdaiCloseModel(model);

            /// 
            /// Now read
            /// 
            var modelRead = RDF.ifcengine.sdaiOpenModelBN(0, "Test.ap", "AP242");

            var entity = RDF.ifcengine.sdaiGetEntity(modelRead, "RATIONAL_B_SPLINE_VOLUME");
            ASSERT(entity!=0);

            var volumes = RDF.ifcengine.sdaiGetEntityExtent(modelRead, entity);
            var N_volumes = RDF.ifcengine.sdaiGetMemberCount(volumes);
            ASSERT(N_volumes == 1);
            for (long i = 0; i < N_volumes; i++)
            {

                long volume = 0;
                RDF.ifcengine.sdaiGetAggrByIndex(volumes, i, RDF.ifcengine.sdaiINSTANCE, out volume);

                list_of_list_of_list_of_double weights2 = ((rational_b_spline_volume)volume).weights_data;

                ASSERT_EQ_LST(weights, weights2);
            }

            RDF.ifcengine.sdaiCloseModel(modelRead);
        }

        static void test_multi_parent()
        {
            ENTER_TEST();

            long model = RDF.ifcengine.sdaiCreateModelBN(0, null as string, "AP242");
            RDF.ifcengine.SetSPFFHeaderItem(model, 9, 0, RDF.ifcengine.sdaiSTRING, "AP242");
            RDF.ifcengine.SetSPFFHeaderItem(model, 9, 1, RDF.ifcengine.sdaiSTRING, null as string);

            //wrapper test
            var inst = a3m_equivalence_criterion_with_specified_elements.Create(model);
            string NAME = "sey Name";
            inst.name = NAME;

            list_of_oriented_edge edge= new list_of_oriented_edge();
            edge.Add (oriented_edge.Create(model));
            edge.Add (oriented_edge.Create(model));

            var eloop = edge_loop.Create(model);
            eloop.name = NAME;
            eloop.put_edge_list(edge);

            RDF.ifcengine.sdaiSaveModelBN(model, "Test.ap");
            RDF.ifcengine.sdaiCloseModel(model);

            /// Now read
            /// 
            var modelRead = RDF.ifcengine.sdaiOpenModelBN(0, "Test.ap", "AP242");

            var entity = RDF.ifcengine.sdaiGetEntity(modelRead, "a3m_equivalence_criterion_with_specified_elements");// "a3m_equivalence_criterion");
            ASSERT(entity!=0);
            var items = RDF.ifcengine.sdaiGetEntityExtent(modelRead, entity);
            var N_items = RDF.ifcengine.sdaiGetMemberCount(items);
            ASSERT(N_items == 1);
            for (long i = 0; i < N_items; i++)
            {

                long item = 0;
                RDF.ifcengine.sdaiGetAggrByIndex(items, i, RDF.ifcengine.sdaiINSTANCE, out item);

                var name = ((a3m_equivalence_criterion)item).name;
                ASSERT(!strcmp(name, NAME));
            }

            entity = RDF.ifcengine.sdaiGetEntity(modelRead, "edge_loop");
            ASSERT(entity!=0);
            items = RDF.ifcengine.sdaiGetEntityExtent(modelRead, entity);
            N_items = RDF.ifcengine.sdaiGetMemberCount(items);
            ASSERT(N_items == 1);
            for (long i = 0; i < N_items; i++)
            {

                long item = 0;
                RDF.ifcengine.sdaiGetAggrByIndex(items, i, RDF.ifcengine.sdaiINSTANCE, out item);

                eloop = (edge_loop)item;
                var name = eloop.name;
                ASSERT(name == NAME);

                var lst = eloop.edge_list;
                ASSERT(lst.Count == 2);
            }


            RDF.ifcengine.sdaiCloseModel(modelRead);

        }

        static bool strcmp (string s1, string s2)
        {
            return s1 != s2;
        }

        private static void ASSERT_EQ_LST(IEnumerable lst1, IEnumerable lst2)
        {
            var it1 = lst1.GetEnumerator();
            var it2 = lst2.GetEnumerator();

            bool m1 = it1.MoveNext();
            bool m2 = it2.MoveNext();
            while (m1 && m2)
            {
                var cmp1 = it1.Current as IComparable;
                var cmp2 = it2.Current as IComparable;
                if (cmp1 != null && cmp2 != null)
                {
                    ASSERT(cmp1.Equals(cmp2));
                }
                else
                {
                    var lst11 = it1.Current as IEnumerable;
                    var lst22 = it2.Current as IEnumerable;
                    if (lst11 != null && lst22 != null)
                    {
                        ASSERT_EQ_LST(lst11, lst22);
                    }
                    else
                    {
                        ASSERT(false); //no comparision is implemented
                    }
                }

                m1 = it1.MoveNext();
                m2 = it2.MoveNext();
            }

            ASSERT(!m1 && !m2);
        }

    }
}
