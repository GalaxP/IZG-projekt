
#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "discradTest"
#include <tests/testCommon.hpp>

#include <tests/draw_raster/util_raster_part_test.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("discard test");

  TestingTriangle testingTriangle;
  testingTriangle.fragmentColors  = true;
  testingTriangle.fragmentDiscard = true;
  testingTriangle.hasColorBuffer  = true;
  testingTriangle.test(0,"discard test");
}
