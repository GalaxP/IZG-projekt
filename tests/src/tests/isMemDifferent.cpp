#include <cstring>
#include<tests/isMemDifferent.hpp>




bool isNull(void const*ptr){
  return ptr == nullptr;
}

bool isBufferShallowDifferent(Buffer const&a,Buffer const&b){
  if(a.size != b.size)return true;
  if(isNull(a.data) != isNull(b.data))return true;
  return false;
}

bool isBufferDifferent(Buffer const&a,Buffer const&b){
  if(a.size != b.size)return true;
  if(isNull(a.data) != isNull(b.data))return true;
  if(std::memcmp(a.data,b.data,a.size) != 0)return true;
  return false;
}

bool isUniformDifferent(Uniform const&a,Uniform const&b){
  if(a.m4 != b.m4)return true;
  return false;
}

bool isProgramDifferent(Program const&a,Program const&b){
  if(a.vertexShader   != b.vertexShader  )return true;
  if(a.fragmentShader != b.fragmentShader)return true;
  for(uint32_t i=0;i<maxAttribs;++i){
    if(a.vs2fs[i] != b.vs2fs[i])return true;
  }
  return false;
}

bool isVertexArrayDifferent(VertexArray const&a,VertexArray const&b){
  if(a.indexBufferID != b.indexBufferID)return true;
  if(a.indexOffset   != b.indexOffset  )return true;
  if(a.indexType     != b.indexType    )return true;

  for(uint32_t i=0;i<maxAttribs;++i){
    auto const&aa=a.vertexAttrib[i];
    auto const&bb=b.vertexAttrib[i];
    if(aa.bufferID != bb.bufferID)return true;
    if(aa.offset   != bb.offset  )return true;
    if(aa.stride   != bb.stride  )return true;
    if(aa.type     != bb.type    )return true;
  }
  return false;
}

bool isImageDifferent(Image const&a,Image const&b,uint32_t h){
  if(a.bytesPerPixel != b.bytesPerPixel)return true;
  if(a.channels      != b.channels     )return true;
  if(a.format        != b.format       )return true;
  if(a.pitch         != b.pitch        )return true;

  for(int i=0;i<4;++i)
    if(a.channelTypes[i] != b.channelTypes[i])
      return true;

  if(isNull(a.data) != isNull(b.data))return true;

  if(a.data && b.data)
    if(memcmp(a.data,b.data,a.pitch*h) != 0)return true;
  return false;
}

bool isImageShallowDifferent(Image const&a,Image const&b){
  if(a.bytesPerPixel != b.bytesPerPixel)return true;
  if(a.channels      != b.channels     )return true;
  if(a.format        != b.format       )return true;
  if(a.pitch         != b.pitch        )return true;

  for(int i=0;i<4;++i)
    if(a.channelTypes[i] != b.channelTypes[i])
      return true;

  if(isNull(a.data) != isNull(b.data))return true;

  return false;
}

bool isTextureDifferent(Texture const&a,Texture const&b){
  if(a.width  != b.width )return true;
  if(a.height != b.height)return true;
  return isImageDifferent(a.img,b.img,a.height);
}

bool isTextureShallowDifferent(Texture const&a,Texture const&b){
  if(a.width  != b.width )return true;
  if(a.height != b.height)return true;
  return isImageShallowDifferent(a.img,b.img);
}

bool isFramebufferDifferent(Framebuffer const&a,Framebuffer const&b){
  if(a.width     != b.width    )return true;
  if(a.height    != b.height   )return true;
  if(a.yReversed != b.yReversed)return true;

  if(isImageDifferent(a.color  ,b.color  ,a.height))return true;
  if(isImageDifferent(a.depth  ,b.depth  ,a.height))return true;
  if(isImageDifferent(a.stencil,b.stencil,a.height))return true;

  return false;
}

bool isFramebufferShallowDifferent(Framebuffer const&a,Framebuffer const&b){
  if(a.width     != b.width    )return true;
  if(a.height    != b.height   )return true;
  if(a.yReversed != b.yReversed)return true;

  if(isImageShallowDifferent(a.color  ,b.color  ))return true;
  if(isImageShallowDifferent(a.depth  ,b.depth  ))return true;
  if(isImageShallowDifferent(a.stencil,b.stencil))return true;
  return false;
}

bool isBlockWritesDifferent    (BlockWrites     const&a,BlockWrites     const&b){
  if(a.color   != b.color  )return true;
  if(a.depth   != b.depth  )return true;
  if(a.stencil != b.stencil)return true;
  return false;
}

bool isBackfaceCullingDifferent(BackfaceCulling const&a,BackfaceCulling const&b){
  if(a.enabled                     != b.enabled                    )return true;
  if(a.frontFaceIsCounterClockWise != b.frontFaceIsCounterClockWise)return true;
  return false;
}

bool isStencilSettingsDifferent(StencilSettings const&a,StencilSettings const&b){
  if(a.enabled         != b.enabled        )return true;
  if(a.func            != b.func           )return true;
  if(a.refValue        != b.refValue       )return true;
  if(a.backOps .sfail  != b.backOps .sfail )return true;
  if(a.backOps .dpfail != b.backOps .dpfail)return true;
  if(a.backOps .dppass != b.backOps .dppass)return true;
  if(a.frontOps.sfail  != b.frontOps.sfail )return true;
  if(a.frontOps.dpfail != b.frontOps.dpfail)return true;
  if(a.frontOps.dppass != b.frontOps.dppass)return true;
  return false;
}

