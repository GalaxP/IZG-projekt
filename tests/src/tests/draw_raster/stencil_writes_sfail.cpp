#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "stencil_writes_sfail"
#include <tests/testCommon.hpp>

#include <tests/draw_raster/util_raster_part_test.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("All Stencil operations on sfail");

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
  testingTriangle.stencilSettings.func = StencilFunc::NEVER;
  testingTriangle.hasStencilBuffer = true;
  testingTriangle.stencilSettings.enabled = true;

  for(auto const&block:blockv)
    for(auto const&init:initv)
      for(auto const&ref:refv)
        for(auto const&stencilOp:stencilOpv)
        {
          testingTriangle.stencilSettings.frontOps.sfail = stencilOp;
          testingTriangle.initStencil = init;
          testingTriangle.stencilSettings.refValue = ref;
          testingTriangle.blockWrites.stencil = block;
          testingTriangle.test(counter++,"stencil writes sfail test");
        }
}
