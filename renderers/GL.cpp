// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: GL.cpp,v 1.69 2003-08-05 14:31:33 simon Exp $

#include <SDL/SDL_image.h>

#include <unistd.h>

#include <sage/sage.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <varconf/Config.h>
#include <wfmath/quaternion.h>
#include <wfmath/vector.h>
#include <Eris/Entity.h>
#include <Eris/World.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "src/Calendar.h"
#include "src/Camera.h"
#include "src/Console.h"
#include "src/Exception.h"
#include "src/Frustum.h"
#include "src/Graphics.h"
#include "src/Model.h"
#include "src/ModelHandler.h"
#include "src/ModelRecord.h"
#include "src/ObjectRecord.h"
#include "src/Sky.h"
#include "src/System.h"
#include "src/Terrain.h"
#include "src/WorldEntity.h"

#include "terrain/ROAM.h"
#include "sky/SkyBox.h"

#include "GL.h"

#ifdef HAVE_CONFIG
  #include "config.h"
#endif

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {
  // Consts
  static const int sleep_time = 5000;

  static const std::string font_texture = "ui_font";
  static const std::string TEXTURE_splash_texture = "ui_splash_screen";
  
  // Config key strings
  
  static const std::string KEY_use_textures = "render_use_textures";
  static const std::string KEY_use_lighting = "render_use_lighting";
  static const std::string KEY_show_fps = "render_show_fps";
  static const std::string KEY_use_stencil = "render_use_stencil";

  static const std::string KEY_character_light_kc = "character_light_kc";
  static const std::string KEY_character_light_kl = "character_light_kl";
  static const std::string KEY_character_light_kq = "character_light_kq";
  
  static const std::string KEY_character_light_ambient_red = "character_light_ambient_red";
  static const std::string KEY_character_light_ambient_blue = "character_light_ambient_blue";
  static const std::string KEY_character_light_ambient_green = "character_light_ambient_green";
  static const std::string KEY_character_light_ambient_alpha = "character_light_ambient_alpha";

  static const std::string KEY_character_light_diffuse_red = "character_light_diffuse_red";
  static const std::string KEY_character_light_diffuse_blue = "character_light_diffuse_blue";
  static const std::string KEY_character_light_diffuse_green = "character_light_diffuse_green";
  static const std::string KEY_character_light_diffuse_alpha = "character_light_diffuse_alpha";
  
  static const std::string KEY_character_light_specular_red = "character_light_specular_red";
  static const std::string KEY_character_light_specular_blue = "character_light_specular_blue";
  static const std::string KEY_character_light_specular_green = "character_light_specular_green";
  static const std::string KEY_character_light_specular_alpha = "character_light_specular_alpha";
  
  static const std::string KEY_sun_light_kc = "sun_light_kc";
  static const std::string KEY_sun_light_kl = "sun_light_kl";
  static const std::string KEY_sun_light_kq = "sun_light_kq";
  
  static const std::string KEY_sun_light_specular_red = "sun_light_specular_red";
  static const std::string KEY_sun_light_specular_blue = "sun_light_specular_blue";
  static const std::string KEY_sun_light_specular_green = "sun_light_specular_green";
  static const std::string KEY_sun_light_specular_alpha = "sun_light_specular_alpha";

  static const std::string KEY_lower_frame_rate_bound = "lower_frame_rate_bound";
  static const std::string KEY_upper_frame_rate_bound = "upper_frame_rate_bound";
  
  static const std::string KEY_speech_offset_x = "speech_offset_x";
  static const std::string KEY_speech_offset_y = "speech_offset_y";
  static const std::string KEY_speech_offset_z = "speech_offset_z";
  
  static const std::string KEY_fog_start = "fog_start";
  static const std::string KEY_fog_end = "fog_end";
	
  static const std::string KEY_far_clip_dist = "far_clip_dist";
  static const std::string KEY_texture_scale = "texture_scale";
  
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
  static const float DEFAULT_texture_scale = 10.0f;

static bool use_ext_compiled_vertex_array = false;
static bool use_multitexturing = false;
static std::string FONT = "font";
static std::string UI = "ui";
static std::string SPLASH = "splash";
static std::string DEFAULT = "default";
static std::string DEFAULT_TEXTURE = "default_texture";
static std::string DEFAULT_FONT = "default_font";
static std::string HALO = "halo";
static std::string MASK = "_mask";
static std::string RENDER = "render";
	
static GLfloat _halo_colour[4] = {1.0f, 0.0f, 1.0f, 1.0f};
static GLfloat activeNameColour[] = { 1.0f, 0.75f, 0.2f, 1.0f};
static GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
static GLfloat red[] =   { 1.0f, 0.0f, 0.0f, 1.0f };
static GLfloat yellow[] =  { 0.0f, 1.0f, 1.0f, 1.0f };
static GLfloat blackLight[]    = { 0.0f,  0.0f, 0.0f, 1.0f };

inline GLuint GL::makeMask(GLuint bits) {
  // Create an 8-bit mask with 'bits' set to 1
  return (0xFF >> (8 - bits));
}

inline WorldEntity *GL::getSelectedID(unsigned int i) {

  if (i >= NUM_COLOURS) return NULL; // Check for array out of bounds
  return entityArray[i]; // Return stored entity pointer
}

void GL::nextColour(WorldEntity *we) {
  entityArray[colour_index] = we; // Store entity in array slot
  glColor3ubv(colourArray[colour_index]); // Set colour from appropriate index
  ++colour_index; // Increment counter for next pass
}

inline void GL::resetColours(){
  colour_index = 1; // Set index to 1 as 0 is black and counts as unused
}

void GL::buildColourSet() {
  // Get the bits info from OpenGL
  glGetIntegerv (GL_RED_BITS, &redBits);
  glGetIntegerv (GL_GREEN_BITS, &greenBits);
  glGetIntegerv (GL_BLUE_BITS, &blueBits);

  // Create masks
  redMask = makeMask(redBits);
  greenMask = makeMask(greenBits);
  blueMask = makeMask(blueBits);
  // Calculate shifts
  redShift =   greenBits + blueBits;
  greenShift =  blueBits;
  blueShift = 0;
  
  // Pre-Calculate each colour and store in array
  for (unsigned int indx = 0; indx < NUM_COLOURS; ++indx) {
    GLubyte red = (indx & (redMask << redShift)) << (8 - redBits);
    GLubyte green = (indx & (greenMask << greenShift)) << (8 - greenBits);
    GLubyte blue = (indx & (blueMask << blueShift)) << (8 - blueBits);
    colourArray[indx][0] = red;
    colourArray[indx][1] = green;
    colourArray[indx][2] = blue;
  }
}

GL *GL::_instance = NULL;

GL::GL() :
  _system(NULL),
  window_width(0),
  window_height(0),
  fov(RENDER_FOV),
  near_clip(RENDER_NEAR_CLIP),
  next_id(1), // was 0 error?
  base(0),
  splash_id(-1),
  activeEntity(NULL),
  terrain(NULL),
  _cur_state(NULL),
  _fog_start(20.0f),
  _fog_end(35.0f),
  _light_level(1.0f),
  _initialised(false),
  _texture_manager(NULL),
  _state_manager(NULL)
{
  _instance = this;
  memset(entityArray, 0, NUM_COLOURS * sizeof(WorldEntity*));
}

GL::GL(System *system, Graphics *graphics) :
  _system(system),
  _graphics(graphics),
  window_width(0),
  window_height(0),
  fov(RENDER_FOV),
  near_clip(RENDER_NEAR_CLIP),
  next_id(1),
  base(0),
  splash_id(-1),
  activeEntity(NULL),
  terrain(NULL),
  _cur_state(NULL),
  _fog_start(20.0f),
  _fog_end(35.0f),
  _light_level(1.0f),
  _initialised(false),
  _texture_manager(NULL),
  _state_manager(NULL)
{
  _instance = this;
  memset(entityArray, 0, NUM_COLOURS * sizeof(WorldEntity*));
}

GL::~GL() {
  if (_initialised) shutdown();
}

void GL::shutdown() {
  writeConfig();
  shutdownFont();
  _texture_manager->shutdown();
  delete _texture_manager;
  _texture_manager = NULL;
  _state_manager->shutdown();
  delete _state_manager;
  _state_manager = NULL;
  _initialised = false;
}

void GL::initWindow(int width, int height) {
  if (debug) Log::writeLog("Render: Initialising Renderer", Log::LOG_DEFAULT);
  // TODO: put this into an info method 
  
    std::string vendor = string_fmt(glGetString(GL_VENDOR));
    std::string renderer = string_fmt(glGetString(GL_RENDERER));
    // TODO - CHECK OPENGL VERSION
    std::string version = string_fmt(glGetString(GL_VERSION));
    std::string extensions = string_fmt(glGetString(GL_EXTENSIONS));
  
//  if (debug) {
    Log::writeLog(std::string("GL_VENDER: ") + vendor, Log::LOG_DEFAULT);
    Log::writeLog(std::string("GL_RENDERER: ") + renderer, Log::LOG_DEFAULT);
    Log::writeLog(std::string("GL_VERSION: ") + version, Log::LOG_DEFAULT);
    Log::writeLog(std::string("GL_EXTENSIONS: ") + extensions, Log::LOG_DEFAULT);
//  }

  // These will be empty if there was a problem initialising the driver 
  if (vendor.empty() || renderer.empty()) {
    throw Exception("Error with OpenGL system");
  }
  
  glLineWidth(4.0f);
  
  //TODO: this needs to go into the set viewport method
  //Check for divide by 0
  if (height == 0) height = 1;
//  glLineWidth(2.0f); 
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Colour used to clear window
  glClearDepth(1.0); // Enables Clearing Of The Depth Buffer
  glClear(GL_DEPTH_BUFFER_BIT);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glDisable(GL_DITHER); 
  
  //Store window size
  window_width = width;
  window_height = height;

  setViewMode(PERSPECTIVE);
  setupExtensions();
  _texture_manager->init();
  _state_manager->init();
  //splash_id = requestTexture(TEXTURE_splash_texture);
  initFont();

}
  
void GL::init() {
  if (_initialised) shutdown();
  
  // Most of this should be elsewhere
  System::instance()->getGeneral().sigsv.connect(SigC::slot(*this, &GL::varconf_callback));
  _texture_manager = new TextureManager();
  _state_manager = new StateManager();
  readConfig();
//  splash_id = requestTexture(TEXTURE_splash_texture);
//  initFont();
  initLighting();
  // TODO: initialisation need to go into system?
  setupStates();
  if (debug) CheckError();
  _initialised = true;
}

void GL::initLighting() {
  if (debug) Log::writeLog("Render: initialising lighting", Log::LOG_DEFAULT);
  float gambient[4] = {0.1f, 0.1f,0.1f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,gambient);
  // Light values and coordinates
           
  // Setup and enable light 0
  glLightfv(GL_LIGHT0, GL_AMBIENT, lights[LIGHT_CHARACTER].ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lights[LIGHT_CHARACTER].diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lights[LIGHT_CHARACTER].specular);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, lights[LIGHT_CHARACTER].attenuation_constant);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, lights[LIGHT_CHARACTER].attenuation_linear);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, lights[LIGHT_CHARACTER].attenuation_quadratic);
