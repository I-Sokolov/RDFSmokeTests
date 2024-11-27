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

static void SchemaRuleByIterator(SdaiModel model)
{
    int numRules[3] = { 0, 0, 0 };

    SchemaDecl rule = NULL;
    while (rule = engiGetSchemaRuleByIterator(model, rule)) {

        auto type = engiGetDeclarationType(rule);
        ASSERT(type >= enum_express_declaration::__FUNCTION && type <= enum_express_declaration::__SCHEMA_RULE);
        numRules[(int)type - (int)enum_express_declaration::__FUNCTION]++;
    }

    ASSERT(numRules[0] == 47 && numRules[1] == 0 && numRules[2] == 2);
}

extern void SchemaReadingTests()
{
    ENTER_TEST;

    auto model = sdaiCreateModelBN("IFC4");

    SchemaRuleByIterator(model);
        
    ReadUniqueRules(model);

    ReadWhereRules(model);

    sdaiCloseModel(model);
}
