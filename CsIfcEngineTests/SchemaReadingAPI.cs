using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading.Tasks.Dataflow;
using RDF;
using static Microsoft.FSharp.Core.ByRefKinds;

namespace CsIfcEngineTests
{
    internal class SchemaReadingAPI : CsTests.TestBase
    {
        public static void Run()
        {
            ENTER_TEST();

            var model = ifcengine.sdaiCreateModelBN("IFC4");

            IterateEntities(model);

            IterateAttributes(model);

            ifcengine.sdaiCloseModel(model);
        }

        class AggrDef
        {
            public enum_express_aggr aggrType;
            public int cardinalityMin;
            public int cardinalityMax;
            public bool optional;
            public bool unique;
            public AggrDef nextLevel;

            public AggrDef(
                     enum_express_aggr aggrType,
                     int cardinalityMin,
                     int cardinalityMax,
                     bool optional,
                     bool unique,
                     AggrDef nextLevel
                )
            {
                this.aggrType = aggrType;
                this.cardinalityMin = cardinalityMin;
                this.cardinalityMax = cardinalityMax;
                this.optional = optional;
                this.unique = unique;
                this.nextLevel = nextLevel;
            }
        }

        class AttrTrairs
        {
            public string name;
            public string definingEntity;
            public enum_express_attr_type attrType;
            public string domainEntity;
            public bool direct;
            public bool inverse;
            public bool optional;
            public AggrDef aggrDef;

            public AttrTrairs(
                string name,
                string definingEntity,
                bool direct,
                bool inverse,
                enum_express_attr_type attrType,
                string domainEntity,
                bool optional,
                AggrDef aggrDef
                )
            {
                this.name = name;
                this.definingEntity = definingEntity;
                this.attrType = attrType;
                this.domainEntity = domainEntity;
                this.direct = direct;
                this.inverse = inverse;
                this.optional = optional;
                this.aggrDef = aggrDef;
            }
        }

        static void IterateAttributes(Int64 model)
        {
            Int64 wall = ifcengine.sdaiGetEntity(model, "IfcWall");
            var wallAttr = new List<AttrTrairs>();

            FillWallAttr(wallAttr);

            IterateAttributes(wall, wallAttr.ToArray());

            var str = ifcengine.engiGetAttrNameByIndex(wall, 2);
            ASSERT(str == "Name");
            bool b = ifcengine.engiIsAttrDirectBN(wall, "Name");
            ASSERT(b);
            b = ifcengine.engiIsAttrInverseBN(wall, "Name");
            ASSERT(!b);
            b = ifcengine.engiIsAttrOptionalBN(wall, "Name");
            ASSERT(b);
            str = ifcengine.engiGetAttrDomainNameBN (wall, "Name");
            ASSERT(str == "IfcLabel");

            Int64 ind = ifcengine.engiGetAttrIndexBN (wall, "WallType");
            ASSERT(ind == -1);
            ind = ifcengine.engiGetAttrIndexBN(wall, "PredefinedType");
            ASSERT(ind == 32);
            ind = ifcengine.engiGetAttrIndexExBN(wall, "PredefinedType", true, false);
            ASSERT(ind == 8);

            var attr = ifcengine.sdaiGetAttrDefinition(wall, "Name");
            var tp = ifcengine.engiGetAttrPrimitiveType(attr);
            ASSERT(tp == ifcengine.sdaiSTRING);

            ifcengine.engiGetAttrPrimitiveTypeByIndex(wall, 900, out tp);
            ASSERT(tp == 0);
            ifcengine.engiGetAttrPrimitiveTypeByIndex(wall, 32, out tp);
            ASSERT(tp == ifcengine.sdaiENUM);
        }

        static void IterateAttributes(Int64 entity, AttrTrairs[] traits)
        {
            Int64 attr = 0;
            int ind = 0;
            while (0 != (attr = ifcengine.engiGetEntityAttributeByIterator(entity, attr)))
            {
                string name;
                Int64 definingEntity;
                bool direct;
                bool inverse;
                enum_express_attr_type attrType;
                Int64 domainEntity;
                Int64 aggregationDefinition;
                bool optional;
                ifcengine.engiGetAttrTraits(attr, out name, out definingEntity, out direct, out inverse, out attrType, out domainEntity, out aggregationDefinition, out optional);

#if WRITE
                Console.WriteLine("wallAttr.Add(new AttrTrairs(");
#endif
                Check(name, traits[ind].name);

                string defEntityName = null;
                if (definingEntity != 0)
                    defEntityName = ifcengine.engiGetEntityName(definingEntity);
                Check(defEntityName, traits[ind].definingEntity);

                Check(direct, traits[ind].direct);
                Check(inverse, traits[ind].inverse);
                Check(attrType, traits[ind].attrType);

                string domainName = null;
                if (domainEntity != 0)
                    domainName = ifcengine.engiGetEntityName(domainEntity);
                Check(domainName, traits[ind].domainEntity);

                Check(optional, traits[ind].optional);

                TestAggregationDefinition(aggregationDefinition, traits[ind].aggrDef);

                var str = ifcengine.engiGetAttrName(attr);
                ASSERT(str == name);
                var b = ifcengine.engiIsAttrDirect(attr);
                ASSERT(b == direct);
                b = ifcengine.engiIsAttrInverse(attr);
                ASSERT(b == inverse);
                b = ifcengine.engiIsAttrOptional(attr);
                ASSERT(b == optional);
                Int64 val = ifcengine.engiGetAttrDefiningEntity(attr);
                ASSERT(val == definingEntity);
                str = ifcengine.engiGetAttrDomainName(attr);
                ASSERT(str == domainName);

#if WRITE
                Console.WriteLine("));");
#endif

                ind++;
            }
            ASSERT(ind == traits.Length);
        }

