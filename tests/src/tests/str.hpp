#pragma once

#include<solutionInterface/gpu.hpp>
#include<solutionInterface/modelFwd.hpp>
#include<string>
#include<sstream>

bool isInt(glm::vec4 const&v);
bool isInt(Uniform const&u);

std::string padding(size_t n = 2);

template<typename TYPE>
std::string typeToStr();

template<>inline std::string typeToStr<float   >(){return "float"   ;}
template<>inline std::string typeToStr<uint32_t>(){return "uint32_t";}
template<>inline std::string typeToStr<uint16_t>(){return "uint16_t";}
template<>inline std::string typeToStr<uint8_t >(){return "uint8_t" ;}

using SizedImage = std::tuple<Image,uint32_t,uint32_t>;
template<typename T>
std::string str(T const&a,size_t n=0){
  std::stringstream ss;
  ss << padding(n) << a;
  return ss.str();
}

template<> std::string str(bool                      const&a,size_t n);
template<> std::string str(uint8_t                   const&a,size_t n);
template<> std::string str(uint16_t                  const&v,size_t n);
template<> std::string str(uint32_t                  const&v,size_t n);
template<> std::string str(uint64_t                  const&v,size_t n);
template<> std::string str(int                       const&v,size_t n);
template<> std::string str(float                     const&v,size_t n);
template<> std::string str(glm::vec2                 const&a,size_t n);
template<> std::string str(glm::vec3                 const&a,size_t n);
template<> std::string str(glm::vec4                 const&a,size_t n);
template<> std::string str(glm::uvec2                const&a,size_t n);
template<> std::string str(glm::uvec3                const&a,size_t n);
template<> std::string str(glm::uvec4                const&a,size_t n);
template<> std::string str(glm::ivec2                const&a,size_t n);
template<> std::string str(glm::ivec3                const&a,size_t n);
template<> std::string str(glm::ivec4                const&a,size_t n);
template<> std::string str(glm::mat4                 const&m,size_t n);
template<> std::string str(IndexType                 const&i,size_t n);
template<> std::string str(AttribType                const&a,size_t n);
template<> std::string str(CommandType               const&a,size_t n);
template<> std::string str(Image::Format             const&v,size_t n);
template<> std::string str(Image::Channel            const&v,size_t n);
template<> std::string str(StencilFunc               const&v,size_t n);
template<> std::string str(StencilOp                 const&v,size_t n);
template<> std::string str(StencilSettings           const&v,size_t n);
template<> std::string str(BlendFunc                 const&v,size_t n);
template<> std::string str(BlendEquation             const&v,size_t n);
template<> std::string str(BlendingSettings          const&v,size_t n);
template<> std::string str(BlockWrites               const&v,size_t n);
template<> std::string str(InVertex                  const&v,size_t n);
template<> std::string str(OutVertex                 const&v,size_t n);
template<> std::string str(Attrib                    const&v,size_t n);
template<> std::string str(BindFramebufferCommand    const&v,size_t n);
template<> std::string str(BindProgramCommand        const&v,size_t n);
template<> std::string str(BindVertexArrayCommand    const&v,size_t n);
template<> std::string str(BlockWritesCommand        const&v,size_t n);
template<> std::string str(BackfaceCulling           const&v,size_t n);
template<> std::string str(SetBackfaceCullingCommand const&v,size_t n);
template<> std::string str(SetFrontFaceCommand       const&v,size_t n);
template<> std::string str(SetStencilCommand         const&v,size_t n);
template<> std::string str(SetBlendingCommand        const&v,size_t n);
template<> std::string str(SetDrawIdCommand          const&v,size_t n);
template<> std::string str(UserCommand               const&v,size_t n);
template<> std::string str(ClearColorCommand         const&v,size_t n);
template<> std::string str(ClearDepthCommand         const&v,size_t n);
template<> std::string str(ClearStencilCommand       const&v,size_t n);
template<> std::string str(DrawCommand               const&v,size_t n);
template<> std::string str(CommandBuffer             const&v,size_t n);
template<> std::string str(SubCommand                const&v,size_t n);
template<> std::string str(Command                   const&v,size_t n);
template<> std::string str(CommandBuffer             const&v,size_t n);
template<> std::string str(SizedImage                const&v,size_t n);
template<> std::string str(Buffer                    const&v,size_t n);
template<> std::string str(VertexArray               const&v,size_t n);
template<> std::string str(Uniform                   const&v,size_t n);
template<> std::string str(Program                   const&v,size_t n);
template<> std::string str(Image                     const&v,size_t n);
template<> std::string str(Texture                   const&v,size_t n);
template<> std::string str(Framebuffer               const&v,size_t n);
template<> std::string str(GPUMemory                 const&v,size_t n);

void printModel(size_t p,Model const&model);

std::string imageToString(size_t p,Image const&img,uint32_t w = 0,uint32_t h = 0);

std::string programToString              (size_t p,Program     const&prg,uint32_t id);
std::string programVertexShaderToString  (size_t p,Program     const&prg,uint32_t id);
std::string programFragmentShaderToString(size_t p,Program     const&prg,uint32_t id);
std::string programVS2FSToString         (size_t p,Program     const&prg,uint32_t id);
std::string vertexArrayToString          (size_t p,VertexArray const&vao,uint32_t id);
std::string vertexArrayIndexingToString  (size_t p,VertexArray const&vao,uint32_t id);
std::string vertexArrayAttribsToString   (size_t p,VertexArray const&vao,uint32_t id);

