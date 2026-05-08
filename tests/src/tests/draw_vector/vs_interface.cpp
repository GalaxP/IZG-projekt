#include <iostream>

#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "vs_interface"
#include<tests/testCommon.hpp>

#include<framework/textureData.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("vertex shader, shader interface");

  MEMCB(10,10);
  mem.framebuffers[0] = Framebuffer{};

  auto&prg = mem.programs[9];
  prg.vertexShader = vertexEmpty;

  pushBindProgramCommand(cb,9);
  pushDrawCommand(cb,3);

  if(memCb->runAndTest())return;

  std::cerr << R".(
  TEST SELHAL

  Vertex Shader by měl obdržet správný ShaderInterface.
  Shader interface obsahuje uniformní proměnné a textury.
  Obě jsou uloženy v paměti grafické karty GPUMemory.
  ).";

  memCb->printDifferencies();

  REQUIRE(false);
}
