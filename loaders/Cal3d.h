// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _CAL3D_H_
#define _CAL3D_H_ 1

#include <cal3d/cal3d.h>

#include "../src/Models.h"

namespace Sear {

class Cal3d : public Models
{
// misc
public:
  static const int STATE_IDLE;
  static const int STATE_FANCY;
  static const int STATE_MOTION;

// member variables
protected:
  int m_state;
  CalCoreModel m_calCoreModel;
  CalModel m_calModel;
  int m_animationId[16];
  int m_animationCount;
  int m_meshId[32];
  int m_meshCount;
  unsigned int m_textureId[32];
  int m_textureCount;
  float m_motionBlend[3];
  float m_renderScale;

  float m_lodLevel;

  bool _use_textures;
  
// constructors/destructor
public:
	Cal3d();
	virtual ~Cal3d();

// member functions
public:
  void executeAction(int action);
  float getLodLevel();
  void getMotionBlend(float *pMotionBlend);
  float getScale();
  int getState();
  bool init(const std::string& strFilename);
  void render(bool select_mode) { render(true, true, select_mode); }
  void render(bool, bool, bool);
  void shutdown();
  void update(float elapsedSeconds);
  void setLodLevel(float lodLevel);
  void setMotionBlend(float *pMotionBlend, float delay);
  void setState(int state, float delay);

  bool useTextures() { return _use_textures; }
  RotationStyle rotationStyle() { return NORMAL; }

protected:
  unsigned int loadTexture(const std::string& strFilename);
  void renderMesh(bool bWireframe, bool bLight, bool);
};

} /* namespace Sear */

#endif /* _CAL3D_H_ */

