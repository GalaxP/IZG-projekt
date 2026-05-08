#include <tests/renderMethodFrame.hpp>

#include <memory>

#include <framework/application.hpp>
#include <examples/izg2026.hpp>
#include <framework/programContext.hpp>

#include <BasicCamera/OrbitCamera.h>
#include <BasicCamera/PerspectiveCamera.h>

#include <solutionInterface/taskFunctions.hpp>

#include <framework/programContext.hpp>

void renderMethodFrame(Framebuffer&frame){

  auto&sm = ProgramContext::get().args.method;
  auto&mr = ProgramContext::get().methods;


  GPUMemory mem;
  mem.framebuffers[mem.defaultFramebuffer] = frame;
  auto method = mr.methodFactories[sm](mem,&*mr.methodConstructData[sm]);

  auto orbitCamera       = basicCamera::OrbitCamera();
  auto perspectiveCamera = basicCamera::PerspectiveCamera();
  glm::vec3 light;

  defaultSceneParameters(orbitCamera,perspectiveCamera,light,frame.width,frame.height);

  SceneParam sceneParam;

  sceneParam.proj   = perspectiveCamera.getProjection();
  sceneParam.view   = orbitCamera      .getView      ();
  sceneParam.camera = glm::vec3(glm::inverse(sceneParam.view)*glm::vec4(0.f,0.f,0.f,1.f));
  sceneParam.light  = light;

  method->onDraw(sceneParam);
}
