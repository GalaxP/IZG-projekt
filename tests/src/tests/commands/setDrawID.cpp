#include<iostream>

#include<catch2/catch_test_macros.hpp>

#define __FILENAME__ "setDrawId"

#include<tests/testCommon.hpp>

using namespace tests;

void setDrawId(){

  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};

  pushSetDrawIdCommand(cb,30);

  if(memCb->runAndTest())return;

  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda funguje příkaz setDrawIdCommand.
  Tento příkaz nastavuje proměnnout gl_DrawID.
  gl_DrawID je počítadlo kreslících příkazů, které se resetuje
  při každém vyvolání student_GPU_run.
  gl_DrawID se využívá pro při kreslení pro zjistění, který
  vykreslující příkaz je právě pouštěn.
  Vybírají se jím uniformní proměnné, textury, materialy, matice.
  .)." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

SCENARIO(TEST_NAME){
  printTestName("setDrawId");
  setDrawId();
}
