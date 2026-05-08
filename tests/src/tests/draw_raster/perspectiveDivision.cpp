#include <iostream>

#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "perspectiveDivision"
#include <tests/testCommon.hpp>

#include <tests/draw_raster/util_raster_part_test.hpp>

using namespace tests;


SCENARIO(TEST_NAME){
  printTestName("Rasterization: perspective division");
  std::vector<Triangle>triangles = {
    {glm::vec4(-1.0,-1.0,0,1)*.1f,glm::vec4(+1.0,-1.0,0,1)*.3f,glm::vec4(-1.0,+1.0,0,1)*4.f},
  };
  uint32_t counter = 0;
  TestingTriangle testingTriangle;
  for(auto const&t:triangles){
    testingTriangle.triangle = t;
    testingTriangle.test(counter++,"perspective division test");
  }
}

