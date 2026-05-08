#include <iostream>

#include <catch2/catch_test_macros.hpp>

#define __FILENAME__ "traverseModelTests"
#include <tests/testCommon.hpp>
#include <tests/str.hpp>

using namespace tests;
using namespace tests::model;

SCENARIO(TEST_NAME){
  printTestName("prepareModel - drawCommand - inserting roots");

  TestModel model;

  model.nofRoots = 1;
  model.roots = new Node[model.nofRoots];
  model.meshes = new Mesh[1];
  model.roots[0].mesh = 0;
  model.meshes[0].nofIndices = 3;

  if(checkModelTranslation(model))return;

  std::cerr<<R".(
  Tento test ověřuje funkcionalitu funkce prepareModel.

  Test zkouší zpracovat model s jednim meshem (3 vertexy) a jedním root bez potomků.)." << std::endl;
  printModel(2,model);
  std::cerr<<R".(
  Ale něco se pokazilo...

  Model obsahuje položku roots.
  To jsou kořeny stromové struktury modelu.
  Položka roots je uložena ve std::vector.

  počet roots se dá zjistit pomocí roots.size().
  přistoupit k jednotlivých kořenům lze přes [] - roots[index].

  můžete je projít třeba takto:
  for(size_t i=0;i<roots.size();++i){
    process(roots[i]...);
  }

  Nápověda k implementaci drawModel:

  void prepareNode(GPUMemory&mem,CommandBuffer&cb,Node const&node,Model const&model,...){
    if(node.mesh >= 0){ // ma tento node mesh?
      Mesh const&mesh = model.meshes[node.mesh];

      addDrawCommand(cb,...);
      ...
      
    }
    ...
  }

  void prepareModel(GPUMemory&mem,CommandBuffer&cb,Model const&model){
    ...
    for(size_t i=0;i<model.roots.size();++i)
      prepareNode(mem,cb,model.roots[i],model,...);
  })." << std::endl;

  REQUIRE(false);
}

SCENARIO(TEST_NAME){
  printTestName("prepareModel - drawCommand - mesh attributes & indexing");

  TestModel model;

  model.nofRoots = 1;
  model.roots = new Node[model.nofRoots];
  model.meshes = new Mesh[1];
  model.roots[0].mesh = 0;
  model.meshes[0].nofIndices = 3;
  model.meshes[0].position.bufferID   = 13;
  model.meshes[0].position.offset     = 1334;
  model.meshes[0].position.stride     = 32;
  model.meshes[0].position.type       = AttribType::VEC3;
  model.meshes[0].normal  .bufferID   = 1;
  model.meshes[0].normal  .offset     = 13;
  model.meshes[0].normal  .stride     = 3423;
  model.meshes[0].normal  .type       = AttribType::VEC3;
  model.meshes[0].texCoord.bufferID   = 17;
  model.meshes[0].texCoord.offset     = 323;
  model.meshes[0].texCoord.stride     = 33;
  model.meshes[0].texCoord.type       = AttribType::VEC2;
  model.meshes[0].indexType           = IndexType::U16;
  model.meshes[0].indexOffset         = 123;
  model.meshes[0].indexBufferID       = 47;

  if(checkModelTranslation(model))return;

  std::cerr<<R".(
  Tento test ověřuje funkcionalitu funkce prepareModel.

  Test zkouší zpracovat model s jednim meshem (3 vertexy) a jedním root a bez potomků.
  Test zkoumá, jestli správně nastavujete attributy a indexing pro kreslení.
  Konkrétně se jedná o nastavení VertexArray (vao).
  Pozice,normaly,tex coord. by měly být nastavené na 0., 1. a 2. attributu vao.
  
  Ale něco se pokazilo...)."<<std::endl;
  REQUIRE(false);
}

SCENARIO(TEST_NAME){
  printTestName("prepareModel - traverse - check roots");

  checkTraverse({3},{0,-1,0});
  checkTraverse({3,6,9},{0,1,2});
  checkTraverse({3,6},{0,1,-1,1,0});
  checkTraverse({3,6,9},{0,0,-1,-1,2});
}

SCENARIO(TEST_NAME){
  printTestName("prepareModel - traverse - node in node");

  auto n1 = NodeI(0);
  auto n2 = NodeI(1,{n1});
  auto n3 = NodeI(2,{n2});
  auto n4 = NodeI(3,{n3});
  checkTraverse({3},{n1});
  checkTraverse({3,6},{n2});
  checkTraverse({3,9,30},{n3});
  checkTraverse({90,60,30,3},{n4});
}

SCENARIO(TEST_NAME){
  printTestName("prepareModel - traverse - binary tree");

  auto n1 = NodeI(0);
  auto n2 = NodeI(0,{n1,n1});
  auto n3 = NodeI(0,{n2,n2});
  auto n4 = NodeI(0,{n3,n3});
  checkTraverse({3},{n1});
  checkTraverse({3},{n2});
  checkTraverse({3},{n3});
  checkTraverse({3},{n4});
}

SCENARIO(TEST_NAME){
  printTestName("prepareModel - traverse - tree");

  auto n0 = NodeI(0);
  auto n1 = NodeI(1);
  auto n2 = NodeI(2);
  auto n3 = NodeI(3);
  auto n4 = NodeI(4);
  auto n5 = NodeI(5);
  auto n6 = NodeI(6);

  auto n7  = NodeI(7,{n0,n1});
  auto n8  = NodeI(8,{n2,n3});
  auto n9  = NodeI(9,{n4,n5});
  auto n10 = NodeI(10,{n6,n0});

  auto n11 = NodeI(11,{n7,n8});
  auto n12 = NodeI(12,{n9,n10});

  auto n13 = NodeI(13,{n11,n12});

  #define MESHES {3,9,12,6,300,600,900,3,9,12,15,3,6,9}
  checkTraverse(MESHES,{n1});
  checkTraverse(MESHES,{n2});
  checkTraverse(MESHES,{n3});
  checkTraverse(MESHES,{n4});
  checkTraverse(MESHES,{n5});
  checkTraverse(MESHES,{n6});
  checkTraverse(MESHES,{n7});
  checkTraverse(MESHES,{n8});
  checkTraverse(MESHES,{n9});
  checkTraverse(MESHES,{n10});
  checkTraverse(MESHES,{n11});
  checkTraverse(MESHES,{n12});
  checkTraverse(MESHES,{n13});
  #undef MESHES
}

