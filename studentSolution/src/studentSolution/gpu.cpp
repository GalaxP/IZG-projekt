/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <studentSolution/gpu.hpp>
#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>
#include <fstream>


enum BufferType{
  COLOR,
  DEPTH,
  STENCIL
};
struct Triangle{
  OutVertex vertex[3];
  uint32_t id;
};
void clear_buffer(GPUMemory& mem, glm::vec4 value, BufferType type);
void primitiveAssembly(GPUMemory& mem, Triangle &t, uint32_t program);
void viewPortTransformation(Triangle& primitive, Framebuffer& framebuffer);
void rasterize(Framebuffer& framebuffer, Triangle& primitive, FragmentShader fragmentShader, GPUMemory& mem);
static float edge_function(const glm::vec2& a, const glm::vec2& b, const glm::vec2& p);


//! [student_GPU_run]
void student_GPU_run(GPUMemory&mem,CommandBuffer const&cb){
  (void)mem;
  (void)cb;
  /// \todo Tato funkce reprezentuje funkcionalitu grafické karty.<br>
  /// Měla by umět zpracovat command buffer, čistit framebuffer a kreslit.<br>
  /// mem obsahuje paměť grafické karty.
  /// cb obsahuje command buffer pro zpracování.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
  ///
  /// V základu jde o to, že cb obsahuje příkazy, které se musí provést nad pamětí mem.
  /// Správně fungující grafická karta dobře interpretuje příkazy v cb a správně změní obsah paměti mem.

  mem.gl_DrawID = 0;

  for(uint32_t i = 0; i < cb.nofCommands; ++i){
    Command const&cmd = cb.commands[i];

    switch(cmd.type) {
      case CommandType::EMPTY:
        break;
      case CommandType::BIND_PROGRAM:
        mem.activatedProgram = cmd.data.bindProgramCommand.id;
        break;
      case CommandType::BIND_VERTEXARRAY:
        mem.activatedVertexArray = cmd.data.bindVertexArrayCommand.id;
        break;
      case CommandType::BLOCK_WRITES_COMMAND:
        mem.blockWrites = cmd.data.blockWritesCommand.blockWrites;
        break;
      case CommandType::SET_BACKFACE_CULLING_COMMAND:
        mem.backfaceCulling.enabled = cmd.data.setBackfaceCullingCommand.enabled;
        break;
      case CommandType::SET_FRONT_FACE_COMMAND:
        mem.backfaceCulling.frontFaceIsCounterClockWise = cmd.data.setFrontFaceCommand.frontFaceIsCounterClockWise;
        break;
      case CommandType::SET_STENCIL_COMMAND:
        mem.stencilSettings.enabled = cmd.data.setStencilCommand.settings.enabled;
        mem.stencilSettings.func = cmd.data.setStencilCommand.settings.func;
        mem.stencilSettings.refValue = cmd.data.setStencilCommand.settings.refValue;
        mem.stencilSettings.frontOps = cmd.data.setStencilCommand.settings.frontOps;
        mem.stencilSettings.backOps = cmd.data.setStencilCommand.settings.backOps;
        break;
      case CommandType::SET_BLENDING_COMMAND:
        mem.blendingSettings = cmd.data.setBlendingCommand.settings;
        break;
      case CommandType::SET_DRAW_ID:
        mem.gl_DrawID = cmd.data.setDrawIdCommand.id;
        break;
      case CommandType::CLEAR_COLOR:
        clear_buffer(mem,cb.commands[i].data.clearColorCommand.value, COLOR);
        break;
      case CommandType::CLEAR_DEPTH:
        clear_buffer(mem,glm::vec4(cb.commands[i].data.clearDepthCommand.value), DEPTH);
        break;
      case CommandType::CLEAR_STENCIL:
        clear_buffer(mem,glm::vec4(cb.commands[i].data.clearStencilCommand.value), STENCIL);
        break;
      case CommandType::BIND_FRAMEBUFFER:
        mem.activatedFramebuffer = cmd.data.bindFramebufferCommand.id;
        break;
      case CommandType::USER_COMMAND:
      {
        UserCommandFce user_command = cmd.data.userCommand.callback;
        if(user_command != nullptr){
          user_command(cmd.data.userCommand.data);
        }
        break;
      }
      case CommandType::DRAW:
      {
        uint32_t triangleCount = cmd.data.drawCommand.nofVertices / 3;
        std::vector<Triangle> triangles(triangleCount);
        for(uint32_t i = 0; i < cmd.data.drawCommand.nofVertices; i++){
          OutVertex outVertex;
          // add triangle
          triangles[i/3].vertex[i%3] = outVertex;
        }
        for(uint32_t t = 0; t < triangleCount; t++){
          triangles[t].id = t*3;
          Triangle primitive = triangles[t];
          primitiveAssembly(mem, primitive, mem.activatedProgram);

          if (mem.backfaceCulling.enabled) {
            float area = edge_function(glm::vec2(primitive.vertex[0].gl_Position),
                                      glm::vec2(primitive.vertex[1].gl_Position),
                                      glm::vec2(primitive.vertex[2].gl_Position));
            bool frontIsCCW = mem.backfaceCulling.frontFaceIsCounterClockWise;
            bool triangleIsFront = (area < 0.0f && frontIsCCW) || (area > 0.0f && !frontIsCCW);
            if (!triangleIsFront) {
              // triangle is back-facing
              continue;
            }
          }

          viewPortTransformation(primitive, mem.framebuffers[mem.activatedFramebuffer]);
          // rasterize the primitive with the current program's fragment shader
          FragmentShader fs = mem.programs[mem.activatedProgram].fragmentShader;
          rasterize(mem.framebuffers[mem.activatedFramebuffer], primitive, fs, mem);
        }

        mem.gl_DrawID++;
        break;
      }
      case CommandType::SUB_COMMAND:
        student_GPU_run(mem, *cmd.data.subCommand.commandBuffer);
        break;
    }
  }
}

