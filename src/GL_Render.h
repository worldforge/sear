// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _GL_RENDER_H_
#define _GL_RENDER_H_ 1

#include <GL/gl.h>
#include <SDL/SDL.h>
#include <string>
#include <list>
#include <map>

#include "Render.h"

#include <wfmath/axisbox.h>
#include <wfmath/quaternion.h>


class System;
class Terrain;
class SkyBox;
class Camera;
class WorldEntity;
class Client;
class Model;
class Character;
class BoundBox;
class BillBoard;
class Impostor;
class WireFrame;
class Models;

class GL_Render : public Render {

typedef std::list<WorldEntity*> Queue;
	
public:
  GL_Render();
  GL_Render(System *);
  
  ~GL_Render();
  void init();
  void initWindow(int width, int height);
  void shutdown() {};

  void initLighting();
  void initFont();
  void shutdownFont();
  void print(GLint x, GLint y, const char*, int set);
  void print3D(const char* string, int set);
  void newLine();

  int requestTexture(const std::string&, bool clamp = false);
  int requestMipMap(const std::string &, bool clamp = false);
  int requestTextureMask(const std::string&, bool clamp = false);
  void createTexture(SDL_Surface*, unsigned int, bool);
  void createMipMap(SDL_Surface*, unsigned int, bool);
  void createTextureMask(SDL_Surface*, unsigned int, bool);
  
  void initModels();
  GLuint getTextureID(int texture_id);
  static GL_Render *instance() { return _instance; }
 void buildColourSet();
  void drawScene(const std::string &,bool);
  void drawBBox(BoundBox*);
  void drawModel(Models*);
  void drawTextRect(int, int, int, int, int);
  void drawBillBoard(BillBoard *);
  void drawImpostor(Impostor*);
  void drawWireFrame(WorldEntity *);
  void stateChange(State);
  void extractFrustum();
  int CubeInFrustum(WorldEntity *);
  bool SphereInFrustum(WorldEntity *);
  int patchInFrustum(WFMath::AxisBox<3>);
  float distFromNear(float,float,float);  
void setColour(float red, float green, float blue, float alpha) { glColor4f(red, green, blue, alpha); }
	  
  bool PointInFrustum(float, float, float);
  void procEvent(int, int);
  int getWindowWidth() { return window_width; }
  int getWindowHeight() { return window_height; }

  void switchTexture(int texture) { glBindTexture(GL_TEXTURE_2D, getTextureID(texture));}
  std::string getActiveID() { return activeID; }
  void setCallyMotion(float, float, float);
  void setCallyState(int);
  void executeCallyAction(int);
  Camera* getCamera() { return camera; }
  Terrain* getTerrain() { return terrain; }
  SkyBox* getSkyBox() { return skybox; }
  void processObjectProperties(ObjectProperties *);
  void checkModelStatus(const std::string &);
  void setModelInUse(const std::string &, bool);
  void buildDisplayLists();

  void readConfig();
  void writeConfig();
  void nextState(int);
  void setupStates();
void readComponentConfig();

  float getLightLevel() { return _light_level; }
protected:
  System *_system;
  int window_width;
  int window_height;

  Uint32 time;
  
  const float fov;
  const float near_clip;
  const float far_clip;
  float _far_clip_dist;

  float frustum[6][4];

  std::map<std::string, int> texture_map;
  
  int next_id;
  GLuint base;
  std::list<GLuint> textureList;

  int font_id;
  int splash_id;
  static GL_Render *_instance;

  std::string activeID;
  std::string active_name;
  int x_pos;
  int y_pos;
  
  Terrain *terrain;
  SkyBox *skybox;
  Camera *camera;
  
  Client *client;
  Model *player_model;
  
  int num_frames;
  float frame_time;
  std::map<std::string, Queue> render_queue;
  std::map<std::string, Queue> model_queue;
  std::map<std::string, Queue> billboard_queue;
  std::map<std::string, Queue> imposter_queue;
  std::map<std::string, Queue> wireframe_queue;

  void buildQueues(WorldEntity*, int);
  void drawModelQueue(bool);
  void drawRenderQueue(bool);
  void drawBBoardQueue(bool);
  void drawImpostorQueue(bool);
  void drawMessageQueue(bool);
  void drawWireFrameQueue(bool);
  
  float frame_rate;
  float model_detail;

  WFMath::Quaternion orient;
  WFMath::AxisBox<3> GL_Render::bboxCheck(WFMath::AxisBox<3> bbox);
 
  typedef struct {
    std::string model_name;
    bool in_use;
    Model *model;
    Models *models;
  } ModelStruct;
  std::map<std::string, ModelStruct*> _entity_models;

  typedef struct {
//    light id;
    float kc;
    float kl;
    float kq;
    float x_pos;
    float y_pos;
    float z_pos;
    float w_pos;
    float ambient[4];
    float diffuse[4];
    float specular[4];
//    colour;
//    spotlight stuff;
  } lightStruct;

  typedef enum {
    LIGHT_CHARACTER = 0,
    LIGHT_SUN,
    LIGHT_LAST
  } lightSources;

  lightStruct lights[LIGHT_LAST];
  GLuint states;
  GLuint _states;

  float _speech_offset_x;
  float _speech_offset_y;
  float _speech_offset_z;
 
  float _lower_frame_rate_bound;
  float _upper_frame_rate_bound;

  Character *_character;

  
  void CheckError();

  State current_state;
  int _current_state;

  typedef struct {
    bool alpha_test;
    bool blend;
    bool lighting;
    bool textures;
    bool colour_material;
    bool depth_test;
    bool cull_face;
    bool cull_face_cw;
    bool stencil;
    bool fog;
  } stateProperty;
  
