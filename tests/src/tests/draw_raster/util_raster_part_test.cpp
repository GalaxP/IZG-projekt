#include<iostream>

#include<catch2/catch_test_macros.hpp>

#include<tests/draw_raster/util_raster_part_test.hpp>
#include<tests/testCommon.hpp>
#include<tests/str.hpp>
#include<tests/isMemDifferent.hpp>

namespace tests{

void TestingTriangle::test(uint32_t counter,std::string const&name){
  MEMCB(resolution.x,resolution.y);

  if(!hasStencilBuffer)memCb->mem.framebuffers[0].stencil.data = nullptr;
  if(!hasDepthBuffer  )memCb->mem.framebuffers[0].depth  .data = nullptr;
  if(!hasColorBuffer  )memCb->mem.framebuffers[0].color  .data = nullptr;

  std::vector<glm::vec4>verts = {
    triangle.a,triangle.b,triangle.c,
  };

  std::vector<glm::vec4>cols = {
    triangle.ac,triangle.bc,triangle.cc,
  };

  if(!flip){
    auto z  = verts[1];verts[1] = verts[2];verts[2] = z ;
    auto zc = cols [1];cols [1] = cols [2];cols [2] = zc;
  }

  memCb->setGeometry(verts,cols);

  if(fragmentColors){
    mem.programs[0].vertexShader   = vertexPosColor;
    if(fragmentDiscard){
      mem.programs[0].fragmentShader = fragmentColorDiscardEveryOdd;
    }else{
      mem.programs[0].fragmentShader = fragmentColor ;
    }
  }else{
    mem.programs[0].vertexShader   = vertexPos;
    if(fragmentDiscard){
      mem.programs[0].fragmentShader = fragmentDiscardEveryOdd;
    }else{
      mem.programs[0].fragmentShader = fragmentEmpty;
    }
  }

  pushClearColorCommand        (cb,initColor);
  pushClearDepthCommand        (cb,initDepth);
  pushClearStencilCommand      (cb,initStencil);
  pushBlockWritesCommand       (cb,blockWrites.color,blockWrites.depth,blockWrites.stencil);
  pushSetBackfaceCullingCommand(cb,backfaceCulling.enabled);
  pushSetFrontFaceCommand      (cb,backfaceCulling.frontFaceIsCounterClockWise);
  if(isStencilSettingsDifferent (stencilSettings ,StencilSettings {}))pushSetStencilCommand        (cb,stencilSettings);
  if(isBlendingSettingsDifferent(blendingSettings,BlendingSettings{}))pushSetBlendingCommand       (cb,blendingSettings);
  pushDrawCommand              (cb,(uint32_t)verts.size());

  std::cerr << "  "<< counter << ": " << name << " - ";
  if(memCb->runAndTest()){std::cerr << "good." << std::endl;return;}
  std::cerr << "bad!" << std::endl;


  std::cerr << R".(
  Tento test kontroluje, zda správně funguje rastrová část grafické karty.
  Rastruje se trojúhelník a porovnává se výsledek s referencí.

  Něco je špatně...
  ).";


  std::cerr << "CHYBA! Rastrová část GPU nefunguje správně!" << std::endl;

  std::cerr << "Raterizuje se trojúhelník: " << "A=" << str(verts[0]) << "B=" << str(verts[1]) << "C=" << str(verts[2]) << std::endl;
  if(fragmentColors)
    std::cerr << "S barvami: " << "CA=" << str(cols[0]) << "CB=" << str(cols[1]) << "CC=" << str(cols[2]) << std::endl;
  std::cerr << "Při rozlišení: " << str(resolution) << std::endl;
  if(fragmentDiscard)
    std::cerr << "Fragment shader šachovnicově zahazuje fragmenty (nastavuje discard = true)" << std::endl;

  bool tooClose[] = {
    verts[0].z < -verts[0].w,
    verts[1].z < -verts[1].w,
    verts[2].z < -verts[2].w,
  };

  if(
      !tooClose[0] && 
      !tooClose[1] && 
      !tooClose[2] ){
    glm::vec3 ndc      [3];for(int i=0;i<3;++i)ndc[i] = glm::vec3(verts[i])/verts[i].w;
    glm::vec3 viewSpace[3];for(int i=0;i<3;++i)viewSpace[i] = glm::vec3((glm::vec2(ndc[i])*.5f+.5f)*glm::vec2(resolution),ndc[i].z);

    std::cerr << R".(
    Což odpovídá rasterizaci trojúhelníků v NDC (po perspektivním dělení):
    A = vec3)."<<str(ndc[0])<<R".(;
    B = vec3)."<<str(ndc[1])<<R".(;
    C = vec3)."<<str(ndc[2])<<R".(;

    Což odpovídá rasterizaci trojúhelníků ve view-space:
    A = vec3)."<<str(viewSpace[0])<<R".(;
    B = vec3)."<<str(viewSpace[1])<<R".(;
    C = vec3)."<<str(viewSpace[2])<<R".(;
    ).";

