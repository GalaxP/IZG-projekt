#include<iomanip>
#include<iostream>
#include<map>

#include<tests/str.hpp>
#include<tests/isMemDifferent.hpp>

bool isInt(glm::vec4 const&v){
  bool isInt = false;
  for(int i=0;i<4;++i)
    isInt |= glm::isnan(v[i]) || glm::isinf(v[i]);
  return isInt;
}

bool isInt(Uniform const&u){
  return isInt(u.m4[0]);
}

std::string padding(size_t n){
  return std::string(n,' ');
}

template<>std::string str(bool const&a,size_t n){
  if(a)return padding(n)+"true" ;
  else return padding(n)+"false";
}

template<>std::string str(uint8_t const&a,size_t n){
  std::stringstream ss;
  ss << padding(n) << (uint32_t)a;
  return ss.str();
}
template<>std::string str(uint16_t const&v,size_t n){std::stringstream ss;ss << padding(n) << v;return ss.str();}
template<>std::string str(uint32_t const&v,size_t n){std::stringstream ss;ss << padding(n) << v;return ss.str();}
template<>std::string str(uint64_t const&v,size_t n){std::stringstream ss;ss << padding(n) << v;return ss.str();}
template<>std::string str(int      const&v,size_t n){std::stringstream ss;ss << padding(n) << v;return ss.str();}
template<>std::string str(float    const&v,size_t n){std::stringstream ss;ss << padding(n) << v;return ss.str();}

template<>std::string str(glm::vec2 const&a,size_t n){
  std::stringstream ss;
  ss << padding(n) << "(" << a.x << "," << a.y << ")";
  return ss.str();
}

template<>std::string str(glm::vec3 const&a,size_t n){
  std::stringstream ss;
  ss << padding(n) << "(" << a.x << "," << a.y << "," << a.z << ")";
  return ss.str();
}

template<>std::string str(glm::vec4 const&a,size_t n){
  std::stringstream ss;
  ss << padding(n) << "(" << a.x << "," << a.y << "," << a.z << "," << a.w << ")";
  return ss.str();
}

template<>std::string str(glm::uvec2 const&a,size_t n){
  std::stringstream ss;
  ss << padding(n) << "(" << a.x << "," << a.y << ")";
  return ss.str();
}

template<>std::string str(glm::uvec3 const&a,size_t n){
  std::stringstream ss;
  ss << padding(n) << "(" << a.x << "," << a.y << "," << a.z << ")";
  return ss.str();
}

template<>std::string str(glm::uvec4 const&a,size_t n){
  std::stringstream ss;
  ss << padding(n) << "(" << a.x << "," << a.y << "," << a.z << "," << a.w << ")";
  return ss.str();
}

template<>std::string str(glm::ivec2 const&a,size_t n){
  std::stringstream ss;
  ss << padding(n) << "(" << a.x << "," << a.y << ")";
  return ss.str();
}

template<>std::string str(glm::ivec3 const&a,size_t n){
  std::stringstream ss;
  ss << padding(n) << "(" << a.x << "," << a.y << "," << a.z << ")";
  return ss.str();
}

template<>std::string str(glm::ivec4 const&a,size_t n){
  std::stringstream ss;
  ss << padding(n) << "(" << a.x << "," << a.y << "," << a.z << "," << a.w << ")";
  return ss.str();
}

template<>std::string str(glm::mat4 const&m,size_t n){
  std::stringstream ss;
  ss << padding(n);
  ss << "(";
  for(int i=0;i<16;++i){
    if(i>0)ss<<",";
    ss << m[i/4][i%4];
  }
  ss << ")";
  return ss.str();
}

template<>std::string str(IndexType const&v,size_t n){
  switch(v){
#define CASE(N) case IndexType::N:return padding(n)+#N
    CASE(U8 );
    CASE(U16);
    CASE(U32);
#undef CASE
  }
  return padding(n)+"unknown";
}

template<>std::string str(AttribType const&a,size_t n){
  switch(a){
#define CASE(N) case AttribType::N:return padding(n)+#N
    CASE(EMPTY);
    CASE(FLOAT);
    CASE(VEC2 );
    CASE(VEC3 );
    CASE(VEC4 );
    CASE(UINT );
    CASE(UVEC2);
    CASE(UVEC3);
    CASE(UVEC4);
#undef CASE
  }
  return padding(n)+"unknown";
}

