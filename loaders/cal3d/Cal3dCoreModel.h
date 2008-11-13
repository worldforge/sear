// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

#ifndef SEAR_LOADERS_CAL3D_CAL3DCOREMODEL_H
#define SEAR_LOADERS_CAL3D_CAL3DCOREMODEL_H 1


#include <string>
#include <map>
#include <list>
#include <memory>

#include <sigc++/trackable.h>
#include <varconf/config.h>
#include <cal3d/cal3d.h>
#include <wfmath/quaternion.h>

#include "renderers/RenderTypes.h"

namespace Sear {

// Forward declarations
class Cal3dModel;

typedef struct {
  TextureID textureID;
  TextureID textureMaskID;
} MapData;
	
class Cal3dCoreModel : public sigc::trackable {
public:
  // Allow Cal3dModel to access our private parts
  friend class Cal3dModel;

  // Typedefs for out data structures  
  typedef std::map <std::string, unsigned int> MeshMap;
  typedef std::map <std::string, unsigned int> AnimationMap;
  typedef std::list <std::string> MaterialList;
  typedef std::map<std::string, unsigned int> MaterialMap;
  typedef std::map<std::string, MaterialMap> MaterialsMap;
  typedef std::map<std::string, unsigned int> PartMap;
  typedef std::map<std::string, unsigned int> SetMap;

  typedef std::pair<std::string, float> AnimWeight;
  typedef std::list<AnimWeight> WeightList;
  typedef std::map<std::string, WeightList> Animations;
  typedef std::map<std::string, std::string> BoneMap;
  typedef std::map<std::string, WFMath::Quaternion> BoneRotation;
  /**
   * Default constructor
   */ 
  Cal3dCoreModel();

  /**
   * Destructor
   */ 
  ~Cal3dCoreModel();

  /**
   * Create an instance of the core model based on config file
   * @param filename Cal3d config file
   */ 
  int init(const std::string &filename);

  /**
   * Clean up this object
   */ 
  int shutdown();

  /**
   * Returns pointer to the Cal3d core model
   * @return Pointer to cal3d core model
   */
  CalCoreModel *getCalCoreModel() const { return m_core_model.get(); }

  /**
   * Crate a Cal3d model based upon this core model
   * @return pointer to new cal3d model
   */ 
  Cal3dModel *instantiate();
 
  /**
   * Get the scale this model should be rendered at
   * @return Scale to render model at
   */ 
  float getScale() const { return m_scale; }

  float getRotate() const  { return m_rotate; }
  
  std::string mapBoneName(const std::string &bone) const;
  WFMath::Quaternion getBoneRotation(const std::string &name) const;

  bool isInitialised() const { return m_initialised; }

private:
  /**
   * This function processes a cal3d config file
   * @param filename cal3d config file
   */ 
  int readConfig(const std::string &filename);

  /**
   * This function is the varconf callback called everytime a line is read
   * from the config file. It is used to get the mesh, material and animation
   * names before they are loaded.
   * @param section The section of the config file the entry is in
   * @param key the key to get the entry
   * @param config Reference to the config object containing the data
   */ 
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);

  /**
   * Callback used by varconf when it detects an error
   * @param message The error message
   */ 
  void varconf_error_callback(const char *message);

  /**
   * Function to load textures. Determines whether it is a cal3d raw file or a 
   * normal image. If its a normal image, it passes control to the System load
   * function, else loads it up itself.
   * @param strFilename image filename
   * @return The texture object id
   */ 
  TextureID loadTexture(const std::string &strFilename, bool mask);
  
  bool m_initialised; ///< Flag indicating whether object has been initialised
  std::auto_ptr<CalCoreModel> m_core_model; ///< Pointer to the cal3d core model we represent

  float m_scale; ///< The scale the model should be rendered at
  MeshMap m_meshes; ///< Mapping between mesh name and id
  AnimationMap m_animations; ///< Mapping between animation and id
  MaterialList m_material_list; ///< List of material names
  MaterialsMap m_materials; ///< Mapping between material part and set to id
  PartMap m_parts; ///< Mapping between part name and id
  SetMap m_sets; ///< mapping between set name and id  
  float m_rotate;
  Animations m_anims;
  BoneMap m_bone_map;
  BoneRotation m_bone_rotation;

  varconf::Config m_appearance_config;
};

} /* namespace Sear */

#endif /* SEAR_LOADERS_CAL3d_CAL3DCOREMODEL_H */
