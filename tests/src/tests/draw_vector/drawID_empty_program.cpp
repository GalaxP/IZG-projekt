#include<iostream>

#include<catch2/catch_test_macros.hpp>

#define __FILENAME__ "drawID_empty_program"

#include<tests/testCommon.hpp>

using namespace tests;

namespace drawID_empty_program{

void oneEqueue(){

  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};

  mem.programs[0].vertexShader = vertexEmpty;

  pushDrawCommand(cb,3);
  pushDrawCommand(cb,3);

  if(memCb->runAndTest())return;

  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda nastavujete gl_DrawID ve struktuře ShaderInterface, která vstupuje do vertex shaderu.
  )." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

void multipleEqueue(){

  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};

  mem.programs[0].vertexShader = vertexEmpty;

  pushBindProgramCommand(cb,0);
  pushBindProgramCommand(cb,1);
  pushBindProgramCommand(cb,0);

  if(memCb->runAndTest(2))return;

  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda kreslící příkazy mají správné gl_DrawID.
  Příkaz student_GPU_run je v tomto testu zavolán několikrát.
  )." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

void drawAndSetDrawId(){

  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};

  mem.programs[0].vertexShader = vertexEmpty;

  pushDrawCommand(cb,3);
  pushDrawCommand(cb,3);
  pushSetDrawIdCommand(cb,30);
  pushDrawCommand(cb,3);
  pushDrawCommand(cb,3);
  pushDrawCommand(cb,3);

  if(memCb->runAndTest())return;

  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda mají kreslící příkazy správné gl_DrawID.
  Využívá se příkaz SetDrawIdCommand, který umožňuje nastavit gl_DrawID v průběhu.
  )." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

SCENARIO(TEST_NAME){
  printTestName("drawID_empty_program");
  oneEqueue();
  multipleEqueue();
  drawAndSetDrawId();
}

}
