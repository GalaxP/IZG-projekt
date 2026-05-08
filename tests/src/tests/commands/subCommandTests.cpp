#include<iostream>

#include<catch2/catch_test_macros.hpp>

#define __FILENAME__ "subCommandTests"

#include<tests/testCommon.hpp>

using namespace tests;

void subCommandTest(){
  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};

  pushBindFramebufferCommand(cb,7);
  CommandBuffer sub;
  pushBindVertexArrayCommand(sub,50);
  pushSubCommand(cb,&sub);
  pushDrawCommand(cb,3);
    
  if(memCb->runAndTest())return;
  
  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda funguje příkaz subCommand.
  SubCommand obsahuje ukazatel na další CommandBuffer.
  Tím je umožněno větvit command buffery a command buffery znovu využít.
  Fungování je podobné jako u sekundárních command bufferů ve Vulkanu:
  https://docs.vulkan.org/spec/latest/chapters/cmdbuffers.html#commandbuffers-secondary

  Test selhal, v paměti GPUMemory nejsou správně provedny změny.)." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

void subCommandDeepTest(){
  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};

  pushBindFramebufferCommand(cb,7);
  struct Subs{
    CommandBuffer sub[4];
  };
  auto subs = std::make_shared<Subs>();
  //CommandBuffer sub[4];
  pushSubCommand(cb          ,subs->sub+0);
  pushSubCommand(subs->sub[0],subs->sub+1);
  pushSubCommand(subs->sub[1],subs->sub+2);
  pushSubCommand(subs->sub[2],subs->sub+3);
  pushBindVertexArrayCommand(subs->sub[3],50);
  pushDrawCommand(cb,3);
    
  if(memCb->runAndTest())return;
  
  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda funguje příkaz subCommand.
  V tomto případě je sub command několikrát zanořen.

  Test selhal, v paměti GPUMemory nejsou správně provedny změny.)." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

SCENARIO(TEST_NAME){
  printTestName("sub command tests");
  subCommandTest();
  subCommandDeepTest();
}
