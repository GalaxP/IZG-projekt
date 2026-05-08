#include<iostream>

#include<catch2/catch_test_macros.hpp>

#define __FILENAME__ "clear"

#include<tests/testCommon.hpp>
#include<tests/str.hpp>

using namespace tests;

void clearColorTest(){
  MEMCB(20,20);

  pushClearColorCommand(cb,glm::vec4(1,1,0,1));

  if(memCb->runAndTest())return;

  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda váš kód čistí barevný buffere framebufferu v paměti grafické karty.

  Paměť grafické karty je ve struktuře GPUMemory.

  Příkaz pro čištění framebufferu uvedený v souboru solutionInterface/src/solutionInterface/gpu.hpp vypadá takto:
  ).";
  
  print(R".(
  struct ClearCommand{
    glm::vec4 color = glm::vec4(0);
  };
  ).",Color::GRAY);

  std::cerr << R".(
  V tomto testu vypadá command buffer takto:
  )." << std::endl;

  print(str(cb,2),Color::GRAY);

  std::cerr << R".(
  Jak můžete vidět, obsahuje jen jeden příkaz - vyčištění barevného bufferu obrazovky.

  Můžete vidět barvu, na kterou se framebuffer čistí (r,g,b,a).
  Framebuffer se skládá ze tří částí: color buffer, depth buffer a stencil buffer - paměť barvy a hloubky a stencil.

  Barvy jsou v ClearCommand uvedeny ve floatech v rozsahu: [0-1].
  Při zápisu do framebufferu je nutné je převést na uint8_t rozsahu: [0,255];
  )." << std::endl;

  std::cerr << R".(

  Test selhal, protože hodnoty ve framebufferu jsou po provedení příkazu nesprávné.)." << std::endl;

  memCb->printDifferencies();

  std::cerr <<R".(

  Měli byste správně vyčistit aktivní framebuffer.
  Upravte funkci student_GPU_run v souboru studentSolution/src/studentSolution/gpu.cpp)." << std::endl;

  memCb->printHelp(R".(
  void student_GPU_run(GPUMemory&mem,CommandBuffer const&cb){
    for(uint32_t i=0;i<cb.nofCommands;++i){
      if(cb.commands[i].type == CommandType::CLEAR_COLOR){
        clearColor(mem,cb.commands[i].commandData.clearCommand);
      }
    }
  }
  ).");

  REQUIRE(false);
}

void clearDepthTest(){
  MEMCB(20,20);

  pushClearDepthCommand(cb,10e9f);

  if(memCb->runAndTest())return;
  
  std::cerr << R".(
  TEST SELHAL!

  V tomto testu se ve framebufferu čistí hloubkovy buffer.

  Test selhal, protože hodnoty ve hloubkovém bufferu jsou špatně.)." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

void clearStencilTest(){
  MEMCB(20,20);

  pushClearStencilCommand(cb,17);

  if(memCb->runAndTest())return;
  
  std::cerr << R".(
  TEST SELHAL!

  V tomto testu se ve framebufferu čistí stencil buffer.

  Test selhal, protože hodnoty ve stencil bufferu jsou špatně.)." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

void multipleClearTest(){
  MEMCB(20,20);

  pushClearColorCommand(cb,glm::vec4(1,1,1,1));
  pushClearColorCommand(cb,glm::vec4(0,1,1,1));
  pushClearDepthCommand(cb,2.f);
  pushClearStencilCommand(cb,3);
  pushClearColorCommand(cb,glm::vec4(0,0,0,1));
  pushClearColorCommand(cb,glm::vec4(1,0,1,1));
  pushClearDepthCommand(cb,10e7f);
  pushClearStencilCommand(cb,12);

  if(memCb->runAndTest())return;

  std::cerr << R".(
  TEST SELHAL!

  V tomto testu obsahuje command buffer vícero čistítích příkazů.

  Test selhal, protože hodnoty ve framebufferu jsou špatně.)." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
  
}

SCENARIO(TEST_NAME){
  printTestName("clear commands");
  clearColorTest();
  clearDepthTest();
  clearStencilTest();
  multipleClearTest();
}



void clearColorEmptyColor(){
  MEMCB(20,20);

  mem.framebuffers[0].color = Image{};

  pushClearColorCommand(cb,glm::vec4(1,0,1,1));

  if(memCb->runAndTest())return;

  std::cerr << R".(
  TEST SELHAL!

  V tomto testu obsahuje command buffer čištění barvy.
  Framebuffer však neobsahuje barevný buffer.

  Test selhal, protože se nemělo nic stát.)." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

void clearDepthEmptyDepth(){
  MEMCB(20,20);

  mem.framebuffers[0].color = Image{};

  pushClearDepthCommand(cb,10e7f);

  if(memCb->runAndTest())return;

  std::cerr << R".(
  TEST SELHAL!

  V tomto testu obsahuje command buffer čištění hloubky.
  Framebuffer však neobsuje hloubkový buffer.

  Test selhal, protože se nemělo nic stát.)." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

void clearStencilEmptyStencil(){
  MEMCB(20,20);

  mem.framebuffers[0].color = Image{};

  pushClearStencilCommand(cb,33);

  if(memCb->runAndTest())return;

  std::cerr << R".(
  TEST SELHAL!

  V tomto testu obsahuje command buffer čištění stencilu.
  Framebuffer však neobsuje stencilový buffer.

  Test selhal, protože se nemělo nic stát.)." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}

SCENARIO(TEST_NAME){
  printTestName("clear commands, partial framebuffer");
  clearColorEmptyColor();
  clearDepthEmptyDepth();
  clearStencilEmptyStencil();
}