//  glEnable(GL_LIGHT0);
  
  glLightfv(GL_LIGHT1, GL_AMBIENT, blackLight);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, blackLight);
  glLightfv(GL_LIGHT1, GL_SPECULAR, blackLight);
  
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, lights[LIGHT_SUN].attenuation_constant);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, lights[LIGHT_SUN].attenuation_linear);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, lights[LIGHT_SUN].attenuation_quadratic);
  glEnable(GL_LIGHT1);
}

void GL::initFont() {
  float cx; // Holds Our X Character Coord
  float cy; // Holds Our Y Character Coord
  if (debug) Log::writeLog("Render: Initailising Fonts", Log::LOG_DEFAULT);
  base=glGenLists(256); // Creating 256 Display Lists
  font_id = requestTexture(DEFAULT_FONT);
  switchTexture(font_id);
  for (int loop=0; loop<256; ++loop) {
    cx=(float)(loop%16)/16.0f; // X Position Of Current Character
    cy=(float)(loop/16)/16.0f; // Y Position Of Current Character
    glNewList(base+loop,GL_COMPILE); // Start Building A List
    glBegin(GL_QUADS); // Use A Quad For Each Character
      glTexCoord2f(cx,1-cy-0.0625f); // Texture Coord (Bottom Left)
      glVertex2i(0,0); // Vertex Coord (Bottom Left)
      glTexCoord2f(cx,1-cy); // Texture Coord (Top Left)
      glVertex2i(0,16); // Vertex Coord (Top Left)
      glTexCoord2f(cx+0.0625f,1-cy); // Texture Coord (Top Right)
      glVertex2i(16,16); // Vertex Coord (Top Right)
      glTexCoord2f(cx+0.0625f,1-cy-0.0625f); // Texture Coord (Bottom Right)
      glVertex2i(16,0);       // Vertex Coord (Bottom Right)
    glEnd(); // Done Building Our Quad (Character)
    glTranslated(10,0,0); // Move To The Right Of The Character
    glEndList(); // Done Building The Display List
  }// Loop Until All 256 Are Built
}

