
// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Cal3dCoreModel.cpp,v 1.7 2003-03-08 14:32:23 simon Exp $

#include "Cal3dModel.h"
#include "Cal3dCoreModel.h"
#include <string>

#include <varconf/Config.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "common/Utility.h"
#include "src/System.h"
#include "src/Graphics.h"
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
static const std::string SECTION_model = "model";
static const std::string SECTION_material = "material";

static const std::string KEY_scale = "scale";
static const std::string KEY_path = "path";
static const std::string KEY_skeleton = "skeleton";
static const std::string KEY_mesh = "mesh";
static const std::string KEY_material = "material";
static const std::string KEY_animation = "animation";

static const std::string KEY_ambient_red = "ambient_red";
static const std::string KEY_ambient_green = "ambient_blue";
static const std::string KEY_ambient_blue = "ambient_green";
static const std::string KEY_ambient_alpha = "ambient_alpha";
static const std::string KEY_diffuse_red = "ambient_red";
static const std::string KEY_diffuse_green = "ambient_blue";
static const std::string KEY_diffuse_blue = "ambient_green";
static const std::string KEY_diffuse_alpha = "ambient_alpha";
static const std::string KEY_specular_red = "ambient_red";
static const std::string KEY_specular_green = "ambient_blue";
static const std::string KEY_specular_blue = "ambient_green";
static const std::string KEY_specular_alpha = "ambient_alpha";
static const std::string KEY_shininess = "shininess";
static const std::string KEY_texture_map = "texture_map";
	
Cal3dCoreModel::Cal3dCoreModel() :
  _initialised(false)
{}

Cal3dCoreModel::~Cal3dCoreModel() {
  if (_initialised) shutdown();
}

void Cal3dCoreModel::init(const std::string &filename) {
  if (_initialised) shutdown();	
  // open the model configuration file
  _core_model = new CalCoreModel();
  // create a core model instance
  if(!_core_model->create("dummy")) {
    CalError::printLastError();
    return;
  }
  std::cerr << "reading config" << std::endl << std::flush;
  readConfig(filename);
  std::cerr << "done reading config" << std::endl << std::flush;
}

void Cal3dCoreModel::shutdown() {
  if (_core_model) {
    _core_model->destroy();
    delete _core_model;
    _core_model = NULL;
  }
  _initialised = false;
}

