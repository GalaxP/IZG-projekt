#include <functional>

#include<tests/model/createModel.hpp>

namespace tests::model{


TestModel::TestModel():Model(){}

TestModel::TestModel(TestModel const&m){
  copy(m);
}

TestModel&TestModel::operator=(TestModel const&m){
  copy(m);
  return *this;
}

TestModel::~TestModel(){
  free(*(Model*)this);
}

void copyNode(Node&o,Node const&i){
  o.mesh        = i.mesh;
  o.modelMatrix = i.modelMatrix;
  o.nofChildren = i.nofChildren;
  o.children = new Node[o.nofChildren];
  for(size_t k=0;k<o.nofChildren;++k)
    copyNode(o.children[k],i.children[k]);
}

void TestModel::copy(TestModel const&m){
  free(*(Model*)this);

  nofBuffers  = m.nofBuffers ;
  nofMeshes   = m.nofMeshes  ;
  nofRoots    = m.nofRoots   ;
  nofTextures = m.nofTextures;

  buffers  = new Buffer [nofBuffers ];
  meshes   = new Mesh   [nofMeshes  ];
  roots    = new Node   [nofRoots   ];
  textures = new Texture[nofTextures];

  for(size_t i=0;i<nofBuffers ;++i)buffers [i] = m.buffers [i];
  for(size_t i=0;i<nofMeshes  ;++i)meshes  [i] = m.meshes  [i];
  for(size_t i=0;i<nofTextures;++i)textures[i] = m.textures[i];
  for(size_t i=0;i<nofRoots   ;++i)copyNode(roots[i],m.roots[i]);
}

NodeI::NodeI(
    int32_t                 m ,  
    std::vector<NodeI>const&c ,   
    glm::mat4         const&mm):mesh(m),children(c),model(mm){}

void insert(Node&o,NodeI const&i){
  o.nofChildren = i.children.size();
  o.mesh        = i.mesh           ;
  o.modelMatrix = i.model          ;
  o.children    = new Node[o.nofChildren];
  for(size_t k=0;k<o.nofChildren;++k)
    insert(o.children[k],i.children[k]);
}

TestModel createModel(
    std::vector<MeshI>const&meshes,
    std::vector<NodeI>const&roots){
  TestModel model;
  model.nofMeshes = meshes.size();
  model.meshes    = new Mesh[model.nofMeshes];
  for(size_t i=0;i<meshes.size();++i){
    auto&mesh = model.meshes[i];
    mesh.nofIndices     = meshes[i].nofIndices    ;
    mesh.diffuseColor   = meshes[i].diffuseColor  ;
    mesh.diffuseTexture = meshes[i].diffuseTexture;
    mesh.doubleSided    = meshes[i].doubleSided   ;
  }

  model.nofRoots = roots.size();
  model.roots = new Node[model.nofRoots];
  for(size_t i=0;i<model.nofRoots;++i)
    insert(model.roots[i],roots.at(i));

  return model;
}
}
