// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_ 1

#include <string>
#include <list>
#include <map>

#include <wfmath/axisbox.h>
#include <wfmath/quaternion.h>

#include "ObjectLoader.h"

namespace Sear {

class ModelHandler;
class BillBoard;
class BoundBox;
class Camera;
class Impostor;
class Render;
class Sky;
class Terrain;
class WorldEntity;
class System;
class Character;
class Graphics {

public:

typedef enum {
  RENDER_UNKNOWN = 0,
  RENDER_LIGHTING,
  RENDER_TEXTURES,
  RENDER_SHADOWS,
  RENDER_FPS,
  RENDER_STENCIL,
  RENDER_LAST_STATE
} RenderState;

typedef enum {
  RES_INVALID = 0,
  RES_POINT,
  RES_LINES,
  RES_TRIANGLES,
  RES_QUADS,
  RES_TRIANGLE_FAN,
  RES_TRIANGLE_STRIP,
  RES_QUAD_STRIP,
  RES_LAST_STYLE
} RenderStyle;

typedef enum {
  ROS_NONE = 0,
  ROS_NORMAL,
  ROS_POSITION,
  ROS_BILLBOARD,
  ROS_HALO
} RotationStyle;


  Graphics(System *system);
  ~Graphics();

  typedef std::list<WorldEntity*> Queue;
  
  void init();
  void initWindow(int width, int height);
  void shutdown();

  float getLightLevel();
  void buildQueues(WorldEntity *we, int depth, bool select_mode); 
  void drawScene(const std::string &command ,bool, float);
  void updateDetailLevels();	  
  
  std::string getActiveID();

  void setRenderer(Render *r) { _renderer = r; }
  
  Render *getRender() { return _renderer; }
  Camera* getCamera() { return _camera; }
  Terrain* getTerrain() { return _terrain; }
  Sky* getSky() { return _sky; }
 
  void setState(RenderState rs, bool state) { _renderState[rs] = state; }
  bool checkState(RenderState rs) { return _renderState[rs]; }

  void setupStates();
  void readConfig();
  void writeConfig();
  void readComponentConfig();
  void writeComponentConfig();

  WFMath::Quaternion getCameraOrientation() { return orient; }

protected:
  bool _renderState[RENDER_LAST_STATE];
  System *_system;
  Render *_renderer;
  Character *_character;
  Camera *_camera;
  Terrain *_terrain;
  Sky *_sky;
  ModelHandler *_model_handler;

  WFMath::Quaternion orient;

  std::map<std::string, Queue> _render_queue;
  
  int _num_frames;
  float _frame_time;
  float _frame_rate;
  float _lower_frame_rate_bound;
  float _upper_frame_rate_bound;

  float frustum[6][4];
  
private:
  // Consts
  static const int sleep_time = 5000;
  
  // Config key strings
  static const char * const KEY_use_textures = "render_use_textures";
  static const char * const KEY_use_lighting = "render_use_lighting";
  static const char * const KEY_show_fps = "render_show_fps";
  static const char * const KEY_use_stencil = "render_use_stencil";

  static const char * const KEY_lower_frame_rate_bound = "lower_frame_rate_bound";
  static const char * const KEY_upper_frame_rate_bound = "upper_frame_rate_bound";
  
  // Default config values
  static const float DEFAULT_use_textures = true;
  static const float DEFAULT_use_lighting = true;
  static const float DEFAULT_show_fps = true;
  static const float DEFAULT_use_stencil = true;

  static const float DEFAULT_lower_frame_rate_bound = 25.0f;
  static const float DEFAULT_upper_frame_rate_bound = 30.0f;
};

} /* namespace Sear */

#endif /* _GRAPHICS_H_ */