    auto s = glm::vec2(viewSpace[1]) - glm::vec2(viewSpace[0]);
    auto n = glm::vec2(-s.y,s.x);
    auto p = glm::vec3(n,glm::dot(n,glm::vec2(viewSpace[0])));
    if(glm::dot(p,glm::vec3(glm::vec2(viewSpace[2]),1.f))>=0.f){
      std::cerr << "trojúhelník je CCW" << std::endl;
    }else{
      std::cerr << "trojúhelník je CW" << std::endl;
    }
  }else{
    std::cerr << "Trojúhelník je v postavení, kdy by měl být ořezán near plane." << std::endl;
    if(tooClose[0])std::cerr << "Vrchol A se nachází na negativní straně near plane a měl by být ořezán." << std::endl;
    if(tooClose[1])std::cerr << "Vrchol B se nachází na negativní straně near plane a měl by být ořezán." << std::endl;
    if(tooClose[2])std::cerr << "Vrchol C se nachází na negativní straně near plane a měl by být ořezán." << std::endl;
  }

  if(!hasColorBuffer  )std::cerr << "Barevný    buffer: není" << std::endl;
  if(!hasDepthBuffer  )std::cerr << "Hloubkový  buffer: není" << std::endl;
  if(!hasStencilBuffer)std::cerr << "Stencilový buffer: není" << std::endl;

  if( hasColorBuffer  )std::cerr << "Barevný    buffer byl inicializován na: " << str(initColor  ) << std::endl;
  if( hasDepthBuffer  )std::cerr << "Hloubkový  buffer byl inicializován na: " << str(initDepth  ) << std::endl;
  if( hasStencilBuffer)std::cerr << "Stencilový buffer byl inicializován na: " << str(initStencil) << std::endl;

  if(isBlockWritesDifferent(blockWrites,BlockWrites{})){
    std::cerr << "Nastavení blockWrites: " << std::endl;
    std::cerr << str(blockWrites);
    std::cerr << R".(
    Pokud je zakázán zápis, hodnoty v daném bufferu (barva, hloubka nebo stencil) nejsou modifikovány.
    
    V OpenGL řídí zakázání zápisu do framebufferu tyto funkce:
    glColorMask()
    glDepthMask()
    glStencilMask()

    https://registry.khronos.org/OpenGL-Refpages/gl4/html/glColorMask.xhtml
    https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDepthMask.xhtml
    https://registry.khronos.org/OpenGL-Refpages/gl4/html/glStencilMask.xhtml
    ).";
  }
  if(isBackfaceCullingDifferent(backfaceCulling,BackfaceCulling{})){
    std::cerr << "Nastavení backface cullingu: " << std::endl;
    std::cerr << str(backfaceCulling);
    std::cerr << R".(
    Backface culling je jednotka, která zahazuje trojúhelníky, které jsou odvrácené.
    Pokud je backface culling zapnut, jsou odvrácené trojúhelníky zahozeny.
    Přivrácené trojúhelníky jsou specifikovány buď proti směru hodinových ručiček, nebo po směru.

    V OpenGL řídí backface culling tyto funkce:
    glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glFrontFace(GL_CW);

    https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml
    https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDisable.xhtml
    https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFrontFace.xhtml
    ).";
  }
  if(isStencilSettingsDifferent(stencilSettings,StencilSettings{})){
    std::cerr << "Nastavení stencilu: " << std::endl;
    std::cerr << str(stencilSettings);
    std::cerr << R".(
    Stencil test je před depth testem a operuje nad stencilovým bufferem.

    V OpenGL řídí stencilové operace funkce:
    glEnable(GL_STENCIL_TEST) - povoluje stencilový test a operace
    glDisable(GL_STENCIL_TEST) - zakazuje stencilový test a operace
    glStencilOpSeparate - nastavuje stencilovou operaci
    glStencilFunc - nastavuje funkci stencilového testu

    https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml
    https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDisable.xhtml
    https://registry.khronos.org/OpenGL-Refpages/gl4/html/glStencilFunc.xhtml
    https://registry.khronos.org/OpenGL-Refpages/gl4/html/glStencilOpSeparate.xhtml
    ).";  
  }
  if(isBlendingSettingsDifferent(blendingSettings,BlendingSettings{})){
    std::cerr << "Nastavení blendingu: " << std::endl;
    std::cerr << str(blendingSettings);
    std::cerr << R".(
    Blending je posledním blokem. Slouží pro zápis/modifikaci barvy v barevném bufferu.

    V OpenGL řídí blending funkce:
    glEnable(GL_BLEND) - povoluje blending
    glDisable(GL_BLEND) - zakazuje blending
    glBlendFunc - nastavuje faktor pro fragment a pro fragment (sFactor a dFactor)
    glBlendEquation - nastavuje způsob míchání

    https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml
    https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDisable.xhtml
    https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBlendFunc.xhtml
    https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBlendEquation.xhtml
    ).";  
  }
  std::cerr << "CHYBA!" << std::endl;
  
  memCb->printDifferencies();

  REQUIRE(false);

}
}
