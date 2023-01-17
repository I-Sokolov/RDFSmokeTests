
#include "pch.h"

//TODO - check parent class to existing instance
//TODO - check add property to existing instance

struct Cardinality
{
    Cardinality (int64_t cardMin_, int64_t cardMax_, int64_t cardMinAggr_, int64_t cardMaxAggr_)
        :cardMin(cardMin_), cardMax(cardMax_), cardMinAggr(cardMinAggr_), cardMaxAggr(cardMaxAggr_) {}

    int64_t     cardMin;
    int64_t     cardMax;
    int64_t     cardMinAggr;
    int64_t     cardMaxAggr;
};

struct PropList : public std::map < std::string, Cardinality >
{
    void Add(const char* name, int64_t cardMin, int64_t cardMax, int64_t cardMinAggr, int64_t cardMaxAggr)
    {
        insert(PropList::value_type(name, Cardinality(cardMin, cardMax, cardMinAggr, cardMaxAggr)));
    }
};

/// <summary>
/// 
/// </summary>
static void CheckPropertiesExpected(OwlInstance instance, PropList& expectedProps)
{
    auto cls = GetInstanceClass(instance);

    auto prop = GetInstancePropertyByIterator(instance, 0);

    while (prop)
    {
        auto name = GetNameOfProperty(prop);

        auto itExpected = expectedProps.find(name);
        ASSERT(itExpected != expectedProps.end()); // strcmp(name, itExpected->name.c_str()));

        int64_t minCard, maxCard;
        GetClassPropertyCardinalityRestriction(cls, prop, &minCard, &maxCard);
        ASSERT(minCard == itExpected->second.cardMin && maxCard == itExpected->second.cardMax);

        GetClassPropertyAggregatedCardinalityRestriction (cls, prop, &minCard, &maxCard);
        ASSERT(minCard == itExpected->second.cardMinAggr && maxCard == itExpected->second.cardMaxAggr);

        expectedProps.erase(itExpected);

        prop = GetInstancePropertyByIterator(instance, prop);        
    }

    ASSERT(expectedProps.empty());
}

/// <summary>
/// 
/// </summary>
static void SubclassChangesCardianlity(bool earlySetParent, int64_t type)
{
    ENTER_TEST_EX2(earlySetParent ? "Early set parent" : "Late set parent", type == OBJECTPROPERTY_TYPE ? "object" : "datatype");

    int64_t model = OpenModel(NULL);

    auto classA = CreateClass(model, "ClassA");

    auto classB = CreateClass(model, "ClassB");

    if (earlySetParent) {
        SetClassParent(classB, classA, 1);
    }

    auto propEnanced = CreateProperty(model, type, "Enhanced");
    auto propResticted = CreateProperty(model, type, "Restricted");
    auto propFinited = CreateProperty(model, type, "Finited");
    auto propInfinited = CreateProperty(model, type, "Infinited");
    auto propEmpty = CreateProperty(model, type, "Empty");
    auto propA = CreateProperty(model, type, "PropClassA");
    auto propB = CreateProperty(model, type, "PropClassB");

    PropList propList;

    auto instanceB0 = CreateInstance(classB);
    CheckPropertiesExpected(instanceB0, propList);

    SetClassPropertyCardinalityRestriction(classA, propA, 2, 3);
    propList.Add("PropClassA", -1, -1, 2, 3);
    
    SetClassPropertyCardinalityRestriction(classB, propB, 4, 5);
    propList.Add ("PropClassB", 4, 5, 4, 5);

    SetClassPropertyCardinalityRestriction(classA, propEnanced, 0, 1);
    SetClassPropertyCardinalityRestriction(classB, propEnanced, 0, 2);
    propList.Add("Enhanced", 0, 2, 0, 1);

    SetClassPropertyCardinalityRestriction(classA, propResticted, 9, 12);
    SetClassPropertyCardinalityRestriction(classB, propResticted, 9, 11);
    propList.Add("Restricted", 9, 11, 9, 11);

    SetClassPropertyCardinalityRestriction(classA, propFinited, 3, -1);
    SetClassPropertyCardinalityRestriction(classB, propFinited, 2, 4);
    propList.Add("Finited", 2, 4, 3, 4);

    SetClassPropertyCardinalityRestriction(classA, propInfinited, 2, 3);
    SetClassPropertyCardinalityRestriction(classB, propInfinited, 1, -1);
    propList.Add("Infinited", 1, -1, 2, 3);

    SetClassPropertyCardinalityRestriction(classA, propEmpty, 2, 3);
    SetClassPropertyCardinalityRestriction(classB, propEmpty, 4, 5);
    propList.Add("Empty", 4, 5, 4, 3);

    if (!earlySetParent) {
        SetClassParent(classB, classA, 1);
    }

    auto instance = CreateInstance(classB);
    CheckPropertiesExpected(instance, propList);

    /** TODO: update properties when add subclass
    CheckPropertiesExpected(instanceB0, propList);*/

    CloseModel(model);
}

