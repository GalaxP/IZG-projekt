#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "depth_writes"
#include <tests/testCommon.hpp>

#include <tests/draw_raster/util_raster_part_test.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("Depth writes");

  std::vector<bool>blockv = {false,true};
  std::vector<bool>hasv = {false,true};

  uint32_t counter = 0;
  TestingTriangle testingTriangle;

  for(auto const&has:hasv)
    for(auto const&block:blockv)
    {
      testingTriangle.blockWrites.depth = block;
      testingTriangle.hasDepthBuffer = has;
      testingTriangle.test(counter++,"depth writes test");
    }
}
