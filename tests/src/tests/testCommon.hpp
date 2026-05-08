#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>

#include <glm/glm.hpp>
#include <solutionInterface/gpu.hpp>
#include <framework/colorPrinting.hpp>
#include <tests/memDeepCopy.hpp>
#include <tests/isMemDifferent.hpp>
#include<tests/model/createModel.hpp>

namespace tests{

#ifndef __FILENAME__
#define __FILENAME__ __FILE__
#endif

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define TEST_NAME __FILENAME__ ":" STRINGIZE(__LINE__)

std::string testCounter(bool first = false);
void printTestName(std::string const&name);
void printFirstTestName(std::string const&name);

glm::uvec4 getColorU(Image       const&img  ,uint32_t x,uint32_t y);
glm:: vec4 getColorF(Image       const&img  ,uint32_t x,uint32_t y);
glm::uvec4 getColorU(Texture     const&tex  ,uint32_t x,uint32_t y);
glm:: vec4 getColorF(Texture     const&tex  ,uint32_t x,uint32_t y);
glm::uvec4 getColorU(Framebuffer const&frame,uint32_t x,uint32_t y);
glm:: vec4 getColorF(Framebuffer const&frame,uint32_t x,uint32_t y);
float      getDepth (Framebuffer const&frame,uint32_t x,uint32_t y);

void setColorU (Image       &tex  ,uint32_t x,uint32_t y,glm::uvec4 const&color  );
void setColorF (Image       &tex  ,uint32_t x,uint32_t y,glm:: vec4 const&color  );
void setColorU (Texture     &tex  ,uint32_t x,uint32_t y,glm::uvec4 const&color  );
void setColorF (Texture     &tex  ,uint32_t x,uint32_t y,glm:: vec4 const&color  );
void setColorU (Framebuffer &frame,uint32_t x,uint32_t y,glm::uvec4 const&color  );
void setColorF (Framebuffer &frame,uint32_t x,uint32_t y,glm:: vec4 const&color  );
void setDepth  (Texture     &tex,  uint32_t x,uint32_t y,float            depth  );
void setDepth  (Framebuffer &frame,uint32_t x,uint32_t y,float            depth  );
void setStencil(Texture     &tex,  uint32_t x,uint32_t y,uint8_t          stencil);
void setStencil(Framebuffer &frame,uint32_t x,uint32_t y,uint8_t          stencil);

float const extern floatErr;

bool equalFloats(float const& a, float const& b,float err = floatErr);
bool equalVec2(glm::vec2 const&a,glm::vec2 const&b,float err = floatErr);
bool equalVec3(glm::vec3 const&a,glm::vec3 const&b,float err = floatErr);
bool equalVec4(glm::vec4 const&a,glm::vec4 const&b,float err = floatErr);
bool equalMat4(glm::mat4 const&a,glm::mat4 const&b,float err = floatErr);

bool equalCounts(size_t a,size_t b,size_t err = 10);
bool equalCountsPercentage(size_t e,size_t s,float p=0.1f);

bool greaterFloat(float a,float b,float err = floatErr);

bool lessFloat(float a,float b,float err = floatErr);



uint8_t    floatColorToByte(float c);
glm::uvec4 floatColorToBytes(glm::vec4 const&col);


bool        sameColor(glm::uvec4 const&a,glm::uvec4 const&b,uint32_t channels);
std::string colorToStr(glm::uvec4 const&a,uint32_t channels);



bool operator==(VertexAttrib const&a,VertexAttrib const&b);
bool operator==(InVertex const&a,InVertex const&b);
bool operator==(OutVertex const&a,OutVertex const&b);


struct AllocatedFramebuffer{
  std::vector<uint8_t>colorBacking  ;
  std::vector<float  >depthBacking  ;
  std::vector<uint8_t>stencilBacking;
  Framebuffer frame;
};

AllocatedFramebuffer createFramebuffer(uint32_t height,uint32_t width,bool flipped = false);

struct VertexShaderData{
  InVertex         in;
  OutVertex       out;
  ShaderInterface  si;
};

struct FragmentShaderData{
  InFragment       in;
  OutFragment     out;
  ShaderInterface  si;
};

struct uvec2Cmp{
  bool operator()(glm::uvec2 const& a,glm::uvec2 const&b)const;
};

struct Packet{
  Packet(){}
  ~Packet(){}
  AllocatedMem memAtStart;
  GPUMemory*   memWithRightPointers = nullptr;
  std::vector<VertexShaderData>vertices  ;
  std::map<glm::uvec2,FragmentShaderData,uvec2Cmp>fragments ;
  std::vector<FragmentShaderData>allFragments;
};

struct StreamDump{
  std::vector<std::shared_ptr<Packet>>gpuStream;
  void store(InVertex   const&in,OutVertex   const&out,ShaderInterface const&si);
  void store(InFragment const&in,OutFragment const&out,ShaderInterface const&si);
  bool isLastPacketGoodForVertices ();
  bool isLastPacketGoodForFragments();
  std::shared_ptr<Packet>getPacketForStoringVertices ();
  std::shared_ptr<Packet>getPacketForStoringFragments();
  std::shared_ptr<Packet>getLastPacketOrAllocateIt(bool allocate);
};

struct GPUProcessDifference;

struct MemCb{
  MemCb();
  MemCb(uint32_t w,uint32_t h);
  AllocatedMem         expectedMem;
  AllocatedMem         studentMem ;
  GPUMemory            mem     ;
  CommandBuffer        cb      ;
  AllocatedFramebuffer aframe  ;
  GPUMemory*           inFlightMem = nullptr;