void GL::shutdownFont() {
  if (debug) Log::writeLog("Render: Shutting down fonts", Log::LOG_DEFAULT);
  glDeleteLists(base,256); // Delete All 256 Display Lists
}

void GL::print(int x, int y, const char * string, int set) {
  if (set > 1) set = 1;
  switchTexture(font_id);
  glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
  glPushMatrix();
  glLoadIdentity(); // Reset The Projection Matrix
  glOrtho(0, window_width, 0 , window_height, -1, 1); // Set Up An Ortho Screen
  glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
  glPushMatrix();
  glLoadIdentity(); // Reset The Modelview Matrix
  glTranslated(x,y,0); // Position The Text (0,0 - Bottom Left)
  glListBase(base-32+(128*set)); // Choose The Font Set (0 or 1)
  glCallLists(strlen(string),GL_BYTE,string); // Write The Text To The Screen
  glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
  glPopMatrix(); // Restore The Old Projection Matrix
  glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
  glPopMatrix(); // Restore The Old Projection Matrix
}

void GL::print3D(const char *string, int set) {
  if (set > 1) set = 1;
//  int texture = requestTexture(font_id);
//  if (!glIsTexture(texture)) {
//    static int default_id = requestTexture( DEFAULT_FONT);
//    texture = default_id;
//  }
//  glBindTexture(GL_TEXTURE_2D, texture);
  switchTexture(font_id);
  glPushMatrix();
  glListBase(base-32+(128*set)); // Choose The Font Set (0 or 1)
  glCallLists(strlen(string),GL_BYTE,string); // Write The Text To The Screen
  glPopMatrix(); // Restore The Old Projection Matrix
}

inline void GL::newLine() {
  glTranslatef(0.0f,  ( FONT_HEIGHT) , 0.0f);
}

void GL::drawTextRect(int x, int y, int width, int height, int texture) {
  switchTexture(texture);
  // TODO should use switchTexture
//  glBindTexture(GL_TEXTURE_2D, getTextureID(texture));
  setViewMode(ORTHOGRAPHIC);
  // TODO: make into arrays?
  glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2i(x, y);
    glTexCoord2i(0, 1);
    glVertex2i(x, y + height);
    glTexCoord2i(1, 1);
    glVertex2i(x + width, y + height);
    glTexCoord2i(1, 0);
    glVertex2i(x + width, y);
  glEnd();
  setViewMode(PERSPECTIVE);
}

void GL::procEvent(int x, int y) {
  GLubyte i[3];
  glClear(GL_COLOR_BUFFER_BIT);
  _graphics->drawScene("", true, 0);
  x_pos = x;
  y = window_height - y;
  y_pos = y;
  glReadPixels(x, y, 1, 1, GL_RGB , GL_UNSIGNED_BYTE, &i);

// TODO pre-cache 8 - bits?
  
  GLubyte red = i[0] >> (8 - redBits);// & redMask;
  GLubyte green = i[1] >> (8 - greenBits);// & greenMask;
  GLubyte blue = i[2] >> (8 - blueBits);// & blueMask;

  unsigned int ic = red;
  ic <<= redBits;
  ic += green;
  ic <<= greenBits;
  ic += blue;
  //selected_id = getSelectedID(ic);
  WorldEntity *selected_entity = getSelectedID(ic);
  if (selected_entity != activeEntity) {
    activeEntity = selected_entity;
    if (debug && activeEntity) Log::writeLog(std::string("ActiveID: ") + activeEntity->getID(), Log::LOG_DEFAULT);
  }
}

void GL::CheckError() {
  GLenum err = glGetError();
  std::string msg;
  switch (err) {
    case GL_NO_ERROR: break;
    case GL_INVALID_ENUM: msg = "GL Error: Invalid enum!"; break;
    case GL_INVALID_VALUE: msg = "GL Error: Invalid value!"; break;
    case GL_INVALID_OPERATION: msg = "GL Error: Invalid operation!"; break;
    case GL_STACK_OVERFLOW: msg = "GL Error: Stack overflow!"; break;
    case GL_STACK_UNDERFLOW: msg = "GL Error: Stack Underflow!"; break;
    case GL_OUT_OF_MEMORY: msg = "GL Error: Out of memory!"; break;
    default: msg = std::string("GL Error: Unknown Error: ") +  string_fmt(err); break; 
  }
  if (!msg.empty()) Log::writeLog(msg, Log::LOG_ERROR);
}

