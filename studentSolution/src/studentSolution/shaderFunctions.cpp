#include<studentSolution/shaderFunctions.hpp>


/**
 * @brief This function reads color from texture.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 student_read_texture(Texture const&texture,glm::vec2 const&uv){
  if(!texture.img.data)return glm::vec4(0.f);
  auto uv1 = glm::fract(glm::fract(uv)+1.f);
  auto uv2 = uv1*glm::vec2(texture.width-1,texture.height-1)+0.5f;
  auto pix = glm::uvec2(uv2);
  return student_texelFetch(texture,pix);
}

/**
 * @brief This function reads color from texture with clamping on the borders.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 student_read_textureClamp(Texture const&texture,glm::vec2 const&uv){
  if(!texture.img.data)return glm::vec4(0.f);
  auto uv1 = glm::clamp(uv,0.f,1.f);
  auto uv2 = uv1*glm::vec2(texture.width-1,texture.height-1)+0.5f;
  auto pix = glm::uvec2(uv2);
  return student_texelFetch(texture,pix);
}

/**
 * @brief This function fetches color from texture.
 *
 * @param texture texture
 * @param pix integer coorinates
 *
 * @return color 4 floats
 */
glm::vec4 student_texelFetch(Texture const&texture,glm::uvec2 const&pix){
  auto&img = texture.img;
  glm::vec4 color = glm::vec4(0.f,0.f,0.f,1.f);
  if(pix.x>=texture.width || pix.y >=texture.height)return color;
  if(img.format == Image::U8){
    auto colorPtr = (uint8_t*)getPixel(img,pix.x,pix.y);
    for(uint32_t c=0;c<img.channels;++c)
      color[c] = colorPtr[img.channelTypes[c]]/255.f;
  }
  if(texture.img.format == Image::F32){
    auto colorPtr = (float*)getPixel(img,pix.x,pix.y);
    for(uint32_t c=0;c<img.channels;++c)
      color[c] = colorPtr[img.channelTypes[c]];
  }
  return color;
}
