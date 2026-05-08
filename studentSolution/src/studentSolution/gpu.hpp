/*!
 * @file
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#pragma once

#include <solutionInterface/gpu.hpp>

/**
 * @brief function that executes work stored in command buffer on the gpu memory.
 * This function represents the functionality of GPU.
 * It can render stuff, it can clear framebuffer.
 * It can process work stored in command buffer
 *
 * @param mem gpu memory
 * @param cb command buffer - packaged of work sent to the gpu
 */
void student_GPU_run(GPUMemory&mem,CommandBuffer const&cb);

