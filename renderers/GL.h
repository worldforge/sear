// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: GL.h,v 1.33 2003-06-12 20:34:53 simon Exp $

#ifndef SEAR_GL_RENDER_H
#define SEAR_GL_RENDER_H 1

#include <GL/gl.h>
#include <SDL/SDL.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

#include <sigc++/object_slot.h>

#include <wfmath/axisbox.h>
#include <wfmath/quaternion.h>

#include "src/Light.h"
#include "src/Render.h"

#include "TextureManager.h"
#include "StateManager.h"

namespace Sear {

class System;
class Terrain;
class Sky;
class Camera;
class WorldEntity;
class Model;
class Graphics;
class ObjectRecord;
class ModelRecord;

class GL : public Render, public SigC::Object {

public:
  GL();
  GL(System *, Graphics *);
  
  ~GL();
  void init();
  void initWindow(int width, int height);
  void shutdown();

  void initLighting();
  void initFont();
  void shutdownFont();
  void print(GLint x, GLint y, const char*, int set);
  void print3D(const char* string, int set);
  inline void newLine();

  TextureID requestTexture(const std::string &texture_name) {
    assert(_texture_manager != NULL);
    return _texture_manager->requestTextureID(texture_name, false);
  }
 
  TextureID requestTextureMask(const std::string &texture_name) {
    assert(_texture_manager != NULL);
    return _texture_manager->requestTextureID(texture_name, true);
  }
 
  static GL *instance() { return _instance; }
  void buildColourSet();
  void drawTextRect(int, int, int, int, int);
//  void stateChange(const std::string &state);
//  void stateChange(StateProperties *state);
  void stateChange(StateID state) {
    _state_manager->stateChange(state);
  }
  StateID getStateID(const std::string &state) {
    return _state_manager->getState(state);
  } 
  float distFromNear(float,float,float);  
  void setColour(float red, float green, float blue, float alpha) { glColor4f(red, green, blue, alpha); }

  int patchInFrustum(WFMath::AxisBox<3>);
  
  void procEvent(int, int);
  int getWindowWidth() { return window_width; }
  int getWindowHeight() { return window_height; }

  void switchTexture(TextureID texture) {
    assert(_texture_manager != NULL);
    _texture_manager->switchTexture(texture);
  }
  void switchTexture(unsigned int unit, TextureID texture) {
    assert(_texture_manager != NULL);
    _texture_manager->switchTexture(unit, texture);
  }
  
  std::string getActiveID();// { return activeID; }
  void checkModelStatus(const std::string &) {}
  void setModelInUse(const std::string &, bool) {}

  void readConfig();
  void writeConfig();
  void setupStates();

  float getLightLevel() { return _light_level; }

  void translateObject(float x, float y, float z);
  void rotate(float angle, float x, float y, float z);
//  void rotateObject(WorldEntity *we, int type);
  inline void rotateObject(ObjectRecord *, ModelRecord *);
  inline void scaleObject(float scale);
  void setViewMode(int type);
  void setMaterial(float *ambient, float *diffuse, float *specular, float shininess, float *emissive);
  void renderArrays(unsigned int type, unsigned int offset, unsigned int number_of_points, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data, bool multitexture);
  void renderElements(unsigned int type, unsigned int number_of_points, int *faces_data, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data, bool multitexture);
  void drawQueue(QueueMap &queue, bool select_mode, float time_elapsed);
  void drawMessageQueue(MessageList &list);
  void drawOutline(ModelRecord *);
 
  inline void store();
  inline void restore();
  void beginFrame();
  void endFrame(bool select_mode);
  void drawFPS(float fps);
  void drawSplashScreen();
  void applyQuaternion(WFMath::Quaternion quaternion);
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
  void setTextureScale(unsigned int unit, float scale);
  TextureManager *getTextureManager() const { return _texture_manager; }
  StateManager *getStateManager() const { return _state_manager; }
protected:
  System *_system;
  Graphics *_graphics;
  int window_width;
  int window_height;

  const float fov;
  const float near_clip;
  float _far_clip_dist;
  float _texture_scale;

  std::map<std::string, int> texture_map;
  
  int next_id;
  GLuint base;
  std::vector<GLuint> textureList;

  int font_id;
  int splash_id;
  static GL *_instance;

  float frustum[6][4];
  
//  std::string activeID;
  std::string active_name;
  WorldEntity *activeEntity;
  int x_pos;
  int y_pos;
  
  Terrain *terrain;
  
  void buildQueues(WorldEntity*, int, bool);

  typedef enum {
    LIGHT_CHARACTER = 0,
    LIGHT_SUN,
    LIGHT_LAST
  } lightSources;

  Light lights[LIGHT_LAST];

  float _speech_offset_x;
  float _speech_offset_y;
  float _speech_offset_z;
 
  void CheckError();

  StateProperties *_cur_state;

  void stateDisplayList(GLuint &, StateProperties *previous_state, StateProperties *next_state);

  float _fog_start;
  float _fog_end;
  float _light_level;

  std::map<std::string, GLuint> _state_map;

  static const unsigned int NUM_COLOURS = 512;
  GLubyte colourArray[NUM_COLOURS][3];
  WorldEntity *entityArray[NUM_COLOURS];
  unsigned int colour_index;
  std::map<unsigned int, std::string> colour_mapped;
  
  GLint redBits, greenBits, blueBits;
  GLuint redMask, greenMask, blueMask;
  int redShift, greenShift, blueShift;
  
  inline static  GLuint makeMask(GLuint bits);
  inline void resetColours();
  inline WorldEntity *getSelectedID(unsigned int i);
  void nextColour(WorldEntity*);

  void setupExtensions();
  bool use_ext_texture_filter_anisotropic;
  bool use_sgis_generate_mipmap;
  bool _initialised;
  bool _multi_texture_mode;
  
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);


  TextureManager *_texture_manager;
  StateManager *_state_manager;
};

} /* namespace Sear */
#endif /* SEAR_GL_RENDER_H */
