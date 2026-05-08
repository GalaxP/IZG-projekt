#include <catch2/interfaces/catch_interfaces_registry_hub.hpp>
#include <assert.h>

#include<framework/window.hpp>
#include<framework/application.hpp>
#include<framework/arguments.hpp>
#include<framework/systemSpecific.hpp>
#include<tests/conformanceTests.hpp>
#include<tests/performanceTest.hpp>
#include<tests/takeScreenShot.hpp>
#include<tests/getNofExamples.hpp>

void mainBody(int argc,char*argv[]){
  auto&args = ProgramContext::get().args = Arguments(argc,argv);

  extern TaskFunctions taskFunctions_teacher;
  if(taskFunctions_teacher.idToBreak)
    *taskFunctions_teacher.idToBreak = args.idToBreak;
  if(taskFunctions_teacher.lineToBreak)
    *taskFunctions_teacher.lineToBreak = args.lineToBreak;

  if(args.stop)
    return;

  if(args.getNofExamples){
    getNofExamples();
    return;
  }

  if(args.runConformanceTests){
    runConformanceTests(args.modelFile,args.mseThreshold,args.selectedTest,args.upToTest);
    return;
  }

  if(args.runPerformanceTests){
    runPerformanceTest(args.perfTests);
    return;
  }

  if(args.takeScreenShot){
    takeScreenShot();
    return;
  }

  auto app = Application(args.windowSize[0],args.windowSize[1]);
  app.setMethod(args.method);
  app.start();
}

int main(int argc,char*argv[]){
  //system specific initialization and deinitialization on object destructor
  auto system = System();

  try{
    mainBody(argc,argv);
  }catch(std::exception&e){
    std::cerr << e.what() << std::endl;
  }

  Catch::cleanUp();
  ProgramContext::free();
  return EXIT_SUCCESS;
}

