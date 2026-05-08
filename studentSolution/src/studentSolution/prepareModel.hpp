/*!
 * @file
 * @brief This file contains function declarations for model rendering
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#pragma once

#include <solutionInterface/modelFwd.hpp>

void student_prepareModel(GPUMemory&mem,CommandBuffer&commandBuffer,Model const&model);

void student_drawModel_vertexShader(OutVertex&outVertex,InVertex const&inVertex,ShaderInterface const&si);

void student_drawModel_fragmentShader(OutFragment&outFragment,InFragment const&inFragment,ShaderInterface const&si);