//TODO should be in general render class
void GL::readConfig() {
  varconf::Variable temp;
  varconf::Config &general = _system->getGeneral();
  if (debug) Log::writeLog("Loading Renderer Config", Log::LOG_DEFAULT);

  // Setup character light source
  temp = general.getItem(RENDER, KEY_character_light_kc);
  lights[LIGHT_CHARACTER].attenuation_constant = (!temp.is_double()) ? (DEFAULT_character_light_kc) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_character_light_kl);
  lights[LIGHT_CHARACTER].attenuation_linear = (!temp.is_double()) ? (DEFAULT_character_light_kl) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_character_light_kq);
  lights[LIGHT_CHARACTER].attenuation_quadratic = (!temp.is_double()) ? (DEFAULT_character_light_kq) : ((double)(temp));
  
  temp = general.getItem(RENDER, KEY_character_light_ambient_red);
  lights[LIGHT_CHARACTER].ambient[0] = (!temp.is_double()) ? (DEFAULT_character_light_ambient_red) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_character_light_ambient_green);
  lights[LIGHT_CHARACTER].ambient[1] = (!temp.is_double()) ? (DEFAULT_character_light_ambient_green) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_character_light_ambient_blue);
  lights[LIGHT_CHARACTER].ambient[2] = (!temp.is_double()) ? (DEFAULT_character_light_ambient_blue) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_character_light_ambient_alpha);
  lights[LIGHT_CHARACTER].ambient[3] = (!temp.is_double()) ? (DEFAULT_character_light_ambient_alpha) : ((double)(temp));

  temp = general.getItem(RENDER, KEY_character_light_diffuse_red);
  lights[LIGHT_CHARACTER].diffuse[0] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_red) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_character_light_diffuse_green);
  lights[LIGHT_CHARACTER].diffuse[1] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_green) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_character_light_diffuse_blue);
  lights[LIGHT_CHARACTER].diffuse[2] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_blue) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_character_light_diffuse_alpha);
  lights[LIGHT_CHARACTER].diffuse[3] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_alpha) : ((double)(temp));

  temp = general.getItem(RENDER, KEY_character_light_specular_red);
  lights[LIGHT_CHARACTER].specular[0] = (!temp.is_double()) ? (DEFAULT_character_light_specular_red) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_character_light_specular_green);
  lights[LIGHT_CHARACTER].specular[1] = (!temp.is_double()) ? (DEFAULT_character_light_specular_green) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_character_light_specular_blue);
  lights[LIGHT_CHARACTER].specular[2] = (!temp.is_double()) ? (DEFAULT_character_light_specular_blue) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_character_light_specular_alpha);
  lights[LIGHT_CHARACTER].specular[3] = (!temp.is_double()) ? (DEFAULT_character_light_specular_alpha) : ((double)(temp));
  //Setup Sun light source
  temp = general.getItem(RENDER, KEY_sun_light_kc);
  lights[LIGHT_SUN].attenuation_constant = (!temp.is_double()) ? (DEFAULT_sun_light_kc) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_sun_light_kl);
  lights[LIGHT_SUN].attenuation_linear = (!temp.is_double()) ? (DEFAULT_sun_light_kl) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_sun_light_kq);
  lights[LIGHT_SUN].attenuation_quadratic = (!temp.is_double()) ? (DEFAULT_sun_light_kq) : ((double)(temp));

  // Setup render states
  temp = general.getItem(RENDER, KEY_use_textures);
  setState(RENDER_TEXTURES, ((!temp.is_bool()) ? (DEFAULT_use_textures) : ((bool)(temp))));
  temp = general.getItem(RENDER, KEY_use_lighting);
  setState(RENDER_LIGHTING, ((!temp.is_bool()) ? (DEFAULT_use_lighting) : ((bool)(temp))));
  temp = general.getItem(RENDER, KEY_show_fps);
  setState(RENDER_FPS, ((!temp.is_bool()) ? (DEFAULT_show_fps) : ((bool)(temp))));
  temp = general.getItem(RENDER, KEY_use_stencil);
  setState(RENDER_STENCIL, ((!temp.is_bool()) ? (DEFAULT_use_stencil) : ((bool)(temp))));
  
  // Setup the speech offsets
  temp = general.getItem(RENDER, KEY_speech_offset_x);
  _speech_offset_x = (!temp.is_double()) ? (DEFAULT_speech_offset_x) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_speech_offset_y);
  _speech_offset_y = (!temp.is_double()) ? (DEFAULT_speech_offset_y) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_speech_offset_z);
  _speech_offset_z = (!temp.is_double()) ? (DEFAULT_speech_offset_y) : ((double)(temp));

  temp = general.getItem(RENDER, KEY_fog_start);
  _fog_start = (!temp.is_double()) ? (DEFAULT_fog_start) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_fog_end);
  _fog_end = (!temp.is_double()) ? (DEFAULT_fog_end) : ((double)(temp));

  temp = general.getItem(RENDER, KEY_far_clip_dist);
  _far_clip_dist = (!temp.is_double()) ? (DEFAULT_far_clip_dist) : ((double)(temp));
  temp = general.getItem(RENDER, KEY_texture_scale);
  _texture_scale = (!temp.is_double()) ? (DEFAULT_texture_scale) : ((double)(temp));
}

void GL::writeConfig() {
  varconf::Config &general = _system->getGeneral();
  
  // Save character light source
  general.setItem(RENDER, KEY_character_light_kc, lights[LIGHT_CHARACTER].attenuation_constant);
  general.setItem(RENDER, KEY_character_light_kl, lights[LIGHT_CHARACTER].attenuation_linear);
  general.setItem(RENDER, KEY_character_light_kq, lights[LIGHT_CHARACTER].attenuation_quadratic);

  general.setItem(RENDER, KEY_character_light_ambient_red, lights[LIGHT_CHARACTER].ambient[0]);
  general.setItem(RENDER, KEY_character_light_ambient_green, lights[LIGHT_CHARACTER].ambient[1]);
  general.setItem(RENDER, KEY_character_light_ambient_blue, lights[LIGHT_CHARACTER].ambient[2]);
  general.setItem(RENDER, KEY_character_light_ambient_alpha, lights[LIGHT_CHARACTER].ambient[3]);

  general.setItem(RENDER, KEY_character_light_diffuse_red, lights[LIGHT_CHARACTER].diffuse[0]);
  general.setItem(RENDER, KEY_character_light_diffuse_green, lights[LIGHT_CHARACTER].diffuse[1]);
  general.setItem(RENDER, KEY_character_light_diffuse_blue, lights[LIGHT_CHARACTER].diffuse[2]);
  general.setItem(RENDER, KEY_character_light_diffuse_alpha, lights[LIGHT_CHARACTER].diffuse[3]);

  general.setItem(RENDER, KEY_character_light_specular_red, lights[LIGHT_CHARACTER].specular[0]);
  general.setItem(RENDER, KEY_character_light_specular_green, lights[LIGHT_CHARACTER].specular[1]);
  general.setItem(RENDER, KEY_character_light_specular_blue, lights[LIGHT_CHARACTER].specular[2]);
  general.setItem(RENDER, KEY_character_light_specular_alpha, lights[LIGHT_CHARACTER].specular[3]);
  
  // Save Sun light source
  general.setItem(RENDER, KEY_sun_light_kc, lights[LIGHT_SUN].attenuation_constant);
  general.setItem(RENDER, KEY_sun_light_kl, lights[LIGHT_SUN].attenuation_linear);
  general.setItem(RENDER, KEY_sun_light_kq, lights[LIGHT_SUN].attenuation_quadratic);

  // Save render states
  general.setItem(RENDER, KEY_use_textures, checkState(RENDER_TEXTURES));
  general.setItem(RENDER, KEY_use_lighting, checkState(RENDER_LIGHTING));
  general.setItem(RENDER, KEY_show_fps, checkState(RENDER_FPS));
  general.setItem(RENDER, KEY_use_stencil, checkState(RENDER_STENCIL));
  
  // Save the speech offsets
  general.setItem(RENDER, KEY_speech_offset_x, _speech_offset_x);
  general.setItem(RENDER, KEY_speech_offset_y, _speech_offset_y);
  general.setItem(RENDER, KEY_speech_offset_z, _speech_offset_z);

  general.setItem(RENDER, KEY_fog_start, _fog_start);
  general.setItem(RENDER, KEY_fog_end, _fog_end);
  general.setItem(RENDER, KEY_far_clip_dist, _far_clip_dist);
}  