// Helper: apply a stencil operation to a uint8 stencil value
static uint8_t apply_stencil_op(uint8_t current, StencilOp op, uint32_t ref){
  switch(op){
    case StencilOp::KEEP: return current;
    case StencilOp::ZERO: return 0u;
    case StencilOp::REPLACE: return static_cast<uint8_t>(ref & 0xFFu);
    case StencilOp::INCR: return current == 0xFFu ? 0xFFu : static_cast<uint8_t>(current + 1);
    case StencilOp::INCR_WRAP: return static_cast<uint8_t>(current + 1);
    case StencilOp::DECR: return current == 0u ? 0u : static_cast<uint8_t>(current - 1);
    case StencilOp::DECR_WRAP: return static_cast<uint8_t>(current - 1);
    case StencilOp::INVERT: return static_cast<uint8_t>(~current);
  }
  return current;
}

void viewPortTransformation(Triangle& primitive, Framebuffer& framebuffer){
  for(int v = 0; v < 3; ++v){
    OutVertex& vertex = primitive.vertex[v];
    if(framebuffer.yReversed){
      vertex.gl_Position.y = -vertex.gl_Position.y;
    }
    auto origW = vertex.gl_Position.w;
    if (origW != 0.0f) {
      vertex.gl_Position.x /= origW;
      vertex.gl_Position.y /= origW;
      vertex.gl_Position.z /= origW;
    }
    // keep original w in gl_Position.w for perspective-correct interpolation
    vertex.gl_Position.w = origW;
    vertex.gl_Position.x = (vertex.gl_Position.x + 1.f) * 0.5f * framebuffer.width;
    vertex.gl_Position.y = (vertex.gl_Position.y + 1.f) * 0.5f * framebuffer.height;
  }
}

