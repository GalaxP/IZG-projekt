#include <glm/common.hpp>
#include <glm/fwd.hpp>
#include <cmath>
#include <memory>
#include <framework/programContext.hpp>
#include <framework/switchSolution.hpp>
#include <tests/testCommon.hpp>
#include <tests/str.hpp>
#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <type_traits>

void drawModel_vertexShader  (OutVertex  &,InVertex   const&,ShaderInterface const&);
void drawModel_fragmentShader(OutFragment&,InFragment const&,ShaderInterface const&);

#define ___ std::cerr << __FILE__ << "/" << __LINE__ << std::endl

namespace tests{

glm::uvec4 getColorU(Image const&img,uint32_t x,uint32_t y){
  auto res   = glm::uvec4(0u);
  auto pixel = (uint8_t*)getPixel(img,x,y);

  for(uint32_t c=0;c<img.channels;++c){
    uint32_t v = 0;
    switch(img.format){
      case Image::F32:{
        float cc = *(float*)pixel;
        v = glm::clamp((uint32_t)(cc*255.f),0u,255u);
        pixel += sizeof(float);
      }break;
      case Image::U8 :{
        v = *(uint8_t*)pixel;
        pixel += sizeof(uint8_t);
      }
    }
    res[img.channelTypes[c]] = v;
  }

  return res;
}

glm:: vec4 getColorF(Image const&img,uint32_t x,uint32_t y){
  auto res   = glm::vec4(0.f);
  auto pixel = (uint8_t*)getPixel(img,x,y);

  for(uint32_t c=0;c<img.channels;++c){
    float v = 0.f;
    switch(img.format){
      case Image::F32:{
        float cc = *(float*)pixel;
        v = cc;
        pixel += sizeof(float);
      }break;
      case Image::U8 :{
        v = (*(uint8_t*)pixel)/255.f;
        pixel += sizeof(uint8_t);
      }
    }
    res[img.channelTypes[c]] = v;
  }

  return res;
}

glm::uvec4 getColorU(Texture const&tex,uint32_t x,uint32_t y){
  return getColorU(tex.img,x,y);
}

glm:: vec4 getColorF(Texture const&tex,uint32_t x,uint32_t y){
  return getColorF(tex.img,x,y);
}

glm::uvec4 getColorU(Framebuffer const&frame,uint32_t x,uint32_t y){
  return getColorU(frame.color,x,y);
}

glm:: vec4 getColorF(Framebuffer const&frame,uint32_t x,uint32_t y){
  return getColorF(frame.color,x,y);
}

float getDepth(Framebuffer const&frame,uint32_t x,uint32_t y){
  return getColorF(frame.depth,x,y).x;
}

void setColorU(Image&img,uint32_t x,uint32_t y,glm::uvec4 const&color){
  auto pixel = (uint8_t*)getPixel(img,x,y);
  for(uint32_t c=0;c<img.channels;++c){
    switch(img.format){
      case Image::F32:{
        auto cc = (float*)pixel;
        *cc = color[img.channelTypes[c]]/255.f;
        pixel += sizeof(float);
      }break;
      case Image::U8 :{
        auto cc = (uint8_t*)pixel;
        *cc = color[img.channelTypes[c]];
        pixel += sizeof(uint8_t);
      }
    }
  }
}

void setColorF(Image&img,uint32_t x,uint32_t y,glm::vec4 const&color){
  auto pixel = (uint8_t*)getPixel(img,x,y);
  for(uint32_t c=0;c<img.channels;++c){
    switch(img.format){
      case Image::F32:{
        auto cc = (float*)pixel;
        *cc = color[img.channelTypes[c]];
        pixel += sizeof(float);
      }break;
      case Image::U8 :{
        auto cc = (uint8_t*)pixel;
        *cc = glm::clamp(uint32_t(color[img.channelTypes[c]]*255u),0u,255u);
        pixel += sizeof(uint8_t);
      }
    }
  }
}

void setColorU(Texture &tex,uint32_t x,uint32_t y,glm::uvec4 const&color){
  setColorU(tex.img,x,y,color);
}

void setColorF(Texture &tex,uint32_t x,uint32_t y,glm:: vec4 const&color){
  setColorF(tex.img,x,y,color);
}

void setColorU(Framebuffer &frame,uint32_t x,uint32_t y,glm::uvec4 const&color){
  setColorU(frame.color,x,y,color);
}

void setColorF(Framebuffer &frame,uint32_t x,uint32_t y,glm:: vec4 const&color){
  setColorF(frame.color,x,y,color);
}

void setDepth (Texture     &tex,  uint32_t x,uint32_t y,float            depth){
  setColorF(tex,x,y,glm::vec4(depth));
}

void setDepth (Framebuffer &frame,uint32_t x,uint32_t y,float            depth){
  setColorF(frame.depth,x,y,glm::vec4(depth));
}

void setStencil(Texture     &tex,  uint32_t x,uint32_t y,uint8_t          stencil){
  setColorU(tex,x,y,glm::uvec4(stencil));
}

void setStencil(Framebuffer &frame,uint32_t x,uint32_t y,uint8_t          stencil){
  setColorU(frame,x,y,glm::uvec4(stencil));
}

std::string testCounter(bool first){
  static int32_t counter = -1;
  if(first)counter=-1;
  ++counter;
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2) << counter;
  return ss.str();
}

void printFirstTestName(std::string const&name){
  std::cerr << testCounter(true) << " - " << name << std::endl;
}

void printTestName(std::string const&name){
  std::cerr << testCounter() << " - " << name << std::endl;
}


float const floatErr = 0.001f;

bool equalFloats(float const& a, float const& b,float err) {
  return glm::abs(a - b) <= err;
}

bool equalMat4(glm::mat4 const&a,glm::mat4 const&b,float err){
  return
    equalVec4(a[0],b[0],err) &&
    equalVec4(a[1],b[1],err) &&
    equalVec4(a[2],b[2],err) &&
    equalVec4(a[3],b[3],err) ;
}

bool equalVec4(glm::vec4 const&a,glm::vec4 const&b,float err){
  return
    equalFloats(a.x,b.x,err) &&
    equalFloats(a.y,b.y,err) &&
    equalFloats(a.z,b.z,err) &&
    equalFloats(a.w,b.w,err) ;
}

bool equalVec3(glm::vec3 const&a,glm::vec3 const&b,float err){
  return
    equalFloats(a.x,b.x,err) &&
    equalFloats(a.y,b.y,err) &&
    equalFloats(a.z,b.z,err) ;
}

bool equalVec2(glm::vec2 const&a,glm::vec2 const&b,float err){
  return
    equalFloats(a.x,b.x,err) &&
    equalFloats(a.y,b.y,err);
}

bool equalCounts(size_t a,size_t b,size_t err){
  if(a<b)return (b-a)<=err;
  return (a-b)<=err;
}

bool equalCountsPercentage(size_t e,size_t s,float p){
  auto err = (size_t)(e*p);
  return equalCounts(e,s,err);
}

bool greaterFloat(float a,float b,float err){
  return a>b-err;
}

bool lessFloat(float a,float b,float err){
  return a<b+err;
}




void createDrawCMD(
    CommandBuffer&cb,
    uint32_t       n
    ){
  auto&a=cb.commands[cb.nofCommands];
  cb.nofCommands++;
  a.type = CommandType::DRAW;
  auto&cmd = a.data.drawCommand;

  cmd.nofVertices = n  ;
}


uint8_t floatColorToByte(float c){
  return (uint8_t)(c*255.f);
}


bool sameColor(glm::uvec4 const&a,glm::uvec4 const&b,uint32_t channels){
  for(uint32_t i=0;i<channels;++i)
    if(a[i] != b[i])return false;
  return true;
}

std::string colorToStr(glm::uvec4 const&a,uint32_t channels){
  std::stringstream ss;
  ss << "(";
  for(uint32_t i=0;i<channels;++i){
    if(i>0)ss << ",";
    ss << a[i];
  }
  ss << ")";
  return ss.str();
}

glm::uvec4 floatColorToBytes(glm::vec4 const&col){
  auto res = glm::uvec4(0);
  for(uint32_t i=0;i<4;++i)
    res[i] = floatColorToByte(col[i]);
  return res;
}

std::shared_ptr<MemCb>memCb;

MemCb::MemCb(){}

MemCb::MemCb(uint32_t w,uint32_t h){
  aframe = createFramebuffer(w,h);
}






void MemCb::printHelp(std::string const&str){
  std::cerr << padding(2) << "Nápověda:" << std::endl;
  print(str,Color::BLUE);
}

bool operator==(Attrib const&a,Attrib const&b){
  return a.v4 == b.v4;
}

bool operator==(InVertex const&a,InVertex const&b){
  for(size_t i=0;i<maxAttribs;++i)
    if(!(a.attributes[i] == b.attributes[i]))return false;
  return a.gl_VertexID == b.gl_VertexID;
}

bool operator==(OutVertex const&a,OutVertex const&b){
  for(size_t i=0;i<maxAttribs;++i)
    if(!(a.attributes[i] == b.attributes[i]))return false;
  return a.gl_Position == b.gl_Position;
}



bool empty(void const*ptr){
  return ptr == nullptr;
}




void vertexEmpty(OutVertex&,InVertex const&,ShaderInterface const&){}

void vertexPos            (OutVertex  &out,InVertex   const&in,ShaderInterface const&){
  out.gl_Position      = in.attributes[0].v4;
}
void vertexPosColor       (OutVertex  &out,InVertex   const&in,ShaderInterface const&){
  out.gl_Position      = in.attributes[0].v4;
  out.attributes[0].v4 = in.attributes[1].v4;
}
void vertexPosColorDiscard(OutVertex  &out,InVertex   const&in,ShaderInterface const&){
  out.gl_Position      = in.attributes[0].v4;
  out.attributes[0].v4 = in.attributes[1].v4;
  out.attributes[1].u1 = in.attributes[2].u1;
}
void fragmentEmpty        (OutFragment&,InFragment const&,ShaderInterface const&){
}
void fragmentColor        (OutFragment&out,InFragment const&in,ShaderInterface const&){
  out.gl_FragColor = glm::vec4(in.attributes[0].v4);
}
void fragmentColorDiscard (OutFragment&out,InFragment const&in,ShaderInterface const&){
  out.gl_FragColor = glm::vec4(in.attributes[0].v4);
  out.discard      = (bool)   (in.attributes[1].u1);
}

void fragmentDiscardEveryOdd (OutFragment&out,InFragment const&in,ShaderInterface const&){
  auto pix = glm::ivec2(in.gl_FragCoord);
  if((pix.x%2==1) == (pix.y%2==1))out.discard = true;
}

void fragmentColorDiscardEveryOdd (OutFragment&out,InFragment const&in,ShaderInterface const&){
  out.gl_FragColor = glm::vec4(in.attributes[0].v4);
  auto pix = glm::ivec2(in.gl_FragCoord);
  if((pix.x%2==1) == (pix.y%2==1))out.discard = true;
}