void GL::setupStates() {
  // TODO: should this be in the init?
  //glAlphaFunc(GL_GREATER, 0.1f);
//  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  GLfloat fog_colour[] = {0.50f, 0.50f, 0.50f, 0.50f};
  glFogfv(GL_FOG_COLOR, fog_colour);
  glFogf(GL_FOG_START, _fog_start);
  glFogf(GL_FOG_END, _fog_end);
}

inline void GL::translateObject(float x, float y, float z) {
  glTranslatef(x, y, z);
}

inline void GL::rotate(float angle, float x, float y, float z) {
  glRotatef(angle, x, y, z);
}

//void GL::rotateObject(WorldEntity *we, int type) {
void GL::rotateObject(ObjectRecord *object_record, ModelRecord *model_record) {
//  if (!we) return; // THROW ERROR;
  switch (model_record->rotation_style) {
    case Graphics::ROS_NONE: return; break;
    case Graphics::ROS_POSITION: {
       WFMath::Point<3> pos = object_record->position;
       glRotatef(pos.x() + pos.y() + pos.z(), 0.0f, 0.0f, 1.0f);
       break;
    }       
    case Graphics::ROS_NORMAL: {
      applyQuaternion(object_record->orient);
      break;
    }
    case Graphics::ROS_BILLBOARD: // Same as HALO, but does not rotate with camera elevation
    case Graphics::ROS_HALO: {
      float rotation_matrix[4][4];
      WFMath::Quaternion  orient2 = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Initial Camera rotation
      orient2 /= _graphics->getCameraOrientation();
      QuatToMatrix(orient2, rotation_matrix); //Get the rotation matrix for base rotation
      glMultMatrixf(&rotation_matrix[0][0]); //Apply rotation matrix
      break;
    }
  }
}

inline void GL::scaleObject(float scale) {
  glScalef(scale, scale, scale);
}

void GL::setViewMode(int type) {
//  Perspective
  glViewport(0, 0, window_width, window_height);
  switch (type) {
    case PERSPECTIVE: {
      if (window_height == 0) window_height = 1;
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity(); // Reset The Projection Matrix
  
      // Calculate The Aspect Ratio Of The Window
      gluPerspective(fov,(GLfloat)window_width/(GLfloat)window_height, near_clip, _far_clip_dist);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      break;
    }
    case ORTHOGRAPHIC: {
      glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
      glLoadIdentity(); // Reset The Projection Matrix
      glOrtho(0, window_width, 0 , window_height, -1, 1); // Set Up An Ortho Screen
      glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
      glLoadIdentity();
      break;
    }			    
    case ISOMETRIC: {
      break;
    }			    
  }	
}
// TODO put into material manager and use display lists to retrieve them
void GL::setMaterial(float *ambient, float *diffuse, float *specular, float shininess, float *emissive) {
  // TODO: set up missing values
  if (ambient)           glMaterialfv (GL_FRONT, GL_AMBIENT,   ambient);
  if (diffuse)           glMaterialfv (GL_FRONT, GL_DIFFUSE,   diffuse);
  if (specular)          glMaterialfv (GL_FRONT, GL_SPECULAR,  specular);
  if (shininess >= 0.0f) glMaterialf  (GL_FRONT, GL_SHININESS, shininess);
  if (emissive)          glMaterialfv (GL_FRONT, GL_EMISSION,  emissive);
  else                   glMaterialfv (GL_FRONT, GL_EMISSION,  black);
}

void GL::renderArrays(unsigned int type, unsigned int offset, unsigned int number_of_points, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data, bool multitexture) {
  if (!use_multitexturing) multitexture = false;
  // TODO: Reduce ClientState switches
  bool textures = checkState(RENDER_TEXTURES);
  bool lighting = checkState(RENDER_LIGHTING);
 
  if (!vertex_data) {
    Log::writeLog("No Vertex Data", Log::LOG_ERROR);
    return; //throw Exception(""); 
  }
  glVertexPointer(3, GL_FLOAT, 0, (float*)vertex_data);
  glEnableClientState(GL_VERTEX_ARRAY);
  if (textures && texture_data) {
    if (multitexture) {
      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, (float*)texture_data);
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, (float*)texture_data);
    } else {	    
      glTexCoordPointer(2, GL_FLOAT, 0, (float*)texture_data);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  }
  if (lighting && normal_data) {
    glNormalPointer(GL_FLOAT, 0, (float*)normal_data);
    glEnableClientState(GL_NORMAL_ARRAY);
  }

  switch (type) {
    case (Graphics::RES_INVALID): Log::writeLog("Trying to render INVALID type", Log::LOG_ERROR); break;
    case (Graphics::RES_POINT): glDrawArrays(GL_POINT, offset, number_of_points); break;
    case (Graphics::RES_LINES): glDrawArrays(GL_LINES, offset, number_of_points); break;
    case (Graphics::RES_TRIANGLES): glDrawArrays(GL_TRIANGLES, offset, number_of_points); break;
    case (Graphics::RES_QUADS): glDrawArrays(GL_QUADS, offset, number_of_points); break;
    case (Graphics::RES_TRIANGLE_FAN): glDrawArrays(GL_TRIANGLE_FAN, offset, number_of_points); break;
    case (Graphics::RES_TRIANGLE_STRIP): glDrawArrays(GL_TRIANGLE_STRIP, offset, number_of_points); break;
    case (Graphics::RES_QUAD_STRIP): glDrawArrays(GL_QUAD_STRIP, offset, number_of_points); break;
    default: Log::writeLog("Unknown type", Log::LOG_ERROR); break;
  }
 
  glDisableClientState(GL_VERTEX_ARRAY);
  if (lighting && normal_data) glDisableClientState(GL_NORMAL_ARRAY);
  if (textures && texture_data) {
    if (multitexture)  {
      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    } else {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  }
}