template<> std::string str(CommandType const&a,size_t n){
  switch(a){
#define CASE(N) case CommandType::N:return padding(n)+#N
    CASE(CLEAR_COLOR                 );
    CASE(CLEAR_DEPTH                 );
    CASE(CLEAR_STENCIL               );
    CASE(DRAW                        );
    CASE(SET_DRAW_ID                 );
    CASE(BIND_FRAMEBUFFER            );
    CASE(BIND_PROGRAM                );
    CASE(BIND_VERTEXARRAY            );
    CASE(SUB_COMMAND                 );
    CASE(SET_STENCIL_COMMAND         );
    CASE(SET_BLENDING_COMMAND        );
    CASE(BLOCK_WRITES_COMMAND        );
    CASE(SET_BACKFACE_CULLING_COMMAND);
    CASE(SET_FRONT_FACE_COMMAND      );
    CASE(USER_COMMAND                );
    CASE(EMPTY                       );
#undef CASE
  }
  return "unknown";
}


template<>std::string str(Image::Format const&v,size_t n){
  switch(v){
#define CASE(N) case Image::N:return padding(n)+#N
    CASE(F32);
    CASE(U8 );
#undef CASE
  }
  return padding(n)+"unknown";
}

template<>std::string str(Image::Channel const&v,size_t n){
  switch(v){
#define CASE(N) case Image::N:return padding(n)+#N
    CASE(RED  );
    CASE(GREEN);
    CASE(BLUE );
    CASE(ALPHA);
#undef CASE
  }
  return padding(n)+"unknown";
}

template<>std::string str(StencilFunc const&v,size_t n){
  switch(v){
#define CASE(N) case StencilFunc::N:return padding(n)+#N
    CASE(NEVER   );
    CASE(LESS    );
    CASE(LEQUAL  );
    CASE(GREATER );
    CASE(GEQUAL  );
    CASE(EQUAL   );
    CASE(NOTEQUAL);
    CASE(ALWAYS  );
#undef CASE
  }
  return padding(n)+"unknown";
}

template<>std::string str(StencilOp const&v,size_t n){
  switch(v){
#define CASE(N) case StencilOp::N:return padding(n)+#N
    CASE(KEEP     );
    CASE(ZERO     );
    CASE(REPLACE  );
    CASE(INCR     );
    CASE(INCR_WRAP);
    CASE(DECR     );
    CASE(DECR_WRAP);
    CASE(INVERT   );
#undef CASE
  }
  return padding(n)+"unknown";
}

template<> std::string str(StencilSettings           const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "StencilSettings::enabled         = " << str(v.enabled        ) << std::endl;
  ss << padding(n) << "StencilSettings::refValue        = " << str(v.refValue       ) << std::endl;
  ss << padding(n) << "StencilSettings::func            = " << str(v.func           ) << std::endl;
  ss << padding(n) << "Stencilsettings::backOps.sfail   = " << str(v.backOps.sfail  ) << std::endl;
  ss << padding(n) << "Stencilsettings::backOps.dpfail  = " << str(v.backOps.dpfail ) << std::endl;
  ss << padding(n) << "Stencilsettings::backOps.dppass  = " << str(v.backOps.dppass ) << std::endl;
  ss << padding(n) << "Stencilsettings::frontOps.sfail  = " << str(v.frontOps.sfail ) << std::endl;
  ss << padding(n) << "Stencilsettings::frontOps.dpfail = " << str(v.frontOps.dpfail) << std::endl;
  ss << padding(n) << "Stencilsettings::frontOps.dppass = " << str(v.frontOps.dppass) << std::endl;
  return ss.str();
}

template<>std::string str(BlendEquation const&v,size_t n){
  switch(v){
#define CASE(N) case BlendEquation::N:return padding(n)+#N
    CASE(ADD             );
    CASE(SUBTRACT        );
    CASE(REVERSE_SUBTRACT);
    CASE(MIN             );
    CASE(MAX             );
#undef CASE
  }
  return padding(n)+"unknown";
}

template<>std::string str(BlendFunc const&v,size_t n){
  switch(v){
#define CASE(N) case BlendFunc::N:return padding(n)+#N
    CASE(ZERO               );
    CASE(ONE                );
    CASE(SRC_COLOR          );
    CASE(ONE_MINUS_SRC_COLOR);
    CASE(DST_COLOR          );
    CASE(ONE_MINUS_DST_COLOR);
    CASE(SRC_ALPHA          );
    CASE(ONE_MINUS_SRC_ALPHA);
    CASE(DST_ALPHA          );
    CASE(ONE_MINUS_DST_ALPHA);
#undef CASE
  }
  return padding(n)+"unknown";
}

template<> std::string str(BlendingSettings const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "BlendingSettings::enabled  = " << str(v.enabled ) << std::endl;
  ss << padding(n) << "BlendingSettings::dFactor  = " << str(v.dFactor ) << std::endl;
  ss << padding(n) << "BlendingSettings::sFactor  = " << str(v.sFactor ) << std::endl;
  ss << padding(n) << "Blendingsettings::equation = " << str(v.equation) << std::endl;
  return ss.str();
}

template<> std::string str(BlockWrites               const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "BlockWrites::color   = " << str(v.color  ) << std::endl;
  ss << padding(n) << "BlockWrites::depth   = " << str(v.depth  ) << std::endl;
  ss << padding(n) << "BlockWrites::stencil = " << str(v.stencil) << std::endl;
  return ss.str();
}

