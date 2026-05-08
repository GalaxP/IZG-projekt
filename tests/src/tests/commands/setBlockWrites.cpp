#include<iostream>

#include<catch2/catch_test_macros.hpp>

#define __FILENAME__ "blockWritesTests"

#include<tests/testCommon.hpp>

using namespace tests;

void blockWrites(){
  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};

  pushBlockWritesCommand(cb,true);

  if(memCb->runAndTest())return;
  
  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda funguje příkaz BlockWritesCommand.
  Cílem tohoto příkazu je nastavit příznaky, které blokují zápis do framebufferu.
  Příkaz se podobá příkazům glColorMask, glDepthMask a glStencilMask z OpenGL.
  https://registry.khronos.org/OpenGL-Refpages/gl4/html/glColorMask.xhtml
  https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDepthMask.xhtml
  https://registry.khronos.org/OpenGL-Refpages/gl4/html/glStencilMask.xhtml

  Test selhal.
  V paměti nejsou po vykonání příkazu správně nastaveny příznaky blokování zápisu.)." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

void multipleBlockWrites(){
  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};
  pushBlockWritesCommand(cb,true);
  pushBlockWritesCommand(cb,false,true);
  pushBlockWritesCommand(cb,false,false,true);
  pushBlockWritesCommand(cb);

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
  printTestName("blockWrites");
  blockWrites();
  multipleBlockWrites();
}