        static void TestAggregationDefinition(Int64 aggregationDefinition, AggrDef aggrDef)
        {
            if (aggregationDefinition == 0)
            {
#if WRITE
                Console.WriteLine("null");
#endif
                ASSERT(aggrDef == null);
                return;
            }
            else
            {
                ASSERT(aggrDef != null);
            }


            enum_express_aggr aggrType;
            Int64 cardinalityMin;
            Int64 cardinalityMax;
            bool optional;
            bool unique;
            Int64 nextAggregationLevel;

            ifcengine.engiGetAggregationDefinition(aggregationDefinition, out aggrType, out cardinalityMin, out cardinalityMax, out optional, out unique, out nextAggregationLevel);

#if WRITE
            Console.WriteLine("new AggrDef(");
#endif

            Check(aggrType, aggrDef.aggrType);
            Check(cardinalityMin, aggrDef.cardinalityMin);
            Check(cardinalityMax, aggrDef.cardinalityMax);
            Check(optional, aggrDef.optional);
            Check(unique, aggrDef.unique);

            TestAggregationDefinition(nextAggregationLevel, aggrDef.nextLevel);

#if WRITE
            Console.WriteLine(")");
#endif
        }

        static void Check(Int64 i, Int64 expected)
            {
#if WRITE
            System.Console.Write (i);
            System.Console.WriteLine(",");
#endif
            ASSERT(i == expected);
        }

        static void Check(string str, string expected)
        {
#if WRITE
            if (str != null)
            {
                System.Console.Write('"');
                System.Console.Write(str);
                System.Console.Write('"');
            }
            else
            {
                System.Console.Write("null");
            }
            System.Console.WriteLine(",");
#endif
            ASSERT(str == expected);
        }

        static void Check(bool b, bool expected)
        {
#if WRITE
            System.Console.Write(b ? "true" : "false");
            System.Console.WriteLine(",");
#endif
            ASSERT(b==expected);
        }

        static void Check (enum_express_attr_type attrType, enum_express_attr_type expected)
        {
#if WRITE
            System.Console.Write("enum_express_attr_type.");
            System.Console.Write (attrType.ToString());
            System.Console.WriteLine(",");
#endif
            ASSERT(attrType == expected);
        }

        static void Check(enum_express_aggr aggrType, enum_express_aggr expected)
        {
#if WRITE
            System.Console.Write("enum_express_aggr.");
            System.Console.Write(aggrType.ToString());
            System.Console.WriteLine(",");
#endif
            ASSERT(aggrType == expected);
        }

        static void IterateEntities(Int64 model)
        {
            int definedTypes = 0;
            int enumTypes = 0;
            int selectTypes = 0;
            int entities = 0;

            Int64 it = 0;
            while (0 != (it = ifcengine.engiGetNextTypeDeclarationIterator(model, it)))
            {
                Int64 decl = ifcengine.engiGetTypeDeclarationFromIterator(model, it);
                ASSERT(decl);

                var type = ifcengine.engiGetDeclarationType(decl);
                switch (type) 
                {
                    case enum_express_declaration.__DEFINED_TYPE:
                        definedTypes++;
                        break;
                    case enum_express_declaration.__SELECT:
                        selectTypes++;
                        break;
                    case enum_express_declaration.__ENUM:
                        enumTypes++;
                        break;
                    case enum_express_declaration.__ENTITY:
                        entities++;
                        break;
                    default:
                        ASSERT(false);
                        break;
                }
            }
            ASSERT(definedTypes == 130 && selectTypes == 60 && enumTypes == 207 & entities == 776);            
        }


