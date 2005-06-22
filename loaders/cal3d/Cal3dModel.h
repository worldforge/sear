// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: Cal3dModel.h,v 1.15 2005-06-22 07:21:51 simon Exp $

#ifndef SEAR_LOADERS_CAL3D_CAL3DMODEL_H
#define SEAR_LOADERS_CAL3D_CAL3DMODEL_H 1

#include <map>
#include <string>

#include <cal3d/cal3d.h>
#include "Cal3dCoreModel.h"
#include "Model.h"
#include "renderers/Graphics.h"


namespace Sear {

class Render;
//class Cal3dCoreModel;
	
class Cal3dModel : public Sear::Model {
public:
  Cal3dModel(Render *);
  ~Cal3dModel();

  int init(Cal3dCoreModel *);
  int shutdown();

  void invalidate() {}

  float getLodLevel() const { return m_lodLevel; }
  float getScale() const { return m_renderScale; }
  void render(bool select_mode) { render(true, true, select_mode); }
  void render(bool, bool, bool);
  void update(float elapsedSeconds);
  void setDetailLevel(float level) { setLodLevel(level); }
  void setLodLevel(float lodLevel);

  void action(const std::string &action);
  void setAppearance(const Atlas::Message::MapType &map);
  Graphics::RotationStyle rotationStyle() { return Graphics::ROS_NORMAL; }
 
  unsigned int getPartID(const std::string &part) { return m_core_model->m_parts[part]; }
  unsigned int getSetID(const std::string &set) { return m_core_model->m_sets[set]; }
  
  void setMaterialSet(const std::string &set) {
    setMaterialSet(m_core_model->m_sets[set]);
  }
  
  void setMaterialSet(unsigned int set);
  
  void setMaterialPartSet(const std::string &mesh, const std::string &set) {
    setMaterialPartSet(m_core_model->m_meshes[mesh], m_core_model->m_sets[set]);
  }
  void setMaterialPartSet(unsigned int part, unsigned int set);
 
  std::list<std::string> getMeshNames();
  std::list<std::string> getMaterialNames();

  void setRotate(float r) { m_rotate = r; }
  
  virtual PosAndOrient getPositionForSubmodel(const std::string& submodelName);

  void addAnimation(const Cal3dCoreModel::WeightList &list);
  void removeAnimation(const Cal3dCoreModel::WeightList &list);
private:
  void renderMesh(bool bWireframe, bool bLight, bool);

  bool m_initialised;

  Cal3dCoreModel *m_core_model;
  CalModel *m_calModel;
  float m_lodLevel;
  float m_renderScale;
  float m_rotate;
  //std::string current_head;
  std::string m_cur_anim;
};


} /* namespace Sear */

#endif /* SEAR_LOADERS_CAL3D_CAL3DMODEL_H */

