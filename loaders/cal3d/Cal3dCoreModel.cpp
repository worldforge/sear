// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall, University of Southampton

// $Id: Cal3dCoreModel.cpp,v 1.47 2007-05-07 10:31:57 simon Exp $

#include <string>

#include <SDL/SDL.h>
#include <sigc++/object_slot.h>

#include <varconf/Config.h>

#include "common/Utility.h"
#include "src/System.h"
#include "src/FileHandler.h"

#include "renderers/RenderSystem.h"

#include "Cal3dModel.h"
#include "Cal3dCoreModel.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {
static const std::string SECTION_model = "model";
static const std::string SECTION_material = "material";
static const std::string SECTION_bone_map = "bone_mapping";
static const std::string SECTION_bone_rotation = "bone_rotations";

static const std::string KEY_scale = "scale";
static const std::string KEY_scale_isotropic_z = "scale_isotropic_z";

static const std::string KEY_path = "path";
static const std::string KEY_appearance = "appearance";
static const std::string KEY_skeleton = "skeleton";
static const std::string KEY_mesh = "mesh";
static const std::string KEY_material = "material";
static const std::string KEY_animation = "animation";

static const std::string KEY_ambient_red = "ambient_red";
static const std::string KEY_ambient_green = "ambient_green";
static const std::string KEY_ambient_blue = "ambient_blue";
static const std::string KEY_ambient_alpha = "ambient_alpha";
static const std::string KEY_diffuse_red = "diffuse_red";
static const std::string KEY_diffuse_green = "diffuse_green";
static const std::string KEY_diffuse_blue = "diffuse_blue";
static const std::string KEY_diffuse_alpha = "diffuse_alpha";
static const std::string KEY_specular_red = "specular_red";
static const std::string KEY_specular_green = "specular_green";
static const std::string KEY_specular_blue = "specular_blue";
static const std::string KEY_specular_alpha = "specular_alpha";
static const std::string KEY_shininess = "shininess";
static const std::string KEY_texture_map = "texture_map";
static const std::string KEY_rotate = "rotate";
	
Cal3dCoreModel::Cal3dCoreModel() :
  m_initialised(false),
  m_rotate(90.0f)
{}

Cal3dCoreModel::~Cal3dCoreModel() {
  if (m_initialised) shutdown();
}

int Cal3dCoreModel::init(const std::string &filename) {
  assert(m_initialised == false);
  // open the model configuration file
  m_core_model = std::auto_ptr<CalCoreModel>(new CalCoreModel("dummy"));
  // create a core model instance
  if (m_core_model.get() == 0) {
    CalError::printLastError();
    return 1;
  }

  if (readConfig(filename)) {
    printf("Error while loading %s\n", filename.c_str());
    m_core_model.release();

    return 1;
  }

  m_initialised = true;
  return 0;
}

int Cal3dCoreModel::shutdown() {
  assert(m_initialised == true);
  // Clean up user data
  // Loop through each material
  for (int i = 0; i < m_core_model->getCoreMaterialCount(); ++i) {
    CalCoreMaterial *m = m_core_model->getCoreMaterial(i);
    MapData *md;
    // Loop through each map
    for (int j = 0; j < m->getMapCount(); ++j) {
      md = reinterpret_cast<MapData*>(m->getMapUserData(j));
      if (md) {
        RenderSystem::getInstance().releaseTexture(md->textureID);
        RenderSystem::getInstance().releaseTexture(md->textureMaskID);
        delete md;
      }
    }
  }

  m_core_model.release();

  m_initialised = false;
  return 0;
}

