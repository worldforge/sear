// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: Cal3dModel.cpp,v 1.33 2006-02-15 14:39:54 simon Exp $

#include <cal3d/cal3d.h>
#include "Cal3dModel.h"
#include "Cal3dCoreModel.h"
#include <string>

#include "common/Log.h"
#include "common/Utility.h"
#include "src/System.h"

#include "renderers/Render.h"
#include "renderers/RenderSystem.h"

#include "DynamicObject.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

static const std::string IDLE     = "idle";
static const std::string STANDING = "standing";
static const std::string WALKING  = "walking";
static const std::string RUNNING  = "running";

static const std::string ANIM_default = "default";

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

Cal3dModel::Cal3dModel() :
  Model(),
  m_initialised(false),
  m_core_model(NULL),
  m_calModel(NULL),
  m_rotate(90.0f)
{
  m_renderScale = 1.0f;
  m_lodLevel = 1.0f;
}

Cal3dModel::~Cal3dModel() {
  assert(m_initialised == false);
}

int Cal3dModel::init(Cal3dCoreModel *core_model) {
  assert(m_initialised == false);

  assert(core_model && "Core model is NULL");
  m_core_model = core_model;
  // create the model instance from the loaded core model
  if(!(m_calModel = new CalModel(m_core_model->getCalCoreModel()))) {
    CalError::printLastError();
    return false;
  }

  // attach all meshes to the model
  for(int meshId = 0; meshId < m_core_model->getCalCoreModel()->getCoreMeshCount(); ++meshId) {
    m_calModel->attachMesh(meshId);
  }

  // Set default material set
  m_calModel->setMaterialSet(0);

  // set initial animation state
  m_calModel->getMixer()->blendCycle(m_core_model->m_animations[STANDING], 1.0f, 0.0f);

  m_renderScale = m_core_model->getScale();

  m_rotate = m_core_model->getRotate();
  m_initialised = true;
  return 0;
}

