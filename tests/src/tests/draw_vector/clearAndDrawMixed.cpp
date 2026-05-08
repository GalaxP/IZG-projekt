#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "clearAndDrawMixed"

#include <tests/testCommon.hpp>
#include <tests/str.hpp>

#include <iostream>
#include <string.h>
#include <sstream>
#include<functional>
#include<map>

#include <algorithm>
#include <numeric>
#include <iostream>

#include <solutionInterface/taskFunctions.hpp>

using namespace tests;



SCENARIO(TEST_NAME){
  printTestName("mixed clear and draw commands");

  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};

  mem.programs[0].vertexShader = vertexEmpty;

  pushClearColorCommand(cb);
  pushClearDepthCommand(cb);
  pushDrawCommand(cb,3);
  pushClearColorCommand(cb);
  pushClearDepthCommand(cb);
  pushDrawCommand(cb,6);
  pushDrawCommand(cb,9);
  pushClearColorCommand(cb);
  pushClearDepthCommand(cb);
  pushDrawCommand(cb,12);

  if(memCb->runAndTest())return;

  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší command buffer s několik promíchanými čistícímí a kreslícími příkazy.)." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}


