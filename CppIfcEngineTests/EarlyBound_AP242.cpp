
#include "pch.h"

using namespace AP242;


static void test_list3();
static void test_multi_parent();

extern void EarlyBound_AP242_test()
{
    ENTER_TEST

    test_multi_parent();
    
    test_list3();
}


template <typename L> static void ASSERT_EQ_LST(L& lst1, L& lst2)
{
    ASSERT(lst1.size() == lst2.size());
    auto it1 = lst1.begin();
    auto it2 = lst2.begin();
    for (; it1 != lst1.end() && it2 != lst2.end(); it1++, it2++) {
        ASSERT(*it1 == *it2);
    }
}


template <typename L> static void ASSERT_EQ_LST2(L& lst1, L& lst2)
{
    ASSERT(lst1.size() == lst2.size());
    auto it1 = lst1.begin();
    auto it2 = lst2.begin();
    for (; it1 != lst1.end() && it2 != lst2.end(); it1++, it2++) {
        ASSERT_EQ_LST(*it1, *it2);
    }
}


template <typename L> static void ASSERT_EQ_LST3(L& lst1, L& lst2)
{
    ASSERT(lst1.size() == lst2.size());
    auto it1 = lst1.begin();
    auto it2 = lst2.begin();
    for (; it1 != lst1.end() && it2 != lst2.end(); it1++, it2++) {
        ASSERT_EQ_LST2(*it1, *it2);
    }
}