        static void FillWallAttr(List<AttrTrairs> wallAttr)
        {
            wallAttr.Add(new AttrTrairs(
            "GlobalId",
            "IfcRoot",
            true,
            false,
            enum_express_attr_type.__NONE,
            "IfcGloballyUniqueId",
            false,
            null
            ));

            wallAttr.Add(new AttrTrairs(
            "OwnerHistory",
            "IfcRoot",
            true,
            false,
            enum_express_attr_type.__NONE,
            "IfcOwnerHistory",
            true,
            null
            ));

            wallAttr.Add(new AttrTrairs(
            "Name",
            "IfcRoot",
            true,
            false,
            enum_express_attr_type.__NONE,
            "IfcLabel",
            true,
            null
            ));

            wallAttr.Add(new AttrTrairs(
            "Description",
            "IfcRoot",
            true,
            false,
            enum_express_attr_type.__NONE,
            "IfcText",
            true,
            null
            ));

            wallAttr.Add(new AttrTrairs(
            "HasAssignments",
            "IfcObjectDefinition",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelAssigns",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));

            wallAttr.Add(new AttrTrairs(
            "Nests",
            "IfcObjectDefinition",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelNests",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            1,
            false,
            false,
            null
            )
            ));

            wallAttr.Add(new AttrTrairs(
            "IsNestedBy",
            "IfcObjectDefinition",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelNests",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));

            wallAttr.Add(new AttrTrairs(
            "HasContext",
            "IfcObjectDefinition",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelDeclares",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            1,
            false,
            false,
            null
            )
            ));

            wallAttr.Add(new AttrTrairs(
            "IsDecomposedBy",
            "IfcObjectDefinition",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelAggregates",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));

            wallAttr.Add(new AttrTrairs(
            "Decomposes",
            "IfcObjectDefinition",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelAggregates",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            1,
            false,
            false,
            null
            )
            ));

            wallAttr.Add(new AttrTrairs(
            "HasAssociations",
            "IfcObjectDefinition",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelAssociates",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));

            wallAttr.Add(new AttrTrairs(
            "ObjectType",
            "IfcObject",
            true,
            false,
            enum_express_attr_type.__NONE,
            "IfcLabel",
            true,
            null
            ));

            wallAttr.Add(new AttrTrairs(
            "IsDeclaredBy",
            "IfcObject",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelDefinesByObject",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            1,
            false,
            false,
            null
            )
            ));

            wallAttr.Add(new AttrTrairs(
            "Declares",
            "IfcObject",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelDefinesByObject",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));

            wallAttr.Add(new AttrTrairs(
            "IsTypedBy",
            "IfcObject",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelDefinesByType",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            1,
            false,
            false,
            null
            )
            ));

            wallAttr.Add(new AttrTrairs(
            "IsDefinedBy",
            "IfcObject",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelDefinesByProperties",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));

            wallAttr.Add(new AttrTrairs(
            "ObjectPlacement",
            "IfcProduct",
            true,
            false,
            enum_express_attr_type.__NONE,
            "IfcObjectPlacement",
            true,
            null
            ));

            wallAttr.Add(new AttrTrairs(
            "Representation",
            "IfcProduct",
            true,
            false,
            enum_express_attr_type.__NONE,
            "IfcProductRepresentation",
            true,
            null
            ));

            wallAttr.Add(new AttrTrairs(
            "ReferencedBy",
            "IfcProduct",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelAssignsToProduct",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));

            wallAttr.Add(new AttrTrairs(
            "Tag",
            "IfcElement",
            true,
            false,
            enum_express_attr_type.__NONE,
            "IfcIdentifier",
            true,
            null
            ));

            wallAttr.Add(new AttrTrairs(
            "FillsVoids",
            "IfcElement",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelFillsElement",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            1,
            false,
            false,
            null
            )
            ));

            wallAttr.Add(new AttrTrairs(
            "ConnectedTo",
            "IfcElement",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelConnectsElements",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));
            wallAttr.Add(new AttrTrairs(
            "IsInterferedByElements",
            "IfcElement",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelInterferesElements",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));
            wallAttr.Add(new AttrTrairs(
            "InterferesElements",
            "IfcElement",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelInterferesElements",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));
            wallAttr.Add(new AttrTrairs(
            "HasProjections",
            "IfcElement",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelProjectsElement",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));
            wallAttr.Add(new AttrTrairs(
            "ReferencedInStructures",
            "IfcElement",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelReferencedInSpatialStructure",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));
            wallAttr.Add(new AttrTrairs(
            "HasOpenings",
            "IfcElement",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelVoidsElement",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));
            wallAttr.Add(new AttrTrairs(
            "IsConnectionRealization",
            "IfcElement",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelConnectsWithRealizingElements",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));
            wallAttr.Add(new AttrTrairs(
            "ProvidesBoundaries",
            "IfcElement",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelSpaceBoundary",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));
            wallAttr.Add(new AttrTrairs(
            "ConnectedFrom",
            "IfcElement",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelConnectsElements",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));
            wallAttr.Add(new AttrTrairs(
            "ContainedInStructure",
            "IfcElement",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelContainedInSpatialStructure",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            1,
            false,
            false,
            null
            )
            ));
            wallAttr.Add(new AttrTrairs(
            "HasCoverings",
            "IfcElement",
            false,
            true,
            enum_express_attr_type.__NONE,
            "IfcRelCoversBldgElements",
            false,
            new AggrDef(
            enum_express_aggr.__SET,
            0,
            -1,
            false,
            false,
            null
            )
            ));
            wallAttr.Add(new AttrTrairs(
            "PredefinedType",
            "IfcWall",
            true,
            false,
            enum_express_attr_type.__NONE,
            "IfcWallTypeEnum",
            true,
            null
            ));
        }

    }
}