/// <summary>
/// 
/// </summary>
static void MultiParentsCardinality (int64_t type)
{
    ENTER_TEST_EX(type == OBJECTPROPERTY_TYPE ? "object" : "datatype");

    int64_t model = OpenModel(NULL);

    auto classA1 = CreateClass(model, "ClassA1");
    auto classA2 = CreateClass(model, "ClassA2");
    auto classA3 = CreateClass(model, "ClassA3");

    auto classB = CreateClass(model, "classB");
    SetClassParent(classB, classA1, 1);
    SetClassParent(classB, classA2, 1);

    auto instance1 = CreateInstance(classB);

    PropList propList;
    CheckPropertiesExpected(instance1, propList);

    auto propEnanced = CreateProperty(model, type, "Enhanced");
    auto propResticted = CreateProperty(model, type, "Restricted");
    auto propFinited = CreateProperty(model, type, "Finited");
    auto propInfinited = CreateProperty(model, type, "Infinited");
    auto propEmpty = CreateProperty(model, type, "Empty");

    SetClassPropertyCardinalityRestriction(classA1, propEnanced, 0, 1);
    SetClassPropertyCardinalityRestriction(classB, propEnanced, 0, 2);
    propList.Add("Enhanced", 0, 2, 0, 1);

    SetClassPropertyCardinalityRestriction(classA2, propResticted, 9, 12);
    SetClassPropertyCardinalityRestriction(classB, propResticted, 9, 11);
    propList.Add("Restricted", 9, 11, 9, 11);

    SetClassPropertyCardinalityRestriction(classA3, propFinited, 3, -1);
    SetClassPropertyCardinalityRestriction(classB, propFinited, 2, 4);
    propList.Add("Finited", 2, 4, 3, 4);

    SetClassPropertyCardinalityRestriction(classA1, propInfinited, 2, 3);
    SetClassPropertyCardinalityRestriction(classB, propInfinited, 1, -1);
    propList.Add("Infinited", 1, -1, 2, 3);

    SetClassPropertyCardinalityRestriction(classA3, propEmpty, 2, 3);
    SetClassPropertyCardinalityRestriction(classB, propEmpty, 4, 5);
    propList.Add("Empty", 4, 5, 4, 3);


    //
    auto instance2 = CreateInstance(classB);

    SetClassParent(classB, classA3, 1);

    auto instance3 = CreateInstance(classB);

    //
    CheckPropertiesExpected(instance3, propList);
    CheckPropertiesExpected(instance2, propList);
    CheckPropertiesExpected(instance1, propList);

    CloseModel(model);
}

/// <summary>
/// 
/// </summary>
void InstancePropertiesTests()
{
    /**TODO: mutlipatent test
    MultiParentsCardinality(DATATYPEPROPERTY_TYPE_CHAR);
    MultiParentsCardinality(OBJECTPROPERTY_TYPE);*/

    SubclassChangesCardianlity(true, DATATYPEPROPERTY_TYPE_CHAR);
    SubclassChangesCardianlity(true, OBJECTPROPERTY_TYPE);
    /** TODO: late SetCardinality
    SubclassChangesCardianlity(false, OBJECTPROPERTY_TYPE);
    SubclassChangesCardianlity(false, DATATYPEPROPERTY_TYPE_CHAR);
    */
}