void clearFrame(Framebuffer&frame,glm::uvec3 const&color,float d){
  for(uint32_t y=0;y<frame.height;++y)
    for(uint32_t x=0;x<frame.width;++x){
      auto pix = glm::uvec2(x,y);
      setColorU(frame,pix.x,pix.y,glm::uvec4(color,0u));
      setDepth (frame,pix.x,pix.y,d);
    }
}

void clearFrame(Framebuffer&frame,glm:: vec3 const&c,float d){
  clearFrame(frame,glm::uvec3(glm::clamp(c*255.f,glm::vec3(0.f),glm::vec3(255.))),d);
}




bool operator==(VertexAttrib const&a,VertexAttrib const&b){
  if(a.bufferID != b.bufferID)return false;
  if(a.offset   != b.offset  )return false;
  if(a.stride   != b.stride  )return false;
  if(a.type     != b.type    )return false;
  return true;
}


int32_t getTestNumberAndIncrement(bool first){
  static int32_t counter = 0;
  if(first)counter = 0;
  auto res = counter;
  ++counter;
  return res;
}

bool shouldTestBeBroken(int32_t test){
  return test == ProgramContext::get().args.testToBreak;
}

bool shouldTestBeIgnored(int32_t test){
  return test < ProgramContext::get().args.ignoreUpToTest;
}


bool breakTest(bool first){
  auto test = getTestNumberAndIncrement(first);
  if(shouldTestBeBroken(test))return true;
  return false;
}


AllocatedFramebuffer createFramebuffer(uint32_t width,uint32_t height,bool flipped){
  AllocatedFramebuffer res;

  res.colorBacking   = std::vector<uint8_t>(width*height*3,0  );
  res.depthBacking   = std::vector<float  >(width*height  ,0.f);
  res.stencilBacking = std::vector<uint8_t>(width*height  ,0  );

  auto&frame = res.frame;
  frame.width     = width  ;
  frame.height    = height ;
  frame.yReversed = flipped;

  auto&c = frame.color;
  c.channels      = 3                          ;
  c.bytesPerPixel = c.channels*sizeof(uint8_t) ;
  c.data          = res.colorBacking.data()    ;
  c.format        = Image::U8                  ;
  c.pitch         = frame.width*c.bytesPerPixel;

  auto&d = frame.depth;
  d.channels      = 1                          ;
  d.bytesPerPixel = d.channels*sizeof(float)   ;
  d.data          = res.depthBacking.data()    ;
  d.format        = Image::F32                 ;
  d.pitch         = frame.width*d.bytesPerPixel;

  auto&s = frame.stencil;
  s.channels      = 1                          ;
  s.bytesPerPixel = s.channels*sizeof(uint8_t) ;
  s.data          = res.stencilBacking.data()  ;
  s.format        = Image::U8                  ;
  s.pitch         = frame.width*s.bytesPerPixel;
  return res;
}




bool StreamDump::isLastPacketGoodForVertices  (){
  if(gpuStream.empty())return false;
  if(!gpuStream.back()->allFragments.empty())return false;
  if(gpuStream.back()->vertices.size() == 3)return false;
  if(isMemShallowDifferent(gpuStream.back()->memAtStart.mem,*memCb->inFlightMem))return false;
  return true;
}

bool StreamDump::isLastPacketGoodForFragments(){
  if(gpuStream.empty())return false;
  if(isMemShallowDifferent(gpuStream.back()->memAtStart.mem,*memCb->inFlightMem))return false;
  return true;
}

std::shared_ptr<Packet>StreamDump::getLastPacketOrAllocateIt(bool allocate){
  std::shared_ptr<Packet>packet;
  if(allocate){
    packet = std::make_shared<Packet>();
    packet->memAtStart = deepCopy(*memCb->inFlightMem);
    packet->memWithRightPointers = memCb->inFlightMem;
    gpuStream.push_back(packet);
  }else{
    packet = std::dynamic_pointer_cast<Packet>(gpuStream.back());
  }
  return packet;
}

std::shared_ptr<Packet>StreamDump::getPacketForStoringVertices(){
  return getLastPacketOrAllocateIt(!isLastPacketGoodForVertices());
}

std::shared_ptr<Packet>StreamDump::getPacketForStoringFragments(){
  return getLastPacketOrAllocateIt(!isLastPacketGoodForFragments());
}

bool uvec2Cmp::operator()(glm::uvec2 const&a,glm::uvec2 const&b)const{
  if(a.x < b.x)return true ;
  if(a.x > b.x)return false;
  if(a.y < b.y)return true ;
  if(a.y > b.y)return false;
  return false;
}

void StreamDump::store(InVertex const&in,OutVertex const&out,ShaderInterface const&si){
  auto packet = getPacketForStoringVertices();
  packet->vertices.push_back({in,out,si});
}

void StreamDump::store(InFragment const&in,OutFragment const&out,ShaderInterface const&si){
  auto packet = getPacketForStoringFragments();
  auto key = glm::uvec2(in.gl_FragCoord);
  packet->fragments[key] = {in,out,si};
  packet->allFragments.push_back({in,out,si});
}

void dumpVertexShaderGeneral(
    OutVertex            &out,
    InVertex        const&in ,
    ShaderInterface const&si ){
  auto&realPrg = memCb->programs.at(memCb->inFlightMem->activatedProgram);
  realPrg.vertexShader(out,in,si);
  memCb->streamDump.store(in,out,si);
}

void dumpFragmentShaderGeneral(
    OutFragment          &out,
    InFragment      const&in ,
    ShaderInterface const&si ){
  auto&realPrg = memCb->programs.at(memCb->inFlightMem->activatedProgram);
  realPrg.fragmentShader(out,in,si);
  memCb->streamDump.store(in,out,si);
}

void MemCb::hookAllCallbacks(GPUMemory&mem){
  memCb->programs.clear();
  for(uint32_t i=0;i<mem.maxPrograms;++i){
    auto&prg = mem.programs[i];
    memCb->programs.push_back(prg);
    if(prg.vertexShader  )prg.vertexShader   = dumpVertexShaderGeneral  ;
    if(prg.fragmentShader)prg.fragmentShader = dumpFragmentShaderGeneral;
  }
}

void MemCb::unhookAllCallbacks(GPUMemory&mem){
  for(uint32_t i=0;i<mem.maxPrograms;++i){
    mem.programs[i] = memCb->programs.at(i);
  }
  memCb->programs.clear();
}

bool isStreamVertexPacketSame(Packet const&exp,Packet const&stu){
  if(exp.vertices.size() != stu.vertices.size())return false;
  for(size_t i=0;i<exp.vertices.size();++i){
    auto const&exp_inOut = exp.vertices.at(i);
    auto const&stu_inOut = stu.vertices.at(i);
    if(!(exp_inOut.in  == stu_inOut.in ))return false;
    if(!(exp_inOut.out == stu_inOut.out))return false;
  }
  return true;
}

struct ConvexHull{
  std::vector<glm::vec2>points        ;
  std::vector<glm::vec3>lines         ;
  float                 perimetr = 0.f;
  bool            isCCW = true;
  BackfaceCulling backfaceCulling;
  bool isInside(glm::vec2 const&pix,float tolerance = 1.f){
    if(points.size() == 0){
      return false;
    }
    if(points.size() == 1){
      if(glm::length(points.front()-pix) > tolerance)return false;
      return true;
    }

    bool geomInside = true;
    for(auto const&l:lines){
      if(glm::dot(l,glm::vec3(pix,1.f))+tolerance < 0.f){geomInside = false;break;}
    }
    if(!geomInside)return false;
    if(!backfaceCulling.enabled)return true;
    if(isCCW == backfaceCulling.frontFaceIsCounterClockWise)return true;
    return false;
  }
};

int cmp(glm::vec2 const&a,glm::vec2 const&b){
  if(a.x > b.x)return +1;
  if(a.x < b.x)return -1;
  if(a.y > b.y)return +1;
  if(a.y < b.y)return -1;
  return 0;
}

glm::vec3 getLine(glm::vec2 const&a,glm::vec2 const&b){
  auto s = b-a;
  auto n = glm::vec2(-s.y,s.x);
  auto p = glm::vec3(n,-glm::dot(n,a));
  //std::cerr << "    a: " << str(a) << " b: " << str(b) << " s: " << str(s) << " n: " << str(n) << " p: " << str(p) << std::endl;
  return p;
}

glm::vec2 visibleEdgeRange(glm::vec4 const&a,glm::vec4 const&b){
  //X(t) = A + t*(B-A)
  //X(t) = A + t*S
  //
  //  -Xw(t) <= Xi(t)
  //       0 <= Xi(t) + Xw(t)
  //       0 <= Ai+t*Si + Aw+t*Sw
  //       0 <= (Ai+Aw) + t*(Si+Sw)
  //
  //  -Ai-Aw <= t*(+Si+Sw)
  //       M <= t*N
  //  
  //     M/N <= t   if N  > 0
  //     M/N >= t   if N  < 0
  //       M <= 0   if N == 0
  //       all t    if N == 0 && M <= 0
  //       no  t    if N == 0 && M  > 0
  //  
  //  +Xw(t) >= Xi(t)
  //       0 >= Xi(t) - Xw(t)
  //       0 >= Ai+t*Si - Aw-t*Sw
  //       0 >= (Ai-Aw) + t*(Si-Sw)
  //  -Ai+Aw >= t*(+Si-Sw)
  //
  //  +Ai-Aw <= t*(-Si+Sw)
  //       M <= t*N
  //
  //     M/N <= t   if N  > 0
  //     M/N >= t   if N  < 0
  //       M <= 0   if N == 0
  //       all t    if N == 0 && M <= 0
  //       no  t    if N == 0 && M  > 0
  //
  
  auto s = b-a;

  auto tt = glm::vec2(0,1);
  for(int j=1;j>=-1;j-=2) // i>=-w | i<=+w
    for(int i=0;i<3;++i){
      if(j==-1 && i==2)continue; // skip far plane
      float M = j*(-a[i]) - a.w;
      float N = j*(+s[i]) + s.w;
      if(N==0){
        if(M>0)tt = glm::vec2(1,0);
      }else{
        float t = M/N;
        if(N>0)
          tt[0] = glm::max(tt[0],t);
        else
          tt[1] = glm::min(tt[1],t);
      }
    }
  return tt;
}


