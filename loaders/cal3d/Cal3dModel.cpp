// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: Cal3dModel.cpp,v 1.16 2005-01-06 12:46:54 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <cal3d/cal3d.h>
#include "Cal3dModel.h"
#include "Cal3dCoreModel.h"
#include <string>

#include "common/Log.h"
#include "common/Utility.h"
#include "src/System.h"
#include "renderers/Render.h"

#include "renderers/RenderSystem.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {
const int STATE_IDLE = 0;
const int STATE_FANCY = 1;
const int STATE_MOTION = 2;

float strut_blend[] = {0.5, 0.0, 0.0, 0.5};
float walk_blend[] = {0.0, 0.5, 0.0, 0.5};
float run_blend[] = {0.0, 0.0, 0.5, 0.5};

static const std::string STANDING = "standing";
static const std::string WALKING = "walking";
static const std::string RUNNING = "running";

static const std::string ANIM_default = "default";

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

Cal3dModel::Cal3dModel(Render *render) :
  Model(render),
  _initialised(false),
  _core_model(NULL),
  m_rotate(90.0f)
{
  m_renderScale = 1.0f;
  m_lodLevel = 1.0f;
  _height = 1.0f;
}

Cal3dModel::~Cal3dModel() {
  if (_initialised) shutdown();
}

bool Cal3dModel::init(Cal3dCoreModel *core_model) {
  if (_initialised) shutdown();
  assert(core_model && "Core model is NULL");
  _core_model = core_model;
  // create the model instance from the loaded core model
  if(!m_calModel.create(_core_model->getCalCoreModel())) {
    CalError::printLastError();
    return false;
  }

  // attach all meshes to the model
  for(int meshId = 0; meshId < _core_model->getCalCoreModel()->getCoreMeshCount(); ++meshId) {
    m_calModel.attachMesh(meshId);
  }

  // Set default material set
  m_calModel.setMaterialSet(0);

  // set initial animation state
  m_calModel.getMixer()->blendCycle(_core_model->_animations[STANDING], 1.0f, 0.0f);

  m_renderScale = _core_model->getScale();

  m_rotate = _core_model->getRotate();
  _initialised = true;
  return true;
}