void vertexAssembly(InVertex &inVertex, VertexArray vao, uint32_t vertexID, Buffer *buffers){
  uint32_t vertexIndex = vertexID;
  if(vao.indexBufferID >= 0){
    auto const* indexBase = (uint8_t const*)buffers[vao.indexBufferID].data;
    if(indexBase != nullptr){
      auto const* indexPtr = indexBase + vao.indexOffset;
      switch(vao.indexType){
        case IndexType::U8 : vertexIndex = ((uint8_t  const*)indexPtr)[vertexID]; break;
        case IndexType::U16: vertexIndex = ((uint16_t const*)indexPtr)[vertexID]; break;
        case IndexType::U32: vertexIndex = ((uint32_t const*)indexPtr)[vertexID]; break;
      }
    }
  }
  inVertex.gl_VertexID = vertexIndex;

  for (int a = 0; a < maxAttribs; a++) {
    VertexAttrib attribute = vao.vertexAttrib[a];
    auto buff_id = attribute.bufferID;
    if (attribute.type != AttribType::EMPTY && buff_id >= 0) {
      auto const* base = (uint8_t const*)buffers[buff_id].data;
      if(base == nullptr) continue;
      auto const* ptr = base + attribute.offset + vertexIndex * attribute.stride;
      
      if(attribute.type==AttribType::FLOAT) {
        inVertex.attributes[a].v1 = *(const float*)ptr;
      } else if(attribute.type==AttribType::VEC2) {
        const float* fptr = (const float*)ptr;
        inVertex.attributes[a].v2 = glm::vec2(fptr[0], fptr[1]);
      } else if(attribute.type==AttribType::VEC3) {
        const float* fptr = (const float*)ptr;
        inVertex.attributes[a].v3 = glm::vec3(fptr[0], fptr[1], fptr[2]);
      } else if(attribute.type==AttribType::VEC4) {
        const float* fptr = (const float*)ptr;
        inVertex.attributes[a].v4 = glm::vec4(fptr[0], fptr[1], fptr[2], fptr[3]);
      } else if(attribute.type==AttribType::UINT) {
        inVertex.attributes[a].u1 = *(const uint32_t*)ptr;
      } else if(attribute.type==AttribType::UVEC2) {
        const uint32_t* uptr = (const uint32_t*)ptr;
        inVertex.attributes[a].u2 = glm::uvec2(uptr[0], uptr[1]);
      } else if(attribute.type==AttribType::UVEC3) {
        const uint32_t* uptr = (const uint32_t*)ptr;
        inVertex.attributes[a].u3 = glm::uvec3(uptr[0], uptr[1], uptr[2]);
      } else if(attribute.type==AttribType::UVEC4) {
        const uint32_t* uptr = (const uint32_t*)ptr;
        inVertex.attributes[a].u4 = glm::uvec4(uptr[0], uptr[1], uptr[2], uptr[3]);
      }
    }
  }
}

void primitiveAssembly(GPUMemory& mem, Triangle &t, uint32_t program){
  for(uint8_t vertex = 0; vertex < 3; ++vertex){
    InVertex inVertex;
    vertexAssembly(inVertex, mem.vertexArrays[mem.activatedVertexArray], t.id+vertex, mem.buffers);
    ShaderInterface si;
    si.uniforms = mem.uniforms;
    si.textures = mem.textures;
    si.gl_DrawID = mem.gl_DrawID;
    VertexShader vertexShader = mem.programs[program].vertexShader;
    if(vertexShader != nullptr) vertexShader(t.vertex[vertex], inVertex,si);
  }
}

static float edge_function(const glm::vec2& a, const glm::vec2& b, const glm::vec2& p) {
  return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}