void Cal3dCoreModel::readConfig(const std::string &filename) {
  varconf::Config config;
  config.sigsv.connect(SigC::slot(*this, &Cal3dCoreModel::varconf_callback));
  config.sige.connect(SigC::slot(*this, &Cal3dCoreModel::varconf_error_callback));
  config.readFromFile(filename);
  unsigned int part_counter = 1;
  unsigned int set_counter = 1;
  
  // Get path of files
  // Will be overwritten by any entry in the config file
  std::string path;
  std::string::size_type pos = filename.find_last_of("/");
  if (pos == std::string::npos) pos = filename.find_last_of("\\");
  if (pos != std::string::npos) path = filename.substr(0, pos) + "/";
  
  if (config.findItem(SECTION_model, KEY_path)) {
    path = (std::string)config.getItem(SECTION_model, KEY_path);
  }
  // Load skeleton
  if (!_core_model->loadCoreSkeleton(path + (std::string)config.getItem(SECTION_model, KEY_skeleton)))  {
    CalError::printLastError();
    return;
  }
  // Get scale
  _scale = (double)config.getItem(SECTION_model, KEY_scale);
  // Load all meshes 
  for (MeshMap::const_iterator I = _meshes.begin(); I != _meshes.end(); ++I) {
    std::string mesh_name = I->first;
    int mesh = _core_model->loadCoreMesh(path + (std::string)config.getItem(SECTION_model, KEY_mesh + "_" + mesh_name));
    if (mesh == -1) {
      std::cerr << "Error loading mesh - " << path + (std::string)config.getItem(SECTION_model, KEY_mesh + "_" + mesh_name) << std::endl;
      CalError::printLastError();
    } else {
      _meshes[mesh_name] = mesh;
    }
  }
  // Load all animations
  for (AnimationMap::const_iterator I = _animations.begin(); I != _animations.end(); ++I) {
    std::string animation_name = I->first;
    int animation = _core_model->loadCoreAnimation(path + (std::string)config.getItem(SECTION_model, KEY_animation + "_" + animation_name));
    if (animation == -1) {
      std::cerr << "Error loading animation - " << path + (std::string)config.getItem(SECTION_model, KEY_animation + "_" + animation_name) << std::endl;
      CalError::printLastError();
    } else {
      _animations[animation_name] = animation;
    }
  }
  // Load all materials
  for (MaterialList::const_iterator I = _material_list.begin(); I != _material_list.end(); ++I) {
    std::string material_name = *I;
    int length =  material_name.find_first_of("_");
    std::string set = material_name.substr(0,length);
    std::string part = material_name.substr(length + 1);
    
    int material = _core_model->loadCoreMaterial(path + (std::string)config.getItem(SECTION_model, KEY_material + "_" + material_name));
    if (material == -1) {
      std::cerr << "Error loading material - " << path + (std::string)config.getItem(SECTION_model, KEY_material + "_" + material_name) << std::endl;
      CalError::printLastError();
    } else {
      _materials[set][part] = material;
    }
    // Create material thread and assign material to a set;
    if (_sets[set] == 0) {
      _sets[set] = set_counter++;
      std::cout << "Creating set " << set << " with id  " << _sets[set] << std::endl;
    }
    if (_parts[part] == 0) {
      _parts[part] = part_counter++;
      std::cout << "Creating part " << part << " with id  " << _parts[part] << std::endl;
    }
    _core_model->createCoreMaterialThread(_parts[part] - 1);
//     _core_model->createCoreMaterialThread(material);
    // initialize the material thread
    _core_model->setCoreMaterialId(_parts[part] - 1, _sets[set] - 1, material);    
  }
  // Check for custom material settings
  for (MaterialsMap::const_iterator I = _materials.begin(); I != _materials.end(); ++I) {
    std::string set = I->first;
    for (MaterialMap::const_iterator J = I->second.begin(); J != I->second.end(); ++J) {
      std::string part = J->first;
      std::string section = SECTION_material + "_" + set + "_" + part;
      CalCoreMaterial *material = _core_model->getCoreMaterial(J->second);
      if (!material) continue;
      // Check all keys
      if (config.findItem(section, KEY_ambient_red)) {
        material->getAmbientColor().red = (int)config.getItem(section, KEY_ambient_red);
//	std::cout << "Setting ambient red to " << (int)material->getAmbientColor().red << std::endl;
      }
      if (config.findItem(section, KEY_ambient_green)) {
        material->getAmbientColor().green = (int)config.getItem(section, KEY_ambient_green);
//	std::cout << "Setting ambient green to " << (int)material->getAmbientColor().green << std::endl;
      }
      if (config.findItem(section, KEY_ambient_blue)) {
        material->getAmbientColor().blue = (int)config.getItem(section, KEY_ambient_blue);
//	std::cout << "Setting ambient blue to " << (int)material->getAmbientColor().blue << std::endl;
      }
      if (config.findItem(section, KEY_ambient_alpha)) {
        material->getAmbientColor().alpha = (int)config.getItem(section, KEY_ambient_alpha);
//	std::cout << "Setting ambient alpha to " << (int)material->getAmbientColor().alpha << std::endl;
      } 
      if (config.findItem(section, KEY_diffuse_red)) {
        material->getDiffuseColor().red = (int)config.getItem(section, KEY_diffuse_red);
      }
      if (config.findItem(section, KEY_diffuse_green)) {
        material->getDiffuseColor().green = (int)config.getItem(section, KEY_diffuse_green);
      }
      if (config.findItem(section, KEY_diffuse_blue)) {
        material->getDiffuseColor().blue = (int)config.getItem(section, KEY_diffuse_blue);
      }
      if (config.findItem(section, KEY_diffuse_alpha)) {
        material->getDiffuseColor().alpha = (int)config.getItem(section, KEY_diffuse_alpha);
      }
      if (config.findItem(section, KEY_specular_red)) {
        material->getSpecularColor().red = (int)config.getItem(section, KEY_specular_red);
      }
      if (config.findItem(section, KEY_specular_green)) {
        material->getSpecularColor().green = (int)config.getItem(section, KEY_specular_green);
      }
      if (config.findItem(section, KEY_specular_blue)) {
        material->getSpecularColor().blue = (int)config.getItem(section, KEY_specular_blue);
      }
      if (config.findItem(section, KEY_specular_alpha)) {
        material->getSpecularColor().alpha = (int)config.getItem(section, KEY_specular_alpha);
      }
      if (config.findItem(section, KEY_shininess)) {
        material->setShininess((double)config.getItem(section, KEY_shininess));
      }
      // Load textures
      for (unsigned int i = 0; i < 2; ++i) {
	std::string key = KEY_texture_map + "_" + string_fmt(i);
        if (config.findItem(section, key)) { // Is texture name over-ridden?
          std::string texture = (std::string)config.getItem(section, key);
	  std::cout << texture << std::endl;
          unsigned int textureId = loadTexture(path + texture);
	  if (material->getMapCount() < i) {
            if (!material->setMap(i, CalCoreMaterial::Map())) {
              std::cerr << "Error setting map data" << std::endl;
            }
	  }
          if (!material->setMapUserData(i, (Cal::UserData)textureId)) {
            std::cerr << "Error setting map user data" << std::endl;
	  }
        } else { // Use default texture
          std::string texture = material->getMapFilename(i);
	  std::cout << texture << std::endl;
	  if (texture.empty()) continue;
          unsigned int textureId = loadTexture(path + texture);
          if (!material->setMapUserData(i, (Cal::UserData)textureId)) {
            std::cerr << "Error setting map user data" << std::endl;
	  }
	}
      }
    }
  }
}

