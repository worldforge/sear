// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton


#include <GL/gl.h>
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

float Cal3d::_walk_blend[] = {0.8, 0.2, 0.0};
float Cal3d::_run_blend[] = {0.0, 0.0, 1.0};

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

Cal3d::Cal3d()
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
}

//----------------------------------------------------------------------------//
// Execute an action of the model                                             //
//----------------------------------------------------------------------------//

void Cal3d::executeAction(int action)
{
  switch(action)
  {
    case 0:
      m_calModel.getMixer()->executeAction(m_animationId[5], 0.3f, 0.3f);
      break;
    case 1:
      m_calModel.getMixer()->executeAction(m_animationId[6], 0.3f, 0.3f);
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

GLuint Cal3d::loadTexture(const std::string& strFilename)
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  GLuint textureId;
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

  // generate texture
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, (depth == 3) ? GL_RGB : GL_RGBA, width, height, 0, (depth == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, &pBuffer[width * height * depth]);

  // free the allocated memory
  delete [] pBuffer;
  } else {
    //SDL IMAGE LOAD - NEEDS TESTING
	Log::writeLog("Experimental Texture Loader being used", Log::LOG_INFO);
    SDL_Surface * image = System::loadImage(strFilename);
    width = image->w;
    height = image->h;
    depth = image->format->BytesPerPixel;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, (depth == 3) ? GL_RGB : GL_RGBA, width, height, 0, (depth == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
    free(image);

    

  }

  return textureId;
}

//----------------------------------------------------------------------------//
// Initialize the model                                                       //
//----------------------------------------------------------------------------//

bool Cal3d::init(const std::string& strFilename)
{
  // open the model configuration file
  std::ifstream file;
  file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
  if(!file)
  {
    Log::writeLog(std::string("Failed to open model configuration file '") + strFilename + std::string("'."), Log::LOG_ERROR);
    return false;
  }

  // create a core model instance
  if(!m_calCoreModel.create("dummy"))
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
      if(!m_calCoreModel.loadCoreSkeleton(strPath + strData))
      {
        CalError::printLastError();
        return false;
      }
    }
    else if(strKey == "animation")
    {
      // load core animation
//      std::cout << "Loading animation '" << strData << "'..." << std::endl;
      m_animationId[animationCount] = m_calCoreModel.loadCoreAnimation(strPath + strData);
      if(m_animationId[animationCount] == -1)
      {
        CalError::printLastError();
        return false;
      }

      animationCount++;
    }
    else if(strKey == "mesh")
    {
      // load core mesh
//      std::cout << "Loading mesh '" << strData << "'..." << std::endl;
      if(m_calCoreModel.loadCoreMesh(strPath + strData) == -1)
      {
        CalError::printLastError();
        return false;
      }
    }
    else if(strKey == "material")
    {
      // load core material
//      std::cout << "Loading material '" << strData << "'..." << std::endl;
      if(m_calCoreModel.loadCoreMaterial(strPath + strData) == -1)
      {
        CalError::printLastError();
        return false;
      }
    }
    else
    {
      Log::writeLog(strFilename + std::string("(") + string_fmt(line) + std::string("): Invalid syntax."), Log::LOG_ERROR);
      return false;
    }
  }

  // explicitely close the file
  file.close();

  // load all textures and store the opengl texture id in the corresponding map in the material
  int materialId;
  for(materialId = 0; materialId < m_calCoreModel.getCoreMaterialCount(); materialId++)
  {
    // get the core material
    CalCoreMaterial *pCoreMaterial;
    pCoreMaterial = m_calCoreModel.getCoreMaterial(materialId);

    // loop through all maps of the core material
    int mapId;
    for(mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
    {
      // get the filename of the texture
      std::string strFilename;
      strFilename = pCoreMaterial->getMapFilename(mapId);

      // load the texture from the file
      GLuint textureId;
      textureId = loadTexture(strPath + strFilename);

      // store the opengl texture id in the user data of the map
      pCoreMaterial->setMapUserData(mapId, (Cal::UserData)textureId);
    }
  }

  // make one material thread for each material
  // NOTE: this is not the right way to do it, but this viewer can't do the right
  // mapping without further information on the model etc.
  for(materialId = 0; materialId < m_calCoreModel.getCoreMaterialCount(); materialId++)
  {
    // create the a material thread
    m_calCoreModel.createCoreMaterialThread(materialId);

    // initialize the material thread
    m_calCoreModel.setCoreMaterialId(materialId, 0, materialId);
  }

  // create the model instance from the loaded core model
  if(!m_calModel.create(&m_calCoreModel))
  {
    CalError::printLastError();
    return false;
  }

  // attach all meshes to the model
  int meshId;
  for(meshId = 0; meshId < m_calCoreModel.getCoreMeshCount(); meshId++)
  {
    m_calModel.attachMesh(meshId);
  }

  // set the material set of the whole model
  m_calModel.setMaterialSet(0);

  // set initial animation state
  m_state = STATE_MOTION;
  m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION], m_motionBlend[0], 0.0f);
  m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION + 1], m_motionBlend[1], 0.0f);
  m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION + 2], m_motionBlend[2], 0.0f);

  return true;
}

