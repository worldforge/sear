// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _CAL3D_H_
#define _CAL3D_H_ 1

#include <map>

#include <cal3d/cal3d.h>

#include "src/Model.h"
#include "src/Graphics.h"


namespace Sear {

class Render;
	
class Cal3d : public Model {
// misc
public:

  typedef enum {
    IDLE = 0,
    WALK,
    RUN,
    STRUT,
    WAVE,
    SHOOT_ARROW,
    FUNKY,
    NUM_ANIMATIONS
  } Animation;

  typedef enum {
    AXE = 0,
    SWORD,
    STAFF,
    BOW,
    NUM_WEAPONS
  } Weapons;
	
  static const int STATE_IDLE;
  static const int STATE_FANCY;
  static const int STATE_MOTION;

// member variables
protected:
  int m_state;
  CalCoreModel *m_calCoreModel;
  CalModel m_calModel;
  int m_animationId[NUM_ANIMATIONS];
  int m_weaponId[NUM_WEAPONS];
  int m_animationCount;
  int m_meshId[32];
  int m_meshCount;
  unsigned int m_textureId[32];
  int m_textureCount;
  float m_motionBlend[3];
  float m_renderScale;

  float m_lodLevel;

  bool _use_textures;
 
  static float _walk_blend[];
  static float _run_blend[];
  static float _strut_blend[];
  
// constructors/destructor
public:
	Cal3d(Render*);
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
  void setDetailLevel(float level) { setLodLevel(level); }
  void setLodLevel(float lodLevel);
  void setMotionBlend(float *pMotionBlend, float delay);
  void setState(int state, float delay);

  bool useTextures() { return _use_textures; }
  void action(const std::string &action);
  Graphics::RotationStyle rotationStyle() { return Graphics::ROS_NORMAL; }
  
protected:
  unsigned int loadTexture(const std::string& strFilename);
  void renderMesh(bool bWireframe, bool bLight, bool);

  static int instance_count;
  static std::map<std::string, CalCoreModel*> core_models;
  std::map<int, int> material_map;
  std::map<int, int> part_map;
  std::map<std::string, int> material_set_map;
  std::map<std::string, int> material_part_map;
};

} /* namespace Sear */

#endif /* _CAL3D_H_ */

