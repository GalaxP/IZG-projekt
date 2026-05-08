/*!
 * @file
 * @brief This file contains declaration of function that can be used in shaders.
 * Students are required to implement these functions.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include<solutionInterface/gpu.hpp>

glm::vec4 student_read_texture     (Texture const&texture,glm::vec2  const&uv);
glm::vec4 student_read_textureClamp(Texture const&texture,glm::vec2  const&uv);
glm::vec4 student_texelFetch       (Texture const&texture,glm::uvec2 const&uv);
