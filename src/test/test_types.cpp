#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cstdlib>
#include <iostream>
#include <cmath>

#include "../../include/types.h"
#include "../../include/errors.h"


using namespace mru;
using namespace std;

class TypesTest: public CppUnit::TestFixture {
  void testScalar() {
    Scalar<> pii, pi = M_PI;
    Scalar<> pif = std::modf(pi, &pii);
    CPPUNIT_ASSERT_EQUAL((Scalar<>)3, pii);
  }
  void testRotScalarSize() {
    float a = 1.0;
    auto b = RotScalar<>(1.0);
    CPPUNIT_ASSERT_EQUAL(sizeof(a), sizeof(b));
  }
  void testRotScalarSetValue() {
    RotScalar<-1, 1, double> pitch;
    pitch.set_value(d2r(100.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(80.0, r2d(pitch.get_value()), 5E-12);
    pitch.set_value(d2r(820.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(80.0, r2d(pitch.get_value()), 5E-12);
    pitch.set_value(d2r(-30.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-30.0, r2d(pitch.get_value()), 5E-12);
    pitch.set_value(d2r(-90.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-90.0, r2d(pitch.get_value()), 5E-12);
    pitch.set_value(d2r(90.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(90.0, r2d(pitch.get_value()), 5E-12);
    pitch.set_value(d2r(-460.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-80.0, r2d(pitch.get_value()), 5E-12);
    RotScalar<-2, 2> yaw;
    yaw.set_value(d2r(370.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0, r2d(yaw.get_value()), 5E-5);
    yaw.set_value(d2r(190.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-170.0, r2d(yaw.get_value()), 5E-5);
    yaw.set_value(d2r(360.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0, r2d(yaw.get_value()), 5E-5);
    yaw.set_value(d2r(180.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-180.0, r2d(yaw.get_value()), 5E-5);
    yaw.set_value(d2r(-180.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-180.0, r2d(yaw.get_value()), 5E-5);
    yaw.set_value(d2r(-460.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-100.0, r2d(yaw.get_value()), 5E-5);
    RotScalar< 0, 4> heading;
    heading.set_value(d2r(-460.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(260.0, r2d(heading.get_value()), 5E-5);
    heading.set_value(d2r(360.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, r2d(heading.get_value()), 5E-5);
    RotScalar< 0, 1> quart;
    quart.set_value(d2r(-30.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(30.0, r2d(quart.get_value()), 5E-5);
    quart.set_value(d2r(30.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(30.0, r2d(quart.get_value()), 5E-5);
    quart.set_value(d2r(150.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(30.0, r2d(quart.get_value()), 5E-5);
    quart.set_value(d2r(210.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(30.0, r2d(quart.get_value()), 5E-5);
    quart.set_value(d2r(-210.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(30.0, r2d(quart.get_value()), 5E-5);
    quart.set_value(d2r(90.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(90.0, r2d(quart.get_value()), 5E-5);
    quart.set_value(d2r(0.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, r2d(quart.get_value()), 5E-5);
  }
  void testRotScalarOperators() {
    auto rs = RotScalar<>(d2r(45.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(45.0, r2d(rs.get_value()), 5E-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(45.0, rs.to_degrees(), 5E-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5 * M_SQRT2, sin(rs), 5E-5);
    auto rs2 = rs + d2r(405.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(90.0, rs2.to_degrees(), 5E-5);
    auto rs3 = rs * 10;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(90.0, rs3.to_degrees(), 5E-5);
    float val = rs3;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(d2r(90.0), val, 5E-5);
    val = rs3 / 3.0;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(d2r(30.0), val, 5E-5);
    auto rs4 = RotScalar<4, 8, double>(700.0, true);
    auto rs5 = RotScalar<>(380, true);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(d2r(700.0), rs4, 5E-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(d2r(20.0), rs5, 5E-5);
    auto rs6 = rs4 - rs5;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-40.0, rs6.to_degrees(), 5E-5);
    auto rs7 = rs4 - d2r(150.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-170.0, rs7.to_degrees(), 5E-5);
  }
public:
  CPPUNIT_TEST_SUITE(TypesTest);
  CPPUNIT_TEST(testScalar);
  CPPUNIT_TEST(testRotScalarSize);
  CPPUNIT_TEST(testRotScalarSetValue);
  CPPUNIT_TEST(testRotScalarOperators);
  CPPUNIT_TEST_SUITE_END();
};

int main()
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(TypesTest::suite());
  if (runner.run())
    return 0;
  else
    return 1;
} 

