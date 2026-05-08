#include<iostream>

#include<catch2/catch_test_macros.hpp>

#define __FILENAME__ "user"

#include<tests/testCommon.hpp>

using namespace tests;

struct UserData{
  std::shared_ptr<MemCb>mcb;
};

void userFce(void*d){
  auto&uData = *(UserData*)d;
  uData.mcb->inFlightMem->activatedProgram = 3;
}

SCENARIO(TEST_NAME){
  printTestName("user");

  MEMCB(100,100);
  mem.framebuffers[0] = Framebuffer{};

  UserData uData;
  uData.mcb = memCb;
  pushUserCommand(cb,userFce,&uData);

  if(memCb->runAndTest())return;
  
  std::cerr << R".(
  TEST SELHAL!

  Tento test zkouší, zda funguje příkaz userCommand.
  User command je uživatelský příkaz - jedná se o callback a data.
  Pokud je specifikován callback, měl by se zavolat s dodanými daty.

  Test selhal.
  Uživatelský příkaz nastavuje aktivní program na 3.
  )." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}
