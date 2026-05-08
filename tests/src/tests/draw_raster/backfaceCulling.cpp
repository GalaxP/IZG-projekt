#include <iostream>

#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "perspectiveDivision"
#include <tests/testCommon.hpp>

#include <tests/draw_raster/util_raster_part_test.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("Rasterization: backface culling tests");

  std::vector<Triangle>triangles = {
    {glm::vec4(-1.0,-1.0,0,1),glm::vec4(+1.0,-1.0,0,1),glm::vec4(-1.0,+1.0,0,1)},
    {glm::vec4(-1.0,-1.0,0,1),glm::vec4(-1.0,+1.0,0,1),glm::vec4(+1.0,-1.0,0,1)},
  };
  std::vector<bool>frontIsCCW = {true,false};
  std::vector<bool>cullIsEnabled = {true,false};

  TestingTriangle testingTriangle;
  uint32_t counter = 0;
  for(auto const&t:triangles)
    for(auto const&fIsCCW:frontIsCCW)
      for(auto const&cull:cullIsEnabled){
        testingTriangle.triangle = t;
        testingTriangle.backfaceCulling.frontFaceIsCounterClockWise = fIsCCW;
        testingTriangle.backfaceCulling.enabled = cull;
        testingTriangle.test(counter++);
      }
}