template<>std::string str(BindFramebufferCommand const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "BindFramebufferCommand::id = " << v.id << std::endl;
  return ss.str();
}

template<>std::string str(BindProgramCommand const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "BindProgramCommand::id = " << v.id << std::endl;
  return ss.str();
}

template<>std::string str(BindVertexArrayCommand const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "BindVertexArrayCommand::id = " << v.id << std::endl;
  return ss.str();
}

template<>std::string str(BlockWritesCommand const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "BlockWritesCommand::blockWrites = {" << std::endl;
  ss << str(v.blockWrites,n+2);
  ss << padding(n) << "}" << std::endl;
  return ss.str();
}

template<> std::string str(BackfaceCulling           const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "BackfaceCulling::enabled                     = " << str(v.enabled                    ) << std::endl;
  ss << padding(n) << "BackfaceCulling::frontFaceIsCounterClockWise = " << str(v.frontFaceIsCounterClockWise) << std::endl;
  return ss.str();
}

template<>std::string str(SetBackfaceCullingCommand const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "SetBackfaceCullingCommand::enabled = " << str(v.enabled) << std::endl;
  return ss.str();
}

template<>std::string str(SetFrontFaceCommand const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "SetFrontFaceCommand::frontFaceIsCounterClockWise = " << str(v.frontFaceIsCounterClockWise) << std::endl;
  return ss.str();
}

template<>std::string str(SetStencilCommand const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "SetStencilCommand::settings = {"  << std::endl;
  ss << str(v.settings,n+2);
  ss << padding(n) << "}"  << std::endl;
  return ss.str();
}

template<>std::string str(SetBlendingCommand const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "SetBlendingCommand::settings = {"  << std::endl;
  ss << str(v.settings,n+2);
  ss << padding(n) << "}"  << std::endl;
  return ss.str();
}

template<>std::string str(SetDrawIdCommand const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "SetDrawIdCommand::id = " << v.id << std::endl;
  return ss.str();
}

template<>std::string str(UserCommand const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "UserCommand::callback = " << v.callback << std::endl;
  ss << padding(n) << "UserCommand::data     = " << v.data     << std::endl;
  return ss.str();
}

template<>std::string str(ClearColorCommand const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "ClearColorCommand::value = " << str(v.value) << std::endl;
  return ss.str();
}

template<>std::string str(ClearDepthCommand const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "ClearDepthCommand::value = " << str(v.value) << std::endl;
  return ss.str();
}

template<>std::string str(ClearStencilCommand const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "ClearStencilCommand::value = " << str(v.value) << std::endl;
  return ss.str();
}

template<>std::string str(DrawCommand const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "DrawCommand::nofVertices = " << str(v.nofVertices) << std::endl;
  return ss.str();
}

template<>std::string str(SubCommand const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "SubCommand.commandBuffer = " << str(v.commandBuffer) << std::endl;
  if(v.commandBuffer){
    ss << padding(n) << "{" << std::endl;
    ss << str(*v.commandBuffer,n+2);
    ss << padding(n) << "{" << std::endl;
  }
  return ss.str();
}

template<> std::string str(VertexArray const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "VertexArray::indexBufferID = " <<     v.indexBufferID  << std::endl;
  ss << padding(n) << "VertexArray::indexOffset   = " <<     v.indexOffset    << std::endl;
  ss << padding(n) << "VertexArray::indexType     = " << str(v.indexType    ) << std::endl;
  for(uint32_t j=0;j<maxAttribs;++j){
    if(v.vertexAttrib[j].type == AttribType::EMPTY)continue;
    ss << padding(n) << "VertexArray::vertexAttrib["<<j<<"].bufferID = " << str(v.vertexAttrib[j].bufferID) << std::endl;
    ss << padding(n) << "VertexArray::vertexAttrib["<<j<<"].offset   = " << str(v.vertexAttrib[j].offset  ) << std::endl;
    ss << padding(n) << "VertexArray::vertexAttrib["<<j<<"].stride   = " << str(v.vertexAttrib[j].stride  ) << std::endl;
    ss << padding(n) << "VertexArray::vertexAttrib["<<j<<"].type     = " << str(v.vertexAttrib[j].type    ) << std::endl;
  }
  return ss.str();
}