bool isBlendingSettingsDifferent(BlendingSettings const&a,BlendingSettings const&b){
  if(a.enabled  != b.enabled )return true;
  if(a.sFactor  != b.sFactor )return true;
  if(a.dFactor  != b.dFactor )return true;
  if(a.equation != b.equation)return true;
  return false;
}

bool isMemDifferent(GPUMemory const&a,GPUMemory const&b){
  if(a.activatedFramebuffer                        != b.activatedFramebuffer                       )return true;
  if(a.activatedProgram                            != b.activatedProgram                           )return true;
  if(a.activatedVertexArray                        != b.activatedVertexArray                       )return true;

  if(a.gl_DrawID                                   != b.gl_DrawID                                  )return true;

  if(isBlockWritesDifferent     (a.blockWrites     ,b.blockWrites     ))return true;
  if(isBackfaceCullingDifferent (a.backfaceCulling ,b.backfaceCulling ))return true;
  if(isStencilSettingsDifferent (a.stencilSettings ,b.stencilSettings ))return true;
  if(isBlendingSettingsDifferent(a.blendingSettings,b.blendingSettings))return true;

  for(uint32_t i=0;i<a.maxBuffers;++i)
    if(isBufferDifferent(a.buffers[i],b.buffers[i]))return true;

  for(uint32_t i=0;i<a.maxUniforms;++i)
    if(isUniformDifferent(a.uniforms[i],b.uniforms[i]))return true;

  for(uint32_t i=0;i<a.maxPrograms;++i)
    if(isProgramDifferent(a.programs[i],b.programs[i]))return true;

  for(uint32_t i=0;i<a.maxVertexArrays;++i)
    if(isVertexArrayDifferent(a.vertexArrays[i],b.vertexArrays[i]))return true;

  for(uint32_t i=0;i<a.maxTextures;++i)
    if(isTextureDifferent(a.textures[i],b.textures[i]))return true;

  for(uint32_t i=0;i<a.maxFramebuffers;++i)
    if(isFramebufferDifferent(a.framebuffers[i],b.framebuffers[i]))return true;

  return false;
}

bool isMemShallowDifferent(GPUMemory const&a,GPUMemory const&b){
  if(a.activatedFramebuffer                        != b.activatedFramebuffer                       )return true;
  if(a.activatedProgram                            != b.activatedProgram                           )return true;
  if(a.activatedVertexArray                        != b.activatedVertexArray                       )return true;

  if(a.gl_DrawID                                   != b.gl_DrawID                                  )return true;

  if(a.blockWrites.color                           != b.blockWrites.color                          )return true;
  if(a.blockWrites.depth                           != b.blockWrites.depth                          )return true;
  if(a.blockWrites.stencil                         != b.blockWrites.stencil                        )return true;

  if(a.backfaceCulling.enabled                     != b.backfaceCulling.enabled                    )return true;
  if(a.backfaceCulling.frontFaceIsCounterClockWise != b.backfaceCulling.frontFaceIsCounterClockWise)return true;

  if(a.stencilSettings.enabled                     != b.stencilSettings.enabled                    )return true;
  if(a.stencilSettings.func                        != b.stencilSettings.func                       )return true;
  if(a.stencilSettings.refValue                    != b.stencilSettings.refValue                   )return true;
  if(a.stencilSettings.backOps .sfail              != b.stencilSettings.backOps .sfail             )return true;
  if(a.stencilSettings.backOps .dpfail             != b.stencilSettings.backOps .dpfail            )return true;
  if(a.stencilSettings.backOps .dppass             != b.stencilSettings.backOps .dppass            )return true;
  if(a.stencilSettings.frontOps.sfail              != b.stencilSettings.frontOps.sfail             )return true;
  if(a.stencilSettings.frontOps.dpfail             != b.stencilSettings.frontOps.dpfail            )return true;
  if(a.stencilSettings.frontOps.dppass             != b.stencilSettings.frontOps.dppass            )return true;

  for(uint32_t i=0;i<a.maxBuffers;++i)
    if(isBufferShallowDifferent(a.buffers[i],b.buffers[i]))return true;

  for(uint32_t i=0;i<a.maxUniforms;++i)
    if(isUniformDifferent(a.uniforms[i],b.uniforms[i]))return true;

  for(uint32_t i=0;i<a.maxPrograms;++i)
    if(isProgramDifferent(a.programs[i],b.programs[i]))return true;

  for(uint32_t i=0;i<a.maxVertexArrays;++i)
    if(isVertexArrayDifferent(a.vertexArrays[i],b.vertexArrays[i]))return true;

  for(uint32_t i=0;i<a.maxTextures;++i)
    if(isTextureShallowDifferent(a.textures[i],b.textures[i]))return true;

  for(uint32_t i=0;i<a.maxFramebuffers;++i)
    if(isFramebufferShallowDifferent(a.framebuffers[i],b.framebuffers[i]))return true;

  return false;
}