  StreamDump             streamDump;
  StreamDump             expectedStreamDump;
  StreamDump             studentStreamDump;
  std::vector<Program>programs;
  std::vector<std::shared_ptr<GPUProcessDifference>>diff;
  void setGeometry(
      std::vector<glm::vec4>const&pos     ,
      std::vector<glm::vec4>const&col = {},
      std::vector<uint32_t >const&dis = {});
  bool runAndTest(uint32_t n=1,bool first = false);
  void printDifferencies();
  void printStatus();
  void printHelp(std::string const&str);
  void hookAllCallbacks(GPUMemory&mem);
  void unhookAllCallbacks(GPUMemory&mem);
};

bool checkModelTranslation(tests::model::TestModel const&model,bool first = false);
void checkTraverse(std::vector<tests::model::MeshI>const&meshes,std::vector<tests::model::NodeI>const&roots);

extern std::shared_ptr<MemCb>memCb;


#define MEMCB(w,h)                                                                                   \
  memCb                                    = std::make_shared<MemCb>(w,h)                           ;\
  GPUMemory    &mem                        = memCb->mem                                             ;\
  CommandBuffer&cb                         = memCb->cb                                              ;\
  mem.framebuffers[mem.defaultFramebuffer] = memCb->aframe.frame                                    ;\
  auto channels                            = mem.framebuffers[mem.defaultFramebuffer].color.channels;\
  (void)channels                                                                                    ;\
  (void)mem                                                                                         ;\
  (void)cb

  
  
template<typename T>
Buffer vectorToBuffer(std::vector<T>const&d){
  Buffer res;
  res.data = d.data();
  res.size = sizeof(T)*d.size();
  return res;
}

void vertexEmpty                 (OutVertex  &,InVertex   const&,ShaderInterface const&);
void vertexPos                   (OutVertex  &,InVertex   const&,ShaderInterface const&);
void vertexPosColor              (OutVertex  &,InVertex   const&,ShaderInterface const&);
void vertexPosColorDiscard       (OutVertex  &,InVertex   const&,ShaderInterface const&);
void fragmentEmpty               (OutFragment&,InFragment const&,ShaderInterface const&);
void fragmentColor               (OutFragment&,InFragment const&,ShaderInterface const&);
void fragmentColorDiscard        (OutFragment&,InFragment const&,ShaderInterface const&);
void fragmentColorDiscardEveryOdd(OutFragment&,InFragment const&,ShaderInterface const&);
void fragmentDiscardEveryOdd     (OutFragment&,InFragment const&,ShaderInterface const&);

void clearFrame(Framebuffer&frame,glm::vec3 const&c = glm::vec3(0.f),float d=1.f);
void clearFrame(Framebuffer&frame,glm::uvec3 const&c = glm::uvec3(0),float d=1.f);
glm::uvec3 alphaMix(glm::vec4 const&frameColor,glm::vec4 const&fragColor);


bool breakTest(bool first = false);
int getTestNumberAndIncrement(bool first = false);
bool shouldTestBeIgnored(int testNumber);
bool shouldTestBeBroken (int testNumber);


}