void GL::renderElements(unsigned int type, unsigned int number_of_points, int *faces_data, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data, bool multitexture) {
  if (!use_multitexturing) multitexture = false;
  // TODO: Reduce ClientState switches
  bool textures = checkState(RENDER_TEXTURES);
  bool lighting = checkState(RENDER_LIGHTING);
 
  if (!vertex_data) return; //throw Exception(""); 
  glVertexPointer(3, GL_FLOAT, 0, (float*)vertex_data);
  glEnableClientState(GL_VERTEX_ARRAY);
  if (textures && texture_data) {
     if (multitexture) {
      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, (float*)texture_data);
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, (float*)texture_data);
    } else {	    
      glTexCoordPointer(2, GL_FLOAT, 0, (float*)texture_data);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  }
  if (lighting && normal_data) {
    glNormalPointer(GL_FLOAT, 0, (float*)normal_data);
    glEnableClientState(GL_NORMAL_ARRAY);
  }
  if (use_ext_compiled_vertex_array) glLockArraysEXT(0, number_of_points);
  switch (type) {
    case (Graphics::RES_INVALID): Log::writeLog("Trying to render INVALID type", Log::LOG_ERROR); break;
    case (Graphics::RES_POINT): glDrawElements(GL_POINT, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Graphics::RES_LINES): glDrawElements(GL_LINES, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Graphics::RES_TRIANGLES): glDrawElements(GL_TRIANGLES, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Graphics::RES_QUADS): glDrawElements(GL_QUADS, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Graphics::RES_TRIANGLE_FAN): glDrawElements(GL_TRIANGLE_FAN, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Graphics::RES_TRIANGLE_STRIP): glDrawElements(GL_TRIANGLE_STRIP, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Graphics::RES_QUAD_STRIP): glDrawElements(GL_QUAD_STRIP, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    default: Log::writeLog("Unknown type", Log::LOG_ERROR); break;
  }
  if (use_ext_compiled_vertex_array) glUnlockArraysEXT();
  glDisableClientState(GL_VERTEX_ARRAY);
  if (lighting && normal_data) glDisableClientState(GL_NORMAL_ARRAY);
  if (textures && texture_data) {
    if (multitexture) {
      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    } else {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  }
}

void GL::drawQueue(QueueMap &queue, bool select_mode, float time_elapsed) {
//  static MoelHandler *model_handler = _system->getModelHandler();
  for (QueueMap::const_iterator I = queue.begin(); I != queue.end(); I++) {
    // Change state for this queue
    stateChange(I->first);
    for (Queue::const_iterator J = I->second.begin(); J != I->second.end(); ++J) {

      ObjectRecord *object_record = J->first;
      ModelRecord *model_record = _system->getModelHandler()->getModel(this, object_record, J->second);
      if (!model_record) {
//        std::cerr << "No model record!" << std::endl;	      
        continue;
      }
      Model *model = model_record->model;
      // Get model
      if (!model) {  // ERROR GETTING MODEL
	Log::writeLog("Trying to render NULL model", Log::LOG_ERROR);
        continue;
      }
      glPushMatrix();

      // Translate Model
      WFMath::Point<3> pos = object_record->position;//we->getAbsPos();
      // TODO remove terrain->getHeight when server handles its own z coords
      translateObject(pos.x(), pos.y(), pos.z() + terrain->getHeight(pos.x(), pos.y()));
     
      // Rotate Model
      rotateObject(object_record, model_record);

      // Scale Object
      float scale = model_record->scale;
      // Do not perform scaling if it is to zero or has no effect
      if (scale != 0.0f && scale != 1.0f) glScalef(scale, scale, scale);

      // Update Model
      if (!select_mode) { // Onl needs to be done once a frame
        model->update(time_elapsed);
        model->setLastTime(System::instance()->getTimef());
      }      
      // Draw Model
      if (select_mode) {
        nextColour(object_record->entity);
	model->render(true);
      } else {
        if (object_record->entity == activeEntity) {
          active_name = object_record->name;
	  drawOutline(model_record);
	}
	else model->render(false);
      }
      glPopMatrix();
    }
  }
}

void GL::drawMessageQueue(MessageList &list) {
  glColor4fv(yellow);
  stateChange(getStateID(FONT));
  for (MessageList::const_iterator I = list.begin(); I != list.end(); ++I) {
    WorldEntity *we = (WorldEntity*)*I;
    glPushMatrix();
    WFMath::Point<3> pos = we->getAbsPos();
    glTranslatef(pos.x(), pos.y(), pos.z() + terrain->getHeight(pos.x(), pos.y()));
    WFMath::Quaternion  orient2 = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Initial Camera rotation
    orient2 /= _graphics->getCameraOrientation(); 
    applyQuaternion(orient2);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(0.025f, 0.025f, 0.025f);
    glTranslatef(_speech_offset_x, _speech_offset_y, _speech_offset_z);
    we->renderMessages();
    glPopMatrix();
  }
}
 
inline float GL::distFromNear(float x, float y, float z) {
  return Frustum::distFromNear(frustum, x, y, z);
}
	
inline int GL::patchInFrustum(WFMath::AxisBox<3> bbox) {
  return Frustum::patchInFrustum(frustum, bbox);
}

void GL::drawOutline(ModelRecord *model_record) {
//  StateProperties *sp = _cur_state; // Store current state
  StateID cur_state = _state_manager->getCurrentState();
  Model *model = model_record->model;
  bool use_stencil = checkState(RENDER_STENCIL) && model_record->outline;
  if (use_stencil) { // Using Stencil Buffer
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, -1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glPushMatrix();
    model->render(false);
    glPopMatrix();
    //TODO hard code halo in static const variable
    stateChange(model_record->select_state);
    glStencilFunc(GL_NOTEQUAL, -1, 1);
    glColor4fv(_halo_colour);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    model->render(true);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_STENCIL_TEST);
    glColor4fv(white);
  } else { // Just use solid colour on object 
    stateChange(model_record->select_state);
    glColor4fv(_halo_colour);  
    model->render(true);
    glColor4fv(white);
  }
  stateChange(cur_state); // Restore state
}


inline void GL::store() { glPushMatrix(); }
inline void GL::restore() { glPopMatrix(); }

inline void GL::beginFrame() {
  // TODO into display list
  terrain = _graphics->getTerrain();
  active_name = "";
  if (checkState(RENDER_STENCIL)) {
    glClearStencil(1);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear The Screen And The Depth Buffer
    } else {
      glClear(GL_DEPTH_BUFFER_BIT);
    }
  glLoadIdentity(); // Reset The View
  //Rotate Coordinate System so Z points upwards and Y points into the screen. 
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
}

inline void GL::endFrame(bool select_mode) {
  glFlush();
  if (!select_mode) SDL_GL_SwapBuffers();
  if (debug) CheckError();
}
  
inline void GL::drawFPS(float fps) {
  std::string frame_rate_string = string_fmt(fps).substr(0, 4);
  stateChange(getStateID(FONT));
  glColor4fv(red);
  print(10, 100, frame_rate_string.c_str(), 0);
}
  