template<>std::string str(Command const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "Command::type = " << str(v.type) << std::endl;
  auto const&vv = v.data;
  switch(v.type){
    case CommandType::EMPTY                       :ss<< padding(n)+"empty"                 ;break;
    case CommandType::BIND_FRAMEBUFFER            :ss<< str(vv.bindFramebufferCommand   ,n);break;
    case CommandType::BIND_PROGRAM                :ss<< str(vv.bindProgramCommand       ,n);break;
    case CommandType::BIND_VERTEXARRAY            :ss<< str(vv.bindVertexArrayCommand   ,n);break;
    case CommandType::BLOCK_WRITES_COMMAND        :ss<< str(vv.blockWritesCommand       ,n);break;
    case CommandType::SET_BACKFACE_CULLING_COMMAND:ss<< str(vv.setBackfaceCullingCommand,n);break;
    case CommandType::SET_FRONT_FACE_COMMAND      :ss<< str(vv.setFrontFaceCommand      ,n);break;
    case CommandType::SET_STENCIL_COMMAND         :ss<< str(vv.setStencilCommand        ,n);break;
    case CommandType::SET_BLENDING_COMMAND        :ss<< str(vv.setBlendingCommand       ,n);break;
    case CommandType::SET_DRAW_ID                 :ss<< str(vv.setDrawIdCommand         ,n);break;
    case CommandType::USER_COMMAND                :ss<< str(vv.userCommand              ,n);break;
    case CommandType::CLEAR_COLOR                 :ss<< str(vv.clearColorCommand        ,n);break;
    case CommandType::CLEAR_DEPTH                 :ss<< str(vv.clearDepthCommand        ,n);break;
    case CommandType::CLEAR_STENCIL               :ss<< str(vv.clearStencilCommand      ,n);break;
    case CommandType::DRAW                        :ss<< str(vv.drawCommand              ,n);break;
    case CommandType::SUB_COMMAND                 :ss<< str(vv.subCommand               ,n);break;
  }
  return ss.str();
}

template<>std::string str(CommandBuffer const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "CommandBuffer::nofCommands = " << v.nofCommands << std::endl;
  ss << padding(n) << "CommandBuffer::commands = {" << std::endl;
  for(uint32_t i=0;i<v.nofCommands;++i){
    ss << padding(n+2) << i << ":" << std::endl;
    ss << str(v.commands[i],n+4);
  }
  ss << padding(n) << "}" << std::endl;
  return ss.str();
}

std::string strChannels(Image::Channel const* const&c,size_t p = 0){
  std::stringstream ss;
  ss << padding(p) << "{";
  for(int i=0;i<4;++i){
    if(i>0)ss << ",";
    ss << str(c[i]);
  }
  ss << "}";
  return ss.str();
}

std::string imageToStr(Image const&img,uint32_t w,uint32_t h,size_t p){
  std::stringstream ss;
  ss << padding(p) << "Image::bytesPerPixel = " << str        (img.bytesPerPixel) << std::endl;
  ss << padding(p) << "Image::channels      = " << str        (img.channels     ) << std::endl;
  ss << padding(p) << "Image::pitch         = " << str        (img.pitch        ) << std::endl;
  ss << padding(p) << "Image::format        = " << str        (img.format       ) << std::endl;
  ss << padding(p) << "Image::channelTypes  = " << strChannels(img.channelTypes ) << std::endl;
  if(w == 0 || h == 0 || !img.data){
    ss << padding(p) << "Image::data = " << img.data << std::endl;
  }else{
    ss << padding(p) << "Image::data[] = {";
    for(uint32_t y=0;y<h;++y){
      for(uint32_t x=0;x<w;++x){
        auto sss = (uint8_t*)img.data+y*img.pitch+x*img.bytesPerPixel;
        if(img.format == Image::U8){
          for(uint32_t k=0;k<img.bytesPerPixel;++k){
            ss << std::setw(2) << std::setfill('0') << std::hex << (int)sss[k] << " ";
          }
        }
        if(img.format == Image::F32){
          for(uint32_t c=0;c<img.channels;++c)
            ss << ((float*)sss)[c] << " ";
        }
      }
    }
    ss << std::endl;
    ss << padding(p) << "}" << std::endl;
  }
  return ss.str();
}

template<>std::string str(Image const&img,size_t n){
  return imageToStr(img,1,1,n);
}

template<>std::string str(SizedImage const&img,size_t n){
  return imageToStr(std::get<0>(img),std::get<1>(img),std::get<2>(img),n);
}

template<> std::string str(Texture const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "Texture::width  = " << v.width  << std::endl;
  ss << padding(n) << "Texture::height = " << v.height << std::endl;
  ss << padding(n) << "Texture::img = {" << std::endl;
  ss << imageToStr(v.img,v.width,v.height,n+2);
  ss << padding(n) << "}" << std::endl;
  return ss.str();
}

template<>std::string str(Buffer  const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "Buffer::size = " << v.size << std::endl;
  ss << padding(n) << "Buffer::data = " << v.data << std::endl;
  if(v.data){
    ss << padding(n) << "{" << std::endl;
    ss << padding(n+2);
    for(size_t i=0;i<v.size;++i)
      ss << str(((uint8_t*)v.data)[i]) << " ";
    ss << std::endl;
    ss << padding(n) << "}" << std::endl;
  }
  return ss.str();
}

