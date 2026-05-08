#include<iostream>

#include<catch2/catch_test_macros.hpp>

#define __FILENAME__ "setBackfaceCulling"

#include<tests/testCommon.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("setBackfaceCulling");

  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};

  pushSetBackfaceCullingCommand(cb,true);

  if(memCb->runAndTest())return;
  
  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda funguje příkaz setBackfaceCullingCommand.
  Cílem tohoto příkazu je nastavit povolit nebo zakázat zahazávání odvrácených stran trojúhelníků.
  Příkaz se podobá příkazu glEnable(GL_CULL_FACE) z OpenGL.
  https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml

  Test selhal.
  )." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}
