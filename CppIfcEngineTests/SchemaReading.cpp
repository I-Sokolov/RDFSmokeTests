#include "pch.h"

static void ReadUniqueRules(SdaiModel model)
{
    auto app = sdaiGetEntity(model, "IfcApplication");
    ASSERT(app);
    
    UniqueRule uniq = engiGetEntityUniqueRuleByIterator(app, 0);
    ASSERT(uniq);

    SdaiString label;
    uniq = engiGetEntityUniqueRuleByIterator(app, uniq, &label);
    ASSERT(uniq);

    uniq = engiGetEntityUniqueRuleByIterator(app, uniq);
    ASSERT(!uniq);
}

static void ReadWhereRules(SdaiModel model)
{
    auto angle = sdaiGetEntity(model, "IfcCompoundPlaneAngleMeasure");
    ASSERT(angle);

    ExpressScript rule = engiGetEntityWhereRuleByIterator(angle, 0);
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

    rule = engiGetEntityWhereRuleByIterator(angle, rule);
    ASSERT(rule);
    ASSERT(engiGetDeclarationType(rule) == enum_express_declaration::__WHERE_RULE);

    rule = engiGetEntityWhereRuleByIterator(angle, rule);
    ASSERT(rule);
    ASSERT(engiGetDeclarationType(rule) == enum_express_declaration::__WHERE_RULE);

    rule = engiGetEntityWhereRuleByIterator(angle, rule);
    ASSERT(!rule);
}

static void SchemaRuleByIterator(SdaiModel model)
{
    int numRules[3] = { 0, 0, 0 };
    const char* name1 = "IfcAssociatedSurface";

    SchemaDecl rule = NULL;
    while (rule = engiGetSchemaScriptDeclarationByIterator(model, rule)) {

        auto type = engiGetDeclarationType(rule);
        ASSERT(type >= enum_express_declaration::__FUNCTION && type <= enum_express_declaration::__GLOBAL_RULE);
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

static void IterateAllAttr(SdaiModel model)
{
    int nEntities = 0;
    int nAttributes = 0;
    SchemaTypeIterator it = 0;
    while (0 != (it = engiGetNextTypeDeclarationIterator(model, it))) {
        
        auto entity = engiGetTypeDeclarationFromIterator(model, it);

        if (engiGetDeclarationType(entity) == enum_express_declaration::__ENTITY) {
            nEntities++;

            SdaiAttr attr = 0;
            while (0 != (attr = ::engiGetEntityAttributeByIterator(entity, attr))) {
                nAttributes++;

            }

        }
    }
    
    ASSERT(nEntities==2140 &&nAttributes==9765);
}

static void CheckRedeclaration(SdaiModel model,
    const char* entityName,
    const char* attrName,
    const char* definingEntityName,
    const char* defindedDomain,
    const char* redeclaredEnity,
    const char* redeclaredDomain
)
{
    auto entity = sdaiGetEntity(model, entityName);
    ASSERT(entity);

    auto attr = sdaiGetAttrDefinition(entity, attrName);
    ASSERT(attr);

    auto definingEntity = engiGetAttrDefiningEntity(attr);
    ASSERT(definingEntity && definingEntity == sdaiGetEntity(model, definingEntityName));

    auto domain = engiGetAttrDomain(attr);
    ASSERT(0 == strcmp(domain, defindedDomain));

    auto redeclaration = engiGetAttrRedeclarationByIterator(entity, attr, NULL);
    ASSERT(redeclaration);

    auto redeclEntity = engiGetAttrDefiningEntity(redeclaration);
    ASSERT(redeclEntity && redeclEntity == sdaiGetEntity(model, redeclaredEnity));

    auto redeclDomain = engiGetAttrDomain(redeclaration);
    ASSERT(0 == strcmp(redeclDomain, redeclaredDomain));

    auto next = engiGetAttrRedeclarationByIterator(entity, attr, redeclaration);
    ASSERT(!next);
}

static void CheckAP242Redeclarations(SdaiModel model)
{
    /*
ENTITY shape_aspect_occurrence
  definition : shape_aspect_or_characterized_object;

ENTITY counterbore_hole_occurrence  SUBTYPE OF (shape_aspect_occurrence);
  SELF\shape_aspect_occurrence.definition : counterbore_hole_definition;

ENTITY counterbore_hole_occurrence_in_assembly  SUBTYPE OF (counterbore_hole_occurrence);

ENTITY spotface_occurrence_in_assembly  SUBTYPE OF (counterbore_hole_occurrence_in_assembly);
  SELF\counterbore_hole_occurrence.definition : spotface_hole_definition;
    */
    CheckRedeclaration(model, 
        "spotface_occurrence_in_assembly","definition", 
        "shape_aspect_occurrence", "shape_aspect_or_characterized_object",
        "spotface_occurrence_in_assembly", "spotface_hole_definition");

    CheckRedeclaration(model,
        "counterbore_hole_occurrence_in_assembly", "definition",
        "shape_aspect_occurrence", "shape_aspect_or_characterized_object",
        "counterbore_hole_occurrence", "counterbore_hole_definition");
}

extern void SchemaReadingTests()
{
    ENTER_TEST;
    
    //
    auto model = sdaiCreateModelBN("AP242");

    IterateAllAttr(model);

    CheckAP242Redeclarations(model);

    sdaiCloseModel(model);

    //
    model = sdaiCreateModelBN("IFC4");

    SchemaRuleByIterator(model);
        
    ReadUniqueRules(model);

    ReadWhereRules(model);

    sdaiCloseModel(model);
}