void GL::drawSplashScreen() {
//  stateChange(getStateID("default"));
  stateChange(getStateID(SPLASH));
  #ifndef _WIN32
    // TODO Need to find a win32 version
//    usleep(sleep_time);
  #endif
  setViewMode(ORTHOGRAPHIC);
  
  glColor4fv(white);
//  std::cout << splash_id << std::endl;
  if (splash_id == -1) splash_id = requestTexture(TEXTURE_splash_texture);
//  std::cout << splash_id << std::endl;
  switchTexture(splash_id);
  // TODO into vertex array?
  glBegin(GL_QUADS); 
    glTexCoord2i(0, 0); glVertex2f(0.0f, 0.0f);
    glTexCoord2i(0, 1); glVertex2f(0.0f, window_height);
    glTexCoord2i(1, 1); glVertex2f(window_width, window_height);
    glTexCoord2i(1, 0); glVertex2f(window_width, 0.0f);
  glEnd(); 
  setViewMode(PERSPECTIVE);
}
  
inline void GL::applyQuaternion(WFMath::Quaternion quaternion) {
  float rotation_matrix[4][4];
  QuatToMatrix(quaternion, rotation_matrix); //Get the rotation matrix for base rotation
  glMultMatrixf(&rotation_matrix[0][0]); //Apply rotation matrix
}
  
void GL::applyCharacterLighting(float x, float y, float z) {
  float ps[] = {x, y, z, 1.0f};
  glLightfv(GL_LIGHT0,GL_POSITION, ps);
}


void GL::applyLighting() {
  Calendar *calendar = System::instance()->getCalendar();
  float tim = calendar->getTimeInArea();
  float dawn_time = calendar->getDawnStart();
  float day_time = calendar->getDayStart();
  float dusk_time = calendar->getDuskStart();
  float night_time = calendar->getNightStart();
  
  float modifier = calendar->getSecondsPerMinute() * calendar->getMinutesPerHour();
  
  static GLfloat fog_colour[4];// = {0.50f, 0.50f, 0.50f, 0.50f};
  switch (calendar->getTimeArea()) {
    case Calendar::INVALID: break;
    case Calendar::DAWN: {
      _light_level = tim / ((day_time - dawn_time) * modifier);
      float pos_mod = tim / ((night_time - dawn_time) * modifier);
      lights[LIGHT_SUN].position[0] = -200.0f * (pos_mod - 0.5f);
      break;
    }
    case Calendar::DAY: {
      _light_level = 1.0f;
      float pos_mod = tim / ((night_time - dawn_time) * modifier);
      lights[LIGHT_SUN].position[0] = -200.0f * (pos_mod - 0.5f);
      break;
    }
    case Calendar::DUSK: {
      _light_level = 1.0f - (tim / ((night_time - dusk_time) * modifier));
      float pos_mod = tim / ((night_time - dawn_time) * modifier);
      lights[LIGHT_SUN].position[0] = -200.0f * (pos_mod - 0.5f);
      break;
    }
    case Calendar::NIGHT: {
      _light_level = 0.0f;
      break;
    }
  }
   
  fog_colour[0] = fog_colour[1] = fog_colour[2] = fog_colour[3] = 0.5f * _light_level;
  glFogfv(GL_FOG_COLOR, fog_colour);
  float sun_pos[] = {lights[LIGHT_SUN].position[0], 0.0f, 100.0f, 1.0f};
  lights[LIGHT_SUN].ambient[0] = lights[LIGHT_SUN].ambient[1] = lights[LIGHT_SUN].ambient[2] = _light_level * 0.5f;
  lights[LIGHT_SUN].diffuse[0] = lights[LIGHT_SUN].diffuse[1] = lights[LIGHT_SUN].diffuse[2] = _light_level;
  glLightfv(GL_LIGHT1,GL_POSITION,sun_pos);
  glLightfv(GL_LIGHT1, GL_AMBIENT, lights[LIGHT_SUN].ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, lights[LIGHT_SUN].diffuse);
}

inline void GL::resetSelection() {
  resetColours();
}

inline void GL::renderActiveName() {
  stateChange(getStateID(FONT));
  glColor4fv(activeNameColour);
  print(x_pos, y_pos, active_name.c_str(), 1);
}

inline void GL::getFrustum(float frust[6][4]) {
  float  proj[16];
  float  modl[16];
  /* Get the current PROJECTION matrix from OpenGraphics */
  glGetFloatv(GL_PROJECTION_MATRIX, proj );
  /* Get the current MODELVIEW matrix from OpenGraphics */
  glGetFloatv(GL_MODELVIEW_MATRIX, modl );
  Frustum::getFrustum(frust, proj, modl);
  // Copy frustum - local copy plus one from graphics object
  for (int i = 0; i < 6; ++i) {
    for (int j = 0; j < 4; ++j) {
      frustum[i][j] = frust[i][j];
    }
  }
}

void GL::setupExtensions() {
  sage_init();
  use_sgis_generate_mipmap = sage_ext[GL_SGIS_GENERATE_MIPMAP];
  use_multitexturing = sage_ext[GL_ARB_MULTITEXTURE];
  if (use_multitexturing) {
    std::cout << "Using arb_multitexture" << std::endl;
  }
//  use_ext_compiled_vertex_array = sage_ext[GL_EXT_COMPILED_VERTEX_ARRAY];
  if (use_ext_compiled_vertex_array) {
    std::cout << "Using use_ext_compiled_vertex_array" << std::endl; 
  }
}

