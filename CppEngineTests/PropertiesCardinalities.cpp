
#include "pch.h"

struct Cardinality
{
    Cardinality (int64_t cardMin_=0, int64_t cardMax_=0, int64_t cardMinAggr_=0, int64_t cardMaxAggr_=0)
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
static void CheckPropertiesExpected(OwlInstance instance, PropList expectedProps/*copy intended*/,                                     
                                    const char* namea = NULL, const wchar_t* namew = NULL)
{
    auto cls = GetInstanceClass(instance);

    PropList actualProps;
    auto prop = GetInstancePropertyByIterator(instance, 0);
    while (prop)
    {
        auto name = GetNameOfProperty(prop);
        auto& info = actualProps[name];

        GetClassPropertyCardinalityRestriction(cls, prop, &info.cardMin, &info.cardMax);
        GetClassPropertyAggregatedCardinalityRestriction (cls, prop, &info.cardMinAggr, &info.cardMaxAggr);

        prop = GetInstancePropertyByIterator(instance, prop);        
    }

#if 0 //print actual props
    for (auto& p : actualProps) {
        printf("%s:\t%lld, %lld, %lld, %lld\n", p.first.c_str(), p.second.cardMin, p.second.cardMax, p.second.cardMinAggr, p.second.cardMaxAggr);
    }
    printf("---------------------------------\n");
#endif

    auto itA = actualProps.begin();
    auto itE = expectedProps.begin();
    while (itA != actualProps.end() && itE != expectedProps.end()) {
        ASSERT(itA->first == itE->first);
        ASSERT(itA->second.cardMin == itE->second.cardMin);
        ASSERT(itA->second.cardMax == itE->second.cardMax);
        ASSERT(itA->second.cardMinAggr == itE->second.cardMinAggr);
        ASSERT(itA->second.cardMaxAggr == itE->second.cardMaxAggr);
        itA++;
        itE++;
    }

    ASSERT(itA == actualProps.end() && itE == expectedProps.end());

    //instance name
    if (namea) {
        char* nameA;
        GetNameOfInstance(instance, &nameA);
        ASSERT(0 == strcmp(namea, nameA));
    }
    if (namew) {
        wchar_t* nameW;
        GetNameOfInstanceW(instance, &nameW);
        ASSERT(0 == wcscmp(namew, nameW));
    }

}

/// <summary>
/// 
/// </summary>
static void SubclassChangesCardianlity(bool earlySetParent, int64_t type)
{
    ENTER_TEST_EX2(earlySetParent ? "Early set parent" : "Late set parent", type == OBJECTPROPERTY_TYPE ? "object" : "datatype");

    int64_t model = OpenModel((const char*)0);

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

    auto instanceB1 = CreateInstance(classB);

    if (!earlySetParent) {
        SetClassParent(classB, classA, 1);
    }

    auto instance = CreateInstance(classB);
    CheckPropertiesExpected(instance, propList);

    //instance created before SetClassParent
    CheckPropertiesExpected(instanceB1, propList);
    //instance created before assigning properties
    CheckPropertiesExpected(instanceB0, propList);

    CloseModel(model);
}

/// <summary>
/// 
/// </summary>
static void MultiParentsCardinalityAndInstanceName(int64_t type)
{
    ENTER_TEST_EX(type == OBJECTPROPERTY_TYPE ? "object" : "datatype");

    int64_t model = OpenModel((const char*)0);

    auto classA1 = CreateClass(model, "ClassA1");
    auto classA2 = CreateClass(model, "ClassA2");
    auto classA3 = CreateClass(model, "ClassA3");

    auto classB = CreateClass(model, "classB");
    SetClassParent(classB, classA1, 1);
    SetClassParent(classB, classA2, 1);

    auto instance1 = CreateInstance(classB, "instance1");

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

    auto propOnlyA3 = CreateProperty(model, type, "OnlyForClassA3");
    SetClassPropertyCardinalityRestriction(classA3, propOnlyA3, 2, -1);
    propList.Add("OnlyForClassA3", -1, -1, 2, -1);

    auto propOnlyA2 = CreateProperty(model, type, "OnlyForClassA2");
    SetClassPropertyCardinalityRestriction(classA2, propOnlyA2, 2, 8);
    propList.Add("OnlyForClassA2", -1, -1, 2, 8);

    auto propOnlyB = CreateProperty(model, type, "OnlyForClassB");
    SetClassPropertyCardinalityRestriction(classB, propOnlyB, 1, 8);
    propList.Add("OnlyForClassB", 1, 8, 1, 8);

    //
    auto instance2 = CreateInstanceW(classB, L"instance2");

    SetClassParent(classB, classA3, 1);

    auto instance3 = CreateInstance(classB, "instance3");

    //
    SaveModel(model, "testCardinalities.bin");
    CloseModel(model);
    model = OpenModel("testCardinalities.bin");

    auto read = GetInstancesByIterator(model, 0);
    CheckPropertiesExpected(read, propList, "instance3", L"instance3");

    //instance created before SetClassParent
    read = GetInstancesByIterator(model, read);
    CheckPropertiesExpected(read, propList, "instance2", 0);

    //instance created before assigning properties
    read = GetInstancesByIterator(model, read);
    CheckPropertiesExpected(read, propList, 0, L"instance1");

    ASSERT(!GetInstancesByIterator(model, read));

    CloseModel(model);
}

static void InstanceNameTest()
{
    //
    auto model = OpenModel((const char*)NULL);

    auto cls = GetClassByName(model, "Box");

    CreateInstance(cls, "Test1");
    CreateInstanceW(cls, L"Test2");

    SaveModel(model, "InstancceNameTest.bin");
    CloseModel(model);

    //
    model = OpenModel("InstancceNameTest.bin");

    //
    auto inst = GetInstancesByIterator(model, 0);

    auto nameW = GetNameOfInstanceW(inst);
    ASSERT(0 == wcscmp(nameW, L"Test2"));

    auto nameA = GetNameOfInstance(inst);
    ASSERT(0 == strcmp(nameA, "Test2"));

    //
    inst = GetInstancesByIterator(model, inst);

    nameW = GetNameOfInstanceW(inst);
    ASSERT(0 == wcscmp(nameW, L"Test1"));

    nameA = GetNameOfInstance(inst);
    ASSERT(0 == strcmp(nameA, "Test1"));

    ASSERT(!GetInstancesByIterator(model, inst));

    CloseModel(model);
}

/// <summary>
/// 
/// </summary>
void InstancePropertiesTests()
{
    InstanceNameTest();

    SubclassChangesCardianlity(true, DATATYPEPROPERTY_TYPE_CHAR);
    SubclassChangesCardianlity(true, OBJECTPROPERTY_TYPE);
    SubclassChangesCardianlity(false, OBJECTPROPERTY_TYPE);
    SubclassChangesCardianlity(false, DATATYPEPROPERTY_TYPE_CHAR);

    MultiParentsCardinalityAndInstanceName(DATATYPEPROPERTY_TYPE_CHAR);
    MultiParentsCardinalityAndInstanceName(OBJECTPROPERTY_TYPE);
}
