#include "pch.h"

static void ReadUniqueRules(SdaiModel model)
{
    auto app = sdaiGetEntity(model, "IfcApplication");
    ASSERT(app);
    
    UniqueRule uniq = engiGetEntityUniqueRuleByIterator(app, 0, nullptr);
    ASSERT(uniq);

    SdaiString label;
    uniq = engiGetEntityUniqueRuleByIterator(app, uniq, &label);
    ASSERT(uniq);

    uniq = engiGetEntityUniqueRuleByIterator(app, uniq, nullptr);
    ASSERT(!uniq);
}

static void ReadWhereRules(SdaiModel model)
{
    auto angle = sdaiGetEntity(model, "IfcCompoundPlaneAngleMeasure");
    ASSERT(angle);

    ExpressScript rule = engiGetEntityWhereRuleByIterator(angle, 0, NULL);
    ASSERT(rule);
    ASSERT(engiGetDeclarationType(rule) == enum_express_declaration::__WHERE_RULE);

    const char* name = NULL;
    const char* body = NULL;
    engiGetScriptText(rule, &name, &body);
    ASSERT(!strcmp(name, "MinutesInRange") && !strcmp(body, "ABS(SELF[2]) < 60;"));

    rule = engiGetEntityWhereRuleByIterator(angle, rule, &name);
    ASSERT(rule); 
    ASSERT(engiGetDeclarationType(rule) == enum_express_declaration::__WHERE_RULE);
    ASSERT(!strcmp(name, "SecondsInRange"));

    engiGetScriptText(rule, &name, &body);
    ASSERT(!strcmp(name, "SecondsInRange") && !strcmp(body, "ABS(SELF[3]) < 60;"));

    rule = engiGetEntityWhereRuleByIterator(angle, rule, NULL);
    ASSERT(rule);
    ASSERT(engiGetDeclarationType(rule) == enum_express_declaration::__WHERE_RULE);

    rule = engiGetEntityWhereRuleByIterator(angle, rule, NULL);
    ASSERT(rule);
    ASSERT(engiGetDeclarationType(rule) == enum_express_declaration::__WHERE_RULE);

    rule = engiGetEntityWhereRuleByIterator(angle, rule, NULL);
    ASSERT(!rule);
}

static void SchemaRuleByIterator(SdaiModel model)
{
    int numRules[3] = { 0, 0, 0 };
    const char* name1 = "IfcAssociatedSurface";

    SchemaDecl rule = NULL;
    while (rule = engiGetSchemaScriptDeclarationByIterator(model, rule)) {

        auto type = engiGetDeclarationType(rule);
        ASSERT(type >= enum_express_declaration::__FUNCTION && type <= enum_express_declaration::__SCHEMA_RULE);
        numRules[(int)type - (int)enum_express_declaration::__FUNCTION]++;

        if (name1) {
            const char* name = NULL;
            engiGetScriptText(rule, &name, NULL);
            ASSERT(0==strcmp(name, name1));
            name1 = NULL;
        }
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
