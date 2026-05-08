#pragma once

#include<solutionInterface/gpu.hpp>

bool isBlockWritesDifferent     (BlockWrites      const&a,BlockWrites      const&b);
bool isBackfaceCullingDifferent (BackfaceCulling  const&a,BackfaceCulling  const&b);
bool isStencilSettingsDifferent (StencilSettings  const&a,StencilSettings  const&b);
bool isBlendingSettingsDifferent(BlendingSettings const&a,BlendingSettings const&b);

bool isBufferShallowDifferent(Buffer const&a,Buffer const&b);
bool isImageShallowDifferent(Image const&a,Image const&b);
bool isTextureShallowDifferent(Texture const&a,Texture const&b);
bool isFramebufferShallowDifferent(Framebuffer const&a,Framebuffer const&b);
bool isMemShallowDifferent(GPUMemory const&a,GPUMemory const&b);

bool isBufferDifferent(Buffer const&a,Buffer const&b);
bool isUniformDifferent(Uniform const&a,Uniform const&b);
bool isProgramDifferent(Program const&a,Program const&b);
bool isVertexArrayDifferent(VertexArray const&a,VertexArray const&b);
bool isTextureDifferent(Texture const&a,Texture const&b);
bool isImageDifferent(Image const&a,Image const&b,uint32_t h);
bool isFramebufferDifferent(Framebuffer const&a,Framebuffer const&b);
bool isMemDifferent(GPUMemory const&a,GPUMemory const&b);
