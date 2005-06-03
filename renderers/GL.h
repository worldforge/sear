// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

#ifndef SEAR_GL_RENDER_H
#define SEAR_GL_RENDER_H 1

#include <sage/sage.h>
#include <sage/GL.h>

#include <SDL/SDL.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

#include <sigc++/object_slot.h>

#include <wfmath/axisbox.h>
#include <wfmath/quaternion.h>

#include "Light.h"
#include "Render.h"
#include "RenderSystem.h"

namespace Sear {

class System;
class Camera;
class WorldEntity;
class Model;
class Graphics;
class ObjectRecord;
class ModelRecord;
class Console;

class GL : public Render, public SigC::Object {
public:GL();
~GL();
  void init();
  void initContext();
  void shutdown();
  void invalidate();
  bool createWindow(unsigned int width, unsigned int height, bool fullscreen);
  void destroyWindow();
  void toggleFullscreen();

  void registerCommands(Console *console) {}
  void runCommands(const std::string &command) {}

private:

  bool m_fontInitialised;
  unsigned int m_width, m_height;
  bool m_fullscreen;
  SDL_Surface *m_screen;

  static void checkError();


public:
  void renderMeshArrays( Mesh &mesh, unsigned int offset, bool multitexture);
  void vboMesh( Mesh &mesh);
  void cleanVBOMesh(Mesh &mesh);
  

  void initLighting();
  void initFont();
  void shutdownFont();
  void print(int x, int y, const char*, int set);
  void print3D(const char* string, int set);
  inline void newLine();

  void buildColourSet();
  void drawTextRect(int, int, int, int, int);
  float distFromNear(float,float,float);  
  void setColour(float red, float green, float blue, float alpha) { glColor4f(red, green, blue, alpha); }

  int patchInFrustum(const WFMath::AxisBox<3> &);
  
  void procEvent(int, int);
  int getWindowWidth() const { return m_width; }
  int getWindowHeight() const { return m_height; }

  std::string getActiveID();// { return activeID; }
  void checkModelStatus(const std::string &) {}
  void setModelInUse(const std::string &, bool) {}

  void readConfig(varconf::Config &config);
  void writeConfig(varconf::Config &config);
  void setupStates();

  float getLightLevel() { return m_light_level; }

  void translateObject(float x, float y, float z);
  void rotate(float angle, float x, float y, float z);
//  void rotateObject(WorldEntity *we, int type);
  inline void rotateObject(ObjectRecord *, ModelRecord *);
  inline void scaleObject(float scale);
  void setViewMode(int type);
  void setMaterial(float *ambient, float *diffuse, float *specular, float shininess, float *emissive);
  void renderArrays(unsigned int type, unsigned int offset, unsigned int number_of_points, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data, bool multitexture);
  void renderElements(unsigned int type, unsigned int number_of_points, int *faces_data, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data, bool multitexture);
  void drawQueue(QueueMap &queue, bool select_mode);
  void drawMessageQueue(MessageList &list);
  void drawNameQueue(MessageList &list);
  void drawOutline(ModelRecord *);
 
  inline void store();
  inline void restore();
  void beginFrame();
  void endFrame(bool select_mode);
  void drawSplashScreen();
  void applyQuaternion(const WFMath::Quaternion & quaternion);
  void applyLighting();
  inline void resetSelection();
  inline void renderActiveName();
  inline void applyCharacterLighting(float x, float y, float z);
  inline void getFrustum(float [6][4]);
 
  void beginRecordList(unsigned int list) { glNewList(list, GL_COMPILE_AND_EXECUTE); }
  void endRecordList() { glEndList(); }
  void playList(unsigned int list) { glCallList(list); }
  unsigned int getNewList() { return glGenLists(1); }
  void freeList(unsigned int list) { if (glIsList(list)) glDeleteLists(list, 1); };
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

  float m_frustum[6][4];
  
//  std::string activeID;
  std::string m_active_name;
  WorldEntity *m_activeEntity;
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

  static const unsigned int NUM_COLOURS = 512;
  GLubyte m_colourArray[NUM_COLOURS][3];
  WorldEntity *m_entityArray[NUM_COLOURS];
  unsigned int m_colour_index;
  std::map<unsigned int, std::string> m_colour_mapped;
  
  GLint m_redBits, m_greenBits, m_blueBits;
  GLuint m_redMask, m_greenMask, m_blueMask;
  int m_redShift, m_greenShift, m_blueShift;
  
  static GLuint makeMask(GLuint bits) {
    // Create an 8-bit mask with 'bits' set to 1
    return (0xFF >> (8 - bits));
  }
  void resetColours() { m_colour_index = 1; }
  inline WorldEntity *getSelectedID(unsigned int i);
  void nextColour(WorldEntity*);

  void setupExtensions();
  bool m_use_sgis_generate_mipmap;
  bool m_use_fsaa;
  bool m_multi_texture_mode;
  bool m_initialised;
  
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);


};


} // namespace Sear

#endif