enum class UniformType{
  V1,V2,V3,V4,
  U1,U2,U3,U4,
  I1,I2,I3,I4,
  M4
};

std::string uniformToStr(Uniform const&u,UniformType const&t){
  switch(t){
    case UniformType::V1:return "v1 = "+str(u.v1);
    case UniformType::V2:return "v2 = "+str(u.v2);
    case UniformType::V3:return "v3 = "+str(u.v3);
    case UniformType::V4:return "v4 = "+str(u.v4);
    case UniformType::U1:return "u1 = "+str(u.u1);
    case UniformType::U2:return "u2 = "+str(u.u2);
    case UniformType::U3:return "u3 = "+str(u.u3);
    case UniformType::U4:return "u4 = "+str(u.u4);
    case UniformType::I1:return "i1 = "+str(u.i1);
    case UniformType::I2:return "i2 = "+str(u.i2);
    case UniformType::I3:return "i3 = "+str(u.i3);
    case UniformType::I4:return "i4 = "+str(u.i4);
    case UniformType::M4:return "m4 = "+str(u.m4);
    default             :return "?? = unknown";
  }
}

UniformType determineUniformType(Uniform const&u){
  glm::mat4 m = u.m4;
  m[0] = glm::vec4(1,0,0,0);
  if(m != glm::mat4(1))return UniformType::M4;
  if(isInt(u))return UniformType::I4;
  return UniformType::M4;
}

template<>std::string str(Uniform const&v,size_t n){
  auto type = determineUniformType(v);
  return padding(n)+uniformToStr(v,type);
}

template<> std::string str(Program const&v,size_t n){
  std::stringstream ss;
  ss << padding(n) << "Program::fragmentShader = " << (v.fragmentShader?"shader":"nullptr") << std::endl;
  ss << padding(n) << "Program::vertexShader   = " << (v.vertexShader  ?"shader":"nullptr") << std::endl;
  for(uint32_t i=0;i<maxAttribs;++i)
    ss << padding(n) << "Program::vs2fs["<<i<<"]   = " << str(v.vs2fs[i])   << std::endl;
  return ss.str();
}

template<>std::string str(InVertex const&in,size_t n){
  std::stringstream ss;
  ss << padding(n) << "InVertex::gl_VertexID = " << in.gl_VertexID << std::endl;
  for(uint32_t a=0;a<4;++a)
    ss << padding(n) << "InVertex::atttributes["<<a<<"] = " << str(in.attributes[a].v4) << std::endl;
  return ss.str();
}

template<>std::string str(OutVertex const&out,size_t n){
  std::stringstream ss;
  ss << padding(n) << "OutVertex::gl_Position = " << str(out.gl_Position);
  for(uint32_t a=0;a<4;++a)
    ss << padding(n) << "OutVertex::atttributes["<<a<<"] = " << str(out.attributes[a].v4) << std::endl;
  return ss.str();
}

template<>std::string str(Attrib const&att,size_t n){
  std::stringstream ss;
  if(isInt(att.v4))ss << padding(n) << str(att.u4) << std::endl;
  else             ss << padding(n) << str(att.v4) << std::endl;
  return ss.str();
}


void printChild(Node const&node,size_t child,size_t p);

void printNodeContent(Node const&node,size_t p){
  std::cerr << padding(p) << "  mesh = " << node.mesh << std::endl;
  //if(node.modelMatrix != glm::mat4(1))
  std::cerr << padding(p) << "  modelMatrix = " << str(node.modelMatrix) << std::endl;
  std::cerr << padding(p) << "  children["<<node.nofChildren<<"] = {";
  if(node.nofChildren>0){
    std::cerr << std::endl;
    for(size_t i=0;i<node.nofChildren;++i)
      printChild(node.children[i],i,p+2);
    std::cerr << padding(p) << "  ";
  }
  std::cerr << "}" << std::endl;
}

void printChild(Node const&node,size_t child,size_t p){
  std::cerr << padding(p+2) << "child" << child << "{" << std::endl;
  printNodeContent(node,p+2);
  std::cerr << padding(p+2) << "}" << std::endl;
}

void printVertexAttrib(std::string const&name,VertexAttrib const&a,uint32_t p){
  if(a.bufferID <0)return;
  std::cerr << padding(p) << name<<"{" << std::endl;
  std::cerr << padding(p) << "  bufferID   = " << a.bufferID  << std::endl;
  std::cerr << padding(p) << "  type       = " << str(a.type) << std::endl;
  std::cerr << padding(p) << "  stride     = " << a.stride    << std::endl;
  std::cerr << padding(p) << "  offset     = " << a.offset    << std::endl;
  std::cerr << padding(p) << "}" << std::endl;
}

