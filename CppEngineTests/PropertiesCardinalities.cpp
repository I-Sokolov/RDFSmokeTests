
#include "pch.h"

//TODO - check parent class to existing instance
//TODO - check add property to existing instance

struct PropInfo
{
    PropInfo (const char* name_, int64_t cardMin_, int64_t cardMax_, int64_t cardMinAggr_, int64_t cardMaxAggr_)
        :name(name_), cardMin(cardMin_), cardMax(cardMax_), cardMinAggr(cardMinAggr_), cardMaxAggr(cardMaxAggr_) {}

    std::string name;
    int64_t     cardMin;
    int64_t     cardMax;
    int64_t     cardMinAggr;
    int64_t     cardMaxAggr;
};

typedef std::list<PropInfo> PropList;

/// <summary>
/// 
/// </summary>
static void CheckPropertiesExpected(OwlInstance instance, PropList& expectedProps)
{
    auto cls = GetInstanceClass(instance);

    auto prop = GetInstancePropertyByIterator(instance, 0);
    auto itExpected = expectedProps.begin();

    while (prop && itExpected != expectedProps.end()) 
    {
        auto name = GetNameOfProperty(prop);

        ASSERT(!strcmp(name, itExpected->name.c_str()));

        int64_t minCard, maxCard;
        GetClassPropertyCardinalityRestriction(cls, prop, &minCard, &maxCard);
        ASSERT(minCard == itExpected->cardMin && maxCard == itExpected->cardMax);

        GetClassPropertyAggregatedCardinalityRestriction (cls, prop, &minCard, &maxCard);
        ASSERT(minCard == itExpected->cardMinAggr && maxCard == itExpected->cardMaxAggr);

        prop = GetInstancePropertyByIterator(instance, prop);
        itExpected++;
    }

    ASSERT(prop == NULL && itExpected == expectedProps.end());
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

    PropList propList;

    auto instanceB0 = CreateInstance(classB);
    CheckPropertiesExpected(instanceB0, propList);

    SetClassPropertyCardinalityRestriction(classA, propEnanced, 0, 1);
    SetClassPropertyCardinalityRestriction(classB, propEnanced, 0, 2);
    propList.push_back(PropInfo("Enhanced", 0, 2, 0, 1));

    SetClassPropertyCardinalityRestriction(classA, propResticted, 9, 12);
    SetClassPropertyCardinalityRestriction(classB, propResticted, 9, 11);
    propList.push_back(PropInfo("Restricted", 9, 11, 9, 11));

    SetClassPropertyCardinalityRestriction(classA, propFinited, 3, -1);
    SetClassPropertyCardinalityRestriction(classB, propFinited, 2, 4);
    propList.push_back(PropInfo("Finited", 2, 4, 3, 4));

    SetClassPropertyCardinalityRestriction(classA, propInfinited, 2, 3);
    SetClassPropertyCardinalityRestriction(classB, propInfinited, 1, -1);
    propList.push_back(PropInfo("Infinited", 1, -1, 2, 3));

    SetClassPropertyCardinalityRestriction(classA, propEmpty, 2, 3);
    SetClassPropertyCardinalityRestriction(classB, propEmpty, 4, 5);
    propList.push_back(PropInfo("Empty", 4, 5, 4, 3));

    auto instance = CreateInstance(classB);

    if (!earlySetParent) {
        SetClassParent(classB, classA, 1);
    }

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
    propList.push_back(PropInfo("Enhanced", 0, 2, 0, 1));

    SetClassPropertyCardinalityRestriction(classA2, propResticted, 9, 12);
    SetClassPropertyCardinalityRestriction(classB, propResticted, 9, 11);
    propList.push_back(PropInfo("Restricted", 9, 11, 9, 11));

    SetClassPropertyCardinalityRestriction(classA3, propFinited, 3, -1);
    SetClassPropertyCardinalityRestriction(classB, propFinited, 2, 4);
    propList.push_back(PropInfo("Finited", 2, 4, 3, 4));

    SetClassPropertyCardinalityRestriction(classA1, propInfinited, 2, 3);
    SetClassPropertyCardinalityRestriction(classB, propInfinited, 1, -1);
    propList.push_back(PropInfo("Infinited", 1, -1, 2, 3));

    SetClassPropertyCardinalityRestriction(classA3, propEmpty, 2, 3);
    SetClassPropertyCardinalityRestriction(classB, propEmpty, 4, 5);
    propList.push_back(PropInfo("Empty", 4, 5, 4, 3));


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
