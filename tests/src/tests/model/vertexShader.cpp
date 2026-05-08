#include<iostream>

#include<catch2/catch_test_macros.hpp>

#include<glm/ext/matrix_clip_space.hpp>
#include<glm/trigonometric.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include<solutionInterface/taskFunctions.hpp>
#include<solutionInterface/gpu.hpp>
#include<solutionInterface/uniformLocations.hpp>

#define __FILENAME__ "vertexShader"
#include<tests/testCommon.hpp>
#include<tests/str.hpp>

#include<framework/switchSolution.hpp>

using namespace tests;
using namespace tests::model;

SCENARIO(TEST_NAME){
  printTestName("drawModel_vertexShader");

  auto proj  = glm::perspective(glm::radians(60.f),1.f,1.f,100.f);
  auto view  = glm::lookAt(glm::vec3(100.f,120.f,-30.f),glm::vec3(0.f,0.f,10.f),glm::vec3(0.f,1.f,0.f));

  auto lightProj = glm::perspective(glm::radians(55.f),1.f,.2f,100.f);
  auto lightView = glm::lookAt(glm::vec3(300.f),glm::vec3(0.f),glm::vec3(0.f,1.f,0.f));

  auto model = glm::translate(glm::rotate(glm::scale(glm::mat4(1.f),glm::vec3(3.f,5.f,-3.f)),glm::radians(5.f),glm::vec3(1.f,2.f,1.f)),glm::vec3(10.f,30.f,-4.f));
  auto pos   = glm::vec3(1.f,2.f,-3.f);
  auto nor   = glm::vec3(glm::cos(1)*glm::cos(2),glm::sin(1),glm::sin(1)*glm::cos(2));
  auto coord = glm::vec2(0.3,0.8);

  InVertex inV;
  inV.attributes[0].v3 = pos;
  inV.attributes[1].v3 = nor;
  inV.attributes[2].v2 = coord;

  Uniform uniforms[10000];//TODO FIX?

  ShaderInterface si;
  si.uniforms = uniforms;
  si.gl_DrawID = 13;

  uniforms[getUniformLocation(si.gl_DrawID,PROJECTION_VIEW_MATRIX        )].m4 = proj*view;
  uniforms[getUniformLocation(si.gl_DrawID,USE_SHADOW_MAP_MATRIX         )].m4 = lightProj*lightView;
  uniforms[getUniformLocation(si.gl_DrawID,MODEL_MATRIX                  )].m4 = model;
  uniforms[getUniformLocation(si.gl_DrawID,INVERSE_TRANSPOSE_MODEL_MATRIX)].m4 = glm::transpose(glm::inverse(model));

  OutVertex expV;
  switchToTeacherSolution();
  drawModel_vertexShader(expV,inV,si);

  OutVertex stuV;
  switchToStudentSolution();
  drawModel_vertexShader(stuV,inV,si);

  auto printInfo = [&](){
    std::cerr<<R".(

    Tento test zkouší, zda je funkce drawModel_vertexShader správně naimplementována.
    Tato funkce reprezentuje vertex shader, který je využíván při kreslení modelů.
    Cílem je ztransformovat vstupní vrchol pomocí matic na výstupní vrchol.
    Vstupní vrchol je tvořen třemi atributy: pozice, normála, texturovací koordináty.
    Vstupní vrchol má také informace o čísle vrcholu: gl_VertexID.

    inVertex.attributes[0].v3 = /* pozice vertexu v modelovém prostoru  */ )." << str(pos  ) << R".(;
    inVertex.attributes[1].v3 = /* normala vertexu v modelovém prostoru */ )." << str(nor  ) << R".(;
    inVertex.attributes[2].v2 = /* tex. coordinaty                      */ )." << str(coord) << R".(;

    Shader ma přístup ke čtyřem maticím:
     - matice kamery                          - jedna pro celý model
     - matice světla                          - pro shadow mapping, jedna pro celý model
     - modelová matice                        - různá pro každý DrawCommand
     - inverzní transponovaná modelová matice - pro normálové vektory

    si.uniforms[getUniformLocation(si.gl_DrawID,PROJECTION_VIEW_MATRIX        )].m4 // matice kamery - cameraProj * cameraView
    si.uniforms[getUniformLocation(si.gl_DrawID,LIGHT_PROJECTION_VIEW_MATRIX  )].m4 // matice světla - lightProj * lightView - pro shadow mapping
    si.uniforms[getUniformLocation(si.gl_DrawID,MODEL_MATRIX                  )].m4 // modelMatrix
    si.uniforms[getUniformLocation(si.gl_DrawID,INVERSE_TRANSPOSE_MODEL_MATRIX)].m4 // invese transpose modelMatrix - pro normály

    si.gl_DrawId = )." << si.gl_DrawID << R".(

    Výstupem shaderu je:
     - outVertex.gl_Position      = ...; // pozice vertexu v clip-space kamery
     - outVertex.attributes[0].v3 = ...; // pozice vertexu ve world-space - pro výpočet osvětlení ve FS
     - outVertex.attributes[1].v3 = ...; // normala ve world-space - pro výpočet osvětlení ve FS
     - outVertex.attributes[2].v2 = ...; // texturovací souřadnice - pro čtení barvy povrchu z textury
     - outVertex.attributes[3].v4 = ...; // pozice vertexu v clip-space světla - pro stíny
    
    Pamatujte, pro transformaci pozice do clip-space je potřeba pronásobit vertex projekční, view i modelovou maticí.

    Normály by se měly násobit inverzní transponovanou maticí.
    Normály by měly mít homogenní složku = 0 protože to jsou vektory.
    Souřadnice do stínové mapy by měly být vynásobeny lightProjView a modelovou maticí.

    Něco se pokazilo...)."<<std::endl;
  };

  if(breakTest() || !equalVec4(expV.gl_Position,stuV.gl_Position)){
    printInfo();
    std::cerr << R".(
    Pozice gl_Position není správně.
    Měla být: )."<<str(expV.gl_Position)<<R".(
    Ale  je : )."<<str(stuV.gl_Position)<<R".(
    ).";
    REQUIRE(false);
  }

  if(breakTest() || !equalVec3(expV.attributes[0].v3,stuV.attributes[0].v3)){
    printInfo();
    std::cerr << R".(
    Pozice ve world space není správná.
    Měla být: )."<<str(expV.attributes[0].v3)<<R".(
    Ale  je : )."<<str(stuV.attributes[0].v3)<<R".(
    ).";
    REQUIRE(false);
  }

  if(breakTest() || !equalVec3(expV.attributes[1].v3,stuV.attributes[1].v3)){
    printInfo();
    std::cerr << R".(
    Normála ve world space není správná.
    Měla být: )."<<str(expV.attributes[1].v3)<<R".(
    Ale  je : )."<<str(stuV.attributes[1].v3)<<R".(
    ).";
    REQUIRE(false);
  }

  if(breakTest() || !equalVec2(expV.attributes[2].v2,stuV.attributes[2].v2)){
    printInfo();
    std::cerr << R".(
    Texturovací koordináty nejsou správné.
    Měly být: )."<<str(expV.attributes[2].v2)<<R".(
    Ale  je : )."<<str(stuV.attributes[2].v2)<<R".(
    ).";
    REQUIRE(false);
  }

  if(breakTest() || !equalVec4(expV.attributes[3].v4,stuV.attributes[3].v4)){
    printInfo();
    std::cerr << R".(
    Pozice vertexu v prostoru světla je nesprávná.
    Měly být: )."<<str(expV.attributes[3].v4)<<R".(
    Ale  je : )."<<str(stuV.attributes[3].v4)<<R".(
    ).";
    REQUIRE(false);
  }

}


