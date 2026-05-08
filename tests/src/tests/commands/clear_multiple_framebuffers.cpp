#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <string.h>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <cstring>

#include <glm/gtc/matrix_transform.hpp>

#include <solutionInterface/taskFunctions.hpp>

#define __FILENAME__ "clear_multiple_framebuffers"

#include <tests/testCommon.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("clear multiple framebuffers");

  MEMCB(100,100);

  auto secondFramebuffer = createFramebuffer(50,50);

  mem.framebuffers[1] = secondFramebuffer.frame;

  pushBindFramebufferCommand(cb,1);
  pushClearColorCommand(cb,glm::vec4(0.f,0.f,1.f,1.f));
  pushBindFramebufferCommand(cb,0);
  pushClearColorCommand(cb,glm::vec4(1.f,0.f,0.f,1.f));

  if(memCb->runAndTest())return;

  std::cerr << R".(
  Tento test zkouší, zda je možné využívat vícero framebufferů.
  Struktura GPUMemory obsahuje identifikátor aktivního framebufferu - activatedFramebuffer.
  Pomocí něj je možné zvolit framebuffer, který se využívá.

  Příkaz BindFramebufferCommand slouží pro nastavení daného indentifikátoru.

  Příkazy, který framebuffer využívájí (kreslení, čistění) se provádí nad
  vybraným framebufferem.

  Tento test selhal, protože to nefuguje.
  Tento test využívá dva framebuffery.
  Test slehal, protože jsou ve framebufferech špatné hodnoty.)." << std::endl;

  memCb->printDifferencies();

  REQUIRE(false);
}
