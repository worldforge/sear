// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Graphics.h,v 1.15 2004-05-23 21:28:35 jmt Exp $

#ifndef SEAR_GRAPHICS_H
#define SEAR_GRAPHICS_H 1

#include <string>
#include <list>
#include <map>

#include <sigc++/object_slot.h>

#include <wfmath/axisbox.h>
#include <wfmath/quaternion.h>

#include "Render.h"
#include "interfaces/ConsoleObject.h"

namespace varconf {
class Config;

}

namespace Sear {
class TerrainRenderer;
class ModelHandler;
class BillBoard;
class BoundBox;
class Camera;
class Impostor;
class Render;
class WorldEntity;
class System;
class Character;
class Console;
class Compass;

class Graphics : public ConsoleObject, public SigC::Object{

public:

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

  
  void init();
  void initWindow(int width, int height);
  void shutdown();

  float getLightLevel();
  void buildQueues(WorldEntity *we, int depth, bool select_mode, Render::QueueMap &queue, Render::MessageList &list); 
  void drawScene(const std::string &command ,bool, float);
  void drawWorld(const std::string &command ,bool, float);
  void updateDetailLevels(float);
  
  std::string getActiveID();

  void setRenderer(Render *r) { _renderer = r; }
  
  Render *getRender() { return _renderer; }
  Camera* getCamera() { return _camera; }
  void setupStates();
  void readConfig();
  void writeConfig();
  void readComponentConfig();
  void writeComponentConfig();

  WFMath::Quaternion getCameraOrientation() { return orient; }

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);

  TerrainRenderer *getTerrainRenderer() const { return tr; }
protected:
  System *_system;
  Render *_renderer;
  Character *_character;
  Camera *_camera;
  ModelHandler *_model_handler;

  WFMath::Quaternion orient;

  Render::QueueMap _render_queue;
  Render::MessageList _message_list;
  
  int _num_frames;
  float _frame_time;
  float _frame_rate;
  float _lower_frame_rate_bound;
  float _upper_frame_rate_bound;

  float frustum[6][4];
  bool _initialised;
  
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
private:
    Compass* m_compass;
  
 TerrainRenderer *tr;
};

} /* namespace Sear */

#endif /* SEAR_GRAPHICS_H */