  stateProperty stateProperties[LAST_STATE];
  void stateDisplayList(GLuint, State, State);

  float _fog_start;
  float _fog_end;
  float _light_level;
  bool _depth_buffer_flag;

  std::map<std::string, Models*> _boundbox_cache;
  std::map<std::string, BillBoard*> _billboard_cache;
  std::map<std::string, Impostor*> _impostor_cache;
  
  static float _halo_blend_colour[4];// = {0.0f, 0.0f, 1.0f, 0.4f};
  static float _halo_colour[3];// = {0.0f, 0.0f, 1.0f};
private:
  // Consts
  static const int sleep_time = 5000;

  static const char * const font_texture = "ui_font";
  static const char * const splash_texture = "ui_splash";
  
  // Config key strings
  
  static const char * const KEY_use_textures = "render_use_textures";
  static const char * const KEY_use_lighting = "render_use_lighting";
  static const char * const KEY_show_fps = "render_show_fps";
  static const char * const KEY_use_stencil = "render_use_stencil";

  static const char * const KEY_character_light_kc = "character_light_kc";
  static const char * const KEY_character_light_kl = "character_light_kl";
  static const char * const KEY_character_light_kq = "character_light_kq";
  
  static const char * const KEY_character_light_ambient_red = "character_light_ambient_red";
  static const char * const KEY_character_light_ambient_blue = "character_light_ambient_blue";
  static const char * const KEY_character_light_ambient_green = "character_light_ambient_green";
  static const char * const KEY_character_light_ambient_alpha = "character_light_ambient_alpha";

  static const char * const KEY_character_light_diffuse_red = "character_light_diffuse_red";
  static const char * const KEY_character_light_diffuse_blue = "character_light_diffuse_blue";
  static const char * const KEY_character_light_diffuse_green = "character_light_diffuse_green";
  static const char * const KEY_character_light_diffuse_alpha = "character_light_diffuse_alpha";
  
  static const char * const KEY_character_light_specular_red = "character_light_specular_red";
  static const char * const KEY_character_light_specular_blue = "character_light_specular_blue";
  static const char * const KEY_character_light_specular_green = "character_light_specular_green";
  static const char * const KEY_character_light_specular_alpha = "character_light_specular_alpha";
  
  static const char * const KEY_sun_light_kc = "sun_light_kc";
  static const char * const KEY_sun_light_kl = "sun_light_kl";
  static const char * const KEY_sun_light_kq = "sun_light_kq";
  
  static const char * const KEY_sun_light_specular_red = "sun_light_specular_red";
  static const char * const KEY_sun_light_specular_blue = "sun_light_specular_blue";
  static const char * const KEY_sun_light_specular_green = "sun_light_specular_green";
  static const char * const KEY_sun_light_specular_alpha = "sun_light_specular_alpha";

  static const char * const KEY_lower_frame_rate_bound = "lower_frame_rate_bound";
  static const char * const KEY_upper_frame_rate_bound = "upper_frame_rate_bound";
  
  static const char * const KEY_speech_offset_x = "speech_offset_x";
  static const char * const KEY_speech_offset_y = "speech_offset_y";
  static const char * const KEY_speech_offset_z = "speech_offset_z";
  
  static const char * const KEY_fog_start = "fog_start";
  static const char * const KEY_fog_end = "fog_end";

  static const char * const KEY_far_clip_dist = "far_clip_dist";
  
  // Default config values
  static const float DEFAULT_character_light_kc = 1.0f;
  static const float DEFAULT_character_light_kl = 0.0f;
  static const float DEFAULT_character_light_kq = 0.0f;

  static const float DEFAULT_character_light_ambient_red = 0.0f;
  static const float DEFAULT_character_light_ambient_green = 0.0f;
  static const float DEFAULT_character_light_ambient_blue = 0.0f;
  static const float DEFAULT_character_light_ambient_alpha = 0.0f;
  
  static const float DEFAULT_character_light_diffuse_red = 0.0f;
  static const float DEFAULT_character_light_diffuse_green = 0.0f;
  static const float DEFAULT_character_light_diffuse_blue = 0.0f;
  static const float DEFAULT_character_light_diffuse_alpha = 0.0f;

  static const float DEFAULT_character_light_specular_red = 0.0f;
  static const float DEFAULT_character_light_specular_green = 0.0f;
  static const float DEFAULT_character_light_specular_blue = 0.0f;
  static const float DEFAULT_character_light_specular_alpha = 0.0f;
  
  static const float DEFAULT_sun_light_kc = 1.0f;
  static const float DEFAULT_sun_light_kl = 0.0f;
  static const float DEFAULT_sun_light_kq = 0.0f;

  static const float DEFAULT_sun_light_specular_red = 0.0f;
  static const float DEFAULT_sun_light_specular_green = 0.0f;
  static const float DEFAULT_sun_light_specular_blue = 0.0f;
  static const float DEFAULT_sun_light_specular_alpha = 0.0f;

  static const float DEFAULT_use_textures = true;
  static const float DEFAULT_use_lighting = true;
  static const float DEFAULT_show_fps = true;
  static const float DEFAULT_use_stencil = true;

  static const float DEFAULT_lower_frame_rate_bound = 25.0f;
  static const float DEFAULT_upper_frame_rate_bound = 30.0f;

  static const float DEFAULT_speech_offset_x = 0.0f;
  static const float DEFAULT_speech_offset_y = 0.0f;
  static const float DEFAULT_speech_offset_z = 0.0f;

  static const float DEFAULT_fog_start = 20.0f;
  static const float DEFAULT_fog_end = 35.0f;
  static const float DEFAULT_far_clip_dist = 100.0f;
};

#endif /* _GL_RENDER_H_ */