//----------------------------------------------------------------------------//
// Render the mesh of the model                                               //
//----------------------------------------------------------------------------//

void Cal3d::renderMesh(bool useTextures, bool useLighting, bool select_mode)
{
  static Render *renderer = System::instance()->getGraphics()->getRender();
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
	
        unsigned char meshColor[4];
        float ambient[4];
        float diffuse[4];
        float specular[4];
	if (!select_mode) {
          pCalRenderer->getAmbientColor(&meshColor[0]);
          ambient[0] = meshColor[0] / 255.0f;  ambient[1] = meshColor[1] / 255.0f; ambient[2] = meshColor[2] / 255.0f; ambient[3] = meshColor[3] / 255.0f;

          // set the material diffuse color
          pCalRenderer->getDiffuseColor(&meshColor[0]);
          diffuse[0] = meshColor[0] / 255.0f;  diffuse[1] = meshColor[1] / 255.0f; diffuse[2] = meshColor[2] / 255.0f; diffuse[3] = meshColor[3] / 255.0f;

          // set the material specular color
          pCalRenderer->getSpecularColor(&meshColor[0]);
          specular[0] = meshColor[0] / 255.0f;  specular[1] = meshColor[1] / 255.0f; specular[2] = meshColor[2] / 255.0f; specular[3] = meshColor[3] / 255.0f;


          renderer->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
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
          renderer->switchTextureID((unsigned int)pCalRenderer->getMapUserData(0));
          renderer->renderElements(Graphics::RES_TRIANGLES, faceCount * 3, &meshFaces[0][0], &meshVertices[0][0], &meshTextureCoordinates[0][0], &meshNormals[0][0]);
	} else {
          renderer->renderElements(Graphics::RES_TRIANGLES, faceCount * 3, &meshFaces[0][0], &meshVertices[0][0], NULL, &meshNormals[0][0]);
	}


      }
    }
  }
  pCalRenderer->endRendering();
}

//----------------------------------------------------------------------------//
// Render the model                                                           //
//----------------------------------------------------------------------------//

void Cal3d::render(bool useTextures, bool useLighting, bool select_mode)
{
    glRotatef(90.0f,0.0f,0.0f,1.0f); //so zero degrees points east
    
    renderMesh(useTextures, useLighting, select_mode);
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
  // destroy the model instance
  m_calModel.destroy();

  // destroy the core model instance
  m_calCoreModel.destroy();
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

  m_calModel.getMixer()->clearCycle(m_animationId[STATE_IDLE], delay);
  m_calModel.getMixer()->clearCycle(m_animationId[STATE_FANCY], delay);
  m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION], m_motionBlend[0], delay);
  m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION + 1], m_motionBlend[1], delay);
  m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION + 2], m_motionBlend[2], delay);

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
      m_calModel.getMixer()->blendCycle(m_animationId[STATE_IDLE], 1.0f, delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_FANCY], delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_MOTION], delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_MOTION + 1], delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_MOTION + 2], delay);
      m_state = STATE_IDLE;
    }
    else if(state == STATE_FANCY)
    {
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_IDLE], delay);
      m_calModel.getMixer()->blendCycle(m_animationId[STATE_FANCY], 1.0f, delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_MOTION], delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_MOTION + 1], delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_MOTION + 2], delay);
      m_state = STATE_FANCY;
    }
    else if(state == STATE_MOTION)
    {
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_IDLE], delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_FANCY], delay);
      m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION], m_motionBlend[0], delay);
      m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION + 1], m_motionBlend[1], delay);
      m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION + 2], m_motionBlend[2], delay);
      m_state = STATE_MOTION;
    }
  }
}

void Cal3d::action(const std::string &action) {
  if (action == "walk") {
    setMotionBlend((float *)&_walk_blend[0], 0);
  } else if (action == "run") {
    setMotionBlend((float *)&_run_blend[0], 0);
  } else if (action == "wave") {
    executeAction(1);
  }
}

} /* namespace Sear */
