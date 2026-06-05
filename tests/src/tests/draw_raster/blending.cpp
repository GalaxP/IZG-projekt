#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "blending"
#include <tests/testCommon.hpp>
#include <tests/str.hpp>

#include <tests/draw_raster/util_raster_part_test.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("All blending operations");

  std::vector<BlendFunc>sFactorv = {
    BlendFunc::ZERO               ,
    BlendFunc::ONE                ,
    BlendFunc::SRC_COLOR          ,
    BlendFunc::ONE_MINUS_SRC_COLOR,
    //BlendFunc::DST_COLOR          ,
    //BlendFunc::ONE_MINUS_DST_COLOR,
    BlendFunc::SRC_ALPHA          ,
    BlendFunc::ONE_MINUS_SRC_ALPHA,
    //BlendFunc::DST_ALPHA          ,
    //BlendFunc::ONE_MINUS_DST_ALPHA,
  };

  auto dFactorv = sFactorv;

  std::vector<BlendEquation>blendEquationv = {
    BlendEquation::ADD             ,
    //BlendEquation::SUBTRACT        , //fix negativ values to be clamped
    //BlendEquation::REVERSE_SUBTRACT,
    BlendEquation::MIN             ,
    BlendEquation::MAX             ,
  };

  uint32_t counter = 0;
  TestingTriangle testingTriangle;

  testingTriangle.initColor = glm::vec4(0.5f,0.25f,0.125f,0.5f);
  testingTriangle.initDepth = 10e10f;
  testingTriangle.triangle.ac = glm::vec4(0.1,0.2,0.3,0.4);
  testingTriangle.triangle.bc = glm::vec4(0.7,0.8,0.1,0.9);
  testingTriangle.triangle.cc = glm::vec4(0.4,0.4,0.9,0.1);

  testingTriangle.resolution.x = 8;
  testingTriangle.resolution.y = 8;

  testingTriangle.hasColorBuffer = true;

  for(auto const&sFactor:sFactorv)
    for(auto const&dFactor:dFactorv)
      for(auto const&equation:blendEquationv)
      {
        auto&bs = testingTriangle.blendingSettings;
        bs.enabled  = true    ;
        bs.sFactor  = sFactor ;
        bs.dFactor  = dFactor ;
        bs.equation = equation;
        std::string testName;
        testName = "blending test";
        testName += "(";
        testName += "sFactor"  "=" + str(sFactor ) + ",";
        testName += "dFactor"  "=" + str(dFactor ) + ",";
        testName += "equation" "=" + str(equation) + ",";
        testName += ")";

        testingTriangle.test(counter++,testName);
      }
}

