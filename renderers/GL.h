// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall, University of Southampton

#ifndef SEAR_GL_RENDER_H
#define SEAR_GL_RENDER_H 1

#include <Eris/EntityRef.h>

#include <sage/sage.h>
#include <sage/GL.h>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

#include <sigc++/trackable.h>

#include <wfmath/axisbox.h>

#include "Light.h"
#include "Render.h"

#include "interfaces/ConsoleObject.h"

namespace WFMath {
class Quaternion;
}

namespace Sear {

class System;
class Camera;
class WorldEntity;
class Model;
class Graphics;
class ObjectRecord;
class ModelRecord;
class Console;

class GL : public Render, public sigc::trackable, public ConsoleObject {
public:
  GL();
  virtual ~GL();

  void init();
  void shutdown();
  
  bool isInitialised() const { return m_initialised; }

  int contextCreated();
  void contextDestroyed(bool check);

  bool createWindow(unsigned int width, unsigned int height, bool fullscreen);
  void destroyWindow();
  void toggleFullscreen();

  void registerCommands(Console *console) ;
  void runCommand(const std::string &command, const std::string &args) ;

private:

  bool m_fontInitialised;
  unsigned int m_width, m_height;
  bool m_fullscreen;
  struct SDL_Surface *m_screen;

  /**
   * Checks to see if OpenGL logged an error.
   * @return 1 on error, 0 otherwise.
   */
  static int checkError();


public:
  void initLighting();
  void initFont();
  void shutdownFont(bool check);
  void print(int x, int y, const char*, int set);
  void print3D(const char* string, int set);
  inline void newLine() const;

  void getScreenCoords(int & x, int & y, double z_offset) const;

  void buildColourSet();
  void drawTextRect(int, int, int, int, int) const;
  float distFromNear(float,float,float) const;
  void setColour(float red, float green, float blue, float alpha) const { glColor4f(red, green, blue, alpha); }

  int axisBoxInFrustum(const WFMath::AxisBox<3> &bbox) const;
  
  void procEvent(int, int);
  int getWindowWidth() const { return m_width; }
  int getWindowHeight() const { return m_height; }
  bool isFullScreen() const { return m_fullscreen; }

  std::string getActiveID() const;
  WorldEntity *getActiveEntity() const;

  void readConfig(varconf::Config &config);
  void writeConfig(varconf::Config &config);
  void setupStates();

  float getLightLevel() const { return m_light_level; }

  void translateObject(float x, float y, float z) const;
  void rotate(float angle, float x, float y, float z) const;
  inline void rotateObject(ObjectRecord*, ModelRecord*) const;
  inline void scaleObject(float scale) const;
  void setViewMode(int type) const;
  void setMaterial(float *ambient, float *diffuse, float *specular, float shininess, float *emissive) const;
  void renderArrays(unsigned int type, unsigned int offset, unsigned int number_of_points, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data, bool multitexture) const;
  void drawQueue(QueueMap &queue, bool select_mode);
  void drawMessageQueue(MessageList &list);
  void drawNameQueue(MessageList &list);
  void drawOutline(ModelRecord*);

  void drawQueue(const QueueStaticObjectMap &object_map,
                   const QueueMatrixMap &matrix_map,
                   const QueueStateMap &state_map,
                   bool select_mode);


  void drawQueue(const QueueDynamicObjectMap &object_map,
                   const QueueMatrixMap &matrix_map,
                   const QueueStateMap &state_map,
                   bool select_mode);

 
  void store() const { glPushMatrix(); }
  void restore() const { glPopMatrix(); }
  void beginFrame();
  void endFrame(bool select_mode);
  void drawSplashScreen();
  void applyQuaternion(const WFMath::Quaternion & quaternion) const;
  void applyLighting();
  inline void resetSelection();
  inline void renderActiveName();
  inline void applyCharacterLighting(float x, float y, float z);
  inline void getFrustum(float [6][4]);
  virtual void getModelviewMatrix(float m[4][4]);

  void selectTerrainColour(WorldEntity * we);
  void resize(int width, int height);

  bool getWorldCoords(int x, int y, float &wx, float &wy,float &wz);

protected:
  System *m_system;
  Graphics *m_graphics;

  const float m_fov;
  float m_near_clip;
  float m_far_clip_dist;

  GLuint m_base;

  int m_font_id;
  int m_splash_id;

  StateID m_state_font, m_state_splash;

  float m_frustum[6][4];
  
  std::string m_active_name;
  Eris::EntityRef m_activeEntity;
  int m_x_pos;
  int m_y_pos;
  
  void buildQueues(WorldEntity*, int, bool);

  typedef enum {
    LIGHT_CHARACTER = 0,
    LIGHT_SUN,
    LIGHT_LAST
  } lightSources;

  Light m_lights[LIGHT_LAST];

  float m_speech_offset_x;
  float m_speech_offset_y;
  float m_speech_offset_z;

  float m_fog_start;
  float m_fog_end;
  float m_light_level;

  std::vector<WorldEntity*> m_entityArray;
  std::map<WorldEntity*, uint32_t> m_entityColourMap;

  uint32_t m_colour_index;
  std::map<uint32_t, std::string> m_colour_mapped;
  
  GLint m_redBits, m_greenBits, m_blueBits;
  GLuint m_redMask, m_greenMask, m_blueMask;
  uint32_t m_redShift, m_greenShift, m_blueShift;
  
  static GLuint makeMask(GLuint bits) {
    // Create an 8-bit mask with 'bits' set to 1
    return (0xFF >> (8 - bits));
  }
  void resetColours() { 
    m_colour_index = 1; 
    m_entityColourMap.clear();
  }
  WorldEntity *getSelectedID(unsigned int i) const  {
    return (i >= m_entityArray.size()) ? (NULL) : (m_entityArray[i]);
  }
  void nextColour(WorldEntity*, bool);

  int setupExtensions();
  bool m_use_fsaa;
  bool m_use_vbo;
  bool m_initialised;
  
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);

  int m_sdl_gl_red_size;
  int m_sdl_gl_green_size;
  int m_sdl_gl_blue_size;
  int m_sdl_gl_alpha_size;

  int m_sdl_gl_accum_red_size;
  int m_sdl_gl_accum_green_size;
  int m_sdl_gl_accum_blue_size;
  int m_sdl_gl_accum_alpha_size;

  int m_sdl_gl_doublebuffer;
  int m_sdl_gl_buffer_size;
  int m_sdl_gl_depth_size;
  int m_sdl_gl_stencil_size;
  int m_sdl_gl_stereo;
  int m_sdl_gl_swap_control;
  int m_sdl_gl_accelerated_visual;

  int m_sdl_gl_multisamplesamples;
  int m_sdl_gl_multisamplebuffers;
  int m_selection_counter;
};


} // namespace Sear

#endif
