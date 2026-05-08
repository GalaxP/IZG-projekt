#include<iostream>

#include<catch2/catch_test_macros.hpp>

#define __FILENAME__ "bindVertexArrayTests"

#include<tests/testCommon.hpp>

using namespace tests;

void bindVertexArrayTest(){
  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};
  pushBindVertexArrayCommand(cb,3);

  if(memCb->runAndTest())return;
  
  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda funguje příkaz bindVertexArray.
  Cílem příkazu bindVertexArray je aktivovat správný vertex array.
  Příkaz se podobá příkazu glBindVertexArray z OpenGL.
  https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindVertexArray.xhtml

  Test selhal.
  V paměti není po vykonání příkazu správně nastaven activatedVertexArray.
  )." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

void multipleBindVertexArrayTest(){
  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};
  uint32_t correct = 10;
  pushBindVertexArrayCommand(cb,3);
  pushBindVertexArrayCommand(cb,2);
  pushBindVertexArrayCommand(cb,correct);

  if(memCb->runAndTest())return;
  
  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda funguje příkaz bindVertexArray.
  Příkaz se vložil do command bufferu vícekrát.

  Test selhal.
  V paměti není po vykonání příkazu správně nastaven activatedVertexArray.
  )." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

SCENARIO(TEST_NAME){
  printTestName("bind vertex array");
  bindVertexArrayTest();
  multipleBindVertexArrayTest();
}