glm::vec4 getPointOnTriangle(glm::vec4 const&A,glm::vec4 const&B,glm::vec4 const&C,glm::vec2 const&xy){
  // X(t,l)  = C + l*(A+t*(B-A)-C)
  // X(t,l)  = C + l*((A-C)+t*(B-A))
  //
  // X(t,l)  = C + l*(N+t*O)
  //
  // X(t,l)x = xy.x * X(t,l)w
  // X(t,l)y = xy.y * X(t,l)w
  //
  // Cx + l*(Nx+tOx) = xy.x * (Cw + l*(Nw+tOw)
  // Cy + l*(Ny+tOy) = xy.y * (Cw + l*(Nw+tOw)
  //
  // l*(Nx+tOx - xy.x*(Nw+tOw)) = xy.x*Cw-Cx
  // l*(Ny+tOy - xy.y*(Nw+tOw)) = xy.y*Cw-Cy
  //
  // l*((Nx-xy.x*Nw) + t*(Ox - xy.x*Ow)) = (xy.x*Cw-Cx)
  // l*((Ny-xy.y*Nw) + t*(Oy - xy.y*Ow)) = (xy.y*Cw-Cy)
  //
  // l*(Kx+t*Lx) = Mx
  // l*(Ky+t*Ly) = My
  //
  // l = Mx / (Kx+t*Lx)
  // l = My / (Ky+t*Ly)
  //
  // Mx*(Ky+t*Ly) = My*(Kx+t*Lx)
  // Mx*Ky+t*Ly*Mx = My*Kx+t*Lx*My
  //
  // (Mx*Ky-My*Kx) = t*(Lx*My-Ly*Mx)
  // 
  // t = (Mx*Ky-My*Kx) / (Lx*My-Ly*Mx)
  // t = I / J
  //
  // if J == 0 -> xy-C is parallel with B-A
  float Nx = A.x-C.x;
  float Ny = A.y-C.y;
  float Nw = A.w-C.w;

  float Ox = B.x-A.x;
  float Oy = B.y-A.y;
  float Ow = B.w-A.w;

  float Kx = Nx-xy.x*Nw;
  float Ky = Ny-xy.y*Nw;

  float Lx = Ox-xy.x*Ow;
  float Ly = Oy-xy.y*Ow;
  
  float Mx = xy.x*C.w-C.x;
  float My = xy.y*C.w-C.y;

  auto I = (Mx*Ky-My*Kx);
  auto J = (Lx*My-Ly*Mx);

  if(J==0){
    return glm::vec4(0);
  }

  float t = I / J;

  float W = Kx+t*Lx;
  if(W==0){
    if(Mx==0 && My==0)return C;
    return glm::vec4(0);
  }

  float l = Mx / W;

  //std::cerr << "t: " << t << " l: " << l << std::endl;

  auto X = C + l*(A+t*(B-A)-C);
  if(t>=0 && t <= 1 && l >= 0 && l <= 1 && X.z >= -X.w){
    return X;
  }
  return glm::vec4(0);
}

/**
 * @brief Computes CCW in clip space
 *
 * @return true is ABC is CCW
 */
bool isCCW(glm::vec4 const&A,glm::vec4 const&B,glm::vec4 const&C){
  auto s = glm::vec2(B) - glm::vec2(A);
  auto n = glm::vec2(-s.y,s.x);
  return glm::dot(n,glm::vec2(C-A))>=0;
}

// ab forms an oriented edge a -> b
// c is tested if it lies inside positive space
bool isPointInPositiveSpaceRobust(glm::vec2 const&a,glm::vec2 const&b,glm::vec2 const&c){
  if(equalVec2(a,c,0.0001f))return true;
  if(equalVec2(b,c,0.0001f))return true;
  if(cmp(b,a)>0){
    if(cmp(c,b)>0){// a<b<c
      auto line = getLine(a,c);
      return -glm::dot(line,glm::vec3(b,1.f))>0.;
    }else{
      if(cmp(c,a)>0){// a<c<b
        auto line = getLine(a,b);
        return glm::dot(line,glm::vec3(c,1.f))>0.;
      }else{// c<a<b
        auto line = getLine(c,b);
        return -glm::dot(line,glm::vec3(a,1.f))>0.;
      }
    }
  }else{
    if(cmp(c,a)>0){// b<a<c
      auto line = getLine(b,c);
      return glm::dot(line,glm::vec3(a,1.f))>0.;
    }else{
      if(cmp(c,b)>0){// b<c<a
        auto line = getLine(b,a);
        return -glm::dot(line,glm::vec3(c,1.f))>0.;
      }else{// c<b<a
        auto line = getLine(c,a);
        return glm::dot(line,glm::vec3(b,1.f))>0.;
      }
    }
  }
  return false;
}

void insertPointIntoConvexHull(std::vector<glm::vec2>&res,glm::vec2 const&p){
  if(res.size()==0){res.push_back(p);return;}
  if(res.size()==1){if(!equalVec2(res.back(),p,0.001f))res.push_back(p);return;}
  std::vector<glm::vec2>npts;
  bool wasInserted = false;
  for(size_t i=0;i<res.size();++i){
    auto a=res[ i              ];
    auto b=res[(i+1)%res.size()];
    if(isPointInPositiveSpaceRobust(a,b,p)){
      npts.push_back(a);
    }else{
      if(!wasInserted){
        npts.push_back(a);
        npts.push_back(p);
        wasInserted = true;
      }
    }
  }
  if(wasInserted)res = npts;
}

std::vector<glm::vec2>getConvexHullVertices(std::vector<glm::vec2>const&input){
  if(input.size()<3)return input;
  std::vector<glm::vec2>res;
  for(auto const&p:input)
    insertPointIntoConvexHull(res,p);
  return res;
}

ConvexHull computeConvexHull(Packet const&exp){
  ConvexHull res;

  //std::cerr << "compute ConvexHull" << std::endl;
  if(exp.vertices.size() != 3)return res;

  res.backfaceCulling = exp.memAtStart.mem.backfaceCulling;

  glm::vec4 v[3]={
    exp.vertices.at(0).out.gl_Position,
    exp.vertices.at(1).out.gl_Position,
    exp.vertices.at(2).out.gl_Position,
  };
  res.isCCW = isCCW(v[0],v[1],v[2]);

  if(!res.isCCW){auto z = v[1];v[1] = v[2];v[2] = z;}

  std::vector<glm::vec2>all2DPoints;
  glm::vec2 corners[] = {
    glm::vec2(-1,-1),
    glm::vec2(+1,-1),
    glm::vec2(+1,+1),
    glm::vec2(-1,+1),
  };
  for(int i=0;i<4;++i){
    //std::cerr << "WINDOW POINTS: " << str(corners[i]) << std::endl;
    auto np = getPointOnTriangle(v[0],v[1],v[2],corners[i]);
    if(np == glm::vec4(0))continue;
    //std::cerr << "np: " << str(np) << std::endl;
    all2DPoints.push_back(glm::vec2(np) / np.w);
  }

  //std::cerr << "all2DPoints: " << std::endl;
  //for(auto const&p:all2DPoints)
  //  std::cerr << "a: " << str(p) << std::endl;

  for(int i=0;i<3;++i){
    auto a = v[i      ];
    auto b = v[(i+1)%3];
    auto tt = visibleEdgeRange(a,b);
    if(tt[0]>tt[1])continue;
    for(int j=0;j<2;++j){
      auto np = a+tt[j]*(b-a);
      all2DPoints.push_back(glm::vec2(np) / np.w);
    }
  }

  //std::cerr << "all2DPoints: " << std::endl;
  //for(auto const&p:all2DPoints)
  //  std::cerr << "a: " << str(p) << std::endl;

  if(all2DPoints.size() < 3)
    res.points = all2DPoints;
  else
    res.points = getConvexHullVertices(all2DPoints);


  auto const&mem = exp.memAtStart.mem;
  auto const&fbo = mem.framebuffers[mem.activatedFramebuffer];
  for(auto&p:res.points){
    p *= .5f;
    p += .5f;
    p *= glm::vec2(fbo.width ,fbo.height) ;
  }

  auto&pts = res.points;
  auto&lns = res.lines;
  for(size_t i=0;i<pts.size();++i){
    auto const&a = pts[i];
    auto const&b = pts[(i+1)%pts.size()];
    res.perimetr += glm::distance(a,b);
    lns.push_back(getLine(a,b));
  }

  res.backfaceCulling = exp.memAtStart.mem.backfaceCulling;

  return res;
}

struct GPUProcessDifference{
  GPUProcessDifference(){}
  virtual ~GPUProcessDifference(){}
  virtual std::string toStr()const{
    std::stringstream ss;
    ss << "Message not implemented!" << std::endl;
    return ss.str();
  }
};

struct DifferentNofPackets: public GPUProcessDifference{
  DifferentNofPackets(size_t e,size_t s):exp(e),stu(s){}
  size_t exp = 0;
  size_t stu = 0;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Počet sestavených primitiv není správný!" << std::endl;
    ss << "Referenční řešení sestavilo: " << exp << " primitiva" << std::endl;
    ss << "Vaše řešení sestavilo      : " << stu << " primitiva" << std::endl;
    return ss.str();
  }
};

struct ExtraPacket: public GPUProcessDifference{
  ExtraPacket(
      size_t i,
      std::shared_ptr<Packet>const&s):i(i),stu(s){}
  size_t i = 0;
  std::shared_ptr<Packet>stu;
  std::string toStr()const override{
    std::stringstream ss;
    auto const&vs=*stu;
    ss << "Sestavili jste moc primitiv (pouštěli jste příliš mnohokrát vertex shader)!" << std::endl;
    ss << "Primitivum číslo: " << i << " jste zpracovali programem: " << vs.memAtStart.mem.activatedProgram << std::endl;
    ss << "A obsahuje: " << vs.vertices.size() << " vertexů" << std::endl;
    if(vs.allFragments.empty())return ss.str();
    ss << "A potom jste navíc spustili fragment shader: " << vs.allFragments.size() << "x" << std::endl;
    return ss.str();
  }
};

struct MissingPacket: public GPUProcessDifference{
  MissingPacket(
      size_t i,
      std::shared_ptr<Packet>const&e):i(i),exp(e){}
  size_t i = 0;
  std::shared_ptr<Packet>exp;
  std::string toStr()const override{
    std::stringstream ss;
    auto const&ee=*exp;
    ss << "Sestavili jste moc málo primitiv (pouštěli jste málokrát vertex shader!" << std::endl;
    ss << "Referenční řešení vytvořilo primitivum s číslem:" << i;
    ss << ", které jste vy nevytvořili, a zpracovalo jej programem: "<< ee.memAtStart.mem.activatedProgram << std::endl;
    if(ee.allFragments.empty())return ss.str();
    ss << "A při tom zavolalo ještě fragment shader: " << ee.allFragments.size() << "x" << std::endl;
    return ss.str();
  }
};

struct DifferentVertexPacketNofVertices: public GPUProcessDifference{
  DifferentVertexPacketNofVertices(Packet const&e,Packet const&s):exp(e),stu(s){}
  Packet exp;
  Packet stu;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Spustili jste vertex shader nesprávný počet krát!" << std::endl;
    ss << "Spustili jste jej    : " << stu.vertices.size() << "x" << std::endl;
    ss << "Měli jste jej spustit: " << exp.vertices.size() << "x" << std::endl;
    return ss.str();
  }
};


struct MissingVertex: public GPUProcessDifference{
  MissingVertex(VertexShaderData const&m,size_t i):missingVertex(m),i(i){}
  VertexShaderData missingVertex;
  size_t i=0;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Vertex Shader jste měli pustit vícekrát, neprošel jím vertex s pořadovým čáslem: "<<i<<"." << std::endl;
    ss << str(missingVertex.in ) << std::endl;
    ss << str(missingVertex.out) << std::endl;
    return ss.str();
  }
};

