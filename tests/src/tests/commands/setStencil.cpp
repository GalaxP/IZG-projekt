#include<iostream>

#include<catch2/catch_test_macros.hpp>

#define __FILENAME__ "setStencil"

#include<tests/testCommon.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("setStencil");

  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};

  StencilSettings s;
  s.enabled         = true;
  s.refValue        = 34;
  s.func            = StencilFunc::LEQUAL;
  s.backOps.sfail   = StencilOp::REPLACE;
  s.backOps.dpfail  = StencilOp::DECR_WRAP;
  s.backOps.dppass  = StencilOp::INVERT;
  s.frontOps.sfail  = StencilOp::ZERO;
  s.frontOps.dpfail = StencilOp::DECR;
  s.frontOps.dppass = StencilOp::INCR;

  pushSetStencilCommand(cb,s);

  if(memCb->runAndTest())return;
  
  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda funguje příkaz setStencilCommand.
  Tento příkaz je kombinace několik OpenGL příkazů pro nastavení stencilového testu.
  glEnable(GL_STENCIL_TEST);
  glStencilOpSeparate();
  glStencilFunction();

  https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml
  https://registry.khronos.org/OpenGL-Refpages/gl4/html/glStencilOpSeparate.xhtml
  https://registry.khronos.org/OpenGL-Refpages/gl4/html/glStencilFunc.xhtml
  
  Stencil test je test spouštěný na úrovní fragmentů.
  Je k němu využit stencilový buffer (součást framebufferu).
  Test může být zapnut nebo vypnut.
  Se stencilovým testem se vážou stencilové operace.
  Operace se stencil bufferem jsou řízeny rasterizací.
  Operace se stencilovým testem mohou nastat ve třech různých případech.
  Fragment po rasterizaci prochází per-fragment operacemi (PFO).
  Mezi ně patří depth test, stencil test a blending.

  Stencilové operace mohou nastat ve třech případech:
  * pokud neprojde depth test
  * pokud projde depth test
  * pokud neprojde stencil test
  Je možné specifikovat odlišnou operaci pro přivrácené a odvrácené fragmenty.

  Test selhal.
  )." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}
