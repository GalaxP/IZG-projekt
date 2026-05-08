#include <iostream>

#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "rasterization"
#include <tests/testCommon.hpp>

#include <tests/draw_raster/util_raster_part_test.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("Raterization: basic rasterization to check primitive assembly unit and viewport transformation");
  uint32_t counter=0;
  TestingTriangle t;
  t.test(counter++);
}

SCENARIO(TEST_NAME){
  printTestName("Rasterization: out of window tests");
  std::vector<Triangle>triangles = {
    {glm::vec4(-10.0,-10.0,0,1),glm::vec4(-9.0,-10.0,0,1),glm::vec4(-10.0,-9.0,0,1)},
    {glm::vec4(+9.0,+9.0,0,1),glm::vec4(+10.0,+9.0,0,1),glm::vec4(+9.0,+10.0,0,1)},
    {glm::vec4(-1.0,-1.0,0,1),glm::vec4(+2.0,-1.0,0,1),glm::vec4(-1.0,+1.0,0,1)},
    {glm::vec4(-1.0,-1.0,0,1),glm::vec4(+1.0,-1.0,0,1),glm::vec4(-1.0,+2.0,0,1)},
    {glm::vec4(-2,-1,0,1),glm::vec4(+1,-1,0,1),glm::vec4(-1,+1,0,1)},
    {glm::vec4(-1,-2,0,1),glm::vec4(+1,-1,0,1),glm::vec4(-1,+1,0,1)},
  };
  uint32_t counter = 0;
  TestingTriangle testingTriangle;
  for(auto const&t:triangles){
    testingTriangle.triangle = t;
    testingTriangle.test(counter++);
  }
}


SCENARIO(TEST_NAME){
  printTestName("rasterization should produce correct number of fragments");

  std::vector<glm::uvec2>resolutions = {
    glm::uvec2(10,10),
    glm::uvec2(19,13),
    glm::uvec2(13,19),
  };

  std::vector<Triangle>triangles = {
    {glm::vec4(-1.0,-1.0,0,1),glm::vec4(+1.0,-1.0,0,1),glm::vec4(-1.0,+1.0,0,1)},
    {glm::vec4(-1.0,-1.0,0,1),glm::vec4(+2.0,-1.0,0,1),glm::vec4(-1.0,+1.0,0,1)},
    {glm::vec4(-1.0,-2.0,0,1),glm::vec4(+1.0,-2.0,0,1),glm::vec4(-1.0,+1.0,0,1)},
    {glm::vec4(-0.5,-0.6,0,1),glm::vec4(+0.3,-0.1,0,1),glm::vec4(-0.6,+0.9,0,1)},
    {glm::vec4(-0.0,-0.0,0,1),glm::vec4(+1.0,-0.0,0,1),glm::vec4(-0.0,+1.0,0,1)},
    {glm::vec4(-2.0,-1.0,0,1),glm::vec4(+1.0,-1.0,0,1),glm::vec4(-2.0,+1.0,0,1)},
    {glm::vec4(-0.4,-1.3,0,1),glm::vec4(+1.2,+0.2,0,1),glm::vec4(-0.9,+1.4,0,1)},
    {glm::vec4(-2.0,-2.0,0,1),glm::vec4(+8.0,-2.0,0,1),glm::vec4(-2.0,+8.0,0,1)},

    // degenerated triangles
    {glm::vec4(+0.0,+0.0,0,1),glm::vec4(+0.0,+0.0,0,1),glm::vec4(+0.0,+0.0,0,1)},
    {glm::vec4(+0.0,+0.0,0,1),glm::vec4(+0.0,+0.0,0,1),glm::vec4(+1.0,+1.0,0,1)},
    {glm::vec4(+0.0,+0.0,0,1),glm::vec4(+0.0,+0.0,0,1),glm::vec4(-1.0,+1.0,0,1)},
    {glm::vec4(+0.0,+0.0,0,1),glm::vec4(+0.0,+0.0,0,1),glm::vec4(-1.0,-1.0,0,1)},
    {glm::vec4(+0.0,+0.0,0,1),glm::vec4(+0.0,+0.0,0,1),glm::vec4(+1.0,-1.0,0,1)},

  };

  auto culls = std::vector<bool>{false,true };
  auto flips = std::vector<bool>{false,true };
  auto ccws  = std::vector<bool>{true ,false};

  uint32_t counter = 0;
  TestingTriangle testingTriangle;
  for(auto const&r:resolutions){
    for(auto const&t:triangles)
      for(auto const&flip:flips)
        for(auto const&cull:culls)
          for(auto const&ccw:ccws){
            testingTriangle.resolution = r   ;
            testingTriangle.triangle   = t   ;
            testingTriangle.flip       = flip;
            testingTriangle.backfaceCulling.enabled = cull;
            testingTriangle.backfaceCulling.frontFaceIsCounterClockWise = ccw;
            testingTriangle.test(counter++);
          }
      }
}

