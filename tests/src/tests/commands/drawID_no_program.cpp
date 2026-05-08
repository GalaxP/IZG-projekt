#include<iostream>

#include<catch2/catch_test_macros.hpp>

#define __FILENAME__ "drawID_no_program"

#include<tests/testCommon.hpp>

using namespace tests;

void oneEqueue(){

  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};

  pushDrawCommand(cb,3);
  pushDrawCommand(cb,3);

  if(memCb->runAndTest())return;

  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda vám grafická karta počítá vykreslovací příkazy v proměnné gl_DrawID.
  Všechny kreslící příkazy v by měly být číslovány pořadovým číslem.
  Číslují se pouze kreslící příkazy.
  Pokud jsou v command bufferu například 4 kreslící příkazy proloženy
  jinými příkazy, budou mít tyto kreslící příkazy číslo 0,1,2,3,...

  Podle gl_DrawID se obvykle vybírá nastavení pro danou část modelu (například textury, materiály, matice).
  )." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

void multipleEqueue(){

  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};

  pushDrawCommand(cb,3);
  pushDrawCommand(cb,3);
  pushDrawCommand(cb,3);

  if(memCb->runAndTest(2))return;

  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda kreslící příkazy mají správné gl_DrawID.
  Příkaz student_GPU_run je v tomto testu zavolán několikrát.
  Při každém zavolání by se mělo gl_DrawID nastavit zpět na 0.
  )." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

void drawAndSetDrawId(){

  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};

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
  printTestName("drawID_no_program");
  oneEqueue();
  multipleEqueue();
  drawAndSetDrawId();
}
