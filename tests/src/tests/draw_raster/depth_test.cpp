#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "depth_test"
#include <tests/testCommon.hpp>

#include <tests/draw_raster/util_raster_part_test.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("All Depth Tests");

  std::vector<bool>hasv = {false,true};
  std::vector<float>initv = {.9f,-.9f};

  uint32_t counter = 0;
  TestingTriangle testingTriangle;

  for(auto const&has:hasv)
    for(auto const&init:initv)
    {
      testingTriangle.initDepth = init;
      testingTriangle.hasDepthBuffer = has;
      testingTriangle.test(counter++,"depth test");
    }
}