int Cal3dCoreModel::readConfig(const std::string &filename) {
  varconf::Config config;
  config.sigsv.connect(sigc::mem_fun(this, &Cal3dCoreModel::varconf_callback));
  config.sige.connect(sigc::mem_fun(this, &Cal3dCoreModel::varconf_error_callback));
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
    System::instance()->getFileHandler()->getFilePath(path);
  }

  if (config.findItem(SECTION_model, KEY_appearance)) {
    std::string app = path + (std::string)config.getItem(SECTION_model, KEY_appearance);
    System::instance()->getFileHandler()->getFilePath(app);
    m_appearance_config.sige.connect(sigc::mem_fun(this, &Cal3dCoreModel::varconf_error_callback));
    m_appearance_config.readFromFile(app);
  }


  // Load skeleton
  if (m_core_model->loadCoreSkeleton(path + "/" + (std::string)config.getItem(SECTION_model, KEY_skeleton)) == 0)  {
    CalError::printLastError();
    return 1;
  }

  // Get scale
  if (config.findItem(SECTION_model, KEY_scale)) {
    m_scale = (double)config.getItem(SECTION_model, KEY_scale);
  } else {
    m_scale = 1.0f;
  }

  if (config.findItem(SECTION_model, KEY_rotate)) {
    m_rotate = (double)config.getItem(SECTION_model, KEY_rotate);
  }

  // Load all meshes 
  for (MeshMap::const_iterator I = m_meshes.begin(); I != m_meshes.end(); ++I) {
    const std::string &mesh_name = I->first;
    int mesh = m_core_model->loadCoreMesh(path + (std::string)config.getItem(SECTION_model, KEY_mesh + "_" + mesh_name));
    if (mesh == -1) {
      std::cerr << "Error loading mesh - " << path + (std::string)config.getItem(SECTION_model, KEY_mesh + "_" + mesh_name) << std::endl;
      CalError::printLastError();
    } else {
      m_meshes[mesh_name] = mesh;
    }
  }

  // Load all animations
  for (AnimationMap::const_iterator I = m_animations.begin(); I != m_animations.end(); ++I) {
    const std::string &animation_name = I->first;
    int animation = m_core_model->loadCoreAnimation(path + (std::string)config.getItem(SECTION_model, KEY_animation + "_" + animation_name));
    if (animation == -1) {
      std::cerr << "Error loading animation - " << path + (std::string)config.getItem(SECTION_model, KEY_animation + "_" + animation_name) << std::endl;
      CalError::printLastError();
    } else {
      m_animations[animation_name] = animation;
    }
  }

  // Load all materials
  for (MaterialList::const_iterator I = m_material_list.begin();
                                    I != m_material_list.end(); ++I) {
    const std::string &material_name = *I;
    int length =  material_name.find_first_of("_");
    const std::string &set = material_name.substr(0,length);
    const std::string &part = material_name.substr(length + 1);

    int material = m_core_model->loadCoreMaterial(path + (std::string)config.getItem(SECTION_model, KEY_material + "_" + material_name));
    if (material == -1) {
      std::cerr << "Error loading material - " << path + (std::string)config.getItem(SECTION_model, KEY_material + "_" + material_name) << std::endl;
      CalError::printLastError();
    } else {
      m_materials[set][part] = material;
    }
    // Create material thread and assign material to a set;
    if (m_sets[set] == 0) {
      m_sets[set] = set_counter++;
    }
    if (m_parts[part] == 0) {
      m_parts[part] = part_counter++;
    }
    m_core_model->createCoreMaterialThread(m_parts[part] - 1);
    // initialize the material thread
    m_core_model->setCoreMaterialId(m_parts[part] - 1, m_sets[set] - 1, material);
  }
  // Check for custom material settings
  for (MaterialsMap::const_iterator I = m_materials.begin(); I != m_materials.end(); ++I) {
    const std::string &set = I->first;
    for (MaterialMap::const_iterator J = I->second.begin(); J != I->second.end(); ++J) {
      const std::string &part = J->first;
      const std::string &section = SECTION_material + "_" + set + "_" + part;
      CalCoreMaterial *material = m_core_model->getCoreMaterial(J->second);
      if (!material) continue;
      // Check all keys
      if (config.findItem(section, KEY_ambient_red)) {
        material->getAmbientColor().red = (int)config.getItem(section, KEY_ambient_red);
      }
      if (config.findItem(section, KEY_ambient_green)) {
        material->getAmbientColor().green = (int)config.getItem(section, KEY_ambient_green);
      }
      if (config.findItem(section, KEY_ambient_blue)) {
        material->getAmbientColor().blue = (int)config.getItem(section, KEY_ambient_blue);
      }
      if (config.findItem(section, KEY_ambient_alpha)) {
        material->getAmbientColor().alpha = (int)config.getItem(section, KEY_ambient_alpha);
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
      // TODO this limit should not be hardcoded!
      // Need to query object
      for (int i = 0; i < 2; ++i) {
	const std::string &key = KEY_texture_map + "_" + string_fmt(i);
        if (config.findItem(section, key)) { // Is texture name over-ridden?
          const std::string &texture = (std::string)config.getItem(section, key);
          unsigned int textureId = loadTexture(texture, false);
          unsigned int textureMaskId = loadTexture(texture, true);
	  if (material->getMapCount() <= i) {
            // Increase the space available to store data
	    material->reserve(i + 1);
            if (!material->setMap(i, CalCoreMaterial::Map())) {
              std::cerr << "Error setting map data" << std::endl;
            }
	  }
          MapData *md = new MapData();
          md->textureID = textureId;
          md->textureMaskID = textureMaskId;
          if (!material->setMapUserData(i, reinterpret_cast<Cal::UserData>(md))) {
            std::cerr << "Error setting map user data" << std::endl;
	  }
        } else { // Use default texture
          const std::string &texture = material->getMapFilename(i);
	  if (texture.empty()) continue;
          unsigned int textureId = loadTexture(texture, false);
          unsigned int textureMaskId = loadTexture(texture, true);
          MapData *md = new MapData();
          md->textureID = textureId;
          md->textureMaskID = textureMaskId;
          if (!material->setMapUserData(i, reinterpret_cast<Cal::UserData>(md))) {
            std::cerr << "Error setting map user data" << std::endl;
	  }
	}
      }
    }
  }
  // Scale the object
  m_core_model->scale(m_scale);
  if (config.findItem(SECTION_model, KEY_scale_isotropic_z)) {
    if ((bool)config.getItem(SECTION_model, KEY_scale_isotropic_z)) {
      // Instantiate a model to get its scale.
      Cal3dModel *model = new Cal3dModel();
      if (model->init(this)) {
        delete model;
        model = NULL;
      } else {    
        model->update(1);
        CalBoundingBox bbox = model->m_calModel->getBoundingBox();
        CalVector p[6];
        bbox.computePoints(p);
        float min_z,max_z;
        min_z = max_z = p[0].z;
        for (int  i = 1; i < 6; ++i) {
          if (min_z > p[i].z) min_z = p[i].z;
          if (max_z < p[i].z) max_z = p[i].z;
        }
        // Clean up temporary object
        model->shutdown();
        delete model;
        float s = 1.0f / (max_z - min_z);
        m_core_model->scale(s);
      }
    }
  }
 
  return 0;
}

