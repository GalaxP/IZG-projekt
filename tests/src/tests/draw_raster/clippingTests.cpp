#include <iostream>
#include <string.h>
#include <algorithm>
#include <numeric>

#include <catch2/catch_test_macros.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <solutionInterface/taskFunctions.hpp>
#include <framework/method.hpp>

#define __FILENAME__ "clippingTests"
#include <tests/testCommon.hpp>
#include <tests/str.hpp>

#include <tests/draw_raster/util_raster_part_test.hpp>

using namespace tests;


SCENARIO(TEST_NAME){
  printTestName("clipping - CW triangle behind near plane");

  TestingTriangle testingTriangle;
  testingTriangle.triangle = Triangle{
    glm::vec4(-1.f,-1.f,-1.1f,+1.f),
    glm::vec4(+1.f,-1.f,-1.1f,+1.f),
    glm::vec4(-1.f,+1.f,-1.1f,+1.f),
  };
  testingTriangle.test();
}


SCENARIO(TEST_NAME){
  printTestName("clipping - CCW triangle behind near plane");

  TestingTriangle testingTriangle;
  testingTriangle.triangle = Triangle{
    glm::vec4(-1.f,-1.f,-7.f,1.f),
    glm::vec4(-1.f,+1.f,-7.f,1.f),
    glm::vec4(+1.f,-1.f,-7.f,1.f),
  };
  testingTriangle.test();

}


SCENARIO(TEST_NAME){
  printTestName("clipping - 1 vertex behind near plane");

  float nnear = 1.f;
  float ffar  = 2.f;
  float e = - (ffar + nnear) / (ffar - nnear);
  float f = - 2.f * ffar * nnear / (ffar - nnear);
  float z = 2.f/3.f;

  TestingTriangle testingTriangle;
  testingTriangle.triangle = Triangle{
    glm::vec4(-2.f,-2.f,+2.f,+2.f),
    glm::vec4(+2.f,-2.f,+2.f,+2.f),
    glm::vec4(-z  ,+z  ,-z*e+f,+z),
  };
  testingTriangle.test();
}

SCENARIO(TEST_NAME){
  printTestName("clipping - 2 vertices behind near plane");

  float nnear = 1.f;
  float ffar  = 2.f;
  float e = - (ffar + nnear) / (ffar - nnear);
  float f = - 2.f * ffar * nnear / (ffar - nnear);
  float z = 2.f/3.f;

  TestingTriangle testingTriangle;
  testingTriangle.triangle = Triangle{
    glm::vec4(-z,-z,-z*e+f,+z),
    glm::vec4(+z,-z,-z*e+f,+z),
    glm::vec4(-2,+2,+2,+2),
  };
  testingTriangle.test();
}

