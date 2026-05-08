#include<iostream>

#include<catch2/catch_test_macros.hpp>

#define __FILENAME__ "setFrontFace"

#include<tests/testCommon.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("setFrontFace");

  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};

  pushSetFrontFaceCommand(cb,false);

  if(memCb->runAndTest())return;
  
  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda funguje nastavení výběru, co je přivrácená strana trojúhelníku.
  Přivrácená strana trojúhelníku je specifikována buď vrcholy proti směru hodinových ručiček nebo naopak.
  Příkaz se podobá příkazu glFronFace(GL_CCW/GL_CW) z OpenGL.
  https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFrontFace.xhtml

  Test selhal.
  )." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}
