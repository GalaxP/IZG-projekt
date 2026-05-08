#include <iostream>

#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "gl_VertexID_no_indexing"
#include<tests/testCommon.hpp>
#include<tests/str.hpp>

using namespace tests;

SCENARIO(TEST_NAME){
  printTestName("vertex shader, gl_VertexID (no indexing)");

  MEMCB(10,10);
  mem.framebuffers[0] = Framebuffer{};

  auto&prg = mem.programs[7];
  prg.vertexShader   =   vertexEmpty;

  pushBindProgramCommand(cb,7);
  pushDrawCommand(cb,9);

  if(memCb->runAndTest())return;
  std::cerr << R".(
  Vrcholy chodí do vertex shaderu číslované.
  Číslo vrcholu je uloženo v položce gl_VertexID ve struktuře InVertex.
  Když není zapnuté indexování (v tomto testu není), tak je číslo vrcholu rovno číslu zavolání vertex shaderu.

  Proměnná gl_VertexID odpovída proměnné gl_VertexID z GLSL.)."<<std::endl;

  memCb->printDifferencies();

  memCb->printHelp(R".(
  void draw(DrawCommand const&cmd){
    for(... cmd.nofVertices ... ){//smyčka přes vrcholy
      InVertex  inVertex; // vrchol, co vstupuje do vertex shader
      OutVertex outVertex;// vrchol, co leze ven z vertex shaderu 
      inVertex.gl_VertexID = ??;
      ShaderInterface si;
      if(vertexShader != nullptr)
        vertexShader(outVertex,inVertex,si);
    }
  }).");

  REQUIRE(false);
}