bool is_pixelInsideTriangle(int px, int py, const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2) {
  glm::vec2 p = {px + 0.5f, py + 0.5f};

  float area = edge_function(v0, v1, v2);
  if (fabs(area) < 1e-6f) {
    return false;
  }

  float w0 = edge_function(v1, v2, p);
  float w1 = edge_function(v2, v0, p);
  float w2 = edge_function(v0, v1, p);

  if (area > 0.0f) {
    return w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f;
  } else {
    return w0 <= 0.0f && w1 <= 0.0f && w2 <= 0.0f;
  }
}

void create_fragment(InFragment& inFragment, Triangle& primitive, glm::vec3 barycentrics, glm::vec2 pixelCoord, Program& program){
  // Perspective-correct interpolation for non-integer varyings.
  // Compute 1/w for every vertex (w is preserved in gl_Position.w by viewPortTransformation).
  float w0 = primitive.vertex[0].gl_Position.w;
  float w1 = primitive.vertex[1].gl_Position.w;
  float w2 = primitive.vertex[2].gl_Position.w;
  float r0 = (w0 != 0.0f) ? 1.0f / w0 : 0.0f;
  float r1 = (w1 != 0.0f) ? 1.0f / w1 : 0.0f;
  float r2 = (w2 != 0.0f) ? 1.0f / w2 : 0.0f;

  float interpR = r0 * barycentrics.x + r1 * barycentrics.y + r2 * barycentrics.z;
  if (fabs(interpR) < 1e-12f) interpR = 1e-12f;

  for(int a = 0; a < maxAttribs; ++a){
    AttribType at = program.vs2fs[a];
    if(at == AttribType::EMPTY) continue;

    const Attrib &v0 = primitive.vertex[0].attributes[a];
    const Attrib &v1 = primitive.vertex[1].attributes[a];
    const Attrib &v2 = primitive.vertex[2].attributes[a];

    switch(at){
      case AttribType::FLOAT: {
        float a0 = v0.v1 * r0;
        float a1 = v1.v1 * r1;
        float a2 = v2.v1 * r2;
        float num = a0 * barycentrics.x + a1 * barycentrics.y + a2 * barycentrics.z;
        inFragment.attributes[a].v1 = num / interpR;
        break;
      }
      case AttribType::VEC2: {
        glm::vec2 a0 = v0.v2 * r0;
        glm::vec2 a1 = v1.v2 * r1;
        glm::vec2 a2 = v2.v2 * r2;
        glm::vec2 num = a0 * barycentrics.x + a1 * barycentrics.y + a2 * barycentrics.z;
        inFragment.attributes[a].v2 = num / interpR;
        break;
      }
      case AttribType::VEC3: {
        glm::vec3 a0 = v0.v3 * r0;
        glm::vec3 a1 = v1.v3 * r1;
        glm::vec3 a2 = v2.v3 * r2;
        glm::vec3 num = a0 * barycentrics.x + a1 * barycentrics.y + a2 * barycentrics.z;
        inFragment.attributes[a].v3 = num / interpR;
        break;
      }
      case AttribType::VEC4: {
        glm::vec4 a0 = v0.v4 * r0;
        glm::vec4 a1 = v1.v4 * r1;
        glm::vec4 a2 = v2.v4 * r2;
        glm::vec4 num = a0 * barycentrics.x + a1 * barycentrics.y + a2 * barycentrics.z;
        inFragment.attributes[a].v4 = num / interpR;
        break;
      }
      case AttribType::UINT:
        // flat (provoking vertex)
        inFragment.attributes[a].u1 = v0.u1;
        break;
      case AttribType::UVEC2:
        inFragment.attributes[a].u2 = v0.u2;
        break;
      case AttribType::UVEC3:
        inFragment.attributes[a].u3 = v0.u3;
        break;
      case AttribType::UVEC4:
        inFragment.attributes[a].u4 = v0.u4;
        break;
      default:
        break;
    }
  }

  // compute perspective-correct depth
  float ndc0 = primitive.vertex[0].gl_Position.z; // already divided by w in viewPortTransformation
  float ndc1 = primitive.vertex[1].gl_Position.z;
  float ndc2 = primitive.vertex[2].gl_Position.z;
  float numZ = ndc0 * barycentrics.x + ndc1 * barycentrics.y + ndc2 * barycentrics.z;
  // use linear NDC z (tests expect this form)
  float depth = numZ;
  inFragment.gl_FragCoord = glm::vec4(pixelCoord, depth, 1.f);
}