void printModel(size_t p,Model const&model){
  std::cerr << padding(p)<<"Model vypadá takto:" << std::endl;
  std::cerr << padding(p)<<"model{" << std::endl;
  std::cerr << padding(p)<<"  meshes  [" << model.nofMeshes       << "]"<< std::endl;
  std::cerr << padding(p)<<"  roots   [" << model.nofRoots        << "]"<< std::endl;
  std::cerr << padding(p)<<"  textures[" << model.nofTextures     << "]"<< std::endl;
  std::cerr << padding(p)<<"}" << std::endl;
  for(size_t i=0;i<model.nofMeshes;++i){
    auto const&mesh = model.meshes[i];
    std::cerr << padding(p) << "mesh" << i << "{" << std::endl;
    std::cerr << padding(p) << "  nofIndices     = " << mesh.nofIndices        << std::endl;
    std::cerr << padding(p) << "  diffuseColor   = " << str(mesh.diffuseColor) << std::endl;
    std::cerr << padding(p) << "  doubleSided    = " << str(mesh.doubleSided)  << std::endl;
    std::cerr << padding(p) << "  diffuseTexture = " << mesh.diffuseTexture    << std::endl;
    std::cerr << padding(p) << "  indexBufferID  = " << mesh.indexBufferID     << std::endl;
    std::cerr << padding(p) << "  indexOffset    = " << mesh.indexOffset       << std::endl;
    std::cerr << padding(p) << "  indexType      = " << str(mesh.indexType)    << std::endl;
    printVertexAttrib("position",mesh.position,8);
    printVertexAttrib("normal"  ,mesh.normal  ,8);
    printVertexAttrib("texCoord",mesh.texCoord,8);
    std::cerr << padding(p) << "  }" << std::endl;
    std::cerr << padding(p) << "}" << std::endl;
  }
  for(size_t i=0;i<model.nofTextures;++i){
    auto const&texture = model.textures[i];
    std::cerr << padding(p) << "texture" << i << "{" << std::endl;
    std::cerr << padding(p) << "  width    = " << texture.width       << std::endl;
    std::cerr << padding(p) << "  height   = " << texture.height      << std::endl;
    std::cerr << imageToStr(texture.img,texture.width,texture.height,p) << std::endl;
    std::cerr << padding(p) << "}" << std::endl;
  }
  for(size_t i=0;i<model.nofRoots;++i){
    auto const&node = model.roots[i];
    std::cerr << padding(p) << "root" << i << "{" << std::endl;
    printNodeContent(node,p);
    std::cerr << padding(p) << "}" << std::endl;
  }
}

enum class BufferType{
  FLOAT,
  U8   ,
  U16  ,
  U32  ,
  MIXED,
};

std::string getBufferName(BufferType t,uint32_t i){
  std::stringstream ss;
  switch(t){
    case BufferType::FLOAT:ss << "vertexBuffer" ;break;
    case BufferType::MIXED:ss << "generalBuffer";break;
    default               :ss << "indexBuffer"  ;break;
  }
  ss << i;
  return ss.str();
}

template<typename ELEMENT_TYPE>
std::string declareBufferImpl(size_t p,Buffer const&b,std::string const&name){
  std::stringstream ss;
  size_t n = b.size/sizeof(ELEMENT_TYPE);
  ss << padding(p) << typeToStr<ELEMENT_TYPE>() << " "<<name<< "["<<n<<"] = {" << std::endl;
  for(size_t i=0;i<n;++i)
    ss << padding(p+2) << str<ELEMENT_TYPE>(((ELEMENT_TYPE*)b.data)[i]) << ",";
  ss << std::endl;
  ss << padding(p) << "};" << std::endl;
  return ss.str();
}

std::string declareGeneraBufferImpl(size_t p,Buffer const&b,std::string const&name){
  std::stringstream ss;
  ss << padding(p) << typeToStr<uint8_t>() << " " << name << "["<<b.size<<"] = {...};" << std::endl;
  return ss.str();
}

std::string declareBuffer(size_t p,uint32_t i,Buffer const&b,BufferType type){
  auto const name = getBufferName(type,i);
  if(type == BufferType::FLOAT)return declareBufferImpl<float   >(p,b,name);
  if(type == BufferType::U8   )return declareBufferImpl<uint8_t >(p,b,name);
  if(type == BufferType::U16  )return declareBufferImpl<uint16_t>(p,b,name);
  if(type == BufferType::U32  )return declareBufferImpl<uint32_t>(p,b,name);
  if(type == BufferType::MIXED)return declareGeneraBufferImpl    (p,b,name);
  return "";
}

void insertTypeToBufferTypes(std::map<uint32_t,BufferType>&bufferTypes,uint32_t b,BufferType t){
  if(bufferTypes.count(b)){
    if(bufferTypes[b] != t)
      bufferTypes[b] = BufferType::MIXED;
  }else
    bufferTypes[b] = t;
}

