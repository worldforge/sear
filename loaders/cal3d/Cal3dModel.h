// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall, University of Southampton

// $Id: Cal3dModel.h,v 1.2 2003-03-06 12:52:08 simon Exp $

#ifndef SEAR_LOADERS_CAL3D_CAL3DMODEL_H
#define SEAR_LOADERS_CAL3D_CAL3DMODEL_H 1

#include <map>
#include <string>

#include <cal3d/cal3d.h>
#include "Cal3dCoreModel.h"
#include "src/Model.h"
#include "src/Graphics.h"


namespace Sear {

class Render;
//class Cal3dCoreModel;
	
class Cal3dModel : public Sear::Model {
public:
  Cal3dModel(Render *);
  ~Cal3dModel();

  bool init(Cal3dCoreModel *);
  void shutdown();
  float getLodLevel() const { return m_lodLevel; }
  void getMotionBlend(float *pMotionBlend);
  float getScale() const { return m_renderScale; }
  int getState() const { return m_state; }
  void render(bool select_mode) { render(true, true, select_mode); }
  void render(bool, bool, bool);
  void update(float elapsedSeconds);
  void setDetailLevel(float level) { setLodLevel(level); }
  void setLodLevel(float lodLevel);
  void setMotionBlend(float *pMotionBlend, float delay);
  void setState(int state, float delay);

  void action(const std::string &action);
  Graphics::RotationStyle rotationStyle() { return Graphics::ROS_NORMAL; }
 
  void setHeight(float height) { _height = height; }
  float getHeight() const { return _height; }

  unsigned int getPartID(const std::string &part) { return _core_model->_parts[part]; }
  unsigned int getSetID(const std::string &set) { return _core_model->_sets[set]; }
  
  void setMaterialSet(const std::string &set) {
    setMaterialSet(_core_model->_sets[set]);
  }
  
  void setMaterialSet(unsigned int set);
  
  void setMaterialPartSet(const std::string &part, const std::string &set) {
    setMaterialPartSet(_core_model->_parts[part], _core_model->_sets[set]);
  }
  void setMaterialPartSet(unsigned int part, unsigned int set);
  
private:
  void renderMesh(bool bWireframe, bool bLight, bool);

  bool _initialised;

  Cal3dCoreModel *_core_model;
  float m_motionBlend[3]; 
  float _height;
  int m_state;
  CalModel m_calModel;
  float m_lodLevel;
  float m_renderScale;
//  static float _walk_blend[];
//  static float _run_blend[];
//  static float _strut_blend[];

};


} /* namespace Sear */

#endif /* SEAR_LOADERS_CAL3D_CAL3DMODEL_H */