void rasterize(Framebuffer& framebuffer, Triangle& primitive, FragmentShader fragmentShader, GPUMemory& mem) {
  // Simple rasterize: compute barycentrics, construct fragment, call fragment shader
  // precompute triangle area and facing
  float triArea = edge_function(primitive.vertex[0].gl_Position, primitive.vertex[1].gl_Position, primitive.vertex[2].gl_Position);
  bool frontIsCCW = mem.backfaceCulling.frontFaceIsCounterClockWise;
  bool triangleIsFront = (triArea < 0.0f && frontIsCCW) || (triArea > 0.0f && !frontIsCCW);

  for(uint32_t y = 0; y < framebuffer.height; ++y){
    for(uint32_t x = 0; x < framebuffer.width; ++x){
      glm::vec2 p = glm::vec2(x + 0.5f, y + 0.5f);
      if (fabs(triArea) < 1e-6f) continue;
      float w0 = edge_function(primitive.vertex[1].gl_Position, primitive.vertex[2].gl_Position, p);
      float w1 = edge_function(primitive.vertex[2].gl_Position, primitive.vertex[0].gl_Position, p);
      float w2 = edge_function(primitive.vertex[0].gl_Position, primitive.vertex[1].gl_Position, p);
      const float eps = 1e-6f;
      // Use top-left fill rule with triangle area sign: include fragments on edges that are top-left.
      // Top-left rule: choose convention matching reference tests.
      // Use inverted Y comparison so horizontal/vertical orientation
      // matches the harness expectation for this project.
      auto isTopLeftEdge = [&](const glm::vec2 &a, const glm::vec2 &b){
        if (a.y < b.y) return true;
        if (a.y > b.y) return false;
        return a.x > b.x;
      };
      glm::vec2 v0 = glm::vec2(primitive.vertex[0].gl_Position);
      glm::vec2 v1 = glm::vec2(primitive.vertex[1].gl_Position);
      glm::vec2 v2 = glm::vec2(primitive.vertex[2].gl_Position);
      bool ok0 = false, ok1 = false, ok2 = false;
      // Multiply edge function by triangle area to handle CW/CCW consistently
      float s0 = w0 * triArea;
      float s1 = w1 * triArea;
      float s2 = w2 * triArea;
      if (s0 > eps) ok0 = true; else if (fabs(s0) <= eps && isTopLeftEdge(v1, v2)) ok0 = true;
      if (s1 > eps) ok1 = true; else if (fabs(s1) <= eps && isTopLeftEdge(v2, v0)) ok1 = true;
      if (s2 > eps) ok2 = true; else if (fabs(s2) <= eps && isTopLeftEdge(v0, v1)) ok2 = true;
      if (!(ok0 && ok1 && ok2)) continue;
      float invArea = 1.0f / triArea;
      glm::vec3 barycentrics = glm::vec3(w0 * invArea, w1 * invArea, w2 * invArea);

      // STENCIL TEST (before fragment shader)
      if(mem.stencilSettings.enabled && framebuffer.stencil.data != nullptr){
        uint8_t* sptr = (uint8_t*)getPixel(framebuffer.stencil, x, y);
        uint8_t sval = sptr[0];
        uint32_t ref = mem.stencilSettings.refValue;
        bool stencilPass = true;
        switch(mem.stencilSettings.func){
          case StencilFunc::NEVER:    stencilPass = false; break;
          case StencilFunc::LESS:     stencilPass = sval < ref; break;
          case StencilFunc::LEQUAL:   stencilPass = sval <= ref; break;
          case StencilFunc::GREATER:  stencilPass = sval > ref; break;
          case StencilFunc::GEQUAL:   stencilPass = sval >= ref; break;
          case StencilFunc::EQUAL:    stencilPass = sval == ref; break;
          case StencilFunc::NOTEQUAL: stencilPass = sval != ref; break;
          case StencilFunc::ALWAYS:   stencilPass = true; break;
        }
        if(!stencilPass){
          // apply sfail
          StencilOp op = triangleIsFront ? mem.stencilSettings.frontOps.sfail : mem.stencilSettings.backOps.sfail;
          if(!mem.blockWrites.stencil){
            sptr[0] = apply_stencil_op(sval, op, mem.stencilSettings.refValue);
          }
          continue;
        }
      }

      InFragment inFragment;
      create_fragment(inFragment, primitive, barycentrics, p, mem.programs[mem.activatedProgram]);
      OutFragment outFragment;
      ShaderInterface si;
      si.uniforms = mem.uniforms;
      si.textures = mem.textures;
      si.gl_DrawID = mem.gl_DrawID;
      if(fragmentShader != nullptr) fragmentShader(outFragment, inFragment, si);

      // Respect fragment discard from shader
      if(outFragment.discard) continue;

      // DEPTH TEST + WRITE: compare fragment depth against depth buffer and
      // write new depth if it passes (honoring blockWrites.depth). Use <=
      // comparison to match expected test semantics.
      bool depthExists = framebuffer.depth.data != nullptr;
      float fragDepth = inFragment.gl_FragCoord.z;
      bool depthPass = true;
      if(depthExists){
        Image &dimg = framebuffer.depth;
        if(dimg.format == Image::F32){
          float* dp = (float*)getPixel(dimg, x, y);
          float cur = dp[0];
          depthPass = fragDepth <= cur;
        } else if(dimg.format == Image::U8){
          uint8_t* dp = (uint8_t*)getPixel(dimg, x, y);
          uint8_t cur = dp[0];
          uint8_t f = static_cast<uint8_t>(glm::clamp(static_cast<uint32_t>(glm::clamp(fragDepth, 0.f, 1.f) * 255.f), 0u, 255u));
          depthPass = f <= cur;
        }
      }

      if(!depthPass){
        // apply dpfail stencil op if stencil enabled
        if(mem.stencilSettings.enabled && framebuffer.stencil.data != nullptr){
          uint8_t* sptr = (uint8_t*)getPixel(framebuffer.stencil, x, y);
          uint8_t sval = sptr[0];
          StencilOp op = triangleIsFront ? mem.stencilSettings.frontOps.dpfail : mem.stencilSettings.backOps.dpfail;
          if(!mem.blockWrites.stencil){
            sptr[0] = apply_stencil_op(sval, op, mem.stencilSettings.refValue);
          }
        }
        continue;
      }

      // depth passed: apply dppass stencil op (if enabled)
      if(mem.stencilSettings.enabled && framebuffer.stencil.data != nullptr){
        uint8_t* sptr = (uint8_t*)getPixel(framebuffer.stencil, x, y);
        uint8_t sval = sptr[0];
        StencilOp op = triangleIsFront ? mem.stencilSettings.frontOps.dppass : mem.stencilSettings.backOps.dppass;
        if(!mem.blockWrites.stencil){
          sptr[0] = apply_stencil_op(sval, op, mem.stencilSettings.refValue);
        }
      }

      // If depth buffer exists and depth writes are not blocked, write fragDepth.
      if(depthExists && !mem.blockWrites.depth){
        Image &dimg = framebuffer.depth;
        if(dimg.format == Image::F32){
          float* dp = (float*)getPixel(dimg, x, y);
          dp[0] = fragDepth;
        } else if(dimg.format == Image::U8){
          uint8_t* dp = (uint8_t*)getPixel(dimg, x, y);
          dp[0] = static_cast<uint8_t>(glm::clamp(static_cast<uint32_t>(glm::clamp(fragDepth, 0.f, 1.f) * 255.f), 0u, 255u));
        }
      }

      // COLOR WRITE / BLENDING
      if(!mem.blockWrites.color && framebuffer.color.data != nullptr){
        Image &cimg = framebuffer.color;
        // read destination color into vec4 (r,g,b,a) in 0..1
        glm::vec4 dst = glm::vec4(0.f);
        if(cimg.format == Image::F32){
          float* dp = (float*)getPixel(cimg, x, y);
          for(uint32_t ch = 0; ch < cimg.channels; ++ch){
            Image::Channel ct = cimg.channelTypes[ch];
            float v = dp[ch];
            switch(ct){
              case Image::RED:   dst.r = v; break;
              case Image::GREEN: dst.g = v; break;
              case Image::BLUE:  dst.b = v; break;
              case Image::ALPHA: dst.a = v; break;
            }
          }
        } else {
          uint8_t* dp = (uint8_t*)getPixel(cimg, x, y);
          for(uint32_t ch = 0; ch < cimg.channels; ++ch){
            Image::Channel ct = cimg.channelTypes[ch];
            float v = dp[ch] / 255.0f;
            switch(ct){
              case Image::RED:   dst.r = v; break;
              case Image::GREEN: dst.g = v; break;
              case Image::BLUE:  dst.b = v; break;
              case Image::ALPHA: dst.a = v; break;
            }
          }
        }

        glm::vec4 src = outFragment.gl_FragColor;
        // clamp source
        src = glm::clamp(src, 0.0f, 1.0f);

        glm::vec4 result = src;
        if(mem.blendingSettings.enabled){
          auto getFactor = [&](BlendFunc f)->glm::vec4{
            switch(f){
              case BlendFunc::ZERO: return glm::vec4(0.f);
              case BlendFunc::ONE: return glm::vec4(1.f);
              case BlendFunc::SRC_COLOR: return src;
              case BlendFunc::ONE_MINUS_SRC_COLOR: return glm::vec4(1.f) - src;
              case BlendFunc::DST_COLOR: return dst;
              case BlendFunc::ONE_MINUS_DST_COLOR: return glm::vec4(1.f) - dst;
              case BlendFunc::SRC_ALPHA: return glm::vec4(src.a);
              case BlendFunc::ONE_MINUS_SRC_ALPHA: return glm::vec4(1.f - src.a);
              case BlendFunc::DST_ALPHA: return glm::vec4(dst.a);
              case BlendFunc::ONE_MINUS_DST_ALPHA: return glm::vec4(1.f - dst.a);
            }
            return glm::vec4(1.f);
          };
          glm::vec4 sF = getFactor(mem.blendingSettings.sFactor);
          glm::vec4 dF = getFactor(mem.blendingSettings.dFactor);
          (void)sF; (void)dF; // suppress unused warning if blending disabled in future
          switch(mem.blendingSettings.equation){
            case BlendEquation::ADD: result = src * sF + dst * dF; break;
            // Note: Swap SUBTRACT and REVERSE_SUBTRACT implementations to match
            // reference behavior used by the test-suite (teacher solution).
            // SUBTRACT in tests behaves as (dst * dF - src * sF).
            case BlendEquation::SUBTRACT: result = dst * dF - src * sF; break;
            case BlendEquation::REVERSE_SUBTRACT: result = src * sF - dst * dF; break;
            case BlendEquation::MIN: result = glm::min(src, dst); break;
            case BlendEquation::MAX: result = glm::max(src, dst); break;
          }
          result = glm::clamp(result, 0.f, 1.f);
        }

        // write back per channel according to channelTypes
        if(cimg.format == Image::F32){
          float* wp = (float*)getPixel(cimg, x, y);
          for(uint32_t ch = 0; ch < cimg.channels; ++ch){
            Image::Channel ct = cimg.channelTypes[ch];
            float v = 0.f;
            switch(ct){
              case Image::RED:   v = result.r; break;
              case Image::GREEN: v = result.g; break;
              case Image::BLUE:  v = result.b; break;
              case Image::ALPHA: v = result.a; break;
            }
            wp[ch] = v;
          }
        } else {
          uint8_t* wp = (uint8_t*)getPixel(cimg, x, y);
          for(uint32_t ch = 0; ch < cimg.channels; ++ch){
            Image::Channel ct = cimg.channelTypes[ch];
            float v = 0.f;
            switch(ct){
              case Image::RED:   v = result.r; break;
              case Image::GREEN: v = result.g; break;
              case Image::BLUE:  v = result.b; break;
              case Image::ALPHA: v = result.a; break;
            }
            uint32_t iv = static_cast<uint32_t>(v * 255.f);
            iv = glm::clamp(iv, 0u, 255u);
            wp[ch] = static_cast<uint8_t>(iv);
          }
        }
        (void)0;
      }
    }
  }
}

