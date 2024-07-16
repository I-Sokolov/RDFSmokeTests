using System;
using RDF;
using GEOM;
using System.Diagnostics;

namespace CsEngineTests
{
    class EarlyBinding : CsTests.TestBase
	{
		/// <summary>
		/// 
		/// </summary>
		/// <param name="args"></param>
        public static void Run()
        {
            var model = engine.OpenModel(null as byte[]);

            CreateRedBox(model);

			MoreExamplesToAccessDifferentTypesOfProperties(model);

            engine.CloseModel(model);
        }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="model"></param>
		/// <returns></returns>
        static Int64 CreateRedBox(Int64 model)
        {
			ENTER_TEST();

			//
			// create colored material
			//

			ColorComponent colorComponent = ColorComponent.Create(model);
			colorComponent.R = 0.9;
			colorComponent.G = 0;
			colorComponent.B = 0;

			//you can use instance and property handlers API
			Int64 propW = engine.GetPropertyByName(model, "W");
			double w = 0.5;
			engine.SetDatatypeProperty(colorComponent, propW, ref w, 1);
			//the code above is equivalent to
			colorComponent.W = 0.5;

			//or you easy use existing instance handlers with classes
			Int64 colorClass = engine.GetClassByName(model, "Color");
			Int64 colorInstance = engine.CreateInstance(colorClass, null as string);

			//get wrapper object from instance handler
			Color color = new Color(colorInstance);
			color.ambient = colorComponent;

			Material material = Material.Create(model);
			material.color = color;

			//
			Box box = Box.Create(model);

			box.height = 3;
			box.width = 2;
			box.length = 4;
			box.material = material;  //material is inherited from GeometricItem

			return box;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name=""></param>
		static void MoreExamplesToAccessDifferentTypesOfProperties(Int64 model)
        {
			ENTER_TEST();

			Texture texture = Texture.Create(model);
			var curve = NURBSCurve.Create(model);

			Int64 curve_id = curve;
			Int64 texture_id = texture;

			ASSERT(new GeometricItem(curve) != 0);
			ASSERT(new GeometricItem(texture) == 0);
			//implicit cast altrenative
			ASSERT ((GeometricItem)curve_id != 0); 
			ASSERT ((GeometricItem)texture_id == 0); 

			//double
			double? lseg = curve.segmentationLength;
			ASSERT(lseg == null);
			curve.segmentationLength = 0.5;
			lseg = curve.segmentationLength;
			ASSERT(lseg == 0.5);

			//double []
			double[] org = texture.origin;
			ASSERT(org == null);
			org = new double[] { 1, 2, 3 };
			texture.origin = org;
			double[] ret_org = texture.origin;
			ASSERT_ARR_EQ(org, ret_org);

			//there is ability to identity property by name
			org[1] = 10;
			texture.SetDatatypeProperty("origin", org);
			ret_org = texture.GetDatatypeProperty_double("origin");
			ASSERT_ARR_EQ(org, ret_org);

			//false b/c cardinality restriction violation
			var tooLong = new double[] { 1, 2, 3, 4 };
			bool ok = true;
			try
				{
				texture.origin = tooLong;
				}
			catch (SetPropertyException)
				{
				ok = false;
				}
			ASSERT(!ok);

			//false b/c wrong property name
			ok = texture.SetDatatypeProperty("length", org);
			ASSERT(!ok);
			ret_org = texture.GetDatatypeProperty_double("originnn");
			ASSERT(ret_org == null);

			//long
			long? setting = curve.setting;
			ASSERT(setting == null);
			curve.setting=13;
			setting = curve.setting;
			ASSERT(setting == 13);

			//long[]
			long[] km = curve.knotMultiplicities;
			ASSERT(km == null);
			long[] km_ = new long[] { 3, 5, 6 };
			curve.knotMultiplicities = km_;
			km = curve.knotMultiplicities;
			ASSERT_ARR_EQ(km, km_);

			//string 
			string tname = texture.name;
			ASSERT(tname == null);
			texture.name = "test";
			tname = texture.name;
			ASSERT(tname == "test");

			//string[]
			//No example in Geometry Kernel

			//bool
			bool? closed = curve.closed;
			ASSERT(closed == null);
			curve.closed = true;
			closed = curve.closed;
			ASSERT(closed.Value);

			//bool[]
			//No example in Geometry Kernel

			//object
			Material material = curve.material;
			ASSERT(material == null);
			Int64 mat = Material.Create(model);
			curve.material = new Material(mat);
			material = curve.material;
			ASSERT(material == mat);
			var m2 = curve.material;
			ASSERT(m2 == material);

			//object []
			Point3D[] ptg = curve.controlPoints;
			ASSERT(ptg == null);
			Int64[] ptg64 = curve.controlPoints_h;
			ASSERT(ptg64 == null);

			Point3D[] pts = new Point3D[2];
			pts[0] = Point3D.Create(model);
			pts[1] = Point3D.Create(model);
			ASSERT(pts[0] != pts[1]);

			curve.Set_controlPoints(pts);

			ptg = curve.controlPoints;
			ASSERT(ptg.Length == pts.Length);
			for (int i = 0; i < pts.Length; i++) ASSERT(pts[i] == ptg[i]);
			ASSERT_ARR_EQ(ptg, pts);

			ptg64 = curve.controlPoints_h;
			ASSERT(ptg64.Length == pts.Length);
			for (int i = 0; i < pts.Length; i++) ASSERT(pts[i] == ptg64[i]);

		}

		/// <summary>
		/// 
		/// </summary>
		/// <typeparam name="TElem"></typeparam>
		/// <param name="a"></param>
		/// <param name="b"></param>
		static void ASSERT_ARR_EQ<TElem>(TElem[] a, TElem[] b) where TElem : IComparable
		{
			ASSERT(a.Length == b.Length);

			for (int i = 0; i < a.Length; i++)
			{
				ASSERT(a[i].CompareTo(b[i]) == 0);
			}
		}

	}
}