void GL::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  varconf::Variable temp;
  if (section == RENDER) {
    if (key == KEY_character_light_kc) {
      temp = config.getItem(RENDER, KEY_character_light_kc);
      lights[LIGHT_CHARACTER].attenuation_constant = (!temp.is_double()) ? (DEFAULT_character_light_kc) : ((double)(temp));
    }
    else if (key ==  KEY_character_light_kl) {
      temp = config.getItem(RENDER, KEY_character_light_kl);
      lights[LIGHT_CHARACTER].attenuation_linear = (!temp.is_double()) ? (DEFAULT_character_light_kl) : ((double)(temp));
    }
    else if (key == KEY_character_light_kq) {
      temp = config.getItem(RENDER, KEY_character_light_kq);
      lights[LIGHT_CHARACTER].attenuation_quadratic = (!temp.is_double()) ? (DEFAULT_character_light_kq) : ((double)(temp));
    }
    else if (key == KEY_character_light_ambient_red) {
      temp = config.getItem(RENDER, KEY_character_light_ambient_red);
      lights[LIGHT_CHARACTER].ambient[0] = (!temp.is_double()) ? (DEFAULT_character_light_ambient_red) : ((double)(temp));
    }
    else if (key == KEY_character_light_ambient_green) {
      temp = config.getItem(RENDER, KEY_character_light_ambient_green);
      lights[LIGHT_CHARACTER].ambient[1] = (!temp.is_double()) ? (DEFAULT_character_light_ambient_green) : ((double)(temp));
    }
    else if (key == KEY_character_light_ambient_blue) {
      temp = config.getItem(RENDER, KEY_character_light_ambient_blue);
      lights[LIGHT_CHARACTER].ambient[2] = (!temp.is_double()) ? (DEFAULT_character_light_ambient_blue) : ((double)(temp));
    }
    else if (key == KEY_character_light_ambient_alpha) {
      temp = config.getItem(RENDER, KEY_character_light_ambient_alpha);
      lights[LIGHT_CHARACTER].ambient[3] = (!temp.is_double()) ? (DEFAULT_character_light_ambient_alpha) : ((double)(temp));
    }
    else if (key == KEY_character_light_diffuse_red) {
      temp = config.getItem(RENDER, KEY_character_light_diffuse_red);
      lights[LIGHT_CHARACTER].diffuse[0] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_red) : ((double)(temp));
    }
    else if (key == KEY_character_light_diffuse_green) {
      temp = config.getItem(RENDER, KEY_character_light_diffuse_green);
      lights[LIGHT_CHARACTER].diffuse[1] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_green) : ((double)(temp));
    }
    else if (key == KEY_character_light_diffuse_blue) {
      temp = config.getItem(RENDER, KEY_character_light_diffuse_blue);
      lights[LIGHT_CHARACTER].diffuse[2] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_blue) : ((double)(temp));
    }
    else if (key == KEY_character_light_diffuse_alpha) {
      temp = config.getItem(RENDER, KEY_character_light_diffuse_alpha);
      lights[LIGHT_CHARACTER].diffuse[3] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_alpha) : ((double)(temp));
    }
    else if (key == KEY_character_light_specular_red) {
      temp = config.getItem(RENDER, KEY_character_light_specular_red);
      lights[LIGHT_CHARACTER].specular[0] = (!temp.is_double()) ? (DEFAULT_character_light_specular_red) : ((double)(temp));
    }
    else if (key == KEY_character_light_specular_green) {
      temp = config.getItem(RENDER, KEY_character_light_specular_green);
      lights[LIGHT_CHARACTER].specular[1] = (!temp.is_double()) ? (DEFAULT_character_light_specular_green) : ((double)(temp));
    }
    else if (key == KEY_character_light_specular_blue) {
      temp = config.getItem(RENDER, KEY_character_light_specular_blue);
      lights[LIGHT_CHARACTER].specular[2] = (!temp.is_double()) ? (DEFAULT_character_light_specular_blue) : ((double)(temp));
    }
    else if (key == KEY_character_light_specular_alpha) {
      temp = config.getItem(RENDER, KEY_character_light_specular_alpha);
      lights[LIGHT_CHARACTER].specular[3] = (!temp.is_double()) ? (DEFAULT_character_light_specular_alpha) : ((double)(temp));
    }
    //Setup Sun light source
    else if (key == KEY_sun_light_kc) {
      temp = config.getItem(RENDER, KEY_sun_light_kc);
      lights[LIGHT_SUN].attenuation_constant = (!temp.is_double()) ? (DEFAULT_sun_light_kc) : ((double)(temp));
    }
    else if (key == KEY_sun_light_kl) {
      temp = config.getItem(RENDER, KEY_sun_light_kl);
      lights[LIGHT_SUN].attenuation_linear = (!temp.is_double()) ? (DEFAULT_sun_light_kl) : ((double)(temp));
    }
    else if (key == KEY_sun_light_kq) {
      temp = config.getItem(RENDER, KEY_sun_light_kq);
      lights[LIGHT_SUN].attenuation_quadratic = (!temp.is_double()) ? (DEFAULT_sun_light_kq) : ((double)(temp));
    }
    // Setup render states
    else if (key == KEY_use_textures) {
      temp = config.getItem(RENDER, KEY_use_textures);
      setState(RENDER_TEXTURES, ((!temp.is_bool()) ? (DEFAULT_use_textures) : ((bool)(temp))));
    }
    else if (key == KEY_use_lighting) {
      temp = config.getItem(RENDER, KEY_use_lighting);
      setState(RENDER_LIGHTING, ((!temp.is_bool()) ? (DEFAULT_use_lighting) : ((bool)(temp))));
    }
    else if (key == KEY_show_fps) {
      temp = config.getItem(RENDER, KEY_show_fps);
      setState(RENDER_FPS, ((!temp.is_bool()) ? (DEFAULT_show_fps) : ((bool)(temp))));
    }
    else if (key == KEY_use_stencil) {
      temp = config.getItem(RENDER, KEY_use_stencil);
      setState(RENDER_STENCIL, ((!temp.is_bool()) ? (DEFAULT_use_stencil) : ((bool)(temp))));  
    }
    // Setup the speech offsets
    else if (key == KEY_speech_offset_x) {
      temp = config.getItem(RENDER, KEY_speech_offset_x);
      _speech_offset_x = (!temp.is_double()) ? (DEFAULT_speech_offset_x) : ((double)(temp));
    }
    else if (key == KEY_speech_offset_y) {
      temp = config.getItem(RENDER, KEY_speech_offset_y);
      _speech_offset_y = (!temp.is_double()) ? (DEFAULT_speech_offset_y) : ((double)(temp));
    }
    else if (key == KEY_speech_offset_z) {
      temp = config.getItem(RENDER, KEY_speech_offset_z);
      _speech_offset_z = (!temp.is_double()) ? (DEFAULT_speech_offset_y) : ((double)(temp));
    }
    else if (key == KEY_fog_start) {
      temp = config.getItem(RENDER, KEY_fog_start);
      _fog_start = (!temp.is_double()) ? (DEFAULT_fog_start) : ((double)(temp));
    }
    else if (key == KEY_fog_end) {
      temp = config.getItem(RENDER, KEY_fog_end);
      _fog_end = (!temp.is_double()) ? (DEFAULT_fog_end) : ((double)(temp));
    }
    else if (key == KEY_far_clip_dist) {
      temp = config.getItem(RENDER, KEY_far_clip_dist);
      _far_clip_dist = (!temp.is_double()) ? (DEFAULT_far_clip_dist) : ((double)(temp));
    }
    else if (key == KEY_texture_scale) {
      temp = config.getItem(RENDER, KEY_texture_scale);
      _texture_scale = (!temp.is_double()) ? (DEFAULT_texture_scale) : ((double)(temp));
    }
  }
}

std::string GL::getActiveID() {
  return (activeEntity) ? (activeEntity->getID()) : ("");
} 
void GL::setTextureScale(unsigned int unit, float scale) {
  glActiveTextureARB(GL_TEXTURE0_ARB + unit);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(scale, scale, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glActiveTextureARB(GL_TEXTURE0_ARB);
}

} /* namespace Sear */
