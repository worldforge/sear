// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Cal3d.cpp,v 1.20 2002-09-08 16:15:01 simon Exp $

//#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "src/System.h"
#include "src/Render.h"

#include "Cal3d.h"


//----------------------------------------------------------------------------//
// Static member variables initialization                                     //
//----------------------------------------------------------------------------//

namespace Sear {

const int Cal3d::STATE_IDLE = 0;
const int Cal3d::STATE_FANCY = 1;
const int Cal3d::STATE_MOTION = 2;

float Cal3d::_strut_blend[] = {1.0, 0.0, 0.0};
float Cal3d::_walk_blend[] = {0.0, 1.0, 0.0};
float Cal3d::_run_blend[] = {0.0, 0.0, 1.0};

//std::map<std::string, CalCoreModel*> Cal3d::core_models = std::map<std::string, CalCoreModel*>();
std::map<std::string, Cal3d::ModelAnimPair*> Cal3d::core_models = std::map<std::string, Cal3d::ModelAnimPair*>();
int Cal3d::instance_count = 0;
//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

Cal3d::Cal3d(Render *render) :
  Model(render),
  _initialised(false)
{
  m_state = STATE_IDLE;
  m_motionBlend[0] = 0.6f;
  m_motionBlend[1] = 0.1f;
  m_motionBlend[2] = 0.3f;
  m_animationCount = 0;
  m_meshCount = 0;
  m_renderScale = 1.0f;
  m_lodLevel = 1.0f;
  _use_textures = false;
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

Cal3d::~Cal3d()
{
  if (_initialised) shutdown();
}

//----------------------------------------------------------------------------//
// Execute an action of the model                                             //
//----------------------------------------------------------------------------//

void Cal3d::executeAction(int action)
{
  switch(action)
  {
    case 0:
      m_calModel.getMixer()->executeAction(map->m_animationId[WAVE], 0.3f, 0.3f);
      break;
    case 1:
      m_calModel.getMixer()->executeAction(map->m_animationId[SHOOT_ARROW], 0.3f, 0.3f);
      break;
  }
}

//----------------------------------------------------------------------------//
// Get the lod level of the model                                             //
//----------------------------------------------------------------------------//

float Cal3d::getLodLevel()
{
  return m_lodLevel;
}

//----------------------------------------------------------------------------//
// Get the motion blend factors state of the model                            //
//----------------------------------------------------------------------------//

void Cal3d::getMotionBlend(float *pMotionBlend)
{
  pMotionBlend[0] = m_motionBlend[0];
  pMotionBlend[1] = m_motionBlend[1];
  pMotionBlend[2] = m_motionBlend[2];
}

//----------------------------------------------------------------------------//
// Get the render scale of the model                                          //
//----------------------------------------------------------------------------//

float Cal3d::getScale()
{
  return m_renderScale;
}

//----------------------------------------------------------------------------//
// Get the animation state of the model                                       //
//----------------------------------------------------------------------------//

int Cal3d::getState()
{
  return m_state;
}

//----------------------------------------------------------------------------//
// Load and create a texture from a given file                                //
//----------------------------------------------------------------------------//

unsigned int Cal3d::loadTexture(const std::string& strFilename)
{
//  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  unsigned int textureId;
  int width;
  int height;
  int depth;

  _use_textures = true;
  
  if (strFilename.substr(strFilename.length() - 4) == ".raw") {
    // open the texture file
    std::ifstream file;
    file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
    if(!file)
    {
      Log::writeLog(std::string("Texture file '") + strFilename + std::string("' not found."), Log::LOG_ERROR);
      return 0;
    }
  
    // load the dimension of the texture
    file.read((char *)&width, 4);
    file.read((char *)&height, 4);
    file.read((char *)&depth, 4);
  
    // allocate a temporary buffer to load the texture to
    unsigned char *pBuffer;
    pBuffer = new unsigned char[2 * width * height * depth];
    if(pBuffer == 0)
    {
      Log::writeLog(std::string("Memory allocation for texture '") + strFilename + std::string("' failed."), Log::LOG_ERROR);
      return 0;
    }
  
    // load the texture
    file.read((char *)pBuffer, width * height * depth);
  
    // explicitely close the file
    file.close();
  
    // flip texture around y-axis (-> opengl-style)
    int y;
    for(y = 0; y < height; y++)
    {
      memcpy(&pBuffer[(height + y) * width * depth], &pBuffer[(height - y - 1) * width * depth], width * depth);
    }
    textureId = _render->createTexture(width, height, depth, &pBuffer[width * height * depth], false);
    // free the allocated memory
    delete [] pBuffer;
  } else {
    SDL_Surface * image = System::loadImage(strFilename);
    textureId = _render->createTexture((unsigned int)image->w, (unsigned int)image->h,(unsigned int)image->format->BytesPerPixel,(unsigned char *)image->pixels, false);
    free(image);
  }
  return textureId;
}

//----------------------------------------------------------------------------//
// Initialize the model                                                       //
//----------------------------------------------------------------------------//

bool Cal3d::init(const std::string& strFilename) {
  if (_initialised) shutdown();	
  if (core_models[strFilename]) {
    map = core_models[strFilename];
  } else {
    unsigned int set_counter = 1;
    unsigned int part_counter = 1;
    std::map<int, int> material_map;
    std::map<int, int> part_map;
    std::map<std::string, int> material_set_map;
    std::map<std::string, int> material_part_map;
    // open the model configuration file
    std::ifstream file;
    file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
    if(!file)
    {
      Log::writeLog(std::string("Failed to open model configuration file '") + strFilename + std::string("'."), Log::LOG_ERROR);
      return false;
    }
    map = new Cal3d::ModelAnimPair();
    map->m_calCoreModel = new CalCoreModel();
    memset(map->m_animationId, 0, NUM_ANIMATIONS);
    // create a core model instance
    if(!map->m_calCoreModel->create("dummy"))
    {
      CalError::printLastError();
      return false;
    }

    // initialize the data path
    std::string strPath;

    // initialize the animation count
    int animationCount;
    animationCount = 0;

    // parse all lines from the model configuration file
    int line;
    for(line = 1; ; line++)
    {
      // read the next model configuration line
      std::string strBuffer;
      std::getline(file, strBuffer);
  
      // stop if we reached the end of file
      if(file.eof()) break;

      // check if an error happend while reading from the file
      if(!file)
    {
        Log::writeLog(std::string("Error while reading from the model configuration file '") + strFilename + std::string("'."), Log::LOG_ERROR);
        return false;
      }

      // find the first non-whitespace character
      std::string::size_type pos;
      pos = strBuffer.find_first_not_of(" \t");

    
      // check for empty lines
      if((pos == std::string::npos) || (strBuffer[pos] == '\n') || (strBuffer[pos] == '\r') || (strBuffer[pos] == 0)) continue;
 
      std::string::size_type pos2;
      pos2 = strBuffer.find("\r");
      if (pos2 != std::string::npos) {
        strBuffer.replace(pos2, 1, "");
      }
      // check for comment lines
      if(strBuffer[pos] == '#') continue;
  
      // get the key
      std::string strKey;
      strKey = strBuffer.substr(pos, strBuffer.find_first_of(" =\t\n\r", pos));
      pos += strKey.size();

      // get the '=' character
      pos = strBuffer.find_first_not_of(" \t", pos);
      if((pos == std::string::npos) || (strBuffer[pos] != '='))
      {
        Log::writeLog(strFilename + std::string("(") + string_fmt(line) + std::string("): Invalid syntax."), Log::LOG_ERROR);
        return false;
      }

      // find the first non-whitespace character after the '=' character
      pos = strBuffer.find_first_not_of(" \t", pos + 1);

      // get the data
      std::string strData;
      strData = strBuffer.substr(pos);

      // handle the model creation
      if(strKey == "scale")
     {
        // set rendering scale factor
        m_renderScale = atof(strData.c_str());
      }
      else if(strKey == "path")
      {
        // set the new path for the data files
        strPath = strData;
      }
      else if(strKey == "skeleton")
      {
        // load core skeleton
  //      Log::writeLog(std::string("Loading skeleton '") + strData + std::string("'..."), Log::DEFAULT);
        if(!map->m_calCoreModel->loadCoreSkeleton(strPath + strData))
        {
          CalError::printLastError();
          return false;
        }
      }
      else if(strKey == "animation_idle") {
        map->m_animationId[IDLE] = map->m_calCoreModel->loadCoreAnimation(strPath + strData);
        if(map->m_animationId[IDLE] == -1) {
          CalError::printLastError();
          return false;
        }
        animationCount++;
      }
      else if(strKey == "animation_walk") {
        map->m_animationId[WALK] = map->m_calCoreModel->loadCoreAnimation(strPath + strData);
        if(map->m_animationId[WALK] == -1) {
          CalError::printLastError();
          return false;
        }
        animationCount++;
      }
      else if(strKey == "animation_run") {
        map->m_animationId[RUN] = map->m_calCoreModel->loadCoreAnimation(strPath + strData);
        if(map->m_animationId[RUN] == -1) {
          CalError::printLastError();
          return false;
        }
        animationCount++;
      }
      else if(strKey == "animation_strut") {
        map->m_animationId[STRUT] = map->m_calCoreModel->loadCoreAnimation(strPath + strData);
        if(map->m_animationId[STRUT] == -1) {
          CalError::printLastError();
          return false;
        }
        animationCount++;
      }
      else if(strKey == "animation_wave") {
        map->m_animationId[WAVE] = map->m_calCoreModel->loadCoreAnimation(strPath + strData);
        if(map->m_animationId[WAVE] == -1) {
          CalError::printLastError();
          return false;
        }
        animationCount++;
      }
      else if(strKey == "animation_shoot_arrow") {
        map->m_animationId[SHOOT_ARROW] = map->m_calCoreModel->loadCoreAnimation(strPath + strData);
        if(map->m_animationId[SHOOT_ARROW] == -1) {
          CalError::printLastError();
          return false;
        }
        animationCount++;
      }
      else if(strKey == "animation_funky") {
        map->m_animationId[FUNKY] = map->m_calCoreModel->loadCoreAnimation(strPath + strData);
        if(map->m_animationId[FUNKY] == -1) {
          CalError::printLastError();
          return false;
        }
        animationCount++;
      }
      else if(strKey == "mesh")
      {
        // load core mesh
        if(map->m_calCoreModel->loadCoreMesh(strPath + strData) == -1)
        {
          CalError::printLastError();
          return false;
        }
      }
      else if(strKey.substr(0,5)   == "mesh_") {
        // load core mesh
	int code = map->m_calCoreModel->loadCoreMesh(strPath + strData);
        if(code == -1)
        {
          CalError::printLastError();
          return false;
        }
	std::string weapon = strKey.substr(5);
	//Not best way. should do like with textures and use numbers.
	//that way makes a more generic approach
	//however textual rep is easier to use
	if (weapon == "axe") m_weaponId[AXE] = code;
	else if (weapon == "sword") m_weaponId[SWORD] = code;
	else if (weapon == "staff") m_weaponId[STAFF] = code;
	else if (weapon == "bow") m_weaponId[BOW] = code;
      }
      else if(strKey.substr(0,8) == "material") {
	int length =  strKey.substr(9).find_first_of("_");
	std::string set = strKey.substr(9, length);
	if (material_set_map[set] == 0) {
          material_set_map[set] = set_counter++;
	}
	std::string part = strKey.substr(9 + length + 1);
	if (material_part_map[part] == 0) {
          material_part_map[part] = part_counter++;
	}
        // load core material
	int code = map->m_calCoreModel->loadCoreMaterial(strPath + strData);
        if(code == -1) {
          CalError::printLastError();
          return false;
        }
        material_map[code] = material_set_map[set];
        part_map[code] = material_part_map[part];
	std::cout << set << " - " << part << std::endl;
      }
      else {
        Log::writeLog(strFilename + std::string("(") + string_fmt(line) + std::string("): Invalid syntax."), Log::LOG_ERROR);
        return false;
      }
    }

    // explicitely close the file
    file.close();

    // load all textures and store the opengl texture id in the corresponding map in the material
    int materialId;
    for(materialId = 0; materialId < map->m_calCoreModel->getCoreMaterialCount(); materialId++)
    {
      // get the core material
      CalCoreMaterial *pCoreMaterial;
      pCoreMaterial = map->m_calCoreModel->getCoreMaterial(materialId);
  
      // loop through all maps of the core material
      int mapId;
      for(mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
      {
        // get the filename of the texture
        std::string strFilename;
        strFilename = pCoreMaterial->getMapFilename(mapId);

        // load the texture from the file
        unsigned int textureId;
        textureId = loadTexture(strPath + strFilename);

        // store the opengl texture id in the user data of the map
        pCoreMaterial->setMapUserData(mapId, (Cal::UserData)textureId);
      }
    }

    // make one material thread for each material
    // NOTE: this is not the right way to do it, but this viewer can't do the right
    // mapping without further information on the model etc.
   for(materialId = 0; materialId < map->m_calCoreModel->getCoreMaterialCount(); materialId++)
    {
      // create the a material thread
      map->m_calCoreModel->createCoreMaterialThread(materialId);
  
      // initialize the material thread
      map->m_calCoreModel->setCoreMaterialId(part_map[materialId] - 1, material_map[materialId] - 1, materialId);
    }
    core_models[strFilename] = map;
  }

  // create the model instance from the loaded core model
  if(!m_calModel.create(map->m_calCoreModel))
  {
    CalError::printLastError();
    return false;
  }

  // attach all meshes to the model
  int meshId;
  for(meshId = 0; meshId < map->m_calCoreModel->getCoreMeshCount(); meshId++)
  {
    m_calModel.attachMesh(meshId);
  }

  // set the material set of the whole model
  m_calModel.setMaterialSet(0);

  // set initial animation state
  m_state = STATE_IDLE;
  m_calModel.getMixer()->blendCycle(map->m_animationId[IDLE], 1.0f, 0.0f);

  instance_count++;
  _initialised = true;
  return true;
}

//----------------------------------------------------------------------------//
// Render the mesh of the model                                               //
//----------------------------------------------------------------------------//

void Cal3d::renderMesh(bool useTextures, bool useLighting, bool select_mode)
{
  // get the renderer of the model
  CalRenderer *pCalRenderer;
  pCalRenderer = m_calModel.getRenderer();

  // begin the rendering loop
  if(!pCalRenderer->beginRendering()) return;

  // get the number of meshes
  int meshCount;
  meshCount = pCalRenderer->getMeshCount();

  // render all meshes of the model
  int meshId;
  for(meshId = 0; meshId < meshCount; meshId++)
  {
    // get the number of submeshes
    int submeshCount;
    submeshCount = pCalRenderer->getSubmeshCount(meshId);

    // render all submeshes of the mesh
    int submeshId;
    for(submeshId = 0; submeshId < submeshCount; submeshId++)
    {
      // select mesh and submesh for further data access
      if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
      {
	
        static unsigned char meshColor[4];
        static float ambient[4];
        static float diffuse[4];
        static float specular[4];
	if (!select_mode) {
          pCalRenderer->getAmbientColor(&meshColor[0]);
          ambient[0] = meshColor[0] / 255.0f;  ambient[1] = meshColor[1] / 255.0f; ambient[2] = meshColor[2] / 255.0f; ambient[3] = meshColor[3] / 255.0f;

          // set the material diffuse color
          pCalRenderer->getDiffuseColor(&meshColor[0]);
          diffuse[0] = meshColor[0] / 255.0f;  diffuse[1] = meshColor[1] / 255.0f; diffuse[2] = meshColor[2] / 255.0f; diffuse[3] = meshColor[3] / 255.0f;

          // set the material specular color
          pCalRenderer->getSpecularColor(&meshColor[0]);
          specular[0] = meshColor[0] / 255.0f;  specular[1] = meshColor[1] / 255.0f; specular[2] = meshColor[2] / 255.0f; specular[3] = meshColor[3] / 255.0f;


          _render->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
	}
	
        // get the transformed vertices of the submesh
        static float meshVertices[30000][3];
        int vertexCount;
        vertexCount = pCalRenderer->getVertices(&meshVertices[0][0]);

        // get the transformed normals of the submesh
        static float meshNormals[30000][3];
        pCalRenderer->getNormals(&meshNormals[0][0]);

        // get the texture coordinates of the submesh
        static float meshTextureCoordinates[30000][2];
        int textureCoordinateCount;
        textureCoordinateCount = pCalRenderer->getTextureCoordinates(0, &meshTextureCoordinates[0][0]);

        // get the faces of the submesh
        static int meshFaces[50000][3];
        int faceCount;
        faceCount = pCalRenderer->getFaces(&meshFaces[0][0]);

        // set the vertex and normal buffers
        if(!select_mode && (pCalRenderer->getMapCount() > 0) && (textureCoordinateCount > 0)) {
          _render->switchTextureID((unsigned int)pCalRenderer->getMapUserData(0));
          _render->renderElements(Graphics::RES_TRIANGLES, faceCount * 3, &meshFaces[0][0], &meshVertices[0][0], &meshTextureCoordinates[0][0], &meshNormals[0][0]);
	} else {
          _render->renderElements(Graphics::RES_TRIANGLES, faceCount * 3, &meshFaces[0][0], &meshVertices[0][0], NULL, &meshNormals[0][0]);
	}


      }
    }
  }
  pCalRenderer->endRendering();
}

//----------------------------------------------------------------------------//
// Render the model                                                           //
//----------------------------------------------------------------------------//

void Cal3d::render(bool useTextures, bool useLighting, bool select_mode) {
  if (_render) {
    _render->rotate(90.0f,0.0f,0.0f,1.0f); //so zero degrees points east    
    renderMesh(useTextures, useLighting, select_mode);
  }
}

//----------------------------------------------------------------------------//
// Update the model                                                           //
//----------------------------------------------------------------------------//

void Cal3d::update(float time_elapsed)
{
  // update the model
  m_calModel.update(time_elapsed);
}

//----------------------------------------------------------------------------//
// Shut the model down                                                        //
//----------------------------------------------------------------------------//

void Cal3d::shutdown()
{
  instance_count--;
  // destroy the model instance
  m_calModel.destroy();
  // Hopefully removes all core models once last cal3d has been removed
  // This should really be handled elsewhere
  if (instance_count == 0) {
    for (std::map<std::string, ModelAnimPair*>::const_iterator I = core_models.begin(); I != core_models.end(); I++) {
      (I->second)->m_calCoreModel->destroy();
    }
    while(!core_models.empty()) {
      core_models.erase(core_models.begin());
    }
//  map->m_calCoreModel->destroy();
  }
  _initialised = false;
}

//----------------------------------------------------------------------------//
// Set the lod level of the model                                             //
//----------------------------------------------------------------------------//

void Cal3d::setLodLevel(float lodLevel)
{
  m_lodLevel = lodLevel;

  // set the new lod level in the cal model renderer
  m_calModel.setLodLevel(m_lodLevel);
}

//----------------------------------------------------------------------------//
// Set the motion blend factors state of the model                            //
//----------------------------------------------------------------------------//

void Cal3d::setMotionBlend(float *pMotionBlend, float delay)
{
  m_motionBlend[0] = pMotionBlend[0];
  m_motionBlend[1] = pMotionBlend[1];
  m_motionBlend[2] = pMotionBlend[2];

  m_calModel.getMixer()->clearCycle(map->m_animationId[IDLE], delay);
  m_calModel.getMixer()->clearCycle(map->m_animationId[FUNKY], delay);
  m_calModel.getMixer()->blendCycle(map->m_animationId[STRUT], m_motionBlend[0], delay);
  m_calModel.getMixer()->blendCycle(map->m_animationId[WALK], m_motionBlend[1], delay);
  m_calModel.getMixer()->blendCycle(map->m_animationId[RUN], m_motionBlend[2], delay);

  m_state = STATE_MOTION;
}

//----------------------------------------------------------------------------//
// Set a new animation state within a given delay                             //
//----------------------------------------------------------------------------//

void Cal3d::setState(int state, float delay)
{
  // check if this is really a new state
  if(state != m_state)
  {
    if(state == STATE_IDLE)
    {
      m_calModel.getMixer()->blendCycle(map->m_animationId[IDLE], 1.0f, delay);
      m_calModel.getMixer()->clearCycle(map->m_animationId[FUNKY], delay);
      m_calModel.getMixer()->clearCycle(map->m_animationId[STRUT], delay);
      m_calModel.getMixer()->clearCycle(map->m_animationId[WALK], delay);
      m_calModel.getMixer()->clearCycle(map->m_animationId[RUN], delay);
      m_state = STATE_IDLE;
    }
    else if(state == STATE_FANCY)
    {
      m_calModel.getMixer()->clearCycle(map->m_animationId[IDLE], delay);
      m_calModel.getMixer()->blendCycle(map->m_animationId[FUNKY], 1.0f, delay);
      m_calModel.getMixer()->clearCycle(map->m_animationId[STRUT], delay);
      m_calModel.getMixer()->clearCycle(map->m_animationId[WALK], delay);
      m_calModel.getMixer()->clearCycle(map->m_animationId[RUN], delay);
      m_state = STATE_FANCY;
    }
    else if(state == STATE_MOTION)
    {
      m_calModel.getMixer()->clearCycle(map->m_animationId[IDLE], delay);
      m_calModel.getMixer()->clearCycle(map->m_animationId[FUNKY], delay);
      m_calModel.getMixer()->blendCycle(map->m_animationId[STRUT], m_motionBlend[0], delay);
      m_calModel.getMixer()->blendCycle(map->m_animationId[WALK], m_motionBlend[1], delay);
      m_calModel.getMixer()->blendCycle(map->m_animationId[RUN], m_motionBlend[2], delay);
      m_state = STATE_MOTION;
    }
  }
}

void Cal3d::action(const std::string &action) {
  if (action == "standing") {
    setState(STATE_IDLE, 0);
  }  else if (action == "funky") {
    setState(STATE_FANCY, 0);
  }  else if (action == "strut") {
    setMotionBlend((float *)&_strut_blend[0], 0);
  } else if (action == "walking") {
    setMotionBlend((float *)&_walk_blend[0], 0);
  } else if (action == "running") {
    setMotionBlend((float *)&_run_blend[0], 0);
  } else if (action == "wave") {
    executeAction(0);
  } else if (action == "shoot_arrow") {
    executeAction(1);
  } else if (action.substr(0,11) == "change_set_") {
    int i = 0;
    cast_stream(action.substr(11), i);
    m_calModel.setMaterialSet(i);
  }
  else if (action.substr(0, 4) == "add_") {
    int i = 0;
    cast_stream(action.substr(4), i);
    m_calModel.attachMesh(m_weaponId[i]);
  }
  else if (action.substr(0, 7) == "remove_") {
    int i = 0;
    cast_stream(action.substr(7), i);
    m_calModel.detachMesh(m_weaponId[i]);
  }
}

} /* namespace Sear */