void Cal3dModel::renderMesh(bool useTextures, bool useLighting, bool select_mode) {
  // get the renderer of the model
  CalRenderer *pCalRenderer;
  pCalRenderer = m_calModel.getRenderer();

  // begin the rendering loop
  if(!pCalRenderer->beginRendering()) return;

  // get the number of meshes
  int meshCount;
  meshCount = pCalRenderer->getMeshCount();

  // render all meshes of the model
  for(int meshId = 0; meshId < meshCount; ++meshId)  {
    // get the number of submeshes
    int submeshCount = pCalRenderer->getSubmeshCount(meshId);

    // render all submeshes of the mesh
    for(int submeshId = 0; submeshId < submeshCount; ++submeshId) {
      // select mesh and submesh for further data access
      if(pCalRenderer->selectMeshSubmesh(meshId, submeshId)) {
        static unsigned char meshColor[4];
        static float ambient[4];
        static float diffuse[4];
        static float specular[4];
	static float shininess;
	if (!select_mode) {
          pCalRenderer->getAmbientColor(&meshColor[0]);
          ambient[0] = meshColor[0] / 255.0f;  ambient[1] = meshColor[1] / 255.0f; ambient[2] = meshColor[2] / 255.0f; ambient[3] = meshColor[3] / 255.0f;

          // set the material diffuse color
          pCalRenderer->getDiffuseColor(&meshColor[0]);
          diffuse[0] = meshColor[0] / 255.0f;  diffuse[1] = meshColor[1] / 255.0f; diffuse[2] = meshColor[2] / 255.0f; diffuse[3] = 255.0f;//meshColor[3] / 255.0f;

          // set the material specular color
          pCalRenderer->getSpecularColor(&meshColor[0]);
          specular[0] = meshColor[0] / 255.0f;  specular[1] = meshColor[1] / 255.0f; specular[2] = meshColor[2] / 255.0f; specular[3] = meshColor[3] / 255.0f;

	  shininess = pCalRenderer->getShininess();

          _render->setMaterial(&ambient[0], &diffuse[0], &specular[0], shininess, NULL);
	}
	
        // get the transformed vertices of the submesh
        static Vertex_3 meshVertices[30000];
        int vertexCount;
        vertexCount = pCalRenderer->getVertices((float*)&meshVertices[0]);

        // get the transformed normals of the submesh
        static Normal meshNormals[30000];
        pCalRenderer->getNormals((float*)&meshNormals[0]);

        // get the texture coordinates of the submesh
        static Texel meshTextureCoordinates[30000];
        int textureCoordinateCount = pCalRenderer->getTextureCoordinates(0, (float*)&meshTextureCoordinates[0]);

        // get the faces of the submesh
        static int meshFaces[50000][3];
        int faceCount = pCalRenderer->getFaces(&meshFaces[0][0]);
        bool multitexture = false;
        // set the vertex and normal buffers
        if(!select_mode && (pCalRenderer->getMapCount() > 0) && (textureCoordinateCount > 0)) {
          if (pCalRenderer->getMapCount() == 1) {
            RenderSystem::getInstance().switchTexture((int)pCalRenderer->getMapUserData(0));
	  } else {
	    multitexture = true;
            RenderSystem::getInstance().switchTexture(0, (int)pCalRenderer->getMapUserData(0));
            RenderSystem::getInstance().switchTexture(1, (int)pCalRenderer->getMapUserData(1));
	  }
          _render->renderElements(Graphics::RES_TRIANGLES, faceCount * 3, &meshFaces[0][0], &meshVertices[0], &meshTextureCoordinates[0], &meshNormals[0], multitexture);
	} else {
          _render->renderElements(Graphics::RES_TRIANGLES, faceCount * 3, &meshFaces[0][0], &meshVertices[0], NULL, &meshNormals[0], false);
	}
      }
    }
  }
  pCalRenderer->endRendering();
}

void Cal3dModel::render(bool useTextures, bool useLighting, bool select_mode) {
  if (_render) {
    float scale = _height * getScale();
    _render->scaleObject(scale);
    _render->rotate(m_rotate,0.0f,0.0f,1.0f); //so zero degrees points east    
    renderMesh(useTextures, useLighting, select_mode);
  }
}

void Cal3dModel::update(float time_elapsed) {
  // update the model
  m_calModel.update(time_elapsed);
}

void Cal3dModel::shutdown() {
  // destroy the model instance
  m_calModel.destroy();
  _initialised = false;
}

void Cal3dModel::setLodLevel(float lodLevel) {
  m_lodLevel = lodLevel;
  // set the new lod level in the cal model renderer
  m_calModel.setLodLevel(m_lodLevel);
}

void Cal3dModel::action(const std::string &action) {
  Cal3dCoreModel::AnimationMap animations = _core_model->_animations;
  if (action == "standing") {
    m_calModel.getMixer()->blendCycle(animations[STANDING], 1.0f, 0.2f);
    m_calModel.getMixer()->clearCycle(animations[WALKING],  0.2f);
    m_calModel.getMixer()->clearCycle(animations[RUNNING],  0.2f);
  } else if (action == "walking") {
    m_calModel.getMixer()->blendCycle(animations[WALKING], 1.0f, 0.2f);
    m_calModel.getMixer()->clearCycle(animations[RUNNING],  0.2f);
    m_calModel.getMixer()->clearCycle(animations[STANDING],  0.2f);
  } else if (action == "running") {
    m_calModel.getMixer()->blendCycle(animations[RUNNING], 1.0f, 0.2f);
    m_calModel.getMixer()->clearCycle(animations[WALKING],  0.2f);
    m_calModel.getMixer()->clearCycle(animations[STANDING],  0.2f);
  } else {
//    std::string act = *(animations.find(action)->);
    if (animations.find(action) != animations.end()) {
      m_calModel.getMixer()->executeAction(animations[action], 0.0f, 0.0f);
    } else {
      m_calModel.getMixer()->executeAction(animations[ANIM_default], 0.0f, 0.0f);
    }
  }
}

