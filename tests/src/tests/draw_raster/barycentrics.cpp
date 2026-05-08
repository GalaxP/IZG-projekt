#include <iostream>

#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "barycentrics"
#include <tests/testCommon.hpp>
#include <tests/str.hpp>

#include <tests/draw_raster/util_raster_part_test.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("depth interpolation");

  float hc[3] = {1,2,.5};
  float zz[3] = {.9f,.4f,.8f};

  auto triangle = Triangle{
    glm::vec4(-hc[0],-hc[0],zz[0],hc[0]),
    glm::vec4(+hc[1],-hc[1],zz[1],hc[1]),
    glm::vec4(-hc[2],+hc[2],zz[2],hc[2]),
  };

  TestingTriangle testingTriangle;
  testingTriangle.triangle = triangle;
  testingTriangle.test();
}

SCENARIO(TEST_NAME){
  printTestName("vertex attributes interpolated to fragment attributes");

  TestingTriangle testingTriangle;
  testingTriangle.fragmentColors = true;
  testingTriangle.test();
}



SCENARIO(TEST_NAME){
  printTestName("perspective correct interpolation of vertex attributes to fragment attributes");

  float hc[3] = {1,2,.5};
  float zz[3] = {.9f,.4f,.8f};

  auto triangle = Triangle{
    glm::vec4(-hc[0],-hc[0],zz[0],hc[0]),
    glm::vec4(+hc[1],-hc[1],zz[1],hc[1]),
    glm::vec4(-hc[2],+hc[2],zz[2],hc[2]),
  };

  TestingTriangle testingTriangle;
  testingTriangle.triangle = triangle;
  testingTriangle.fragmentColors = true;
  testingTriangle.test();
}