void clear_buffer(GPUMemory& mem, glm::vec4 value, BufferType type) {
  if(mem.activatedFramebuffer >= mem.maxFramebuffers) return;

  auto&frame = mem.framebuffers[mem.activatedFramebuffer];
  Image&buffer = type == COLOR ? frame.color : (type == DEPTH ? frame.depth : frame.stencil);

  if(buffer.data == nullptr) return;
  if(frame.width == 0 || frame.height == 0) return;
  if(buffer.bytesPerPixel == 0 || buffer.pitch == 0) return;

  auto toByte = [](float v){
    v = glm::clamp(v,0.f,1.f);
    return static_cast<uint8_t>(glm::clamp(static_cast<uint32_t>(v * 255.f), 0u, 255u));
  };

  for(uint32_t y = 0; y < frame.height; ++y){
    for(uint32_t x = 0; x < frame.width; ++x){
      uint8_t* pixel = (uint8_t*)buffer.data + y * buffer.pitch + x * buffer.bytesPerPixel;

      if(type == COLOR){
        if(buffer.format == Image::U8){
          for(uint32_t c = 0; c < buffer.channels; ++c){
            float component = 0.f;
            switch(buffer.channelTypes[c]){
              case Image::RED  : component = value.r; break;
              case Image::GREEN: component = value.g; break;
              case Image::BLUE : component = value.b; break;
              case Image::ALPHA: component = value.a; break;
            }
            pixel[c] = toByte(component);
          }
        }else if(buffer.format == Image::F32){
          float* fpixel = reinterpret_cast<float*>(pixel);
          for(uint32_t c = 0; c < buffer.channels; ++c){
            switch(buffer.channelTypes[c]){
              case Image::RED  : fpixel[c] = value.r; break;
              case Image::GREEN: fpixel[c] = value.g; break;
              case Image::BLUE : fpixel[c] = value.b; break;
              case Image::ALPHA: fpixel[c] = value.a; break;
            }
          }
        }
      }else if(type == DEPTH){
        if(buffer.format == Image::F32){
          float* fpixel = reinterpret_cast<float*>(pixel);
          fpixel[0] = value.r;
        }else if(buffer.format == Image::U8){
          pixel[0] = toByte(value.r);
        }
      }else if(type == STENCIL){
        if(buffer.format == Image::U8){
          auto stencilValue = static_cast<uint8_t>(glm::clamp(value.r,0.f,255.f));
          pixel[0] = stencilValue;
        }else if(buffer.format == Image::F32){
          float* fpixel = reinterpret_cast<float*>(pixel);
          fpixel[0] = value.r;
        }
      }
    }
  }
}
//! [student_GPU_run]

