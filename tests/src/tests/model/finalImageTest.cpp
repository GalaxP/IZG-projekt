#include <catch2/catch_test_macros.hpp>

#include <libs/stb_image/stb_image_write.h>

#include <iostream>
#include <string.h>

#include <algorithm>
#include <numeric>

#include <solutionInterface/taskFunctions.hpp>

#include <tests/conformanceTests.hpp>
#define __FILENAME__ "finalImageTest"
#include <tests/testCommon.hpp>
#include <tests/renderMethodFrame.hpp>

#include <framework/switchSolution.hpp>

float       extern mseThreshold;


using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("image to image comparison");

  uint32_t width  = 500;
  uint32_t height = 500;
  
  switchToTeacherSolution();
  auto expFrame = createFramebuffer(width,height);
  renderMethodFrame(expFrame.frame);

  switchToStudentSolution();
  auto stuFrame = createFramebuffer(width,height);
  renderMethodFrame(stuFrame.frame);

  float meanSquareError = 0;
  for (uint32_t y = 0; y < height; ++y)
    for (uint32_t x = 0; x < width; ++x){
      auto ucol = getColorU(stuFrame.frame,x,y);
      auto gcol = getColorU(expFrame.frame,x,y);
      for(uint32_t c=0;c<3;++c){
        float diff = glm::abs((float)ucol[c] - (float)gcol[c]);
        diff *= diff;
        meanSquareError += diff;
      }
    }

  meanSquareError /= (float)(width * height * 3);
  float tol = mseThreshold;

  if(!breakTest()&&meanSquareError < tol)return;

  std::cerr << "  Finální obrázek se moc liší od reference!" << std::endl;
  std::cerr << "  MSE je: "<<meanSquareError<< std::endl;
  std::cerr << "  Akceptovatelná chyba je: "<<tol<<std::endl;
  REQUIRE(false);
}
