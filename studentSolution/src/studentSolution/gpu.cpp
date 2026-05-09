/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <studentSolution/gpu.hpp>
#include <iostream>
#include <cstring>


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
        //TODO
        
        for(uint32_t i = 0; i < cmd.data.drawCommand.nofVertices; i++){
          InVertex  inVertex;
          OutVertex outVertex ;
          auto const&vao = mem.vertexArrays[mem.activatedVertexArray];
          uint32_t vertexIndex = i;
          if(vao.indexBufferID >= 0){
            auto const* indexBase = (uint8_t const*)mem.buffers[vao.indexBufferID].data;
            if(indexBase != nullptr){
              auto const* indexPtr = indexBase + vao.indexOffset;
              switch(vao.indexType){
                case IndexType::U8 : vertexIndex = ((uint8_t  const*)indexPtr)[i]; break;
                case IndexType::U16: vertexIndex = ((uint16_t const*)indexPtr)[i]; break;
                case IndexType::U32: vertexIndex = ((uint32_t const*)indexPtr)[i]; break;
              }
            }
          }

          inVertex.gl_VertexID = vertexIndex;

          for (int a = 0; a < maxAttribs; a++) {
            VertexAttrib attribute = mem.vertexArrays[mem.activatedVertexArray].vertexAttrib[a];
            auto buff_id = attribute.bufferID;
            if (attribute.type != AttribType::EMPTY && buff_id >= 0) {
              auto const* base = (uint8_t const*)mem.buffers[buff_id].data;
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

          ShaderInterface si;
          si.uniforms = mem.uniforms;
          si.textures = mem.textures;
          si.gl_DrawID = mem.gl_DrawID;
          VertexShader vertexShader = mem.programs[mem.activatedProgram].vertexShader;
          if(vertexShader != nullptr) vertexShader(outVertex,inVertex,si);
          
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

void clear_buffer(GPUMemory& mem, glm::vec4 value, BufferType type) {
  if(mem.activatedFramebuffer >= mem.maxFramebuffers) return;

  auto&frame = mem.framebuffers[mem.activatedFramebuffer];
  Image&buffer = type == COLOR ? frame.color : (type == DEPTH ? frame.depth : frame.stencil);

  if(buffer.data == nullptr) return;
  if(frame.width == 0 || frame.height == 0) return;
  if(buffer.bytesPerPixel == 0 || buffer.pitch == 0) return;

  auto toByte = [](float v){
    v = glm::clamp(v,0.f,1.f);
    return static_cast<uint8_t>(v * 255.f + 0.5f);
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

