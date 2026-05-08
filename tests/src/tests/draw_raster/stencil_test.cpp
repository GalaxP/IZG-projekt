#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "stencil_test"
#include <tests/testCommon.hpp>

#include <tests/draw_raster/util_raster_part_test.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("All Stencil Tests and stencil operations invert on sfail");

  std::vector<StencilFunc>stencilFuncv = {
    StencilFunc::NEVER   ,
    StencilFunc::LESS    ,
    StencilFunc::LEQUAL  ,
    StencilFunc::GREATER ,
    StencilFunc::GEQUAL  ,
    StencilFunc::EQUAL   ,
    StencilFunc::NOTEQUAL,
    StencilFunc::ALWAYS  ,
  };

  std::vector<bool    >enabledv = {false,true};
  std::vector<bool    >hasv     = {false,true};
  std::vector<uint32_t>refv     = {0,1,2};
  std::vector<uint32_t>initv    = {0,1,2};

  uint32_t counter = 0;
  TestingTriangle testingTriangle;
  testingTriangle.stencilSettings.frontOps.sfail = StencilOp::INVERT;
  testingTriangle.stencilSettings.backOps.sfail = StencilOp::INVERT;

  for(auto const&init:initv)
    for(auto const&ref:refv)
      for(auto const&has:hasv)
        for(auto const&enabled:enabledv)
          for(auto const&stencilFunc:stencilFuncv)
          {
            testingTriangle.initStencil              = init;
            testingTriangle.hasStencilBuffer         = has;
            testingTriangle.stencilSettings.enabled  = enabled;
            testingTriangle.stencilSettings.func     = stencilFunc;
            testingTriangle.stencilSettings.refValue = ref;
            testingTriangle.test(counter++,"stencil test");
          }
}