static void test_list3()
{
    ENTER_TEST

    /// 
    /// Create list of list of list of double
    /// 
    int_t  model = sdaiCreateModelBN(0, NULL, "AP242");
    SetSPFFHeaderItem(model, 9, 0, sdaiSTRING, "AP242");
    SetSPFFHeaderItem(model, 9, 1, sdaiSTRING, (const char*)0);

    ASSERT(model);

    auto bspline_volume = rational_b_spline_volume::Create(model);

    list_of_list_of_list_of_double weights;

    bspline_volume.get_weights_data(weights);
    ASSERT(weights.size() == 0);

    for (int i = 0; i < 2; i++) {

        weights.push_back(list_of_list_of_double());
        auto& list2 = weights.back();

        for (int j = 0; j < 3; j++) {

            list2.push_back(list_of_double());
            auto& list = list2.back();

            for (int k = 0; k < 2; k++) {
                list.push_back(100 * i + 10 * j + k);
            }
        }       
    }

    bspline_volume.put_weights_data(weights);

    //bag
    auto segment = composite_curve_segment::Create(model);

    bag_of_composite_curve bag;
    segment.get_using_curves(bag);
    ASSERT(bag.empty());

    //defined types on selects
    auto equiv = equivalence_notable_instance::Create(model);
    
    list_of_equivalence_detected_difference_select lstCompared;
    equiv.get_compared_elements(lstCompared);
    ASSERT(lstCompared.size() == 0);

    auto vertexPoint = vertex_point::Create(model);
    vertexPoint.put_name("Test vertex point");
    equivalence_detected_difference_select valCompared1 (equiv);
    valCompared1._a3ms_inspected_equivalence_element_select().put_vertex_point(vertexPoint);
    lstCompared.push_back(valCompared1);
    equiv.put_compared_elements(lstCompared);

    lstCompared.clear();
    equiv.get_compared_elements(lstCompared);
    ASSERT(lstCompared.size());
    auto test = lstCompared.front()._a3ms_inspected_equivalence_element_select().get_vertex_point().get_name();
    ASSERT(!strcmp(test, "Test vertex point"));

    //
    auto prodDefOccur = product_definition_occurrence::Create(model);
    ASSERT(prodDefOccur.get_definition().get_product_definition() == 0);
    
    auto prodDef = product_definition::Create(model);
    prodDefOccur.put_definition().put_product_definition(prodDef);
    ASSERT(prodDefOccur.get_definition().get_product_definition() == prodDef);

    //
    set_of_ir_usage_item lstUsageItems;
    auto appliedUsageRights = applied_usage_right::Create(model);
    appliedUsageRights.get_items(lstUsageItems);
    ASSERT(lstUsageItems.size() == 0);


    auto usageItem = applied_classification_assignment::Create(model);
    auto role = classification_role::Create(model);
    role.put_name("Test role");
    usageItem.put_role(role);    

    lstUsageItems.push_back(ir_usage_item(appliedUsageRights));
    lstUsageItems.back().put_applied_classification_assignment(usageItem);

    appliedUsageRights.put_items(lstUsageItems);

    lstUsageItems.clear();
    appliedUsageRights.get_items(lstUsageItems);
    ASSERT(lstUsageItems.size() == 1);
    ASSERT(lstUsageItems.back().get_action() == 0);
    test = lstUsageItems.back().get_applied_classification_assignment().get_role().get_name();
    ASSERT(!strcmp(test, "Test role"));

    //
    ListOfLOGICAL_VALUE lstLogical;
    auto listedLogical = listed_logical_data::Create(model);
    listedLogical.get_values(lstLogical);
    ASSERT(lstLogical.size() == 0);

    lstLogical.push_back(LOGICAL_VALUE::True);
    lstLogical.push_back(LOGICAL_VALUE::False);
    lstLogical.push_back(LOGICAL_VALUE::Unknown);

    listedLogical.put_values(lstLogical);

    lstLogical.clear();
    listedLogical.get_values(lstLogical);
    ASSERT(lstLogical.size() == 3 && lstLogical.front() == LOGICAL_VALUE::True && lstLogical.back() == LOGICAL_VALUE::Unknown);

    //
    auto extreme = extreme_instance::Create(model);
    
    auto dir = direction::Create(model);

    set_of_location_of_extreme_value_select setLocations;
    extreme.get_locations_of_extreme_value(setLocations);
    ASSERT(setLocations.size() == 0);

    setLocations.push_back(location_of_extreme_value_select(extreme));
    setLocations.back()._inspected_shape_element_select().put_direction(dir);

    extreme.put_locations_of_extreme_value(setLocations);

    std::vector<location_of_extreme_value_select> getLocations;
    extreme.get_locations_of_extreme_value(getLocations);
    ASSERT(getLocations.size() == 1 && getLocations[0]._inspected_shape_element_select().get_direction() == dir);

    //
    sdaiSaveModelBN(model, "ebTest.ap");
    sdaiCloseModel(model);

    /// 
    /// Now read
    /// 
    auto modelRead = sdaiOpenModelBN(NULL, "ebTest.ap", "AP242");
    
    auto entity = sdaiGetEntity(modelRead, "RATIONAL_B_SPLINE_VOLUME");
    ASSERT(entity);

    auto volumes = sdaiGetEntityExtent(modelRead, entity);
    auto N_volumes = sdaiGetMemberCount(volumes);
    ASSERT(N_volumes == 1);
    for (int_t i = 0; i < N_volumes; i++) {
     
        int_t volume = 0;
        sdaiGetAggrByIndex(volumes, i, sdaiINSTANCE, &volume);

        list_of_list_of_list_of_double weights2;
        rational_b_spline_volume(volume).get_weights_data(weights2);

        ASSERT_EQ_LST3(weights, weights2);
    }
    
    sdaiCloseModel(modelRead);
}

