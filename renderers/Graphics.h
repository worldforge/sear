// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2008 Simon Goodall, University of Southampton

// $Id: Graphics.h,v 1.20 2008-05-08 20:22:56 simon Exp $

#ifndef SEAR_GRAPHICS_H
#define SEAR_GRAPHICS_H 1

#include <memory>
#include <string>
#include <list>
#include <map>

#include <sigc++/trackable.h>

#include <wfmath/axisbox.h>
#include <wfmath/quaternion.h>

#include "Light.h"
#include "Render.h"
#include "RenderTypes.h"
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

class Graphics : public ConsoleObject, public sigc::trackable {

public:

  Graphics(System *system);
  ~Graphics();
  
  void init();
  void initWindow(int width, int height);
  void shutdown();
  bool isInitialised() const { return m_initialised; }

  float getLightLevel();
  
  void buildQueues(WorldEntity *we, 
    int depth,
    bool select_mode,
    Render::QueueMap &queue,
    Render::MessageList &message_list,
    Render::MessageList &name_list,
    float time_elapsed);
     
  void drawScene(bool, float);
  void setCameraTransform();
  void drawWorld(bool, float);
  
  std::string getActiveID();

  void setRenderer(Render *r) { m_renderer = r; }
  
  Render *getRender() { return m_renderer; }
  void setupStates();
  void readConfig(varconf::Config &config);
  void writeConfig(varconf::Config &config);

  WFMath::Quaternion getCameraOrientation() { return m_orient; }

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);

  void contextCreated() {}
  void contextDestroyed(bool check) {}

protected:
  System *m_system;
  Render *m_renderer;

  WFMath::Quaternion m_orient;

  Render::QueueMap m_render_queue;
  Render::MessageList m_message_list;
  Render::MessageList m_name_list;
 
  Render::QueueStaticObjectMap m_static_object_map;
  Render::QueueDynamicObjectMap m_dynamic_object_map;
  Render::QueueMatrixMap m_matrix_map;
  Render::QueueStateMap m_state_map;
  Render::QueueOldMap m_queue_old_map;
 
  int m_num_frames;
  float m_frame_time;
  float m_frame_rate;

  float m_frustum[6][4];
  bool m_initialised;
  
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
private:
  std::auto_ptr<Compass> m_compass;
  
  std::auto_ptr<LightManager> m_lm;

  Light m_fire;
  bool m_show_names, m_show_bbox;
  float m_modelview_matrix[4][4];
  float m_medium_dist, m_high_dist;

  StateID m_state_weather, m_state_terrain, m_state_select, m_state_cursor;

    /**
    Helper to qeueue the models for a single object record
    */
    void drawObject(SPtr<ObjectRecord> obj, 
                        bool select_mode,
                        Render::QueueMap &render_queue,
                        Render::MessageList &message_list,
                        Render::MessageList &name_list,
                        float time_elapsed);

     void drawObjectExt(const std::string &model_id,
                        SPtr<ObjectRecord> obj, 
                        WorldEntity *obj_we,
                        bool select_mode,
                        Render::QueueMap &render_queue,
                        Render::MessageList &message_list,
                        Render::MessageList &name_list,
                        float time_elapsed, float camera_dist);
                        
    void drawFire(WorldEntity*);
    
};

} /* namespace Sear */

#endif /* SEAR_GRAPHICS_H */