void insertIndexBufferToBufferTypes(std::map<uint32_t,BufferType>&bufferTypes,VertexArray const&vao){
  if(vao.indexBufferID < 0)return;
  if(vao.indexType == IndexType::U32)insertTypeToBufferTypes(bufferTypes,vao.indexBufferID,BufferType::U32);
  if(vao.indexType == IndexType::U16)insertTypeToBufferTypes(bufferTypes,vao.indexBufferID,BufferType::U16);
  if(vao.indexType == IndexType::U8 )insertTypeToBufferTypes(bufferTypes,vao.indexBufferID,BufferType::U8 );
}

bool offsetIsNotAlignedWithType(VertexAttrib const&attrib){
  return attrib.offset % (uint32_t)attrib.type != 0;
}

void insertAttribBufferToBufferTypes(std::map<uint32_t,BufferType>&bufferTypes,VertexAttrib const&attrib){
  if(attrib.bufferID < 0)return;
  auto type = BufferType::FLOAT;
  if(offsetIsNotAlignedWithType(attrib))type = BufferType::MIXED;
  insertTypeToBufferTypes(bufferTypes,attrib.bufferID,type);
}

void insertAttribBuffersToBufferTypes(std::map<uint32_t,BufferType>&bufferTypes,VertexArray const&vao){
  for(uint32_t i=0;i<maxAttribs;++i)
    insertAttribBufferToBufferTypes(bufferTypes,vao.vertexAttrib[i]);
}

void insertBuffersMentionedInVertexArraysToBufferTypes(GPUMemory const&mem,std::map<uint32_t,BufferType>&bufferTypes){
  for(uint32_t i=0;i<mem.maxVertexArrays;++i){
    auto const&vao = mem.vertexArrays[i];
    insertIndexBufferToBufferTypes(bufferTypes,vao);
    insertAttribBuffersToBufferTypes(bufferTypes,vao);
  }
}

void appendBuffersNotMentionedInCommandBufferToBufferTypes(std::map<uint32_t,BufferType>&bufferTypes,GPUMemory const&mem){
  for(uint32_t i=0;i<mem.maxBuffers;++i){
    if(!mem.buffers[i].data)continue;
    if(bufferTypes.count(i))continue;
    insertTypeToBufferTypes(bufferTypes,i,BufferType::MIXED);
  }
}

std::map<uint32_t,BufferType>createBufferTypes(GPUMemory const&mem){
  std::map<uint32_t,BufferType>bufferTypes;

  insertBuffersMentionedInVertexArraysToBufferTypes    (mem,bufferTypes);
  appendBuffersNotMentionedInCommandBufferToBufferTypes(bufferTypes,mem);

  return bufferTypes;
}

std::string declareBuffers(size_t p,GPUMemory const&mem){
  std::stringstream ss;

  auto bufferTypes = createBufferTypes(mem);

  for(auto const&bt:bufferTypes)
    ss << declareBuffer(p,bt.first,mem.buffers[bt.first],bt.second);
  return ss.str();
}

void gpuMemBuffersToString(std::stringstream&ss,GPUMemory const&mem,GPUMemory const&,size_t p){
  auto bufferTypes = createBufferTypes(mem);

  bool first = true;
  for(auto const&b:bufferTypes){
    if(first){
      ss << padding(p) << "GPUMemory::buffers["<<mem.maxBuffers<<"] = {" << std::endl;
      first = false;
    }
    ss << padding(p+2) << b.first << ":" << std::endl;
    ss << padding(p+4) << "Buffer::data = " << getBufferName(b.second,b.first) << std::endl;
    ss << padding(p+4) << "Buffer::size = " << mem.buffers[b.first].size << std::endl;
  }
  if(first)return;
  ss << padding(p) << "}" << std::endl;
}



void str(std::stringstream&ss,Image const&img,uint32_t w,uint32_t h,char const*name,size_t p){
  if(!isImageDifferent(img,Image{},h))return;
  ss << padding(p) << name <<" = {" << std::endl;
  ss << imageToStr(img,w,h,p+2);
  ss << padding(p) << "}" << std::endl;
}

std::string str(Framebuffer const&v,size_t p){
  std::stringstream ss;
  ss << padding(p) << "Framebuffer::width     = " << v.width     << std::endl;
  ss << padding(p) << "Framebuffer::height    = " << v.height    << std::endl;
  ss << padding(p) << "Framebuffer::yReversed = " << v.yReversed << std::endl;

  str(ss,v.color  ,v.width,v.height,"Framebuffer::color"  ,p);
  str(ss,v.depth  ,v.width,v.height,"Framebuffer::depth"  ,p);
  str(ss,v.stencil,v.width,v.height,"Framebuffer::stencil",p);

  return ss.str();
}

