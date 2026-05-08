#pragma once

#include <iostream>

#include <solutionInterface/gpu.hpp>

namespace tests{

struct Triangle{
  Triangle(){}
  Triangle(glm::vec4 const&aa,glm::vec4 const&bb,glm::vec4 const&cc):a(aa),b(bb),c(cc){}
  glm::vec4 a  = glm::vec4(-1.f,-1.f,0.f,1.f);
  glm::vec4 b  = glm::vec4(+1.f,-1.f,0.f,1.f);
  glm::vec4 c  = glm::vec4(-1.f,+1.f,0.f,1.f);
  glm::vec4 ac = glm::vec4(1.f,0.f,0.f,1.f);
  glm::vec4 bc = glm::vec4(0.f,1.f,0.f,1.f);
  glm::vec4 cc = glm::vec4(0.f,0.f,1.f,1.f);
};

struct TestingTriangle{
  TestingTriangle(){}
  Triangle         triangle                            ;
  glm::uvec2       resolution       = glm::uvec2(10,10);
  bool             hasStencilBuffer = false            ;
  bool             hasDepthBuffer   = false            ;
  bool             hasColorBuffer   = false            ;
  bool             flip             = false            ;
  uint8_t          initStencil      = 0                ;
  float            initDepth        = +10e10f          ;
  glm::vec4        initColor        = glm::vec4(0)     ;
  BackfaceCulling  backfaceCulling                     ;
  StencilSettings  stencilSettings                     ;
  BlockWrites      blockWrites                         ;
  BlendingSettings blendingSettings                    ;
  bool             fragmentDiscard  = false            ;
  bool             fragmentColors   = false            ;
  void test(uint32_t counter = 0,std::string const&name="raster test");
};

}