void Cal3dModel::setAppearance(Atlas::Message::Element::MapType &map) {
  std::cout << "------" << std::endl;

  // Get mesh atrributes
  Atlas::Message::Element::MapType::const_iterator I = map.find("mesh");
  if (I == map.end()) {
    std::cout << "No 'mesh' found -- using defaults" << std::endl;
    // instantiate defaults
    Atlas::Message::Element::MapType meshes;
    for (Cal3dCoreModel::MeshMap::const_iterator J = _core_model->_meshes.begin(); J != _core_model->_meshes.end(); ++J) {
      std::string name = J->first;
      if (name.find("_") != std::string::npos)
        meshes[name.substr(0, name.find("_"))] = "1";
    }
    map["mesh"] = meshes;
    I = map.find("mesh");   
  }
  const Atlas::Message::Element::MapType meshes = I->second.asMap();
  I = map.find("material");
  if (I == map.end()) {
    // instantiate defaults
    map["material"] = Atlas::Message::Element::MapType();
    I = map.find("material");
  }
  const Atlas::Message::Element::MapType materials = I->second.asMap();


  // Make sure only single val meshes are attached
  for (Cal3dCoreModel::MeshMap::const_iterator J = _core_model->_meshes.begin(); J != _core_model->_meshes.end(); ++J) {
    std::string name = J->first;
    if (name.find("_") != std::string::npos)
      m_calModel.detachMesh(_core_model->_meshes[name]);
  }

  // Attach meshes and set materials 
  for (I = meshes.begin(); I != meshes.end(); ++I) {
    std::string name = I->first;
    std::string value = I->second.asString();
    std::cout << "Name: " << name << " - Value: " << value << std::endl;
    // Attach mesh
    if (_core_model->_meshes.find(name + "_" + value) 
      != _core_model->_meshes.end()) {
      m_calModel.attachMesh(_core_model->_meshes[name + "_" + value]);
    // Set material set
    
    Atlas::Message::Element::MapType::const_iterator K = materials.find(name);
    if (K != materials.end()) {
      setMaterialPartSet(name + "_" + value, K->second.asString());
    } else {
      // set default material
      setMaterialPartSet(name + "_" + value, "default");
//      materials[name] = "default";
    }
    }
  }
  for (I = materials.begin(); I != materials.end(); ++I) {
    std::string name = I->first;
    std::string value = I->second.asString();
    setMaterialPartSet(name, value);
  }
}

void Cal3dModel::setMaterialSet(unsigned int set) {
  m_calModel.setMaterialSet(set);
}

void Cal3dModel::setMaterialPartSet(unsigned int msh, unsigned int set) {
 std::cout << "Mesh: " << msh << " - Set: " << set << std::endl;
  //TODO make this do the correct thing!
//  m_calModel.setMaterialSet(set);
  // Get mesh name
  CalMesh *mesh = m_calModel.getMesh(msh);
  if (mesh) {
    mesh->setMaterialSet(set - 1);
  }
}

std::list<std::string> Cal3dModel::getMaterialNames() {
  return _core_model->_material_list;
}

std::list<std::string> Cal3dModel::getMeshNames() {
  std::list<std::string> l;
  for (Cal3dCoreModel::MeshMap::const_iterator I = _core_model->_meshes.begin(); I != _core_model->_meshes.end(); ++I) {
    l.push_back(I->first);
  }
  return l;
}
/*
void Cal3dModel::showMesh(const std::string &mesh) {
  int meshId = _core_model->meshes[mesh];  
  m_calModel.attachMesh(meshId);
}
void Cal3dModel::showMesh(int meshId) {
  m_calModel.attachMesh(meshId);
}

void Cal3dModel::hideMesh(const std::string &mesh) {
  int meshId = _core_model->meshes[mesh];  
  m_calModel.detachMesh(meshId);
}
void Cal3dModel::hideMesh(int meshId) {
  m_calModel.detachMesh(meshId);
}
*/
} /* namespace Sear */
