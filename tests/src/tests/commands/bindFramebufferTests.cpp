#include<iostream>

#include<catch2/catch_test_macros.hpp>

#define __FILENAME__ "bindFramebufferTests"

#include<tests/testCommon.hpp>

using namespace tests;

void bindFramebufferTest(){
  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};
  pushBindFramebufferCommand(cb,3);

  if(memCb->runAndTest(1,true))return;
  
  std::cerr << R".(
  TEST SELHAL!

  Vašim úkolem v tomto projektu je naprogramovat funkcionalitu grafické karty.
  Funkcionalita grafické karty je reprezentována funkcí:
  student_GPU_run(GPUMemory const& mem,CommnadBuffer const& cmdBuffer)
  v souboru studentSolution/src/studentSolution/gpu.cpp.
  
  Mezi funkcionality grafické karty patří kreslení, čistění obrazovky a další.

  S realnými gpu se komunikuje skrze fronty přikazů, po kterých se posílají úlohy.
  Tyto úlohy bývají zabalené do balíčků práce (tzn. command buffer).
  Podobně je to například v API Vulkan:
  https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkCommandBuffer.html

  V tomto projektu je to simulováno obdobně - funkce student_GPU_run obdrží paměť grafické karty a command buffer.
  Úkolem funkce student_GPU_run je odsimulovat balíček práce nad pamětí grafické karty.

  Paměť grafické karty se skládá z programů, bufferů (lineární paměti), textur, framebufferů, vertex array objektů a dalších částí.
  
  Tento test zkouší, zda funguje příkaz bindFramebuffer.
  Cílem příkazu bindFramebuffer je aktivovat správný framebuffer.
  Příkaz se podobá příkazu glBindFramebuffer z OpenGL.
  https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindFramebuffer.xhtml

  Test selhal.
  V paměti není po vykonání příkazu správně nastaven activatedFramebuffer.)." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

void multipleBindFramebufferTest(){
  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};
  pushBindFramebufferCommand(cb,3);
  pushBindFramebufferCommand(cb,8);
  pushBindFramebufferCommand(cb,4);

  if(memCb->runAndTest())return;
  
  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda funguje příkaz bindFramebuffer pokud se zavolá vícekrát.

  Test selhal.
  V paměti není po vykonání příkazu správně nastaven activatedFramebuffer.)." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

SCENARIO(TEST_NAME){
  printFirstTestName("bind framebuffer");
  bindFramebufferTest();
  multipleBindFramebufferTest();
}
