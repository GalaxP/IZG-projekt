#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "color_writes"
#include <tests/testCommon.hpp>

#include <tests/draw_raster/util_raster_part_test.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("Color writes");

  std::vector<Triangle>tris = {
    Triangle{},
    Triangle{},
  };
  tris[1].ac = glm::vec4(1,0,0,.50);
  tris[1].bc = glm::vec4(0,1,0,.25);
  tris[1].cc = glm::vec4(0,0,1,.75);

  std::vector<bool>blockv = {false,true};
  std::vector<bool>hasv = {false,true};

  uint32_t counter = 0;
  TestingTriangle testingTriangle;
  testingTriangle.fragmentColors = true;

  for(auto const&t:tris)
    for(auto const&has:hasv)
      for(auto const&block:blockv)
      {
        testingTriangle.triangle = t;
        testingTriangle.blockWrites.depth = block;
        testingTriangle.hasColorBuffer = has;
        testingTriangle.test(counter++,"color writes test");
      }
}