void programsToStr(std::stringstream&ss,GPUMemory const&mem,GPUMemory const&def,size_t p){
  bool first = true;
  for(uint32_t i=0;i<mem.maxPrograms;++i){
    if(!isProgramDifferent(mem.programs[i],def.programs[i]))continue;
    if(first){
      ss << padding(p) << "GPUMemory::programs["<<mem.maxPrograms<<"] = {" << std::endl;
      first = false;
    }
    ss << padding(p+2) << i << ":" << std::endl;
    ss << str(mem.programs[i],p+4);

  }
  if(first)return;
  ss << padding(p) << "}" << std::endl;
}

void framebuffersToStr(std::stringstream&ss,GPUMemory const&mem,GPUMemory const&def,size_t p){
  bool first = true;
  for(uint32_t i=0;i<mem.maxFramebuffers;++i){
    if(!isFramebufferDifferent(mem.framebuffers[i],def.framebuffers[i]))continue;
    if(first){
      ss << padding(p) << "GPUMemory::framebuffers["<<mem.maxFramebuffers<<"] = {" << std::endl;
      first = false;
    }
    ss << padding(p+2) << i << ":" << std::endl;
    ss << str(mem.framebuffers[i],p+4);
  }
  if(first)return;
  ss << padding(p) << "}" << std::endl;
}

void vertexArraysToStr(std::stringstream&ss,GPUMemory const&mem,GPUMemory const&def,size_t p){
  bool first = true;
  for(uint32_t i=0;i<mem.maxVertexArrays;++i){
    if(!isVertexArrayDifferent(mem.vertexArrays[i],def.vertexArrays[i]))continue;
    if(first){
      ss << padding(p) << "GPUMemory::vertexArrays["<<mem.maxVertexArrays<<"] = {" << std::endl;
      first = false;
    }
    ss << padding(p+2) << i << ":" << std::endl;
    ss << str(mem.vertexArrays[i],p+4);
  }
  if(first)return;
  ss << padding(p) << "}" << std::endl;
}

void texturesToStr(std::stringstream&ss,GPUMemory const&mem,GPUMemory const&def,size_t p){
  bool first = true;
  for(uint32_t i=0;i<mem.maxTextures;++i){
    if(!isTextureDifferent(mem.textures[i],def.textures[i]))continue;
    if(first){
      ss << padding(p) << "GPUMemory::textures["<<mem.maxTextures<<"] = {" << std::endl;
      first = false;
    }
    ss << padding(p+2) << i << ":" << std::endl;
    ss << str(mem.textures[i],p+4);
    ss << str(Texture{},p+2);
  }
  if(first)return;
  ss << padding(p) << "}" << std::endl;
}

std::string gpuMemStructToString(GPUMemory const&mem,size_t p){
  GPUMemory defaultMem;
  std::stringstream ss;

  if(mem.activatedFramebuffer != defaultMem.activatedFramebuffer)
    ss << padding(p) << "GPUMemory::activatedFramebuffer = " << mem.activatedFramebuffer << std::endl;

  if(mem.activatedProgram != defaultMem.activatedProgram)
    ss << padding(p) << "GPUMemory::activatedProgram     = " << mem.activatedProgram     << std::endl;

  if(mem.activatedVertexArray != defaultMem.activatedVertexArray)
    ss << padding(p) << "GPUMemory::activatedVertexArray = " << mem.activatedVertexArray << std::endl;

  if(mem.gl_DrawID != defaultMem.gl_DrawID)
    ss << padding(p) << "GPUMemory::gl_DrawID            = " << mem.gl_DrawID            << std::endl;

  if(isBlockWritesDifferent(mem.blockWrites,defaultMem.blockWrites)){
    ss << padding(p) << "GPUMemory::blockWrites = {" << std::endl;
    ss << str(mem.blockWrites,p+2);
    ss << padding(p) << "}" << std::endl;
  }

  if(isBackfaceCullingDifferent(mem.backfaceCulling,defaultMem.backfaceCulling)){
    ss << padding(p) << "GPUMemory::backfaceCulling = {" << std::endl;
    ss << str(mem.backfaceCulling,p+2);
    ss << padding(p) << "}" << std::endl;
  }

  if(isStencilSettingsDifferent(mem.stencilSettings,defaultMem.stencilSettings)){
    ss << padding(p) << "GPUMemory::stencilSettings = {" << std::endl;
    ss << str(mem.stencilSettings,p+2);
    ss << padding(p) << "}" << std::endl;
  }

  framebuffersToStr    (ss,mem,defaultMem,p);
  programsToStr        (ss,mem,defaultMem,p);
  gpuMemBuffersToString(ss,mem,defaultMem,p);
  vertexArraysToStr    (ss,mem,defaultMem,p);
  texturesToStr        (ss,mem,defaultMem,p);

  return ss.str();
}

template<>std::string str(GPUMemory const&mem,size_t p){
  std::stringstream ss;
  ss << declareBuffers(p,mem);
  ss << gpuMemStructToString(mem,p);
  return ss.str();
}