void Cal3dCoreModel::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  if (section == SECTION_model) {
    if (key == KEY_mesh) {}
    else if (key.substr(0, KEY_mesh.size()) == KEY_mesh) {
      m_meshes[key.substr(KEY_mesh.size() + 1)] = 0;
    }
    if (key == KEY_animation) {}
    else if (key.substr(0, KEY_animation.size()) == KEY_animation) {
      m_animations[key.substr(KEY_animation.size() + 1)] = 0;
    }
    if (key == KEY_material) {}
    else if (key.substr(0, KEY_material.size()) == KEY_material) {
      m_material_list.push_back(key.substr(KEY_material.size() + 1));;
    }
  } else if (section == SECTION_bone_map) {
    m_bone_map[key] = (std::string)config.getItem(section, key);
  } else if (section == SECTION_bone_rotation) {
    const std::string &rot = (std::string)config.getItem(section, key);
    float w,x,y,z;
    sscanf(rot.c_str(), "%f;%f;%f;%f", &w, &x, &y, &z);
    m_bone_rotation[key] = WFMath::Quaternion(w,x,y,z);
  } else {
    // Add animations weights to map
    // Get weight value
    varconf::Variable temp = config.getItem(section, key);
    const std::string &sec = section.substr(KEY_animation.size() + 1);
    if (section.size() >= KEY_animation.size() && section.substr(0, KEY_animation.size()) == KEY_animation) {
      if (temp.is_double()) {
        // Get animation name
        const std::string &k = key.substr(KEY_animation.size() + 1);
        // Add pair to map.
        m_anims[sec].push_back(AnimWeight(k, (double)temp));
      }
    }
  }
}

void Cal3dCoreModel::varconf_error_callback(const char *message) {
  std::cerr << message << std::endl << std::flush;
}

unsigned int Cal3dCoreModel::loadTexture(const std::string& strFilename, bool mask) {
  unsigned int textureId;
  textureId = RenderSystem::getInstance().requestTexture(strFilename, mask);
  return textureId;
}

Cal3dModel *Cal3dCoreModel::instantiate() {
  Cal3dModel *model = new Cal3dModel();
  if (model->init(this)) {
    delete model;
    model = NULL;
  }

  return model;
}

std::string Cal3dCoreModel::mapBoneName(const std::string &bone) const {
  BoneMap::const_iterator I = m_bone_map.find(bone);
  if (I != m_bone_map.end()) return I->second;
  else return "";
}

WFMath::Quaternion Cal3dCoreModel::getBoneRotation(const std::string &name) const {
  BoneRotation::const_iterator I = m_bone_rotation.find(name);
  if (I != m_bone_rotation.end()) return I->second;
  return WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
}

} /* namespace Sear */
