#include "pch.h"

static void ReadUniqueRules(SdaiModel model)
{
    auto app = sdaiGetEntity(model, "IfcApplication");
    ASSERT(app);
    
    SchemaUniqueRule uniq = engiGetEntityUniqueRuleByIterator(app, 0);
    ASSERT(uniq);

    uniq = engiGetEntityUniqueRuleByIterator(app, uniq);
    ASSERT(uniq);

    uniq = engiGetEntityUniqueRuleByIterator(app, uniq);
    ASSERT(!uniq);
}

static void ReadWhereRules(SdaiModel model)
{
    auto angle = sdaiGetEntity(model, "IfcCompoundPlaneAngleMeasure");
    ASSERT(angle);

    SchemaWhereRule rule = engiGetEntityWhereRuleByIterator(angle, 0);
    ASSERT(rule);

    rule = engiGetEntityWhereRuleByIterator(angle, rule);
    ASSERT(rule); 
    
    rule = engiGetEntityWhereRuleByIterator(angle, rule);
    ASSERT(rule);
    
    rule = engiGetEntityWhereRuleByIterator(angle, rule);
    ASSERT(rule);

    rule = engiGetEntityWhereRuleByIterator(angle, rule);
    ASSERT(!rule);
}


extern void SchemaReadingTests()
{
    ENTER_TEST;

    auto model = sdaiCreateModelBN("IFC4");

    ReadUniqueRules(model);

    ReadWhereRules(model);

    sdaiCloseModel(model);
}
