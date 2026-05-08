#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "stencil_writes_dpfail"
#include <tests/testCommon.hpp>

#include <tests/draw_raster/util_raster_part_test.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("All Stencil operations on dpfail");

  std::vector<StencilOp>stencilOpv = {
    StencilOp::KEEP     ,
    StencilOp::ZERO     ,
    StencilOp::REPLACE  ,
    StencilOp::INCR     ,
    StencilOp::INCR_WRAP,
    StencilOp::DECR     ,
    StencilOp::DECR_WRAP,
    StencilOp::INVERT   ,
  };
  std::vector<uint32_t>refv = {0,1,2};
  std::vector<uint32_t>initv = {0,1,2};
  std::vector<bool>blockv = {false,true};

  uint32_t counter = 0;
  TestingTriangle testingTriangle;
  testingTriangle.hasDepthBuffer = true;
  testingTriangle.initDepth = -.9f;
  testingTriangle.stencilSettings.func = StencilFunc::ALWAYS;
  testingTriangle.hasStencilBuffer = true;
  testingTriangle.stencilSettings.enabled = true;

  for(auto const&block:blockv)
    for(auto const&init:initv)
      for(auto const&ref:refv)
        for(auto const&stencilOp:stencilOpv)
        {
          testingTriangle.stencilSettings.frontOps.dpfail = stencilOp;
          testingTriangle.initStencil = init;
          testingTriangle.stencilSettings.refValue = ref;
          testingTriangle.blockWrites.stencil = block;
          testingTriangle.test(counter++,"stencil writes dpfail test");
        }
}