void Cal3dCoreModel::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  std::cout << section << " - " << key << std::endl << std::flush;
  if (section == SECTION_model) {
    if (key == KEY_mesh) {}
    else if (key.substr(0, KEY_mesh.size()) == KEY_mesh) {
      _meshes[key.substr(KEY_mesh.size() + 1)] = 0;
    }
    if (key == KEY_animation) {}
    else if (key.substr(0, KEY_animation.size()) == KEY_animation) {
      _animations[key.substr(KEY_animation.size() + 1)] = 0;
    }
    if (key == KEY_material) {}
    else if (key.substr(0, KEY_material.size()) == KEY_material) {
      _material_list.push_back(key.substr(KEY_material.size() + 1));;
    }
  }
}

void Cal3dCoreModel::varconf_error_callback(const char *message) {
  std::cerr << message << std::endl;
}

unsigned int Cal3dCoreModel::loadTexture(const std::string& strFilename) {
  unsigned int textureId;
  int width;
  int height;
  int depth;
  
  if (strFilename.substr(strFilename.length() - 4) == ".raw") {
    // open the texture file
    std::ifstream file;
    file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
    if(!file) {
      std::cerr << "Texture file '" << strFilename << "' not found." << std::endl;
      return 0;
    }
  
    // load the dimension of the texture
    file.read((char *)&width, 4);
    file.read((char *)&height, 4);
    file.read((char *)&depth, 4);
  
    // allocate a temporary buffer to load the texture to
    unsigned char *pBuffer;
    pBuffer = new unsigned char[2 * width * height * depth];
    if(pBuffer == 0) {
      std::cerr << "Memory allocation for texture '" <<  strFilename << "' failed." << std::endl;
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
    textureId = System::instance()->getGraphics()->getRender()->createTexture(width, height, depth, &pBuffer[width * height * depth], false);
    // free the allocated memory
    delete [] pBuffer;
  } else {
    SDL_Surface * image = System::loadImage(strFilename);
    if (image == NULL) return 0;
    textureId = System::instance()->getGraphics()->getRender()->createTexture((unsigned int)image->w, (unsigned int)image->h,(unsigned int)image->format->BytesPerPixel,(unsigned char *)image->pixels, false);
    SDL_FreeSurface(image);
  }
  return textureId;
}

Cal3dModel *Cal3dCoreModel::instantiate(varconf::Config &config) {
  Cal3dModel *model = new Cal3dModel(System::instance()->getGraphics()->getRender());
  model->init(this);
  if (config.findItem("model", "default_set")) {
    varconf::Variable v = config.getItem("model", "default_set");
    if (v.is_int()) {
      model->setMaterialSet((int)v);
    } else { // Assume we have a string
      model->setMaterialSet((std::string)v);
    }
  }
  return model;
}

} /* namespace Sear */
