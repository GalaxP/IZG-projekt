#include <iostream>

#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "gl_VertexID_indexing"
#include<tests/testCommon.hpp>
#include<tests/str.hpp>

using namespace tests;

template<typename T = uint32_t>
void indexTest(){
  auto bits = sizeof(T)*8;
  std::cerr << "  ("<<bits<<"bit indexing)" << std::endl;

  std::vector<T> indices = {0,1,2,2,1,3};

  MEMCB(10,10);
  mem.framebuffers[0] = Framebuffer{};

  auto&prg = mem.programs[4];
  prg.vertexShader   =   vertexEmpty;
  mem.buffers[0] = vectorToBuffer(indices);

  auto&vao = mem.vertexArrays[7];
  vao.indexBufferID  = 0;
  vao.indexOffset    = 0;
  vao.indexType      = (IndexType)sizeof(T);

  pushBindProgramCommand(cb,4);
  pushBindVertexArrayCommand(cb,7);
  pushDrawCommand(cb,(uint32_t)indices.size());

  if(memCb->runAndTest())return;

  std::cerr << R".(
  TEST SELHAL!
  ).";

  memCb->printDifferencies();

  std::cerr << R".(

  Test kreslí )."<<indices.size()<<R".( vrcholů.
  Tyto vrcholy by měly mít správné pořadové číslo (gl_VertexID), které je rovno hodnotě z indexačního bufferu.

  Vrcholy chodí do vertex shaderu číslované.
  Číslo vrcholu je uloženo v položce gl_VertexID ve struktuře InVertex.
  Když je zapnuté indexování (v tomto testu je), tak je číslo vrcholu rovno hodnotě v indexačním bufferu.
  V tomto testu se předpokládá )."<<bits<<R".( bitový index.
  Indexování je uloženo v aktivním Vertex Array objektu.
  vao = mem.vertexArrays[mem.activatedVertexArray] - aktivovaný vertex array.
  Číslo bufferu s indexy je v proměnné vao.indexBufferID.
  Typ indexu (8b,16b,32b) je v proměnné vao.indexType.
  Offset indexů je v proměnné vao.indexOffset.

  Nezapomeňte správně adresovat indexační buffer.
  Jedná se o tzn. "pointerou aritmetiku".

  Musíte si uvědomit, že vao.indexBuffer ukazuje do tabulky všech bufferů v GPUMemory.

  Přístup k bufferu s indexy:
  mem.buffers[vao.indexBufferID].data

  Indexační buffer obsahuje indexy, které mají všechny stejnou velikost, která může být 8b, 16b nebo 32 bitů.


  Položka z indexačního bufferu je vybrána číslem invokace vertex shaderu.

  Na buffer je možné nahlížet jako na pole:
  uint)."<<bits<<R".(_t*pole;

  číslo vrcholu je potom: gl_VertexID = pole[číslo invokace vertex shaderu].

  Indexing je vypnutý, pokud je vao.indexBufferID < 0.)." << std::endl;

  memCb->printHelp(R".(
  uint32_t computeVertexID(GPUMemory&mem,VertexArray const&vao,uint32_t shaderInvocation){
    if(vao.indexBufferID<0)return shaderInvocation;

    void*indexBuffer = (uint8_t*)mem.buffers[vao.indexBufferID];

    if(vao.indexType == IndexType::U)."+str(bits)+R".(){
      uint)."+str(bits)+R".(_t*ind = (uint)."+str(bits)+R".(_t*)(vao.indexBuffer+vao.indexOffset);
      return ind[shaderInvocation];
    }
    ...
  }

  void draw(GPUMemory&mem,DrawCommand const&cmd){

    for(...){//smyčka přes vrcholy
      ...
      InVertex  inVertex; // vrchol, co vstupuje do vertex shader
      VertexArray = mem.vertexArrays[mem.activatedVertexArray];
      inVertex.gl_VertexID = computeVertexID(mem,vao,...);
      ...
    }

  }
  ).");

  REQUIRE(false);
}

SCENARIO(TEST_NAME){
  printTestName("vertex shader, gl_VertexID (indexing)");
  indexTest<uint32_t>();
  indexTest<uint16_t>();
  indexTest<uint8_t>();
}