struct ExtraVertex: public GPUProcessDifference{
  ExtraVertex(VertexShaderData const&m,size_t i):extraVertex(m),i(i){}
  VertexShaderData extraVertex   ;
  size_t      i           = 0;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Vertex Shader jste spustili vícekrát, než jste měli, vertex navíc s pořadovým čáslem: "<<i<<"." << std::endl;
    ss << str(extraVertex.in ) << std::endl;
    ss << str(extraVertex.out) << std::endl;
    return ss.str();
  }
};

struct DifferentVS_Uniforms: public GPUProcessDifference{
  DifferentVS_Uniforms(Uniform const* e,Uniform const* s,size_t i):exp(e),stu(s),i(i){}
  Uniform const* exp = nullptr;
  Uniform const* stu = nullptr;
  size_t           i = 0      ;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Váš vertex s číslem: " << i << " má špatnou tabulku uniformů!" << std::endl;
    ss << "vaše    ShaderInterface::uniforms = " << stu << std::endl;
    ss << "správné ShaderInterface::uniforms = " << exp << std::endl;
    ss << "Mělo by to ukazovat na tabulku uniformů - GPUMemory::uniforms" << std::endl;
    return ss.str();
  }
};


struct DifferentVS_Textures: public GPUProcessDifference{
  DifferentVS_Textures(Texture const* e,Texture const* s,size_t i):exp(e),stu(s),i(i){}
  Texture const* exp    ;
  Texture const* stu    ;
  size_t           i = 0;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Váš vertex s číslem: " << i << " má špatnou tabulku textur!" << std::endl;
    ss << "vaše    ShaderInterface::textures = " << stu << std::endl;
    ss << "správné ShaderInterface::textures = " << exp << std::endl;
    ss << "Mělo by to ukazovat na tabulku textur - GPUMemory::textures" << std::endl;
    return ss.str();
  }
};

struct DifferentVS_gl_DrawID: public GPUProcessDifference{
  DifferentVS_gl_DrawID(uint32_t e,uint32_t s,size_t i):exp(e),stu(s),i(i){}
  uint32_t exp;
  uint32_t stu;
  size_t      i = 0;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Váš vertex s číslem: " << i << " má špatné gl_DrawID!" << std::endl;
    ss << "vaše    ShaderInterface::gl_DrawID = " << stu << std::endl;
    ss << "správné ShaderInterface::gl_DrawID = " << exp << std::endl;
    ss << "Správné ShaderInterface::gl_DrawID by mělo obsahovat pořadové číslo vykreslovacího příkazu - GPUMemory::gl_DrawID" << std::endl;
    return ss.str();
  }
};


struct DifferentGLVertexID: public GPUProcessDifference{
  DifferentGLVertexID(InVertex const&e,InVertex const&s,size_t i):exp(e),stu(s),i(i){}
  InVertex exp    ;
  InVertex stu    ;
  size_t   i   = 0;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Váš vertex s číslem: " << i << " má špatné pořadové číslo gl_VertexID!" << std::endl;
    ss << "vaše    gl_VertexID = " << stu.gl_VertexID << std::endl;
    ss << "správné gl_VertexID = " << exp.gl_VertexID << std::endl;
    return ss.str();
  }
};

struct DifferentVertexInputAttrib: public GPUProcessDifference{
  DifferentVertexInputAttrib(
      Attrib const&e,
      Attrib const&s,
      size_t i,
      size_t a):exp(e),stu(s),i(i),att(a){}
  Attrib exp    ;
  Attrib stu    ;
  size_t i   = 0;
  size_t att = 0;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Váš vertex s číslem: " << i << " má špatný vstupní atribut: " << att << std::endl;
    ss << "váš attribut     InVertex::attributes["<<att<<"].v4 = " << str(stu.v4) << std::endl;
    ss << "správný attribut InVertex::attributes["<<att<<"].v4 = " << str(exp.v4) << std::endl;
    return ss.str();
  }
};

struct DifferentGLPosition: public GPUProcessDifference{
  DifferentGLPosition(glm::vec4 const&e,glm::vec4 const&s,size_t i):exp(e),stu(s),i(i){}
  glm::vec4 exp    ;
  glm::vec4 stu    ;
  size_t    i   = 0;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Váš vertex s číslem: " << i << " má špatnou výstupní pozici gl_Position." << std::endl;
    ss << "vaše    gl_Position = " << str(stu) << std::endl;
    ss << "správné gl_Position = " << str(exp) << std::endl;
    return ss.str();
  }
};

struct DifferentVertexOutputAttrib: public GPUProcessDifference{
  DifferentVertexOutputAttrib(
      Attrib const&e,
      Attrib const&s,
      size_t i,
      size_t a):exp(e),stu(s),i(i),att(a){}
  Attrib exp    ;
  Attrib stu    ;
  size_t i   = 0;
  size_t att = 0;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Váš vertex s číslem: " << i << " má špatný výstupní atribut: " << att << std::endl;
    ss << "vaše    OutVertex::attribute["<<att<<"].v4 = " << str(stu) << std::endl;
    ss << "správné OutVertex::attribute["<<att<<"].v4 = " << str(exp) << std::endl;
    return ss.str();
  }
};



struct DifferentFS_Uniforms: public GPUProcessDifference{
  DifferentFS_Uniforms(Uniform const* e,Uniform const* s):exp(e),stu(s){}
  Uniform const* exp = nullptr;
  Uniform const* stu = nullptr;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Váš fragment má špatnou tabulku uniformů!" << std::endl;
    ss << "vaše    ShaderInterface::uniforms = " << stu << std::endl;
    ss << "správné ShaderInterface::uniforms = " << exp << std::endl;
    ss << "Mělo by to ukazovat na tabulku uniformů - GPUMemory::uniforms" << std::endl;
    return ss.str();
  }
};


struct DifferentFS_Textures: public GPUProcessDifference{
  DifferentFS_Textures(Texture const* e,Texture const* s):exp(e),stu(s){}
  Texture const* exp    ;
  Texture const* stu    ;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Váš fragment má špatnou tabulku textur!" << std::endl;
    ss << "vaše    ShaderInterface::textures = " << stu << std::endl;
    ss << "správné ShaderInterface::textures = " << exp << std::endl;
    ss << "Mělo by to ukazovat na tabulku textur - GPUMemory::textures" << std::endl;
    return ss.str();
  }
};

struct DifferentFS_gl_DrawID: public GPUProcessDifference{
  DifferentFS_gl_DrawID(uint32_t e,uint32_t s):exp(e),stu(s){}
  uint32_t exp;
  uint32_t stu;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Váš fragment má špatné gl_DrawID!" << std::endl;
    ss << "vaše    ShaderInterface::gl_DrawID = " << stu << std::endl;
    ss << "správné ShaderInterface::gl_DrawID = " << exp << std::endl;
    ss << "Správné ShaderInterface::gl_DrawID by mělo obsahovat pořadové číslo vykreslovacího příkazu - GPUMemory::gl_DrawID" << std::endl;
    return ss.str();
  }
};



struct DifferentNofFragments: public GPUProcessDifference{
  DifferentNofFragments(size_t e,size_t s,size_t tol):exp(e),stu(s),tolerance(tol){}
  size_t exp       = 0;
  size_t stu       = 0;
  size_t tolerance = 0;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Počet vyrasterizovaných fragmentů se liší!" << std::endl;
    ss << "váš počet    : " << stu << std::endl;
    ss << "Správný počet: " << exp << std::endl;
    ss << "tolerance    : " << tolerance << std::endl;
    return ss.str();
  }
};

struct OutsideFragments: public GPUProcessDifference{
  OutsideFragments(std::vector<FragmentShaderData>const&io):inOut(io){}
  std::vector<FragmentShaderData>inOut;
  std::string toStr()const override{
    std::stringstream ss;
    size_t const n = 10; // how many bad fragments should be printed
    ss << "Vyrasterizovali jste fragmenty na špatném místě!" << std::endl;
    ss << "Tyto fragmenty jsou mimo trojúhelník (zobrazeno jen prvních " << n << " špatných fragmentů): " << std::endl;
    size_t counter = 0;
    for(auto const&io:inOut){
      if(counter >= n)break;
      counter++;
      ss << "gl_FragCoord.xy = " << str(glm::vec2(io.in.gl_FragCoord)) << std::endl;
    }
    return ss.str();
  }
};

struct DifferentFragCoordXY: public GPUProcessDifference{
  DifferentFragCoordXY(std::vector<InFragment>const&e,std::vector<InFragment>const&s):exp(e),stu(s){}
  std::vector<InFragment>exp;
  std::vector<InFragment>stu;
  std::string toStr()const override{
    std::stringstream ss;
    size_t const n = 10; // how many bad fragments should be printed
    ss << "Vaše vyrasterizované fragmenty mají špatné xy pozice!" << std::endl;
    ss << "Tyto fragment jsou na špatném místě v trojúhelníku (zobrazeno jen prvních " << n << " špatných fragmentů): " << std::endl;
    size_t counter = 0;
    for(size_t i=0;i<exp.size();++i){
      if(counter >= n)break;
      counter++;
      ss << "reference.gl_FragCoord.xy = " << str(glm::vec2(exp[i].gl_FragCoord)) << "vaše.gl_FragCoord.xy = " << str(glm::vec2(stu[i].gl_FragCoord)) << std::endl;
    }
    return ss.str();
  }
};

struct DifferentFragCoordZ: public GPUProcessDifference{
  DifferentFragCoordZ(std::vector<InFragment>const&e,std::vector<InFragment>const&s):exp(e),stu(s){}
  std::vector<InFragment>exp;
  std::vector<InFragment>stu;
  std::string toStr()const override{
    std::stringstream ss;
    size_t const n = 10; // how many bad fragments should be printed
    ss << "Vaše vyrasterizované fragmenty mají špatnout z pozice!" << std::endl;
    ss << "Tyto fragment jsou špatně (zobrazeno jen prvním " << n << " špatných fragmentů): " << std::endl;
    size_t counter = 0;
    for(size_t i=0;i<exp.size();++i){
      if(counter >= n)break;
      counter++;
      ss << "reference.gl_FragCoord.xyz = " << str(glm::vec3(exp[i].gl_FragCoord)) << "vaše.gl_FragCoord.xyz = " << str(glm::vec3(stu[i].gl_FragCoord)) << std::endl;
    }
    return ss.str();
  }
};

struct DifferentFragmentInputAttrib: public GPUProcessDifference{
  DifferentFragmentInputAttrib(
      std::vector<InFragment>const&e,
      std::vector<InFragment>const&s,
      std::vector<uint32_t  >const&a):exp(e),stu(s),att(a){}
  std::vector<InFragment>exp;
  std::vector<InFragment>stu;
  std::vector<uint32_t  >att;
  std::string toStr()const override{
    std::stringstream ss;
    size_t const n = 10; // how many bad fragments should be printed
    ss << "Vaše vyrasterizované fragmenty mají špatné vstupní attributy." << std::endl;
    ss << "Tyto fragmenty jsou špatně (zobrazeno jen prvních " << n << " špatných fragmetů): " << std::endl;
    size_t counter = 0;
    for(size_t i=0;i<exp.size();++i){
      if(counter >= n)break;
      counter++;
      ss << "fragment na pozici: " << str(glm::vec2(exp[i].gl_FragCoord));
      ss << " má attribute["<<att[i]<<"].v4 = " << str(stu[i].attributes[att[i]].v4) << " ale má být = ";
      ss << str(exp[i].attributes[att[i]].v4) << std::endl;
    }
    return ss.str();
  }
};


