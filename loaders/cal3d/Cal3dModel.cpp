// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Cal3dModel.cpp,v 1.5 2003-03-23 19:51:49 simon Exp $

#include <cal3d/cal3d.h>
#include "Cal3dModel.h"
#include "Cal3dCoreModel.h"
#include <string>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "common/Log.h"
#include "common/Utility.h"
#include "src/System.h"
#include "src/Render.h"

#ifdef HAVE_CONFIG
  #include "config.h"
#endif

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
  _core_model(NULL)
{
  m_state = STATE_IDLE;
  m_motionBlend[0] = 0.6f;
  m_motionBlend[1] = 0.1f;
  m_motionBlend[2] = 0.3f;
//  m_animationCount = 0;
//  m_meshCount = 0;
  m_renderScale = 1.0f;
  m_lodLevel = 1.0f;
//  _use_textures = false;
//  grip = false;
  _height = 1.0f;
}

Cal3dModel::~Cal3dModel() {
  if (_initialised) shutdown();
}

void Cal3dModel::getMotionBlend(float *pMotionBlend) {
  pMotionBlend[0] = m_motionBlend[0];
  pMotionBlend[1] = m_motionBlend[1];
  pMotionBlend[2] = m_motionBlend[2];
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
  m_state = STATE_IDLE;
  m_calModel.getMixer()->blendCycle(_core_model->_animations[STANDING], 1.0f, 0.0f);

  m_renderScale = _core_model->getScale();
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
          diffuse[0] = meshColor[0] / 255.0f;  diffuse[1] = meshColor[1] / 255.0f; diffuse[2] = meshColor[2] / 255.0f; diffuse[3] = meshColor[3] / 255.0f;

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
            _render->switchTexture((int)pCalRenderer->getMapUserData(0));
	  } else {
	    multitexture = true;
            _render->switchTexture(0, (int)pCalRenderer->getMapUserData(0));
            _render->switchTexture(1, (int)pCalRenderer->getMapUserData(1));
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
    _render->rotate(90.0f,0.0f,0.0f,1.0f); //so zero degrees points east    
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

void Cal3dModel::setMotionBlend(float *pMotionBlend, float delay) {
  m_motionBlend[0] = pMotionBlend[0];
  m_motionBlend[1] = pMotionBlend[1];
  m_motionBlend[2] = pMotionBlend[2];

  Cal3dCoreModel::AnimationMap animations = _core_model->_animations;
  
  m_calModel.getMixer()->clearCycle(animations["idle"], delay);
  m_calModel.getMixer()->clearCycle(animations["funky"], delay);
  m_calModel.getMixer()->blendCycle(animations["strut"], m_motionBlend[0], delay);
  m_calModel.getMixer()->blendCycle(animations["walk"], m_motionBlend[1], delay);
  m_calModel.getMixer()->blendCycle(animations["run"], m_motionBlend[2], delay);

  m_state = STATE_MOTION;
}

void Cal3dModel::setState(int state, float delay) {
  // check if this is really a new state
  if(state != m_state) {
    Cal3dCoreModel::AnimationMap animations = _core_model->_animations;
    if(state == STATE_IDLE) {
      m_calModel.getMixer()->blendCycle(animations["idle"], 1.0f, delay);
      m_calModel.getMixer()->clearCycle(animations["funky"], delay);
      m_calModel.getMixer()->clearCycle(animations["strut"], delay);
      m_calModel.getMixer()->clearCycle(animations["walk"], delay);
      m_calModel.getMixer()->clearCycle(animations["run"], delay);
      m_state = STATE_IDLE;
    }
    else if(state == STATE_FANCY)
    {
      m_calModel.getMixer()->clearCycle(animations["idle"], delay);
      m_calModel.getMixer()->blendCycle(animations["funky"], 1.0f, delay);
      m_calModel.getMixer()->clearCycle(animations["strut"], delay);
      m_calModel.getMixer()->clearCycle(animations["walk"], delay);
      m_calModel.getMixer()->clearCycle(animations["run"], delay);
      m_state = STATE_FANCY;
    }
    else if(state == STATE_MOTION)
    {
      m_calModel.getMixer()->clearCycle(animations["idle"], delay);
      m_calModel.getMixer()->clearCycle(animations["funky"], delay);
      m_calModel.getMixer()->blendCycle(animations["strut"], m_motionBlend[0], delay);
      m_calModel.getMixer()->blendCycle(animations["walk"], m_motionBlend[1], delay);
      m_calModel.getMixer()->blendCycle(animations["run"], m_motionBlend[2], delay);
      m_state = STATE_MOTION;
    }
  }
}

void Cal3dModel::action(const std::string &action) {
  Cal3dCoreModel::AnimationMap animations = _core_model->_animations;
  if (action == "standing") {
    m_calModel.getMixer()->blendCycle(animations[STANDING], 1.0f, 0.1f);
    m_calModel.getMixer()->clearCycle(animations[WALKING],  0.1f);
    m_calModel.getMixer()->clearCycle(animations[RUNNING],  0.1f);
  } else if (action == "walking") {
    m_calModel.getMixer()->blendCycle(animations[WALKING], 1.0f, 0.1f);
    m_calModel.getMixer()->clearCycle(animations[RUNNING],  0.1f);
    m_calModel.getMixer()->clearCycle(animations[STANDING],  0.1f);
  } else if (action == "running") {
    m_calModel.getMixer()->blendCycle(animations[RUNNING], 1.0f, 0.1f);
    m_calModel.getMixer()->clearCycle(animations[WALKING],  0.1f);
    m_calModel.getMixer()->clearCycle(animations[STANDING],  0.1f);
  } else {
    m_calModel.getMixer()->executeAction(animations[ANIM_default], 0.0f, 0.0f);
  }
}

void Cal3dModel::setMaterialSet(unsigned int set) {
  m_calModel.setMaterialSet(set);
}

void Cal3dModel::setMaterialPartSet(unsigned int set, unsigned int msh) {
  //TODO make this do the correct thing!
//  m_calModel.setMaterialSet(set);
  // Get mesh name
  CalMesh *mesh = m_calModel.getMesh(msh);
  mesh->setMaterialSet(set);
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
} /* namespace Sear */
