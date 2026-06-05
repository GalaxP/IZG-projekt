/*!
 * @file
 * @brief This file contains functions for model rendering
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#include <studentSolution/prepareModel.hpp>
#include <studentSolution/gpu.hpp>
#include <studentSolution/shaderFunctions.hpp>
#include <solutionInterface/uniformLocations.hpp>

void student_prepareNode(GPUMemory&mem,CommandBuffer&cb,Node const&node,Model const&model,glm::mat4 const&prubeznaMatice);
/**
 * @brief This function prepares model into memory and creates command buffer
 *
 * @param mem gpu memory
 * @param commandBuffer command buffer
 * @param model model structure
 */
//! [drawModel]
void student_prepareModel(GPUMemory&mem,CommandBuffer&commandBuffer,Model const&model){
  commandBuffer.nofCommands = 0;
  mem.gl_DrawID = 0;
  for(size_t i = 0; i < model.nofBuffers; ++i){
    mem.buffers[i] = model.buffers[i];
  }
  for(size_t i = 0; i < model.nofTextures; ++i){
    mem.textures[i] = model.textures[i];
  }

  if(model.roots == nullptr || model.nofRoots == 0) return;

  glm::mat4 jednotkovaMatice = glm::mat4(1.f);
  for(size_t i=0; i<model.nofRoots; ++i)
    student_prepareNode(mem, commandBuffer, model.roots[i], model, jednotkovaMatice);

  mem.gl_DrawID = 0;
}

void student_prepareNode(GPUMemory&mem,CommandBuffer&cb,Node const&node,Model const&model,glm::mat4 const&prubeznaMatice){
  if(node.mesh >= 0){
    Mesh mesh = model.meshes[node.mesh];
 
    uint32_t drawCounter = mem.gl_DrawID;

    VertexArray vao;
    vao.indexBufferID = mesh.indexBufferID;
    vao.indexOffset = mesh.indexOffset;
    vao.indexType = mesh.indexType;

    vao.vertexAttrib[0] = mesh.position;
    vao.vertexAttrib[1] = mesh.normal;
    vao.vertexAttrib[2] = mesh.texCoord;


    mem.vertexArrays[drawCounter] = vao;

    BindVertexArrayCommand bindVaoCmd;
    bindVaoCmd.id = drawCounter;

    DrawCommand drawCmd;
    mem.backfaceCulling.enabled = mesh.doubleSided;
    drawCmd.nofVertices = mesh.nofIndices;

    Command bindVaoCommand;
    bindVaoCommand.type = CommandType::BIND_VERTEXARRAY;
    bindVaoCommand.data.bindVertexArrayCommand = bindVaoCmd;

    Command setBackfaceCulling;
    setBackfaceCulling.type = CommandType::SET_BACKFACE_CULLING_COMMAND;
    setBackfaceCulling.data.setBackfaceCullingCommand.enabled = mesh.doubleSided ? false : true;

    Command drawCmdCommand;
    drawCmdCommand.type = CommandType::DRAW;
    drawCmdCommand.data.drawCommand = drawCmd;

    cb.commands[cb.nofCommands++] = bindVaoCommand;
    cb.commands[cb.nofCommands++] = setBackfaceCulling;
    cb.commands[cb.nofCommands++] = drawCmdCommand;

    glm::mat4 modelMatrix = prubeznaMatice * node.modelMatrix;
    glm::mat4 inverse = glm::inverse(glm::transpose(modelMatrix));

    mem.uniforms[getUniformLocation(drawCounter, MODEL_MATRIX                  )].m4 = modelMatrix;
    mem.uniforms[getUniformLocation(drawCounter, INVERSE_TRANSPOSE_MODEL_MATRIX)].m4 = inverse;
    mem.uniforms[getUniformLocation(drawCounter, DIFFUSE_COLOR                 )].v4 = mesh.diffuseColor;
    mem.uniforms[getUniformLocation(drawCounter, TEXTURE_ID                    )].i1 = mesh.diffuseTexture >= 0 ? mesh.diffuseTexture : -1;
    mem.uniforms[getUniformLocation(drawCounter, DOUBLE_SIDED                  )].v1 = mesh.doubleSided ? 1.0f : 0.0f;

    mem.gl_DrawID++;
  }

  if(node.children != nullptr && node.nofChildren > 0){
    for(size_t i=0;i<node.nofChildren;++i)
      student_prepareNode(mem,cb,node.children[i],model,prubeznaMatice * node.modelMatrix);
  }
}
//! [drawModel]

/**
 * @brief This function represents vertex shader of texture rendering method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param si shader interface
 */