static void test_multi_parent()
{
    ENTER_TEST

    int_t  model = sdaiCreateModelBN(0, NULL, "AP242");
    ASSERT(model);

    SetSPFFHeaderItem(model, 9, 0, sdaiSTRING, "AP242");
    SetSPFFHeaderItem(model, 9, 1, sdaiSTRING, (const char*)0);

    //engine test
    int_t entity = sdaiGetEntity(model, "a3m_equivalence_criterion");
    
    ASSERT(entity);
    ASSERT(7 == engiGetEntityNoAttributes(entity));

    const char* rAttr[] =
        {"name","assessment_specification","comparing_element_types","compared_element_types","measured_data_type", "detected_difference_types","accuracy_types"};
    const int_t rTypes[] =
        {sdaiSTRING, sdaiINSTANCE, sdaiAGGR, sdaiAGGR, sdaiENUM, sdaiAGGR, sdaiAGGR};
    const bool rInverse[]
        { false, false, false, false, false, false, false };
    const char* rDefiningEntity[] = 
        { "representation_item", "a3m_equivalence_criterion", "a3m_equivalence_criterion", "a3m_equivalence_criterion", "a3m_equivalence_criterion", "a3m_equivalence_criterion", "a3m_equivalence_criterion" };
    enum_express_attr_type rAttrType[] =
        { enum_express_attr_type::__NONE, enum_express_attr_type::__NONE, enum_express_attr_type::__NONE, enum_express_attr_type::__NONE,
        enum_express_attr_type::__NONE ,enum_express_attr_type::__NONE ,enum_express_attr_type::__NONE };
    const char* rDomainEntity[] =
        { "label", "a3m_equivalence_assessment_specification_select", "a3m_element_type_name", "a3m_element_type_name", "a3m_measured_data_type_name", "a3m_detected_difference_type_name", "a3m_accuracy_type_name" };
    bool rIsAggr[] =
        { false, false, false, false, false, false, false };
    bool rOptional[] = 
        { false, false, false, false, false, false, false };

    for (int i = 0; i < 7; i++) {

        auto attribute = engiGetEntityAttributeByIndex(entity, i, true, true);
        const char* name = NULL;
        bool inverse = false;
        SdaiEntity definingEntity = 0;
        enum_express_attr_type attrType = enum_express_attr_type::__NONE;
        SdaiEntity domainEntity = 0;
        SchemaAggr aggrDescr = 0;
        bool optional = false;
        engiGetAttributeTraits(attribute, &name, &definingEntity, &inverse, &attrType, &domainEntity, &aggrDescr, &optional);

        ASSERT(!strcmp(name, rAttr[i]));
        
        auto nm = engiGetEntityName(definingEntity, sdaiSTRING);
        ASSERT(!strcmp(nm, rDefiningEntity[i]));

        auto nm2 = engiGetEntityName(domainEntity, sdaiSTRING);
        ASSERT(!strcmp(nm2, rDomainEntity[i]));

        ASSERT(inverse == rInverse[i] && optional == rOptional[i]);
        ASSERT((aggrDescr != 0) == (i==2 || i==3 || i >= 5));

        int_t type = 0;
        engiGetEntityArgumentType(entity, i, &type);
        ASSERT(type == rTypes[i]);

        //not crashing with nulls
        engiGetAttributeTraits(attribute, &name, 0, 0, 0, 0, 0, 0);
    }
    
    //wrapper test
    auto inst = a3m_equivalence_criterion_with_specified_elements::Create(model);
    const char* NAME = "sey Name";
    inst.put_name(NAME);

    oriented_edge edge[2];
    edge[0] = oriented_edge::Create(model);
    edge[1] = oriented_edge::Create(model);

    auto eloop = edge_loop::Create(model);
    eloop.put_name(NAME);
    eloop.put_edge_list(edge, 2);

    sdaiSaveModelBN(model, "ebTest2.ap");
    sdaiCloseModel(model);

    /// Now read
    /// 
    auto modelRead = sdaiOpenModelBN(NULL, "ebTest2.ap", "AP242");

    entity = sdaiGetEntity(modelRead, "a3m_equivalence_criterion_with_specified_elements");// "a3m_equivalence_criterion");
    ASSERT(entity);

    int_t* items = sdaiGetEntityExtent(modelRead, entity);
    auto N_items = sdaiGetMemberCount(items);
    ASSERT(N_items == 1);
    for (int_t i = 0; i < N_items; i++) {

        int_t item = 0;
        sdaiGetAggrByIndex(items, i, sdaiINSTANCE, &item);

        auto name = a3m_equivalence_criterion(item).get_name();
        ASSERT(!strcmp(name, NAME));
    }


    entity = sdaiGetEntity(modelRead, "edge_loop");
    ASSERT(entity);
    items = sdaiGetEntityExtent(modelRead, entity);
    N_items = sdaiGetMemberCount(items);
    ASSERT(N_items == 1);
    for (int_t i = 0; i < N_items; i++) {

        int_t item = 0;
        sdaiGetAggrByIndex(items, i, sdaiINSTANCE, &item);

        eloop = edge_loop(item);
        auto name = eloop.get_name();
        ASSERT(!strcmp(name, NAME));

        std::list<oriented_edge> lst;
        eloop.get_edge_list(lst);
        ASSERT(lst.size() == 2);
    }

    sdaiCloseModel(modelRead);

}