void Cal3dModel::renderMesh(bool useTextures, bool useLighting, bool select_mode) {
  // get the renderer of the model
  CalRenderer *pCalRenderer = m_calModel->getRenderer();
  assert(pCalRenderer !=  NULL);

  // begin the rendering loop
  if (!pCalRenderer->beginRendering()) {
    // Some kind of error here!
    return;
  }

  // get the number of meshes
  int meshCount = pCalRenderer->getMeshCount();
  int numSubMeshes = 0;
  for (int i = 0; i < meshCount; ++i) {
    numSubMeshes += pCalRenderer->getSubmeshCount(i);
  }
  m_dos.resize(numSubMeshes);

  int counter = -1;
  // render all meshes of the model
  for(int meshId = 0; meshId < meshCount; ++meshId)  {
    // get the number of submeshes
    int submeshCount = pCalRenderer->getSubmeshCount(meshId);

    // render all submeshes of the mesh
    for(int submeshId = 0; submeshId < submeshCount; ++submeshId) {
      // select mesh and submesh for further data access
      if(pCalRenderer->selectMeshSubmesh(meshId, submeshId)) {

        SPtrShutdown<DynamicObject> dyno = m_dos[++counter];
        if (!dyno.isValid()) {
          dyno = SPtrShutdown<DynamicObject>(new DynamicObject);
          dyno->init();
          m_dos[counter] = dyno;
        }

        static unsigned char meshColor[4];
        static float ambient[4];
        static float diffuse[4];
        static float specular[4];
	static float shininess;

        pCalRenderer->getAmbientColor(&meshColor[0]);
        ambient[0] = meshColor[0] / 255.0f;
        ambient[1] = meshColor[1] / 255.0f;
        ambient[2] = meshColor[2] / 255.0f;
        ambient[3] = meshColor[3] / 255.0f;
        dyno->setAmbient(ambient);

        // set the material diffuse color
        pCalRenderer->getDiffuseColor(&meshColor[0]);
        diffuse[0] = meshColor[0] / 255.0f;
        diffuse[1] = meshColor[1] / 255.0f;
        diffuse[2] = meshColor[2] / 255.0f;
        diffuse[3] = 1.0f;//meshColor[3] / 255.0f;
        dyno->setDiffuse(diffuse);

        // set the material specular color
        pCalRenderer->getSpecularColor(&meshColor[0]);
        specular[0] = meshColor[0] / 255.0f;
        specular[1] = meshColor[1] / 255.0f;
        specular[2] = meshColor[2] / 255.0f;
        specular[3] = meshColor[3] / 255.0f;
        dyno->setSpecular(specular);

        dyno->setEmission(0.0f, 0.0f, 0.0f,0.0f);

        shininess = pCalRenderer->getShininess();
        dyno->setShininess(shininess);

        // get the transformed vertices of the submesh
        static Vertex_3 meshVertices[30000];
        int vertexCount;
        vertexCount = pCalRenderer->getVertices((float*)&meshVertices[0]);
        dyno->copyVertexData((float*)meshVertices, vertexCount * 3);

        // get the transformed normals of the submesh
        static Normal meshNormals[30000];
        pCalRenderer->getNormals((float*)&meshNormals[0]);
        dyno->copyNormalData((float*)meshNormals, vertexCount * 3);

        // get the texture coordinates of the submesh
        static Texel meshTextureCoordinates[30000];
        int textureCoordinateCount = pCalRenderer->getTextureCoordinates(0, (float*)&meshTextureCoordinates[0]);
        dyno->copyTextureData((float*)meshTextureCoordinates, textureCoordinateCount * 2);

        // get the faces of the submesh
        static int meshFaces[50000 * 3];
        int faceCount = pCalRenderer->getFaces(&meshFaces[0]);
        dyno->copyIndices(meshFaces, faceCount * 3);

        dyno->setNumPoints(faceCount * 3);

        bool multitexture = false;
        // TODO handle missing MapData more sensibly.
        // set the vertex and normal buffers
        if((pCalRenderer->getMapCount() > 0) && (textureCoordinateCount > 0)) {
          if (pCalRenderer->getMapCount() == 1) {
            MapData *md = reinterpret_cast<MapData*>
                                          (pCalRenderer->getMapUserData(0));
            if (md) {
              dyno->setTexture(0, md->textureID, md->textureMaskID);
            } else {
              dyno->setTexture(0, 0, 0);
            }
	  } else {
	    multitexture = true;
            // Set texture unit 0
            MapData *md = reinterpret_cast<MapData*>
                                          (pCalRenderer->getMapUserData(0));
            if (md) {
              dyno->setTexture(1, md->textureID, md->textureMaskID);
            } else {
              dyno->setTexture(0, 0, 0);
            }
            // Set texture unit 1
            md = reinterpret_cast<MapData*>(pCalRenderer->getMapUserData(1));
            if (md) {
              dyno->setTexture(1, md->textureID, md->textureMaskID);
            } else {
              dyno->setTexture(1, 0, 0);
            }
	  }
	}
      }
    }
  }
  pCalRenderer->endRendering();
}

void Cal3dModel::render(bool useTextures, bool useLighting, bool select_mode) {
  // TODO Make this into a matrix?
  Render *render = RenderSystem::getInstance().getRenderer();
  float scale = getScale();
  render->scaleObject(scale);
  render->rotate(m_rotate,0.0f,0.0f,1.0f); //so zero degrees points east

  DOVec::iterator I = m_dos.begin();
  while (I != m_dos.end()) {
    SPtrShutdown<DynamicObject> dyno = *I++;
    dyno->render(select_mode);
  }

}

void Cal3dModel::update(float time_elapsed) {
  // update the model
  m_calModel->update(time_elapsed);
  renderMesh(true, true, false);
}

int Cal3dModel::shutdown() {
  assert (m_initialised == true);
  // destroy the model instance
  if (m_calModel) {
    delete m_calModel;
    m_calModel = NULL;
  }
  m_initialised = false;
  return 0;
}