//! [drawModel_vs]
void student_drawModel_vertexShader(OutVertex&outVertex,InVertex const&inVertex,ShaderInterface const&si){
  const auto cameraProjectionView = si.uniforms[getUniformLocation(si.gl_DrawID,PROJECTION_VIEW_MATRIX )].m4;
  const auto lightProjectionView = si.uniforms[getUniformLocation(si.gl_DrawID,USE_SHADOW_MAP_MATRIX)].m4;
  const auto modelMatrix = si.uniforms[getUniformLocation(si.gl_DrawID,MODEL_MATRIX)].m4;
  const auto inverseTransposeModelMatrix = si.uniforms[getUniformLocation(si.gl_DrawID,INVERSE_TRANSPOSE_MODEL_MATRIX)].m4;

  outVertex.gl_Position = cameraProjectionView * modelMatrix * glm::vec4(inVertex.attributes[0].v3, 1.0f);
  outVertex.attributes[0].v3 = modelMatrix * glm::vec4(inVertex.attributes[0].v3, 1.0f);
  outVertex.attributes[1].v3 = inverseTransposeModelMatrix * glm::vec4(inVertex.attributes[1].v3, 0.0f);
  outVertex.attributes[2].v2 = inVertex.attributes[2].v2;
  outVertex.attributes[3].v4  = lightProjectionView * modelMatrix * glm::vec4(inVertex.attributes[0].v3, 1.0f);
}
//! [drawModel_vs]

#include<iostream>

/**
 * @brief This functionrepresents fragment shader of texture rendering method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param si shader interface
 */
//! [drawModel_fs]
void student_drawModel_fragmentShader(OutFragment&outFragment,InFragment const&inFragment,ShaderInterface const&si){
  auto lightPosition   = si.uniforms[getUniformLocation(si.gl_DrawID, LIGHT_POSITION           )].v3;
  auto cameraPosition  = si.uniforms[getUniformLocation(si.gl_DrawID, CAMERA_POSITION          )].v3;
  auto shadowmapId     = si.uniforms[getUniformLocation(si.gl_DrawID, SHADOWMAP_ID             )].i1;
  auto ambientLight    = si.uniforms[getUniformLocation(si.gl_DrawID, AMBIENT_LIGHT_COLOR      )].v3;
  auto lightColor      = si.uniforms[getUniformLocation(si.gl_DrawID, LIGHT_COLOR              )].v3;
  auto diffuseColorMat = si.uniforms[getUniformLocation(si.gl_DrawID, DIFFUSE_COLOR            )].v4;
  auto textureId       = si.uniforms[getUniformLocation(si.gl_DrawID, TEXTURE_ID               )].i1;
  auto doubleSided     = si.uniforms[getUniformLocation(si.gl_DrawID, DOUBLE_SIDED             )].v1;

  auto fragPos   = inFragment.attributes[0].v3;
  auto fragNormal  = inFragment.attributes[1].v3;
  auto texCoord    = inFragment.attributes[2].v2;
  auto shadowPos   = inFragment.attributes[3].v4;

  glm::vec3 N = glm::normalize(fragNormal);

  if(doubleSided > 0.f){
    glm::vec3 viewDir = glm::normalize(cameraPosition - fragPos);
    if(glm::dot(N, viewDir) < 0.f){
      N = -N;
    }
  }

  glm::vec4 diffuseColor = diffuseColorMat;
  if(textureId >= 0){
    auto texColor = student_read_textureClamp(si.textures[textureId], texCoord);
    diffuseColor = texColor;
  }

  glm::vec3 L = glm::normalize(lightPosition - fragPos);
  float diff = glm::max(glm::dot(N, L), 0.f);

  float shadow = 0.f;
  if(shadowmapId >= 0){
    glm::vec3 shadowPosNorm = glm::vec3(shadowPos) / shadowPos.w;

    if(shadowPosNorm.x >= 0.f && shadowPosNorm.x <= 1.f &&
       shadowPosNorm.y >= 0.f && shadowPosNorm.y <= 1.f){
      float depthFromShadowMap = student_read_textureClamp(si.textures[shadowmapId], glm::vec2(shadowPosNorm.x, shadowPosNorm.y)).r;

      float currentDepth = shadowPosNorm.z;
      float bias = 0.005f;
      if(currentDepth - bias > depthFromShadowMap){
        shadow = 1.f;
      }
    }
  }

  glm::vec3 finalColor = ambientLight + lightColor * diff * (1.f - 0.5f * shadow);

  finalColor *= glm::vec3(diffuseColor);

  outFragment.gl_FragColor = glm::vec4(finalColor, diffuseColor.a);

  if(diffuseColor.a < 0.5f){
    outFragment.discard = true;
  }
}
//! [drawModel_fs]