struct DifferentGLFragColor: public GPUProcessDifference{
  DifferentGLFragColor(
      std::vector<glm::vec4>const&e,
      std::vector<glm::vec4>const&s,
      std::vector<glm::vec4>const&p):exp(e),stu(s),pos(p){}
  std::vector<glm::vec4>exp;
  std::vector<glm::vec4>stu;
  std::vector<glm::vec4>pos;
  std::string toStr()const override{
    std::stringstream ss;
    size_t const n = 10; // how many bad fragments should be printed
    ss << "Vaše vyrasterizované fragmenty mají špatnou výstupní barvu." << std::endl;
    ss << "Tyto fragmenty jsou špatně (zobrazeno jen prvních " << n << " špatných fragmentů): " << std::endl;
    size_t counter = 0;
    for(size_t i=0;i<exp.size();++i){
      if(counter >= n)break;
      counter++;
      ss << "fragment na pozici: " << str(pos[i]);
      ss << " má barvu gl_FragColor = " << str(stu[i]) << " ale měl mít barvu = " << str(exp[i]) << std::endl;
    }
    return ss.str();
  }
};

struct DifferentDiscard: public GPUProcessDifference{
  DifferentDiscard(
      std::vector<bool     >const&e,
      std::vector<bool     >const&s,
      std::vector<glm::vec4>const&p):exp(e),stu(s),pos(p){}
  std::vector<bool     >exp;
  std::vector<bool     >stu;
  std::vector<glm::vec4>pos;
  std::string toStr()const override{
    std::stringstream ss;
    size_t const n = 10; // how many bad fragments should be printed
    ss << "Vaše vyrasterizované fragmenty mají špatnou hodnotu discard." << std::endl;
    ss << "Tyto fragmenty jsou špatně (zobrazeno jen prvních " << n << " špatných fragmentů): " << std::endl;
    size_t counter = 0;
    for(size_t i=0;i<exp.size();++i){
      if(counter >= n)break;
      counter++;
      ss << "fragment na pozici: " << str(pos[i]);
      ss << " má discard = " << str(stu[i]) << " ale měl mít discard = " << str(exp[i]) << std::endl;
    }
    return ss.str();
  }
};

void getShaderInterfaceDifference(
    std::vector<std::shared_ptr<GPUProcessDifference>>&res,
    Packet const&stu,
    VertexShaderData const&exp_data,
    VertexShaderData const&stu_data,
    size_t i){
  auto const&exp_si = exp_data.si;
  auto const&stu_si = stu_data.si;
  if(stu_si.uniforms != stu.memWithRightPointers->uniforms)
    res.push_back(std::make_shared<DifferentVS_Uniforms>(stu.memWithRightPointers->uniforms,stu_si.uniforms,i));
  if(stu_si.textures != stu.memWithRightPointers->textures)
    res.push_back(std::make_shared<DifferentVS_Textures>(stu.memWithRightPointers->textures,stu_si.textures,i));
  if(exp_si.gl_DrawID != stu_si.gl_DrawID)
    res.push_back(std::make_shared<DifferentVS_gl_DrawID>(exp_si.gl_DrawID,stu_si.gl_DrawID,i));
}

void getInVertexDifference(
    std::vector<std::shared_ptr<GPUProcessDifference>>&res,
    VertexShaderData const&exp_data,
    VertexShaderData const&stu_data,
    size_t i){
  auto const&exp_in = exp_data.in;
  auto const&stu_in = stu_data.in;
  if(exp_in.gl_VertexID != stu_in.gl_VertexID)
    res.push_back(std::make_shared<DifferentGLVertexID>(exp_in,stu_in,i));
  for(uint32_t j=0;j<maxAttribs;++j)
    if(exp_in.attributes[j].v4 != stu_in.attributes[j].v4)
      res.push_back(std::make_shared<DifferentVertexInputAttrib>(exp_in.attributes[j],stu_in.attributes[j],i,j));
}

void getOutVertexDifference(
    std::vector<std::shared_ptr<GPUProcessDifference>>&res,
    VertexShaderData const&exp_data,
    VertexShaderData const&stu_data,
    size_t i){
  auto const&exp_out = exp_data.out;
  auto const&stu_out = stu_data.out;
  if(!equalVec4(exp_out.gl_Position,stu_out.gl_Position))
    res.push_back(std::make_shared<DifferentGLPosition>(exp_out.gl_Position,stu_out.gl_Position,i));
  for(uint32_t j=0;j<maxAttribs;++j)
    if(!equalVec4(exp_out.attributes[j].v4,stu_out.attributes[j].v4))
      res.push_back(std::make_shared<DifferentVertexOutputAttrib>(exp_out.attributes[j],stu_out.attributes[j],i,j));
}


void getStreamVertexPacketDifference(std::vector<std::shared_ptr<GPUProcessDifference>>&res,Packet const&exp,Packet const&stu){
  if(exp.vertices.size() != stu.vertices.size()){
    res.push_back(std::make_shared<DifferentVertexPacketNofVertices>(exp,stu));
  }

  auto n = glm::max(exp.vertices.size(),stu.vertices.size());

  for(size_t i=0;i<n;++i){
    if(i>=stu.vertices.size()){//missing vertices
      res.push_back(std::make_shared<MissingVertex>(exp.vertices.at(i),i));
    }else if(i>=exp.vertices.size()){//extra vertices
      res.push_back(std::make_shared<ExtraVertex>(stu.vertices.at(i),i));
    }else{//both vertices exist
      auto const&exp_data = exp.vertices.at(i);
      auto const&stu_data = stu.vertices.at(i);

      getShaderInterfaceDifference(res,stu,exp_data,stu_data,i);
      getInVertexDifference       (res,    exp_data,stu_data,i);
      getOutVertexDifference      (res,    exp_data,stu_data,i);

    }
  }
}

void getStreamFragmentPacketDifference(
    std::vector<std::shared_ptr<GPUProcessDifference>>&res,
    Packet               const&exp,
    Packet               const&stu,
    float                perimeterPercentageTolerance = 0.1f,
    float                eps = 0.1f){

  auto hull = computeConvexHull(exp);

  //std::cerr << "START =========== " << std::endl;
  //for(auto const&p:hull.points)
  //  std::cerr << "P: " << str(p) << std::endl;
  //for(auto const&l:hull.lines)
  //  std::cerr << "L: " << str(l) << std::endl;
  //std::cerr << "END   =========== " << std::endl;

  auto nofPixelsOnBorderPlusTolerance = (size_t)(hull.perimetr * (1.f + perimeterPercentageTolerance));

  if(!equalCounts(exp.allFragments.size(),stu.allFragments.size(),nofPixelsOnBorderPlusTolerance)){
    res.push_back(std::make_shared<DifferentNofFragments>(exp.allFragments.size(),stu.allFragments.size(),nofPixelsOnBorderPlusTolerance));
  }

  std::vector<FragmentShaderData>outsideFragments;

  std::vector<InFragment>exp_fragCoordXY;
  std::vector<InFragment>stu_fragCoordXY;

  std::vector<InFragment>exp_InAttrib;
  std::vector<InFragment>stu_InAttrib;
  std::vector<uint32_t  >atts;

  std::vector<InFragment>exp_fragCoordZ;
  std::vector<InFragment>stu_fragCoordZ;

  std::vector<glm::vec4>exp_colors;
  std::vector<glm::vec4>stu_colors;
  std::vector<glm::vec4>colorsPos;

  std::vector<bool     >exp_discards;
  std::vector<bool     >stu_discards;
  std::vector<glm::vec4>discardPos;

  //for(auto const&e:exp.allFragments){
  //  if(!hull.isInside(glm::vec2(e.in.gl_FragCoord))){
  //    std::cerr << "--------------exp out!: " << str(e.in.gl_FragCoord) << std::endl;
  //  
  //  }
  //}
  
  for(auto const&i:stu.allFragments){
    if(i.si.uniforms != stu.memWithRightPointers->uniforms){
      res.push_back(std::make_shared<DifferentFS_Uniforms>(stu.memWithRightPointers->uniforms,i.si.uniforms));
      break;
    }
  }

  for(auto const&i:stu.allFragments){
    if(i.si.textures != stu.memWithRightPointers->textures){
      res.push_back(std::make_shared<DifferentFS_Textures>(stu.memWithRightPointers->textures,i.si.textures));
      break;
    }
  }


  bool gl_DrawIDInserted = false;

  for(auto const&i:stu.allFragments){
    auto       key = glm::uvec2(i.in.gl_FragCoord);
    auto const&stu_data  = i;
    if(!hull.isInside(glm::vec2(stu_data.in.gl_FragCoord))){
      outsideFragments.push_back(stu_data);
    }else{
      //skip fragments there were captured due to tolerance
      //they do not have their expected counterpart
      if(exp.fragments.count(key) == 0)continue;



      auto const&exp_data = exp.fragments.at(key);

      auto const&exp_si = exp_data.si;
      auto const&stu_si = stu_data.si;
      if(exp_si.gl_DrawID != stu_si.gl_DrawID){
        if(!gl_DrawIDInserted){
          res.push_back(std::make_shared<DifferentFS_gl_DrawID>(exp_si.gl_DrawID,stu_si.gl_DrawID));
          gl_DrawIDInserted = true;
        }
      }

      auto const&exp_i = exp_data.in;
      auto const&stu_i = stu_data.in;
      if(!equalVec2(exp_i.gl_FragCoord,stu_i.gl_FragCoord,eps)){
        exp_fragCoordXY.push_back(exp_i);
        stu_fragCoordXY.push_back(stu_i);
      }
      if(!equalFloats(exp_i.gl_FragCoord.z,stu_i.gl_FragCoord.z,eps)){
        exp_fragCoordZ.push_back(exp_i);
        stu_fragCoordZ.push_back(stu_i);
      }
      for(uint32_t j=0;j<maxAttribs;++j)
        if(!equalVec4(exp_i.attributes[j].v4,stu_i.attributes[j].v4,eps)){
          exp_InAttrib.push_back(exp_i);
          stu_InAttrib.push_back(stu_i);
          atts        .push_back(j    );
        }
      auto const&exp_o = exp_data.out;
      auto const&stu_o = stu_data.out;
      if(!equalVec4(exp_o.gl_FragColor,stu_o.gl_FragColor,eps)){
        exp_colors.push_back(exp_o.gl_FragColor);
        stu_colors.push_back(stu_o.gl_FragColor);
        colorsPos .push_back(exp_i.gl_FragCoord);
      }
      if(exp_o.discard != stu_o.discard){
        exp_discards.push_back(exp_o.discard);
        stu_discards.push_back(stu_o.discard);
        discardPos  .push_back(exp_i.gl_FragCoord);
      }
    }
  }

  if(!outsideFragments.empty())res.push_back(std::make_shared<OutsideFragments            >(outsideFragments                           ));
  if(!exp_fragCoordXY .empty())res.push_back(std::make_shared<DifferentFragCoordXY        >(exp_fragCoordXY ,stu_fragCoordXY           ));
  if(!exp_fragCoordZ  .empty())res.push_back(std::make_shared<DifferentFragCoordZ         >(exp_fragCoordZ  ,stu_fragCoordZ            ));
  if(!exp_InAttrib    .empty())res.push_back(std::make_shared<DifferentFragmentInputAttrib>(exp_InAttrib    ,stu_InAttrib   ,atts      ));
  if(!exp_colors      .empty())res.push_back(std::make_shared<DifferentGLFragColor        >(exp_colors      ,stu_colors     ,colorsPos ));
  if(!exp_discards    .empty())res.push_back(std::make_shared<DifferentDiscard            >(exp_discards    ,stu_discards   ,discardPos));
}