void Cal3dModel::setLodLevel(float lodLevel) {
  m_lodLevel = lodLevel;
  // set the new lod level in the cal model renderer
  m_calModel->setLodLevel(m_lodLevel);
}

void Cal3dModel::animate(const std::string &action) {
  Cal3dCoreModel::AnimationMap animations = m_core_model->m_animations;
  Cal3dCoreModel::Animations anims = m_core_model->m_anims;

  Cal3dCoreModel::Animations::const_iterator anim_cur = anims.find(m_cur_anim);
  // First clear previous animations.
  if (anim_cur != anims.end()) {
    // Clear previous combined anim
    removeAnimation(anim_cur->second);
  } else {
    // Clear previous single anim
    m_calModel->getMixer()->clearCycle(animations[WALKING],  0.2f);
    m_calModel->getMixer()->clearCycle(animations[RUNNING],  0.2f);
    m_calModel->getMixer()->clearCycle(animations[STANDING], 0.2f);
    m_calModel->getMixer()->clearCycle(animations[IDLE], 0.2f);
  }
 
  // See if there is a combined animation defined.
  Cal3dCoreModel::Animations::const_iterator anim_next = anims.find(action);
  if (anim_next != anims.end()) {
    addAnimation(anim_next->second);
    m_cur_anim = action;
    return;
  }
  // Else play named anim directly
  m_cur_anim = "";
  // Check default 
  if (action == IDLE) {
//    m_calModel->getMixer()->clearCycle(animations[WALKING],  0.2f);
//    m_calModel->getMixer()->clearCycle(animations[RUNNING],  0.2f);
//    m_calModel->getMixer()->clearCycle(animations[STANDING], 0.2f);
    m_calModel->getMixer()->blendCycle(animations[IDLE], 1.0f, 0.2f);
  } else if (action == STANDING) {
//    m_calModel->getMixer()->clearCycle(animations[IDLE], 0.2f);
//    m_calModel->getMixer()->clearCycle(animations[WALKING],  0.2f);
//    m_calModel->getMixer()->clearCycle(animations[RUNNING],  0.2f);
    m_calModel->getMixer()->blendCycle(animations[STANDING], 1.0f, 0.2f);
  } else if (action == WALKING) {
//    m_calModel->getMixer()->clearCycle(animations[IDLE], 0.2f);
//    m_calModel->getMixer()->clearCycle(animations[RUNNING],  0.2f);
//    m_calModel->getMixer()->clearCycle(animations[STANDING],  0.2f);
    m_calModel->getMixer()->blendCycle(animations[WALKING], 1.0f, 0.2f);
  } else if (action == RUNNING) {
//    m_calModel->getMixer()->clearCycle(animations[IDLE], 0.2f);
//    m_calModel->getMixer()->clearCycle(animations[WALKING],  0.2f);
//    m_calModel->getMixer()->clearCycle(animations[STANDING],  0.2f);
    m_calModel->getMixer()->blendCycle(animations[RUNNING], 1.0f, 0.2f);
//  } else {
    // Search map for animation with matching name
//    if (animations.find(action) != animations.end()) {
//      m_calModel->getMixer()->executeAction(animations[action], 0.0f, 0.0f);
//    } else {
//      // Play default animation if none others found
//      m_calModel->getMixer()->executeAction(animations[ANIM_default], 0.0f, 0.0f);
//    }
  }
}


