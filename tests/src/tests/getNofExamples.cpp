#include<framework/programContext.hpp>

void getNofExamples(){
  std::cout << ProgramContext::get().methods.methodFactories.size() << std::endl;
}