void getStreamPacketDifference(
    std::vector<std::shared_ptr<GPUProcessDifference>>&res,
    size_t i,
    std::shared_ptr<Packet>const&exp,
    std::shared_ptr<Packet>const&stu){
  if     (!exp &&  stu){res.push_back(std::make_shared<ExtraPacket  >(i,stu));}
  else if( exp && !stu){res.push_back(std::make_shared<MissingPacket>(i,exp));}
  else{
    getStreamVertexPacketDifference  (res,*exp,*stu);
    getStreamFragmentPacketDifference(res,*exp,*stu);
  }
}

void getStreamDumpDifference(std::vector<std::shared_ptr<GPUProcessDifference>>&res,StreamDump const&exp,StreamDump const&stu){
  auto const&e=exp.gpuStream;
  auto const&s=stu.gpuStream;
  if(e.size() != s.size())res.push_back(std::make_shared<DifferentNofPackets>(e.size(),s.size()));
  auto n = glm::max(e.size(),s.size());
  for(size_t i=0;i<n;++i){
    auto ee = i<e.size()?e.at(i):nullptr;
    auto ss = i<s.size()?s.at(i):nullptr;
    getStreamPacketDifference(res,i,ee,ss);
  }
}

struct AllocatedBuffer{
  AllocatedBuffer(){}
  AllocatedBuffer(Buffer const&b){
    backing = std::vector<uint8_t>(b.size);
    buffer.data = backing.data();
    buffer.size = backing.size();
  }
  AllocatedBuffer(AllocatedBuffer const&b){
    backing = b.backing;
    buffer.data = backing.data();
    buffer.size = backing.size();
  }
  AllocatedBuffer&operator=(AllocatedBuffer const&b){
    backing = b.backing;
    buffer.data = backing.data();
    buffer.size = backing.size();
    return *this;
  }
  Buffer              buffer;
  std::vector<uint8_t>backing;
};

std::string buildPath(std::string const&var){
  std::stringstream ss;
  ss << "mem" << "." << var;
  return ss.str();
}

std::string buildPath(std::string const&var,uint32_t i){
  std::stringstream ss;
  ss << "mem" << "." << var << "[" << i << "]";
  return ss.str();
}

std::string buildPath(std::string const&sp1,std::string const&var){
  std::stringstream ss;
  ss << "mem" << "." << sp1 << "." <<var;
  return ss.str();
}

std::string buildIndex(uint32_t i){
  std::stringstream ss;
  ss << "[" << i << "]";
  return ss.str();
}

std::string buildPath(std::string const&sp1,uint32_t i,std::string const&var){
  std::stringstream ss;
  ss << "mem" << "." << sp1 << buildIndex(i) << "." <<var;
  return ss.str();
}

std::string buildPath(std::string const&sp1,uint32_t i,std::string const&var,uint32_t j){
  std::stringstream ss;
  ss << "mem" << "." << sp1 << buildIndex(i) << "." <<var << buildIndex(j);
  return ss.str();
}

std::string buildPath(std::string const&sp1,std::string const&sp2,std::string const&var){
  std::stringstream ss;
  ss << "mem" << "." << sp1 << "." << sp2 << "." << var;
  return ss.str();
}

std::string buildPath(std::string const&sp1,uint32_t i,std::string const&sp2,std::string const&var){
  std::stringstream ss;
  ss << "mem" << "." << sp1 << buildIndex(i) << "." << sp2 << "." << var;
  return ss.str();
}

std::string buildPath(std::string const&sp1,uint32_t i,std::string const&sp2,std::string const&var,uint32_t j){
  std::stringstream ss;
  ss << "mem" << "." << sp1 << buildIndex(i) << "." << sp2 << "." << var << buildIndex(j);
  return ss.str();
}

std::string buildPath(std::string const&sp1,uint32_t i,std::string const&sp2,uint32_t j,std::string const&var){
  std::stringstream ss;
  ss << "mem" << "." << sp1 << buildIndex(i) << "." << sp2 << buildIndex(j) << "." << var;
  return ss.str();
}











template<typename T>
struct MemoryDifference: GPUProcessDifference{
  MemoryDifference(T const&e,T const&s,std::string const&p):exp(e),stu(s),path(p){}
  T exp;
  T stu;
  std::string path = "";
  std::string toStr()const override{
    std::stringstream ss;
    ss << path << " = " << str(stu) << ";//vaše    " << std::endl;
    ss << path << " = " << str(exp) << ";//mělo být" << std::endl;
    return ss.str();
  }
};

#define REM(a) std::remove_extent<std::remove_pointer<a>::type>::type

#define TYPE1I(a)\
  REM(decltype(GPUMemory::a))

#define TYPE1_(a)\
  decltype(GPUMemory::a)

#define TYPE2II(a,b)\
  REM(decltype(REM(decltype(GPUMemory::a))::b))

#define TYPE2I_(a,b)\
  decltype(REM(decltype(GPUMemory::a))::b)

#define TYPE2__(a,b)\
  decltype(decltype(GPUMemory::a)::b)

#define TYPE3III(a,b,c)\
  REM(decltype(REM(decltype(REM(decltype(GPUMemory::a))::b))::c))

#define TYPE3II_(a,b,c)\
  decltype(REM(decltype(REM(decltype(GPUMemory::a))::b))::c)

#define TYPE3I__(a,b,c)\
  decltype(decltype(REM(decltype(GPUMemory::a))::b)::c)

#define TYPE3I_I(a,b,c)\
  REM(decltype(decltype(REM(decltype(GPUMemory::a))::b)::c))

#define TYPE3___(a,b,c)\
  decltype(decltype(decltype(GPUMemory::a)::b)::c)

bool isBufferDataDifferent(Buffer const&exp,Buffer const&stu){
  return std::memcmp(exp.data,stu.data,exp.size) != 0;
}

bool isImageDataDifferent(uint32_t width,uint32_t height,Image const&exp,Image const&stu,float mseThreshold = 0.01f){
  float meanSquareError = 0;
  for (uint32_t y = 0; y < height; ++y)
    for (uint32_t x = 0; x < width; ++x){
      switch(exp.format){
        case Image::Format::F32:{
          auto ecol = getColorF(exp,x,y);
          auto scol = getColorF(stu,x,y);
          for(uint32_t c=0;c<exp.channels;++c){
            float diff = glm::abs(ecol[c] - scol[c]);
            diff *= diff;
            meanSquareError += diff;
          }
        }break;
        case Image::Format::U8 :{
          auto ecol = getColorU(exp,x,y);
          auto scol = getColorU(stu,x,y);
          for(uint32_t c=0;c<exp.channels;++c){
            float diff = glm::abs((float)ecol[c] - (float)scol[c])/255.f;
            diff *= diff;
            meanSquareError += diff;
          }
        }break;
      }
    }
  meanSquareError /= (float)(width * height * exp.channels);
  return meanSquareError > mseThreshold;
}

void getImageDataDifference(
    std::vector<std::shared_ptr<GPUProcessDifference>>&res,
    uint32_t i,
    uint32_t width,uint32_t height,
    Image const&ee,
    Image const&ss,
    std::string const&sp1,
    std::string const&sp2){
  if( ee.data && !ss.data)res.push_back(std::make_shared<MemoryDifference<TYPE2I_(textures,img)>>(ee,ss,buildPath(sp1,i,sp2,"data")));
  if(!ee.data &&  ss.data)res.push_back(std::make_shared<MemoryDifference<TYPE2I_(textures,img)>>(ee,ss,buildPath(sp1,i,sp2,"data")));
  if(
      ee.channels        == ss.channels        &&
      ee.format          == ss.format          &&
      ee.pitch           == ss.pitch           &&
      ee.bytesPerPixel   == ss.bytesPerPixel   &&
      ee.channelTypes[0] == ss.channelTypes[0] &&
      ee.channelTypes[1] == ss.channelTypes[1] &&
      ee.channelTypes[2] == ss.channelTypes[2] &&
      ee.channelTypes[3] == ss.channelTypes[3] &&
      ee.data            && ss.data            ){
    if(isImageDataDifferent(width,height,ee,ss)){
      res.push_back(std::make_shared<MemoryDifference<SizedImage>>(SizedImage{ee,width,height},SizedImage{ss,width,height},buildPath(sp1,i,sp2,"data")));
    }
  }

  if(ee.bytesPerPixel != ss.bytesPerPixel)res.push_back(std::make_shared<MemoryDifference<TYPE3I__(textures,img,bytesPerPixel)>>(ee.bytesPerPixel,ss.bytesPerPixel,buildPath(sp1,i,sp2,"bytesPerPixel")));
  if(ee.channels      != ss.channels     )res.push_back(std::make_shared<MemoryDifference<TYPE3I__(textures,img,channels     )>>(ee.channels     ,ss.channels     ,buildPath(sp1,i,sp2,"channels     ")));
  if(ee.format        != ss.format       )res.push_back(std::make_shared<MemoryDifference<TYPE3I__(textures,img,format       )>>(ee.format       ,ss.format       ,buildPath(sp1,i,sp2,"format       ")));
  if(ee.pitch         != ss.pitch        )res.push_back(std::make_shared<MemoryDifference<TYPE3I__(textures,img,pitch        )>>(ee.pitch        ,ss.pitch        ,buildPath(sp1,i,sp2,"pitch        ")));

  for(int j=0;j<4;++j)
      if(ee.channelTypes[j] != ss.channelTypes[j])
        res.push_back(std::make_shared<MemoryDifference<TYPE3I_I(textures,img,channelTypes)>>(ee.channelTypes[j],ss.channelTypes[j],buildPath(sp1,i,sp2,"channelTypes",j)));
}




