#include <iostream>
#include <string.h>

#include <catch2/catch_test_macros.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define __FILENAME__ "drawModelTests"

#include <framework/textureData.hpp>
#include <tests/model/createModel.hpp>
#include <tests/testCommon.hpp>

using namespace tests;
using namespace tests::model;


SCENARIO(TEST_NAME){
  std::cerr << testCounter() <<" - prepareModel - memory - shaders" << std::endl;

  auto n1 = NodeI(0);
  auto n2 = NodeI(0,{NodeI(1),NodeI(2)});
  if(checkModelTranslation(createModel({3},{n1})))return;
  REQUIRE(false);

}

SCENARIO(TEST_NAME){
  std::cerr << testCounter() << " - prepareModel - memory - vs2fs" << std::endl;

  auto n1 = NodeI(0);
  auto n2 = NodeI(0,{NodeI(1),NodeI(2)});
  if(checkModelTranslation(createModel({3},{n1})))return;
  REQUIRE(false);
}

SCENARIO(TEST_NAME){
  std::cerr << testCounter() << " - prepareModel - memory - buffers" << std::endl;

  auto buf0 = std::vector<float>(10,0);
  auto buf1 = std::vector<float>(11,0);
  auto buf2 = std::vector<float>(12,0);

  TestModel model;
  model.buffers = new Buffer[3];
  model.buffers[0] = vectorToBuffer(buf0);
  model.buffers[1] = vectorToBuffer(buf1);
  model.buffers[2] = vectorToBuffer(buf2);
  model.nofBuffers = 3;
  if(checkModelTranslation(model))return;
  REQUIRE(false);
}

SCENARIO(TEST_NAME){
  std::cerr << testCounter() << " - prepareModel - memory - textures" << std::endl;

  TestModel model;

  auto tex0 = TextureData(13,14,1,Image::U8);
  auto tex1 = TextureData(14,14,1,Image::U8);
  auto tex2 = TextureData(15,15,1,Image::U8);

  model.textures = new Texture[3];
  model.textures[0] = tex0.getTexture();
  model.textures[1] = tex1.getTexture();
  model.textures[2] = tex2.getTexture();
  model.nofTextures = 3;
  if(checkModelTranslation(model))return;
  REQUIRE(false);
}

SCENARIO(TEST_NAME){
  std::cerr << testCounter() << " - prepareModel - uniforms - diffuse color" << std::endl;

  auto m1 = MeshI(3,glm::vec4(.5));
  auto m2 = MeshI(3,glm::vec4(.2,.3,.4,.1));
  auto m3 = MeshI(3,glm::vec4(.7,.2,.1,.1));
  auto n1 = NodeI(0);
  auto n2 = NodeI(1,{n1,n1});
  auto n3 = NodeI(2,{n2,n2});
  auto model = createModel({m1,m2,m3},{n3});
  if(checkModelTranslation(model))return;
  REQUIRE(false);
}

SCENARIO(TEST_NAME){
  std::cerr << testCounter() << " - prepareModel - uniforms - texture id" << std::endl;

  auto m1 = MeshI(3,glm::vec4(.5),3);
  auto m2 = MeshI(3,glm::vec4(.2,.3,.4,.1),4);
  auto m3 = MeshI(3,glm::vec4(.7,.2,.1,.1),7);
  auto n1 = NodeI(0);
  auto n2 = NodeI(1,{n1,n1});
  auto n3 = NodeI(2,{n2,n2});
  auto model = createModel({m1,m2,m3},{n3});
  if(checkModelTranslation(model))return;
  REQUIRE(false);
}

SCENARIO(TEST_NAME){
  std::cerr << testCounter() << " - prepareModel - uniforms - model matrix" << std::endl;

  std::vector<glm::mat4>mats;
  mats.push_back(glm::translate(glm::mat4(1),     glm::vec3(1,2,3)));
  mats.push_back(glm::rotate   (glm::mat4(1),0.3f,glm::vec3(0,1,0)));
  mats.push_back(glm::scale    (glm::mat4(1),     glm::vec3(.3,-2.1,4.3)));

  auto m1 = MeshI(3,glm::vec4(.5),3);
  auto m2 = MeshI(3,glm::vec4(.2,.3,.4,.1),4);
  auto m3 = MeshI(3,glm::vec4(.7,.2,.1,.1),7);
  auto n1 = NodeI(0,{     },mats[0]);
  auto n2 = NodeI(1,{n1,n1},mats[1]);
  auto n3 = NodeI(2,{n2,n2},mats[2]);

  auto model = createModel({m1,m2,m3},{n3});

  if(checkModelTranslation(model))return;
  REQUIRE(false);
}

SCENARIO(TEST_NAME){
  std::cerr << testCounter() <<" - prepareModel - uniforms - inverse model matrix" << std::endl;

  std::vector<glm::mat4>mats;
  mats.push_back(glm::translate(glm::mat4(1),     glm::vec3(1,2,3)));
  mats.push_back(glm::rotate   (glm::mat4(1),0.3f,glm::vec3(0,1,0)));
  mats.push_back(glm::scale    (glm::mat4(1),     glm::vec3(.3,-2.1,4.3)));

  auto m1 = MeshI(3,glm::vec4(.5),3);
  auto m2 = MeshI(3,glm::vec4(.2,.3,.4,.1),4);
  auto m3 = MeshI(3,glm::vec4(.7,.2,.1,.1),7);
  auto n1 = NodeI(0,{     },mats[0]);
  auto n2 = NodeI(1,{n1,n1},mats[1]);
  auto n3 = NodeI(2,{n2,n2},mats[2]);

  auto model = createModel({m1,m2,m3},{n3});

  if(checkModelTranslation(model))return;
  REQUIRE(false);
}

