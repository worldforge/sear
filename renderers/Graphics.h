// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: Graphics.h,v 1.4 2005-04-12 14:33:13 simon Exp $

#ifndef SEAR_GRAPHICS_H
#define SEAR_GRAPHICS_H 1

#include <string>
#include <list>
#include <map>

#include <sigc++/object_slot.h>

#include <wfmath/axisbox.h>
#include <wfmath/quaternion.h>

#include "Light.h"
#include "Render.h"
#include "interfaces/ConsoleObject.h"

namespace varconf {
class Config;

}

namespace Sear {
class Camera;
class WorldEntity;
class System;
class Character;
class Console;
class Compass;
class LightManager;

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
  void drawScene(bool, float);
  void drawWorld(bool, float);
  
  std::string getActiveID();

  void setRenderer(Render *r) { m_renderer = r; }
  
  Render *getRender() { return m_renderer; }
//  Camera* getCamera() { return m_camera; }
  void setupStates();
  void readConfig(varconf::Config &config);
  void writeConfig(varconf::Config &config);

  WFMath::Quaternion getCameraOrientation() { return m_orient; }

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);

protected:
  System *m_system;
  Render *m_renderer;
//  Character *m_character;
//  Camera *m_camera;

  WFMath::Quaternion m_orient;

  Render::QueueMap m_render_queue;
  Render::MessageList m_message_list;
  
  int m_num_frames;
  float m_frame_time;
  float m_frame_rate;

  float m_frustum[6][4];
  bool m_initialised;
  
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
private:
    Compass* m_compass;
  
  LightManager *m_lm;

  Light m_fire;

    /**
    Helper to qeueue the models for a single object record
    */
    void drawObject(ObjectRecord* obj, 
                        bool select_mode,
                        Render::QueueMap &render_queue,
                        Render::MessageList &message_list);
};

} /* namespace Sear */

#endif /* SEAR_GRAPHICS_H */