#define IF_MEM_DIFFERENT1(var)\
  if(e.var != s.var)\
    res.push_back(std::make_shared<MemoryDifference<TYPE1_(var)>>(e.var,s.var,buildPath(#var)))

#define IF_MEM_DIFFERENT2(sp1,var)\
  if(e.sp1.var != s.sp1.var)\
     res.push_back(std::make_shared<MemoryDifference<TYPE2__(sp1,var)>>(e.sp1.var,s.sp1.var,buildPath(#sp1,#var)))

#define IF_MEM_DIFFERENT3(sp1,sp2,var)\
  if(e.sp1.sp2.var != s.sp1.sp2.var)\
    res.push_back(std::make_shared<MemoryDifference<TYPE3___(sp1,sp2,var)>>(e.sp1.sp2.var,s.sp1.sp2.var,buildPath(#sp1,#sp2,#var)))


bool isSame(Uniform const&e,Uniform const&s){
  if(isInt(e) || isInt(s)){
    if(e.i4 == s.i4)return true;
  }else{
    if(equalMat4(e.m4,s.m4))return true;
  }
  return false;
}

void getBuffersDifference(std::vector<std::shared_ptr<GPUProcessDifference>>&res,GPUMemory const&e,GPUMemory const&s){
  for(uint32_t i=0;i<e.maxBuffers;++i){
    auto const&exp = e.buffers[i];
    auto const&stu = s.buffers[i];
    if( exp.size !=  stu.size)res.push_back(std::make_shared<MemoryDifference<TYPE2I_(buffers,size)>>(exp.size,stu.size,buildPath("buffers",i,"size")));
    if( exp.data && !stu.data)res.push_back(std::make_shared<MemoryDifference<TYPE1I (buffers     )>>(exp     ,stu     ,buildPath("buffers",i,"data")));
    if(!exp.data &&  stu.data)res.push_back(std::make_shared<MemoryDifference<TYPE1I (buffers     )>>(exp     ,stu     ,buildPath("buffers",i,"data")));
    if( 
        exp.data && stu.data &&
        exp.size == stu.size ){
      if(isBufferDataDifferent(exp,stu))
        res.push_back(std::make_shared<MemoryDifference<TYPE1I(buffers)>>(exp,stu,buildPath("buffers",i,"data")));
    }
  }
}

void getUniformsDifference(std::vector<std::shared_ptr<GPUProcessDifference>>&res,GPUMemory const&e,GPUMemory const&s){
  for(uint32_t i=0;i<e.maxUniforms;++i){
    auto const&exp = e.uniforms[i];
    auto const&stu = s.uniforms[i];
    if(!isSame(exp,stu))res.push_back(std::make_shared<MemoryDifference<TYPE1I(uniforms)>>(exp,stu,buildPath("uniforms",i)));
  }
}

#define GETDIFF(v1,v2)\
    if(ee.v2!=ss.v2)\
      res.push_back(std::make_shared<MemoryDifference<TYPE2I_(v1,v2)>>(ee.v2,ss.v2,buildPath(#v1,i,#v2)))

void getProgramsDifference(std::vector<std::shared_ptr<GPUProcessDifference>>&res,GPUMemory const&e,GPUMemory const&s){
  for(uint32_t i=0;i<e.maxPrograms;++i){
    auto const&ee = e.programs[i];
    auto const&ss = s.programs[i];
    GETDIFF(programs,vertexShader  );
    GETDIFF(programs,fragmentShader);
    for(uint32_t a=0;a<maxAttribs;++a)
      if(ee.vs2fs[a] != ss.vs2fs[a])res.push_back(std::make_shared<MemoryDifference<TYPE2II(programs,vs2fs)>>(ee.vs2fs[a],ss.vs2fs[a],buildPath("programs",i,"vs2fs",a)));
  }
}

void getVertexArraysDifference(std::vector<std::shared_ptr<GPUProcessDifference>>&res,GPUMemory const&e,GPUMemory const&s){
  for(uint32_t i=0;i<e.maxVertexArrays;++i){
    auto const&ee = e.vertexArrays[i];
    auto const&ss = s.vertexArrays[i];
    GETDIFF(vertexArrays,indexBufferID);
    GETDIFF(vertexArrays,indexOffset  );
    GETDIFF(vertexArrays,indexType    );

    for(uint32_t j=0;j<maxAttribs;++j){
      auto const&eee=ee.vertexAttrib[j];
      auto const&sss=ss.vertexAttrib[j];
      if(eee.bufferID != sss.bufferID)res.push_back(std::make_shared<MemoryDifference<TYPE3II_(vertexArrays,vertexAttrib,bufferID)>>(eee.bufferID,sss.bufferID,buildPath("vertexArrays",i,"vertexAttib",j,"bufferID")));
      if(eee.offset   != sss.offset  )res.push_back(std::make_shared<MemoryDifference<TYPE3II_(vertexArrays,vertexAttrib,stride  )>>(eee.offset  ,sss.offset  ,buildPath("vertexArrays",i,"vertexAttib",j,"offset  ")));
      if(eee.stride   != sss.stride  )res.push_back(std::make_shared<MemoryDifference<TYPE3II_(vertexArrays,vertexAttrib,offset  )>>(eee.stride  ,sss.stride  ,buildPath("vertexArrays",i,"vertexAttib",j,"stride  ")));
      if(eee.type     != sss.type    )res.push_back(std::make_shared<MemoryDifference<TYPE3II_(vertexArrays,vertexAttrib,type    )>>(eee.type    ,sss.type    ,buildPath("vertexArrays",i,"vertexAttib",j,"type    ")));
    }
  }
}


void getTexturesDifference(std::vector<std::shared_ptr<GPUProcessDifference>>&res,GPUMemory const&e,GPUMemory const&s){
  for(uint32_t i=0;i<e.maxTextures;++i){
    auto const&ee = e.textures[i];
    auto const&ss = s.textures[i];

    GETDIFF(textures,width );
    GETDIFF(textures,height);

    if(ee.width == ss.width && ee.height == ss.height)
      getImageDataDifference(res,i,ee.width,ee.height,ee.img,ss.img,"textures","img");
  }
}

void getFramebuffersDifference(std::vector<std::shared_ptr<GPUProcessDifference>>&res,GPUMemory const&e,GPUMemory const&s){
  for(uint32_t i=0;i<e.maxFramebuffers;++i){
    auto const&ee = e.framebuffers[i];
    auto const&ss = s.framebuffers[i];

    GETDIFF(framebuffers,width    );
    GETDIFF(framebuffers,height   );
    GETDIFF(framebuffers,yReversed);

    if(ee.width == ss.width && ee.height == ss.height){
      getImageDataDifference(res,i,ee.width,ee.height,ee.color  ,ss.color  ,"framebuffers","color"  );
      getImageDataDifference(res,i,ee.width,ee.height,ee.depth  ,ss.depth  ,"framebuffers","depth"  );
      getImageDataDifference(res,i,ee.width,ee.height,ee.stencil,ss.stencil,"framebuffers","stencil");
    }
  }
}

#undef GETDIFF

void getMemDifference(std::vector<std::shared_ptr<GPUProcessDifference>>&res,GPUMemory const&e,GPUMemory const&s){

  IF_MEM_DIFFERENT1(activatedFramebuffer                                   );
  IF_MEM_DIFFERENT1(activatedProgram                                       );
  IF_MEM_DIFFERENT1(activatedVertexArray                                   );
  IF_MEM_DIFFERENT1(gl_DrawID                                              );
  IF_MEM_DIFFERENT2(blockWrites         ,color                             );
  IF_MEM_DIFFERENT2(blockWrites         ,depth                             );
  IF_MEM_DIFFERENT2(blockWrites         ,stencil                           );
  IF_MEM_DIFFERENT2(stencilSettings     ,enabled                           );
  IF_MEM_DIFFERENT2(stencilSettings     ,func                              );
  IF_MEM_DIFFERENT2(stencilSettings     ,refValue                          );
  IF_MEM_DIFFERENT3(stencilSettings     ,backOps                    ,sfail );
  IF_MEM_DIFFERENT3(stencilSettings     ,backOps                    ,dpfail);
  IF_MEM_DIFFERENT3(stencilSettings     ,backOps                    ,dppass);
  IF_MEM_DIFFERENT3(stencilSettings     ,frontOps                   ,sfail );
  IF_MEM_DIFFERENT3(stencilSettings     ,frontOps                   ,dpfail);
  IF_MEM_DIFFERENT3(stencilSettings     ,frontOps                   ,dppass);
  IF_MEM_DIFFERENT2(backfaceCulling     ,enabled                           );
  IF_MEM_DIFFERENT2(backfaceCulling     ,frontFaceIsCounterClockWise       );

  getBuffersDifference     (res,e,s);
  getUniformsDifference    (res,e,s);
  getProgramsDifference    (res,e,s);
  getVertexArraysDifference(res,e,s);
  getTexturesDifference    (res,e,s);
  getFramebuffersDifference(res,e,s);
}


struct NofCommands: public GPUProcessDifference{
  NofCommands(uint32_t e,uint32_t s):exp(e),stu(s){}
  uint32_t exp = 0;
  uint32_t stu = 0;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Rozdílný počet příkazů v command bufferu!" << std::endl;
    ss << "Váš počet    : " << stu << std::endl;
    ss << "Správný počet: " << exp << std::endl;
    return ss.str();
  }
};

struct MissingCommand: public GPUProcessDifference{
  MissingCommand(uint32_t i,Command const&e):i(i),exp(e){}
  uint32_t  i;
  Command exp;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Váš command buffer obsahuje příliš málo příkazů!" << std::endl;
    ss << "Tento command s číslem: " << i << "vám chybí:" << std::endl;
    ss << str(exp) << std::endl;
    return ss.str();
  }
};

struct ExtraCommand: public GPUProcessDifference{
  ExtraCommand(uint32_t i,Command const&s):i(i),stu(s){}
  uint32_t  i;
  Command stu;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Váš command buffer obsahuje příliš mnoho příkazů!" << std::endl;
    ss << "Tento command s číslem: " << i << "máte navíc:" << std::endl;
    ss << str(stu) << std::endl;
    return ss.str();
  }
};

struct DifferentType: public GPUProcessDifference{
  DifferentType(uint32_t i,CommandType const&e,CommandType const&s):i(i),exp(e),stu(s){}
  uint32_t  i;
  CommandType exp;
  CommandType stu;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Váš command s číslem: " << i << " má jiný typ!" << std::endl;
    ss << "Vás command má typ: " << str(stu) << std::endl;
    ss << "Správný typ je    : " << str(exp) << std::endl;
    return ss.str();
  }
};

template<typename T>
struct CommandDifference: public GPUProcessDifference{
  CommandDifference<T>(uint32_t i,CommandType t,T const&e,T const&s):i(i),type(t),exp(e),stu(s){}
  uint32_t       i;
  CommandType type;
  T            exp;
  T            stu;
  std::string toStr()const override{
    std::stringstream ss;
    ss << "Váš command: " << str(type) << "s číslem: " << i << "je jiný!" << std::endl;
    ss << "Váš příkaz:"<<std::endl;
    ss << str(exp) << std::endl;
    ss << "Správný příkaz:"<<std::endl;
    ss << str(stu) << std::endl;
    return ss.str();
  }
};



#define DIFF_COMMAND(cmd)\
  if(std::memcmp(&exp.data.cmd,&stu.data.cmd,sizeof(exp.data.cmd))!=0)\
    res.push_back(std::make_shared<CommandDifference<decltype(CommandData::cmd)>>(i,exp.type,exp.data.cmd,stu.data.cmd));\
  break

void getCommandDifferencies(std::vector<std::shared_ptr<GPUProcessDifference>>&res,uint32_t i,Command const&exp,Command const&stu){
  if(exp.type != stu.type)res.push_back(std::make_shared<DifferentType>(i,exp.type,stu.type));
  else{
    switch(exp.type){
      case CommandType::EMPTY                       :break;
      case CommandType::BIND_FRAMEBUFFER            :DIFF_COMMAND(bindFramebufferCommand   );
      case CommandType::BIND_PROGRAM                :DIFF_COMMAND(bindProgramCommand       );
      case CommandType::BIND_VERTEXARRAY            :DIFF_COMMAND(bindVertexArrayCommand   );
      case CommandType::BLOCK_WRITES_COMMAND        :DIFF_COMMAND(blockWritesCommand       );
      case CommandType::SET_BACKFACE_CULLING_COMMAND:DIFF_COMMAND(setBackfaceCullingCommand);
      case CommandType::SET_FRONT_FACE_COMMAND      :DIFF_COMMAND(setFrontFaceCommand      );
      case CommandType::SET_STENCIL_COMMAND         :DIFF_COMMAND(setStencilCommand        );
      case CommandType::SET_BLENDING_COMMAND        :DIFF_COMMAND(setBlendingCommand       );
      case CommandType::SET_DRAW_ID                 :DIFF_COMMAND(setDrawIdCommand         );
      case CommandType::USER_COMMAND                :DIFF_COMMAND(userCommand              );
      case CommandType::CLEAR_COLOR                 :DIFF_COMMAND(clearColorCommand        );
      case CommandType::CLEAR_DEPTH                 :DIFF_COMMAND(clearDepthCommand        );
      case CommandType::CLEAR_STENCIL               :DIFF_COMMAND(clearStencilCommand      );
      case CommandType::DRAW                        :DIFF_COMMAND(drawCommand              );
      case CommandType::SUB_COMMAND                 :DIFF_COMMAND(subCommand               );
    }
  }
}

void getCommandBufferDifferencies(std::vector<std::shared_ptr<GPUProcessDifference>>&res,CommandBuffer const&exp,CommandBuffer const&stu){
  if(exp.nofCommands != stu.nofCommands)res.push_back(std::make_shared<NofCommands   >(exp.nofCommands,stu.nofCommands));
  auto N = glm::max(exp.nofCommands,stu.nofCommands);
  for(uint32_t i=0;i<N;++i){
    if     (i>=stu.nofCommands)res.push_back(std::make_shared<MissingCommand>(i,exp.commands[i]));
    else if(i>=exp.nofCommands)res.push_back(std::make_shared<ExtraCommand  >(i,stu.commands[i]));
    else{
      getCommandDifferencies(res,i,exp.commands[i],stu.commands[i]);
    }
  }
}


bool checkModelTranslation(tests::model::TestModel const&model,bool first){

  auto testNumber = getTestNumberAndIncrement(first);
  if(shouldTestBeIgnored(testNumber))return true ;
  if(shouldTestBeBroken (testNumber))return false;

  GPUMemory eMem;
  CommandBuffer eCB;
  switchToTeacherSolution();
  prepareModel(eMem,eCB,model);

  GPUMemory sMem;
  CommandBuffer sCB;
  switchToStudentSolution();
  prepareModel(sMem,sCB,model);

  std::vector<std::shared_ptr<GPUProcessDifference>>diff;
  getCommandBufferDifferencies(diff,eCB,sCB);
  getMemDifference(diff,eMem,sMem);

  if(diff.empty())return true;

  std::cerr << "prepareModel připravil CommandBuffer a GPUMemory špatným způsobem:"<<std::endl;

  std::cerr<<R".(
  Tento test ověřuje funkcionalitu funkce prepareModel.
  Je potřeba projít kořenové uzly (roots).
  Pokud nemá uzel mesh (mesh<0), nevytváří se pro něj command buffer, protože není co vykreslit.
  Je však nuté prozkoumat jeho potomky.
  Strom modelu by se měly projít pre order přístupem:
  https://en.wikipedia.org/wiki/Tree_traversal

  Nejsnazší zpracování je rekurzí, pseudokód:

  void prepareNode(Node const&node){
    if(node.mesh>=0){
      //mame mesh vkladame do command bufferu
    }
    for(size_t i=0;i<node.children.size();++i)
      prepareNode(node.children[i]);
  }

  void prepare(Model const&model){
    for(size_t i=0;i<model.roots.size();++i)
      prepareNode(model.roots[i]);
  }

  Test zkouší zpracovat model, který vypadá takto:
  )." << std::endl;
  printModel(2,model);
  std::cerr<<R".(
  A zkoumá, jaký jste vytvořili command bufferu a jak naplnili pamět.
  Ale něco se pokazilo...)."<<std::endl;

  size_t errCounter = 0;
  for(auto const&d:diff){
    std::cerr << "CHYBA" << (errCounter++) << ":" << std::endl;
    print(d->toStr(),Color::RED);
    std::cerr << std::endl;
  }

  return false;
}

void checkTraverse(std::vector<tests::model::MeshI>const&meshes,std::vector<tests::model::NodeI>const&roots){
  auto model = createModel(meshes,roots);
  if(checkModelTranslation(model))return;
  REQUIRE(false);
}


void MemCb::setGeometry(
    std::vector<glm::vec4>const&pos,
    std::vector<glm::vec4>const&col,
    std::vector<uint32_t >const&dis){

  using posType = std::decay<decltype(pos)>::type::value_type;
  using colType = std::decay<decltype(col)>::type::value_type;
  using disType = std::decay<decltype(dis)>::type::value_type;

  mem.buffers[0] = vectorToBuffer(pos);

  bool hasColor   = col.size() == pos.size();
  bool hasDiscard = dis.size() == pos.size();

  if(hasColor  )mem.buffers[1] = vectorToBuffer(col);
  if(hasDiscard)mem.buffers[2] = vectorToBuffer(dis);
  

  mem.vertexArrays[0].vertexAttrib[0].bufferID = 0;
  mem.vertexArrays[0].vertexAttrib[0].offset   = 0;
  mem.vertexArrays[0].vertexAttrib[0].stride   = sizeof(posType);
  mem.vertexArrays[0].vertexAttrib[0].type     = AttribType::VEC4;

  if(hasColor){
    mem.vertexArrays[0].vertexAttrib[1].bufferID = 1;
    mem.vertexArrays[0].vertexAttrib[1].offset   = 0;
    mem.vertexArrays[0].vertexAttrib[1].stride   = sizeof(colType);
    mem.vertexArrays[0].vertexAttrib[1].type     = AttribType::VEC4;
  }
  if(hasDiscard){
    mem.vertexArrays[0].vertexAttrib[1].bufferID = 2;
    mem.vertexArrays[0].vertexAttrib[1].offset   = 0;
    mem.vertexArrays[0].vertexAttrib[1].stride   = sizeof(disType);
    mem.vertexArrays[0].vertexAttrib[1].type     = AttribType::UINT;
  }
  
  if(hasColor){
    mem.programs[0].vs2fs[0]       = AttribType::VEC4;
  }
  if(hasDiscard){
    mem.programs[0].vs2fs[1]       = AttribType::UINT;
  }

  if(hasColor && hasDiscard){
    mem.programs[0].vertexShader   = vertexPosColorDiscard;
    mem.programs[0].fragmentShader = fragmentColorDiscard;
  }
  if(hasColor && !hasDiscard){
    mem.programs[0].vertexShader   = vertexPosColor;
    mem.programs[0].fragmentShader = fragmentColor;
  }
  if(!hasColor && !hasDiscard){
    mem.programs[0].vertexShader   = vertexPos;
    mem.programs[0].fragmentShader = fragmentEmpty;
  }

  mem.activatedProgram     = 0;
  mem.activatedVertexArray = 0;
  mem.activatedFramebuffer = 0;
}

bool MemCb::runAndTest(uint32_t n,bool first){
  // this function does not take into consideration
  // userCallback side efects

  auto testNumber = getTestNumberAndIncrement(first);
  if(shouldTestBeIgnored(testNumber))return true ;
  if(shouldTestBeBroken (testNumber))return false;

  expectedMem = deepCopy(mem);
  studentMem  = deepCopy(mem);

  //TODO copy cb?

  auto runSolution = [&](GPUMemory&m){
    hookAllCallbacks(m);
    inFlightMem = &m;
    for(uint32_t i=0;i<n;++i)gpuRun(*inFlightMem,cb);
    unhookAllCallbacks(m);
    return streamDump;
  };

  streamDump.gpuStream.clear();
  switchToTeacherSolution();
  expectedStreamDump = runSolution(expectedMem.mem);

  streamDump.gpuStream.clear();
  switchToStudentSolution();
  studentStreamDump = runSolution(studentMem.mem);

  diff = {};
  getMemDifference(diff,expectedMem.mem,studentMem.mem);
  getStreamDumpDifference(diff,expectedStreamDump,studentStreamDump);

  if(!diff.empty())return false;

  return true;
}

void MemCb::printDifferencies(){

  std::cerr << R".(
  ==========================
  === DETAILNÍ INFORMACE ===
  ==========================
  ).";

  std::cerr << R".(
  Command buffer v tomto testu vypadá takto:
  )." << std::endl;
  print(str(cb,2),Color::GRAY);

  std::cerr << R".(
  V čem se Vaše grafická karta chovala jinak od referenční:
  )." << std::endl;

  size_t errCounter = 0;
  for(auto const&d:diff){
    std::cerr << "CHYBA" << (errCounter++) << ":" << std::endl;
    print(d->toStr(),Color::RED);
    std::cerr << std::endl;
  }

  if(ProgramContext::get().args.verboseMemoryOutput == 0)return;

  if(!isMemDifferent(this->studentMem.mem,this->expectedMem.mem))return;

  std::cerr << R".(
  Jaké změny jste nad pamětí provedli vy:)." << std::endl;

  auto stuMemStr = str(this->studentMem.mem,2);
  if(stuMemStr == "")stuMemStr = "  žádné\n";
  print(stuMemStr,Color::GRAY);

  std::cerr << R".(
  Jaké změny se nad pamětí měly provést:)." << std::endl;

  auto expMemStr=str(this->expectedMem.mem,2);
  if(expMemStr == "")expMemStr = "  žádné\n";
  print(expMemStr,Color::GRAY);

  std::cerr << R".(
  V jakém stavu byla paměť na začátku:)." << std::endl;

  auto startMemStr = str(this->mem,2);
  if(startMemStr == "")startMemStr = "  Počáteční pamět byla nastavena defaultně\n";
  print(startMemStr,Color::GRAY);
}

}