void Cal3dModel::action(const std::string &action) {
  Cal3dCoreModel::AnimationMap animations = m_core_model->m_animations;
#if(0)
  Cal3dCoreModel::Animations anims = m_core_model->m_anims;

  Cal3dCoreModel::Animations::const_iterator anim_cur = anims.find(m_cur_anim);
  // First clear previous animations.
  if (anim_cur != anims.end()) {
    // Clear previous combined anim
    removeAnimation(anim_cur->second);
  } else {
    // Clear previous single anim
    m_calModel->getMixer()->clearCycle(animations[WALKING],  0.2f);
    m_calModel->getMixer()->clearCycle(animations[RUNNING],  0.2f);
    m_calModel->getMixer()->clearCycle(animations[STANDING], 0.2f);
    m_calModel->getMixer()->clearCycle(animations[IDLE], 0.2f);
  }
 
  // See if there is a combined animation defined.
  Cal3dCoreModel::Animations::const_iterator anim_next = anims.find(action);
  if (anim_next != anims.end()) {
    addAnimation(anim_next->second);
    m_cur_anim = action;
    return;
  }
  // Else play named anim directly
  m_cur_anim = "";
  // Check default 
  if (action == IDLE) {
//    m_calModel->getMixer()->clearCycle(animations[WALKING],  0.2f);
//    m_calModel->getMixer()->clearCycle(animations[RUNNING],  0.2f);
//    m_calModel->getMixer()->clearCycle(animations[STANDING], 0.2f);
    m_calModel->getMixer()->blendCycle(animations[IDLE], 1.0f, 0.2f);
  } else if (action == STANDING) {
//    m_calModel->getMixer()->clearCycle(animations[IDLE], 0.2f);
//    m_calModel->getMixer()->clearCycle(animations[WALKING],  0.2f);
//    m_calModel->getMixer()->clearCycle(animations[RUNNING],  0.2f);
    m_calModel->getMixer()->blendCycle(animations[STANDING], 1.0f, 0.2f);
  } else if (action == WALKING) {
//    m_calModel->getMixer()->clearCycle(animations[IDLE], 0.2f);
//    m_calModel->getMixer()->clearCycle(animations[RUNNING],  0.2f);
//    m_calModel->getMixer()->clearCycle(animations[STANDING],  0.2f);
    m_calModel->getMixer()->blendCycle(animations[WALKING], 1.0f, 0.2f);
  } else if (action == RUNNING) {
//    m_calModel->getMixer()->clearCycle(animations[IDLE], 0.2f);
//    m_calModel->getMixer()->clearCycle(animations[WALKING],  0.2f);
//    m_calModel->getMixer()->clearCycle(animations[STANDING],  0.2f);
    m_calModel->getMixer()->blendCycle(animations[RUNNING], 1.0f, 0.2f);
  } else {
    // Search map for animation with matching name
#endif
    if (animations.find(action) != animations.end()) {
      m_calModel->getMixer()->executeAction(animations[action], 0.0f, 0.0f);
    } else {
      // Play default animation if none others found
      m_calModel->getMixer()->executeAction(animations[ANIM_default], 0.0f, 0.0f);
    }
//  }
}

void Cal3dModel::setAppearance(const Atlas::Message::MapType &appearanceMap) {
//  if (debug) std::cout << "------" << std::endl;
  Atlas::Message::MapType map(appearanceMap);
  
  // Get mesh atrributes
  Atlas::Message::MapType::const_iterator I = map.find("mesh");
  if (I == map.end()) {
    std::cout << "No 'mesh' found -- using defaults" << std::endl;
    // instantiate defaults
    Atlas::Message::MapType meshes;
    for (Cal3dCoreModel::MeshMap::const_iterator J = m_core_model->m_meshes.begin(); J != m_core_model->m_meshes.end(); ++J) {
      std::string name = J->first;
      if (name.find("_") != std::string::npos)
        meshes[name.substr(0, name.find("_"))] = "1";
    }
    map["mesh"] = meshes;
    I = map.find("mesh");   
  }
  const Atlas::Message::MapType meshes = I->second.asMap();
  I = map.find("material");
  if (I == map.end()) {
    // instantiate defaults
    map["material"] = Atlas::Message::MapType();
    I = map.find("material");
  }
  const Atlas::Message::MapType materials = I->second.asMap();


  // Make sure only single val meshes are attached
  for (Cal3dCoreModel::MeshMap::const_iterator J = m_core_model->m_meshes.begin(); J != m_core_model->m_meshes.end(); ++J) {
    std::string name = J->first;
    if (name.find("_") != std::string::npos)
      m_calModel->detachMesh(m_core_model->m_meshes[name]);
  }

  // Attach meshes and set materials 
  for (I = meshes.begin(); I != meshes.end(); ++I) {
    std::string name = I->first;
    std::string value = I->second.asString();
//    if (debug) std::cout << "Name: " << name << " - Value: " << value << std::endl;
    // Attach mesh
    if (m_core_model->m_meshes.find(name + "_" + value) 
      != m_core_model->m_meshes.end()) {
      m_calModel->attachMesh(m_core_model->m_meshes[name + "_" + value]);
    // Set material set
    
    Atlas::Message::MapType::const_iterator K = materials.find(name);
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
  m_calModel->setMaterialSet(set);
}

void Cal3dModel::setMaterialPartSet(unsigned int msh, unsigned int set) {
//  if (debug) std::cout << "Mesh: " << msh << " - Set: " << set << std::endl;
  //TODO make this do the correct thing!
//  m_calModel.setMaterialSet(set);
  // Get mesh name
  CalMesh *mesh = m_calModel->getMesh(msh);
  if (mesh) {
    mesh->setMaterialSet(set - 1);
  }
}

std::list<std::string> Cal3dModel::getMaterialNames() {
  return m_core_model->m_material_list;
}

std::list<std::string> Cal3dModel::getMeshNames() {
  std::list<std::string> l;
  for (Cal3dCoreModel::MeshMap::const_iterator I = m_core_model->m_meshes.begin(); I != m_core_model->m_meshes.end(); ++I) {
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

PosAndOrient Cal3dModel::getPositionForSubmodel(const std::string&)
{
    PosAndOrient po;
    po.orient.identity();
    po.pos = WFMath::Vector<3>(0, 0, 0);

    // Get a pointer to the bone we need from cal3d
    CalSkeleton * cs = m_calModel->getSkeleton();
    if (cs == 0) {
        return po;
    }
    CalCoreSkeleton * ccs = cs->getCoreSkeleton();
    if (ccs == 0) {
        return po;
    }
    int boneId = ccs->getCoreBoneId("Bip01 R Hand");
    if (boneId == -1) {
        return po;
    }
    CalBone * cb1 = cs->getBone(boneId);
    if (cb1 == 0) {
        return po;
    }

    // Get the position and orientation of the bone in cal3d coordinates
    const CalQuaternion & cq2 = cb1->getRotationAbsolute();
    const CalVector & cv1 = cb1->getTranslationAbsolute();

    // Rotate the orienation into out coordinate system
    po.orient = WFMath::Quaternion(0, 3.14) * WFMath::Quaternion(1, 3.14 / 2) * WFMath::Quaternion(cq2.w, -cq2.x, -cq2.y, -cq2.z);
    
    // Rotate the vector into our coordinate system
    po.pos = WFMath::Vector<3>(cv1.x * 0.025, cv1.y * 0.025, cv1.z * 0.025);
    po.pos = po.pos.rotate(WFMath::Quaternion(2, 3.1415927f / 2.f));

    return po;
}

void Cal3dModel::addAnimation(const Cal3dCoreModel::WeightList &list) {
  Cal3dCoreModel::AnimationMap animations = m_core_model->m_animations;
  Cal3dCoreModel::WeightList::const_iterator I = list.begin();
  while (I != list.end()) {
    std::string name = (*I).first;
    double weight = (*I).second;
    m_calModel->getMixer()->blendCycle(animations[name], weight, 0.2f);
    ++I;
  }
}

void Cal3dModel::removeAnimation(const Cal3dCoreModel::WeightList &list) {
  Cal3dCoreModel::AnimationMap animations = m_core_model->m_animations;
  Cal3dCoreModel::WeightList::const_iterator I = list.begin();
  while (I != list.end()) {
    std::string name = (*I).first;
    m_calModel->getMixer()->clearCycle(animations[name], 0.2f);
    ++I;
  }
}
} /* namespace Sear */
