// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "GL_Render.h"

#include <GL/glu.h>

#include "Camera.h"
#include "System.h"
#include "Config.h"
#include "Terrain.h"
#include "SkyBox.h"
#include "Camera.h"
#include "WorldEntity.h"
#include "Console.h"
#include "Utility.h"
#include <Eris/Entity.h>
#include <Eris/World.h>
#include <Eris/TypeInfo.h>
#include "model.h"
#include <wfmath/quaternion.h>
#include <wfmath/vector.h>
#include "Character.h"
#include "ObjectLoader.h"

#include "Models.h"
#include "WireFrame.h"
#include "BoundBox.h"
#include "BillBoard.h"
#include "Impostor.h"
#include <unistd.h>

#include "Log.h"

#include "conf.h"

namespace Sear {

float GL_Render::_halo_blend_colour[4] = {1.0f, 0.0f, 1.0f, 0.4f};
float GL_Render::_halo_colour[3] = {1.0f, 0.0f, 1.0f};

//.TODO put this into a class
	
std::set<int> colourSet;
std::set<int>::const_iterator colourSetIterator;

GLuint makeMask(GLint bits) {
  return (0xFF >> (8 - bits));
}

  GLint redBits, greenBits, blueBits;
  GLuint redMask;
  GLuint greenMask;
  GLuint blueMask;
  int redShift;
  int greenShift;
  int blueShift;
  
void GL_Render::buildColourSet() {
  unsigned int numPrims = 500;
  glGetIntegerv (GL_RED_BITS, &redBits);
  glGetIntegerv (GL_GREEN_BITS, &greenBits);
  glGetIntegerv (GL_BLUE_BITS, &blueBits);

  redMask = makeMask(redBits);
  greenMask = makeMask(greenBits);
  blueMask = makeMask(blueBits);
  redShift =   greenBits + blueBits;
  greenShift =  blueBits;
  blueShift =  0;
  unsigned long indx;
  colourSet = std::set<int>();
  
  for (indx = 0; indx < numPrims; indx++) {
    int ic = 0;
    ic += indx & (redMask << redShift);
    ic += indx & (greenMask << greenShift);
    ic += indx & (blueMask << blueShift);
    colourSet.insert(ic);
  }
  Log::writeLog(std::string("Number of colours: ") + string_fmt(colourSet.size()), Log::DEFAULT);
}

std::map<unsigned int, std::string> colour_mapped;

void resetColors(){
  colour_mapped = std::map<unsigned int, std::string>();
  colourSetIterator = colourSet.begin();
  *colourSetIterator++;
}

std::string getSelectedID(unsigned int i) {
  
  return colour_mapped[i];
}

int nextColour(std::string id) {
  unsigned int ic;
  
  if  (colourSetIterator != colourSet.end()) ic = *colourSetIterator++;
  colour_mapped[ic] = id;
  
  GLubyte red = (ic & (redMask << redShift)) << (8 - redBits);
  GLubyte green = (ic & (greenMask << greenShift)) << (8 - greenBits);
  GLubyte blue = (ic & (blueMask << blueShift)) << (8 - blueBits);
  
  glColor3ub(red, green, blue);
  
  return 0;
}


GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat red[] =   { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat blue[] =  { 0.0f, 0.0f, 1.0f, 1.0f };

//GLfloat  whiteLight[]    = { 1.0f,  1.0f, 1.0f, 1.0f };




GLfloat  whiteLight[]    = { 1.0f,  1.0f, 1.0f, 1.0f };
GLfloat  blackLight[]    = { 0.0f,  0.0f, 0.0f, 1.0f };
GLfloat  ambientLight[]  = { 0.75f, 0.75f, 0.75f, 1.0f };
GLfloat  diffuseLight[]  = { 1.0f,  1.0f, 1.0f, 1.0f };
GLfloat  specularLight[]  = { 1.0f,  1.0f, 1.0f, 1.0f };

GL_Render *GL_Render::_instance = NULL;

GL_Render::GL_Render() :
  _system(NULL),
  window_width(0),
  window_height(0),
  fov(RENDER_FOV),
  near_clip(RENDER_NEAR_CLIP),
  far_clip(RENDER_FAR_CLIP),
  next_id(1),
  base(0),
  textureList(std::list<GLuint>()),
  terrain(NULL),
  skybox(NULL),
  camera(NULL),
  player_model(NULL),
  num_frames(0),
  frame_time(0.0f),
  _character(NULL)
{
  _instance = this;
}

GL_Render::GL_Render(System *system) :
  _system(system),
  window_width(0),
  window_height(0),
  fov(RENDER_FOV),
  near_clip(RENDER_NEAR_CLIP),
  far_clip(RENDER_FAR_CLIP),
  next_id(1),
  base(0),
  textureList(std::list<GLuint>()),
  terrain(NULL),
  skybox(NULL),
  camera(NULL),
  player_model(NULL),
  num_frames(0),
  frame_time(0.0f),
  _character(NULL)
{
  _instance = this;
}

GL_Render::~GL_Render() {
  writeConfig();
  shutdownFont();

  // Clear out models
  for (std::map<std::string, ModelStruct*>::iterator I = _entity_models.begin(); I != _entity_models.end(); I++) {
    if (I->second) {
      ModelStruct *ms = I->second;
      if (ms->model) {
        ms->model->onShutdown();
	delete ms->model;
      }
      if (ms->models) {
        ms->models->shutdown();
	delete ms->models;
      }
    }
  }
  while (!_entity_models.empty()) {
    if (_entity_models.begin()->second) delete _entity_models.begin()->second;
    _entity_models.erase(_entity_models.begin());
  } 

  if (terrain) {
    terrain->shutdown();
//    delete terrain;
  }
  if (skybox) {
    skybox->shutdown();
    delete skybox;
  }
  if (camera) {
    delete camera;
  }
}

void GL_Render::initWindow(int width, int height) {
  Log::writeLog("Render: Initilising Renderer", Log::DEFAULT);
  Log::writeLog(std::string("GL_VENDER: ") + string_fmt(glGetString(GL_VENDOR)), Log::DEFAULT);
  Log::writeLog(std::string("GL_RENDERER: ") + string_fmt(glGetString(GL_RENDERER)), Log::DEFAULT);
  Log::writeLog(std::string("GL_VERSION: ") + string_fmt(glGetString(GL_VERSION)), Log::DEFAULT);
  Log::writeLog(std::string("GL_EXTENSIONS: ") + string_fmt(glGetString(GL_EXTENSIONS)), Log::DEFAULT);
  
  glLineWidth(4);
  //Check for divide by 0
  if (height == 0) height = 1;
  glLineWidth(2.0f); 
  glViewport(0, 0, width, height);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Colour used to clear window
  glClearDepth(1.0); // Enables Clearing Of The Depth Buffer
  glClear(GL_DEPTH_BUFFER_BIT);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glDisable(GL_DITHER); 
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); // Reset The Projection Matrix
  
  // Calculate The Aspect Ratio Of The Window
  gluPerspective(fov,(GLfloat)width/(GLfloat)height, near_clip, _far_clip_dist);
  glMatrixMode(GL_MODELVIEW);

  //Store window size
  window_width = width;
  window_height = height;
}
  
void GL_Render::init() {
  readConfig();
  setupStates();
  splash_id = requestTexture(splash_texture);
  initFont();
  Log::writeLog("Initialising Terrain", Log::DEFAULT);
  terrain = new Terrain(_system, this);
  if (!terrain->init()) {
    Log::writeLog("Error initialising Terrain. Suggest Restart!", Log::ERROR);
  }
  skybox = new SkyBox(_system, this);
  if (!skybox->init()) {
    Log::writeLog("Render: Error - Could not initialise Sky Box", Log::ERROR);
  }
  camera = new Camera();
  camera->init();
//  CheckError();
  initLighting();
}

void GL_Render::initLighting() {
  Log::writeLog("Render: initialising lighting", Log::DEFAULT);
  float gambient[4] = {0.1f, 0.1f,0.1f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,gambient);
//  glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // Should make this specific to billboard and impostors and not globally relevant
  // Light values and coordinates
           
  // Setup and enable light 0
  //glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  
  glLightfv(GL_LIGHT0, GL_AMBIENT, lights[LIGHT_CHARACTER].ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lights[LIGHT_CHARACTER].diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lights[LIGHT_CHARACTER].specular);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, lights[LIGHT_CHARACTER].kc);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, lights[LIGHT_CHARACTER].kl);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, lights[LIGHT_CHARACTER].kq);
  glEnable(GL_LIGHT0);
  
  glLightfv(GL_LIGHT1, GL_AMBIENT, blackLight);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, blackLight);
  glLightfv(GL_LIGHT1, GL_SPECULAR, blackLight);
//  glLightfv(GL_LIGHT1, GL_AMBIENT, lights[LIGHT_SUN].ambient);
//  glLightfv(GL_LIGHT1, GL_DIFFUSE, lights[LIGHT_SUN].diffuse);
//  glLightfv(GL_LIGHT1, GL_SPECULAR, lights[LIGHT_SUN].specular);
  
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, lights[LIGHT_SUN].kc);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, lights[LIGHT_SUN].kl);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, lights[LIGHT_SUN].kq);
  glEnable(GL_LIGHT1);
}

void GL_Render::initFont() {
  int loop;
  float cx;  // Holds Our X Character Coord
  float cy; // Holds Our Y Character Coord
  Log::writeLog("Render: Initilising Fonts", Log::DEFAULT);
  base=glGenLists(256); // Creating 256 Display Lists
  font_id = requestTexture(font_texture);
  if (font_id == -1) return; //ERROR
  glBindTexture(GL_TEXTURE_2D, getTextureID(font_id)); // Select Our Font Texture
  for (loop=0; loop<256; loop++) {
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

void GL_Render::shutdownFont() {
  Log::writeLog("Render: Shutting down fonts", Log::DEFAULT);
  glDeleteLists(base,256); // Delete All 256 Display Lists
}

void GL_Render::print(GLint x, GLint y, const char * string, int set) {
  if (set > 1) set = 1;
  glBindTexture(GL_TEXTURE_2D, getTextureID(font_id)); // Select Our Font Texture
  glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
  glPushMatrix(); // Store The Projection Matrix
  glLoadIdentity(); // Reset The Projection Matrix
  glOrtho(0, window_width, 0 , window_height, -1, 1); // Set Up An Ortho Screen
  glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
  glPushMatrix(); // Store The Modelview Matrix
  glLoadIdentity(); // Reset The Modelview Matrix
  glTranslated(x,y,0); // Position The Text (0,0 - Bottom Left)
  glListBase(base-32+(128*set)); // Choose The Font Set (0 or 1)
  glCallLists(strlen(string),GL_BYTE,string); // Write The Text To The Screen
  glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
  glPopMatrix(); // Restore The Old Projection Matrix
  glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
  glPopMatrix(); // Restore The Old Projection Matrix
}

void GL_Render::print3D(const char *string, int set) {
  if (set > 1) set = 1;
  glBindTexture(GL_TEXTURE_2D, getTextureID(font_id)); // Select Our Font Texture
  glPushMatrix(); // Store The Projection Matrix
  glListBase(base-32+(128*set)); // Choose The Font Set (0 or 1)
  glCallLists(strlen(string),GL_BYTE,string); // Write The Text To The Screen
  glPopMatrix(); // Restore The Old Projection Matrix
}

void GL_Render::newLine() {
  glTranslatef(0.0f,  ( FONT_HEIGHT) , 0.0f);
}

int GL_Render::requestTexture(const std::string &texture_name, bool clamp) {
  SDL_Surface *tmp = NULL;
  unsigned int texture_id = 0;
  int id = texture_map[texture_name];
  if (id != 0) return id;
  glGenTextures(1, &texture_id);
  if (texture_id == 0) return -1;
  std::string file_name = _system->getTexture()->getAttribute(texture_name);
  if (file_name.empty()) return -1;
  tmp = System::loadImage(file_name);
  if (!tmp) {
    Log::writeLog("Error loading texture", Log::ERROR);
    return -1;
  }
  createTexture(tmp, texture_id, clamp);
  free (tmp);
  textureList.push_back(texture_id);
  texture_map[texture_name] = next_id;
  return next_id++;
}

int GL_Render::requestMipMap(const std::string &texture_name, bool clamp) {
  SDL_Surface *tmp = NULL;
  unsigned int texture_id = 0;
  int id = texture_map[texture_name];
  if (id != 0) return id;
  glGenTextures(1, &texture_id);
  if (texture_id == 0) return -1;
  std::string file_name = _system->getTexture()->getAttribute(texture_name);
  if (file_name.empty()) return -1;
  tmp = System::loadImage(file_name);
  if (!tmp) return -1;
  createMipMap(tmp, texture_id, clamp);
  free (tmp);
  textureList.push_back(texture_id);
  texture_map[texture_name] = next_id;
  return next_id++;
}

int GL_Render::requestTextureMask(const std::string &texture_name, bool clamp) {
  SDL_Surface *tmp = NULL;
  unsigned int texture_id = 0;
  int id = texture_map[texture_name + "_mask"];
  if (id != 0) return id;
  glGenTextures(1, &texture_id);
  if (texture_id == 0) return -1;
  std::string file_name = _system->getTexture()->getAttribute(texture_name);
  if (file_name.empty()) return -1;
  tmp = System::loadImage(file_name);
  if (!tmp) {
    Log::writeLog("Error loading texture", Log::ERROR);
    return -1;
  }
  createTextureMask(tmp, texture_id, clamp);
  free (tmp);
  textureList.push_back(texture_id);
  texture_map[texture_name + "_mask"] = next_id;
  return next_id++;
}

void GL_Render::createTexture(SDL_Surface *surface, unsigned int texture, bool clamp) {
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  if (clamp) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
  if (surface->format->BytesPerPixel == 4) {
    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, 3, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
  }
}
 
void GL_Render::createMipMap(SDL_Surface *surface, unsigned int texture, bool clamp)  {
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  if (clamp) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);		  
  }
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, surface->w, surface->h, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
}

void GL_Render::createTextureMask(SDL_Surface *surface, unsigned int texture, bool clamp) {
  int i;
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  if (clamp) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
  if (surface->format->BytesPerPixel == 4) {
    for (i = 0; i < surface->w * surface->h * 4; i += 4) {
      ((unsigned char *)surface->pixels)[i + 0] = (unsigned char)0xff;
      ((unsigned char *)surface->pixels)[i + 1] = (unsigned char)0xff;
      ((unsigned char *)surface->pixels)[i + 2] = (unsigned char)0xff;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
  } else {
    for (i = 0; i < surface->w * surface->h * 3; i += 3) {
      ((unsigned char *)surface->pixels)[i + 0] = (unsigned char)0xff;
      ((unsigned char *)surface->pixels)[i + 1] = (unsigned char)0xff;
      ((unsigned char *)surface->pixels)[i + 2] = (unsigned char)0xff;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, 3, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
  }
}

inline GLuint GL_Render::getTextureID(int texture_id) {
  int i;
  std::list<GLuint>::const_iterator I = textureList.begin();
  for (i = 1; i < texture_id; i++, I++);
  return *I;
}

void GL_Render::drawScene(const std::string& command, bool select_mode) {
//	select_mode = true;
  if (select_mode) resetColors();
  active_name = "";
  // This clears the currently loaded texture
  if (select_mode) glBindTexture(GL_TEXTURE_2D, 0);
//  if (!checkState(RENDER_TEXTURES)) glDisable(GL_TEXTURE_2D);
//  if (!checkState(RENDER_LIGHTING)) glDisable(GL_LIGHTING);
  
  float time_elapsed =  (SDL_GetTicks() - this->time) / 1000.0f;
  this->time = SDL_GetTicks();
  num_frames++;
  frame_time += time_elapsed;

  if (checkState(RENDER_STENCIL)) {
    glClearStencil(1);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear The Screen And The Depth Buffer
    } else {
      glClear(GL_DEPTH_BUFFER_BIT);
    }
  camera->updateCameraPos(time_elapsed);
  glLoadIdentity(); // Reset The View
  
  //Rotate Coordinate System so Z points upwards and Y points into the screen. 
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

  Eris::World *world = Eris::World::Instance();
  if (_system->checkState(SYS_IN_WORLD) && world) {
    if (!_character) _character = _system->getCharacter();
  if (select_mode) stateChange(SELECT);
// Function is still unstable!
//    Eris::World::Instance()->tick(); 
    WorldEntity *focus = (WorldEntity *)world->getFocusedEntity(); //Get the player character entity
    if (focus != NULL) {
      float x = 0.0f, y = 0.0f, z = 0.0f; // Initial camera position
      std::string id = focus->getID();
      orient = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Initial Camera rotation
      orient /= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 0.0f, 1.0f), WFMath::Pi / 2.0f); // Rotate by 90 degrees as WF 0 degrees is East
      WFMath::Point<3> pos = focus->getAbsPos();
      x = -pos.x();
      y = -pos.y();
      z = -pos.z();
      
      
      // Apply camera rotations
      orient /= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 1.0f, 0.0f), camera->getElevation());
      orient /= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 0.0f, 1.0f), camera->getRotation());

      if (_character) orient /= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 0.0f, 1.0f), _character->getAngle());
      float rotation_matrix[4][4];
      QuatToMatrix(orient, rotation_matrix); //Get the rotation matrix for base rotation

      // Draw Sky box, requires the rotation to be done before any translation to keep the camera centered
      // Hence within push/pop matrix black
      if (!select_mode) {
        glPushMatrix();
          glMultMatrixf(&rotation_matrix[0][0]); //Apply rotation matrix
          nextState(FONT_TO_SKYBOX);
          skybox->draw(); //Draw the sky box
        glPopMatrix();
      }

      // Translate camera getDist() units away from the character. Allows closups or large views
      glTranslatef(0.0f, camera->getDistance(), 0.0f);
      glMultMatrixf(&rotation_matrix[0][0]); //Apply rotation matrix
      z -= terrain->getHeight(-x, -y);
      
      glTranslatef(x, y, z - 2.0f); //Translate to accumulated position - Also adjust so origin is nearer head level
      
      float ps[] = {-x, -y, -z + 2.0f, 1.0f};
      glLightfv(GL_LIGHT0,GL_POSITION,ps);

      if (_entity_models[id]) player_model = _entity_models[id]->model;

      extractFrustum();
    }
    // Setup Sun
    if (checkState(RENDER_LIGHTING)) {
      float tim = _system->getTimeOfDay();
      float dawn_time = _system->getDawnTime();
      float day_time = _system->getDayTime();
      float dusk_time = _system->getDuskTime();
      float night_time = _system->getNightTime();
      
      GLfloat fog_colour[4];// = {0.50f, 0.50f, 0.50f, 0.50f};
      switch (_system->getTimeArea()) {
        case System::DAWN: {
          _light_level = (tim - dawn_time) / (day_time - dawn_time);
          float pos_mod = (tim - dawn_time) / (night_time - dawn_time);
          lights[LIGHT_SUN].x_pos = -200.0f * (pos_mod - 0.5f);
          break;
        }
        case System::DAY: {
          _light_level = 1.0f;
          float pos_mod = (tim - dawn_time) / (night_time - dawn_time);
          lights[LIGHT_SUN].x_pos = -200.0f * (pos_mod - 0.5f);
          break;
        }
        case System::DUSK: {
          _light_level = 1.0f - ((tim - dusk_time) / (night_time - dusk_time));
          float pos_mod = (tim - dawn_time) / (night_time - dawn_time);
          lights[LIGHT_SUN].x_pos = -200.0f * (pos_mod - 0.5f);
          break;
        }
        case System::NIGHT: {
          _light_level = 0.0f;
          break;
        }
      }
      
      fog_colour[0] = fog_colour[1] = fog_colour[2] = fog_colour[3] = 0.5f * _light_level;
      glFogfv(GL_FOG_COLOR, fog_colour);
      float sun_pos[] = {lights[LIGHT_SUN].x_pos, 0.0f, 100.0f, 1.0f};
      lights[LIGHT_SUN].ambient[0] = lights[LIGHT_SUN].ambient[1] = lights[LIGHT_SUN].ambient[2] = _light_level * 0.5f;
      lights[LIGHT_SUN].diffuse[0] = lights[LIGHT_SUN].diffuse[1] = lights[LIGHT_SUN].diffuse[2] = _light_level;
      glLightfv(GL_LIGHT1,GL_POSITION,sun_pos);
      glLightfv(GL_LIGHT1, GL_AMBIENT, lights[LIGHT_SUN].ambient);
      glLightfv(GL_LIGHT1, GL_DIFFUSE, lights[LIGHT_SUN].diffuse);
    }
    if (!select_mode) {
      glPushMatrix();
        nextState(SKYBOX_TO_TERRAIN);
        terrain->draw();
      glPopMatrix();
    }
// Draw known entities
    WorldEntity *root = NULL; 
    if ((root = (WorldEntity *)world->getRootEntity())) {

    for (std::map<std::string,ModelStruct*>::const_iterator I = _entity_models.begin(); I != _entity_models.end(); I++) {
      if (I->second) if (I->second->in_use) if (I->second->model) (I->second)->model->onUpdate(time_elapsed);
    }
      if (_character) _character->updateLocals(false);
      render_queue = std::map<std::string, Queue>();
      model_queue = std::map<std::string, Queue>();
      billboard_queue = std::map<std::string, Queue>();
      imposter_queue = std::map<std::string, Queue>();
      wireframe_queue = std::map<std::string, Queue>();
      buildQueues(root, 0);
      if (!select_mode) nextState(TERRAIN_TO_WIREFRAME);
      drawWireFrameQueue(select_mode);
      if (!select_mode) nextState(WIREFRAME_TO_CHARACTERS);
      drawModelQueue(select_mode);
      if (!select_mode) nextState(CHARACTERS_TO_MODELS);
      drawRenderQueue(select_mode);
      if (!select_mode) nextState(MODELS_TO_BILLBOARD);
      glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
      drawBBoardQueue(select_mode);
      drawImpostorQueue(select_mode);
      glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
      if (!select_mode) nextState(BILLBOARD_TO_FONT);
      drawMessageQueue(select_mode);
      if (!select_mode) {
        if (frame_rate < _lower_frame_rate_bound) {
          model_detail -= 0.1f;
          if (model_detail < 0.0f) model_detail = 0.0f;
  
  	for (std::map<std::string, ModelStruct*>::const_iterator J = _entity_models.begin(); J != _entity_models.end(); J++) {
            if (J->second) if (J->second->model) (J->second)->model->setLodLevel(model_detail);
  	}
          terrain->lowerDetail();
        } else if (frame_rate > _upper_frame_rate_bound) {
          model_detail += 0.05f;
          if (model_detail > 1.0f) model_detail = 1.0f;
  
  	for (std::map<std::string, ModelStruct*>::const_iterator J = _entity_models.begin(); J != _entity_models.end(); J++) {
           if (J->second) if (J->second->model) (J->second)->model->setLodLevel(model_detail);
  	}
          terrain->raiseDetail();
        }
      }
    }
  } else {
//    stateChange(FONT);	  
    #ifndef _WIN32
      // Need to find a win32 version
      usleep(sleep_time);
    #endif
    glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
    glPushMatrix(); // Store The Projection Matrix
    glLoadIdentity(); // Reset The Projection Matrix
    glOrtho(0,window_width,0,window_height,-1,1); // Set Up An Ortho Screen
    glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
    glPushMatrix(); // Store The Modelview Matrix
    glLoadIdentity(); // Reset The Modelview Matrix
  
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST); 
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, getTextureID(splash_id));
    glBegin(GL_QUADS); 
      glTexCoord2i(0, 0); glVertex2f(0.0f, 0.0f);
      glTexCoord2i(0, 1); glVertex2f(0.0f, window_height);
      glTexCoord2i(1, 1); glVertex2f(window_width, window_height);
      glTexCoord2i(1, 0); glVertex2f(window_width, 0.0f);
    glEnd(); 
    glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
    glPopMatrix(); // Restore The Old Projection Matrix
    glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
    glPopMatrix(); // Restore The Old Projection Matrix

  }
//  nextState(FONT_TO_PANEL);
if (!select_mode) _system->getConsole()->draw(command);
//  nextState(PANEL_TO_FONT);
//  Calc FPS
  frame_rate = (float)num_frames /frame_time;
  if (checkState(RENDER_FPS)) {
    std::string frame_rate_string = string_fmt(frame_rate).substr(0, 4);
    glColor3f(1.0f, 0.0f, 0.0f);
    print(10, 100, frame_rate_string.c_str(), 0);
  }
  
  if (frame_time > 1.0f) {
    num_frames = 0;
    frame_time = 0.0f;
  }
  glColor3f(1.0f, 0.75f, 0.2f);
  print(x_pos, y_pos, active_name.c_str(), 1);
  
  glFlush();
  
  if (!select_mode)
   SDL_GL_SwapBuffers();

//  CheckError();
}

void GL_Render::buildQueues(WorldEntity *we, int depth) {
  if (depth == 0 || we->isVisible()) {
    if (we->getType() != NULL) {
      std::string type = we->getType()->getName();
      std::string parent = *we->getType()->getParentsAsSet().begin();
      ObjectLoader *ol = _system->getObjectLoader();
      ObjectProperties *op = NULL;
      if (!type.empty()) op = ol->getObjectProperties(type);
      if (op == NULL && !parent.empty()) op = ol->getObjectProperties(parent);
      if (op == NULL) op = ol->getObjectProperties("default");
      
      std::string model_type = std::string(op->model_type);

      if (op->draw_self && SphereInFrustum(we)) { 
        if (model_type == "bbox") render_queue[type].push_back(we);
        else if (model_type == "wire_frame") wireframe_queue[type].push_back(we);
        else if (model_type == "billboard") billboard_queue[type].push_back(we);
      	else if (model_type == "imposter") imposter_queue[type].push_back(we); 
        else model_queue[type].push_back(we);
      }
      if (op->draw_members) {
        for (unsigned int i = 0; i < we->getNumMembers(); i++) {
          buildQueues((WorldEntity*)we->getMember(i), depth + 1);
        }
      }
    }
  }
}

void GL_Render::drawModelQueue(bool select_mode) {
  std::string type, parent, id;
  for (std::map<std::string, Queue>::const_iterator I = model_queue.begin(); I != model_queue.end(); I++) {
    WorldEntity *we = (WorldEntity *)*(I->second.begin());
    type = we->getType()->getName();
    parent = *we->getType()->getParentsAsSet().begin();
    id = we->getID();
    ObjectLoader *ol = _system->getObjectLoader();
    ObjectProperties *op = NULL;
    if (!type.empty()) op = ol->getObjectProperties(type);
    if (op == NULL && !parent.empty()) op = ol->getObjectProperties(parent);
    if (op == NULL) op = ol->getObjectProperties("default");
    std::string model_type = std::string(op->model_type);
    for (Queue::const_iterator J = I->second.begin(); J != I->second.end(); J++) {
      WorldEntity *we = (WorldEntity*)*J;
      id = we->getID();
      ModelStruct *ms;
      ms = _entity_models[id];
      if (ms) {
        //Do nothing
      } else {
        Model *model = new Model();
        ms = new ModelStruct();
        ms->model_name = model_type;
        ms->in_use = true;
        if ((model) && model->onInit(System::instance()->getModel()->getAttribute(model_type))) {
        _entity_models[id] = ms;
	} else {
          if (model) delete model;
	  model = NULL;
	}
        ms->model = model;
	ms->models = NULL;
      }
      if (select_mode) nextColour(we->getID());
      else glColor3f(1.0f, 1.0f, 1.0f);
      glPushMatrix();
      WFMath::Point<3> pos = we->getAbsPos();
      glTranslatef(pos.x(), pos.y(), pos.z() + terrain->getHeight(pos.x(), pos.y()));
      WFMath::Quaternion q = we->getAbsOrient();
      float o[4][4];
      QuatToMatrix(q, o);
      glMultMatrixf(&o[0][0]);
      if (ms->model) {
        float scale = ms->model->getRenderScale();
        glScalef(scale, scale, scale);

        if (!select_mode && we->getID() == activeID) {
	active_name = we->getName();
	  stateChange(HALO);
	  if (checkState(RENDER_STENCIL)) {
            glEnable(GL_STENCIL_TEST);
  	    glStencilFunc(GL_ALWAYS, -1, 1);
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            glPushMatrix();
            ms->model->onRender(checkState(RENDER_TEXTURES), checkState(RENDER_LIGHTING), select_mode);
	    glPopMatrix();
	    glStencilFunc(GL_NOTEQUAL, -1, 1);
            glColor4fv(_halo_blend_colour);
	    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            ms->model->onRender(checkState(RENDER_TEXTURES), checkState(RENDER_LIGHTING), true);
	    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable(GL_STENCIL_TEST);
            glColor4fv(white);
	  } else {
	    glColor4fv(_halo_blend_colour);
	    ms->model->onRender(checkState(RENDER_TEXTURES), checkState(RENDER_LIGHTING), true);
	    glColor4fv(white);
	  }
	 stateChange(CHARACTERS); 
	} else {
          ms->model->onRender(checkState(RENDER_TEXTURES), checkState(RENDER_LIGHTING), select_mode);
	}
      } else {
        // Else downgrade model type
        strncpy(op->model_type,"bbox\0", 5);
	continue;
      }
      glPopMatrix();
    }
  }
}

void GL_Render::drawMessageQueue(bool select_mode) {
  if (select_mode) return;
  std::string type, parent;
  for (std::map<std::string, Queue>::const_iterator I = model_queue.begin(); I != model_queue.end(); I++) {
    WorldEntity *we = (WorldEntity *)*(I->second.begin());
    type = we->getType()->getName();
    parent = *we->getType()->getParentsAsSet().begin();
    for (Queue::const_iterator J = I->second.begin(); J != I->second.end(); J++) {
      glColor3f(1.0f, 1.0f, 1.0f);
      WorldEntity *we = (WorldEntity*)*J;
      glPushMatrix();
      WFMath::Point<3> pos = we->getAbsPos();
      glTranslatef(pos.x(), pos.y(), pos.z() + terrain->getHeight(pos.x(), pos.y()));

      float rotation_matrix[4][4];
      WFMath::Quaternion  orient2 = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Initial Camera rotation
      orient2 /= orient;
      QuatToMatrix(orient2, rotation_matrix); //Get the rotation matrix for base rotation

      glMultMatrixf(&rotation_matrix[0][0]); //Apply rotation matrix
      glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
      glScalef(0.025f, 0.025f, 0.025f);
      glTranslatef(_speech_offset_x, _speech_offset_y, _speech_offset_z);
      we->renderMessages();
      glPopMatrix();
    }
  }
}

void GL_Render::drawRenderQueue(bool select_mode) {
  ObjectLoader *ol = _system->getObjectLoader();
  std::string type, parent;
//  bool textures_enabled = true;
  for (std::map<std::string, Queue>::const_iterator I = render_queue.begin(); I != render_queue.end(); I++) {
    bool loaded = false;
    WorldEntity *we = (WorldEntity *)*(I->second.begin());
    type = we->getType()->getName();
    parent = *we->getType()->getParentsAsSet().begin();
    
    ObjectProperties *op = NULL;
    if (!type.empty()) op = ol->getObjectProperties(type);
    if (op == NULL && !parent.empty()) op = ol->getObjectProperties(parent);
    if (op == NULL) op = ol->getObjectProperties("default"); 
    int texture_id = -1;
    if (!select_mode) {
      texture_id = requestTexture("boundbox_" + type);
      if (texture_id == -1) texture_id = requestTexture("boundbox_" + parent);
      if (texture_id != -1) { 
        loaded = true;
        switchTexture(texture_id);
      } else {
	strncpy(op->model_type,"wire_frame\0", 11);
	continue;
      }
    }
  
    processObjectProperties(op);
    float texture_scale = op->texture_scale;
    if (texture_scale != 1.0f) {
      glMatrixMode(GL_TEXTURE);
      glPushMatrix();
      glScalef(op->texture_scale, op->texture_scale, 1.0f);
      glMatrixMode(GL_MODELVIEW);
    }
    for (Queue::const_iterator J = I->second.begin(); J != I->second.end(); J++) {
      WorldEntity *we = (WorldEntity*)*J;
      if (select_mode) nextColour(we->getID());
      else glColor4fv(white);
      glPushMatrix();
      WFMath::Point<3> pos = we->getAbsPos();
      glTranslatef(pos.x(), pos.y(), pos.z() + terrain->getHeight(pos.x(), pos.y()));
      Models *boundbox = NULL;
      ModelStruct *ms;
      ms = _entity_models[we->getID()];
      if (ms) {
        boundbox = (BoundBox*)ms->models;
      } else {
        WFMath::AxisBox<3> bbox = we->getBBox();
        if (!we->hasBBox()) {
          WFMath::Point<3> lc = WFMath::Point<3>(0.0f, 0.0f, 0.0f);
          WFMath::Point<3> hc = WFMath::Point<3>(1.0f, 1.0f, 1.0f);
          bbox = WFMath::AxisBox<3>(lc, hc);
        } 
        bbox = bboxCheck(bbox);
        boundbox = new BoundBox(bbox, op->wrap_texture);
        ms = new ModelStruct();
        ms->model_name = "bbox";
        ms->in_use = true;
        if ((boundbox) && boundbox->init()) {
          _entity_models[we->getID()] = ms;
        } else {
          if (boundbox) delete boundbox;
          boundbox = NULL;
          strncpy(op->model_type, "wireframe\0", 10);
          continue;
        }
        ms->model = NULL;
        ms->models = boundbox;
      }
        if (!select_mode && we->getID() == activeID) {
	active_name = we->getName();
	  stateChange(HALO);
	  if (checkState(RENDER_STENCIL)) {
            glEnable(GL_STENCIL_TEST);
            glStencilFunc(GL_ALWAYS, -1 ,1);
  	    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	    if (stateProperties[MODELS].textures) glEnable(GL_TEXTURE_2D);
	    drawModel(boundbox);
	    if (stateProperties[MODELS].textures) glDisable(GL_TEXTURE_2D);
	    glStencilFunc(GL_NOTEQUAL, -1, 1);
	    glPolygonMode(GL_FRONT, GL_LINE);
	    glColor4fv(_halo_blend_colour);
	    drawModel(boundbox);
	    glDisable(GL_STENCIL_TEST);
	    glColor4fv(white);
	    glPolygonMode(GL_FRONT, GL_FILL);
	  } else {
	    glColor4fv(_halo_blend_colour);
	    drawModel(boundbox);
	    glColor4fv(white);
	  }
	  stateChange(MODELS);
	} else
	  drawModel(boundbox);
      glPopMatrix();
    }
    if (texture_scale != 1.0f) {
      glMatrixMode(GL_TEXTURE);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
    }
  }
}

void GL_Render::stateChange(State state) {
  if (state == current_state) return; 
  current_state = state;
  if (stateProperties[state].alpha_test) glEnable(GL_ALPHA_TEST);
  else glDisable(GL_ALPHA_TEST);
  if (stateProperties[state].blend) glEnable(GL_BLEND);
  else glDisable(GL_BLEND);
  if (stateProperties[state].lighting) glEnable(GL_LIGHTING);
  else glDisable(GL_LIGHTING);
  if (stateProperties[state].textures) glEnable(GL_TEXTURE_2D);
  else glDisable(GL_TEXTURE_2D);
  if (stateProperties[state].colour_material) glEnable(GL_COLOR_MATERIAL);
  else glDisable(GL_COLOR_MATERIAL);
  if (stateProperties[state].depth_test) glEnable(GL_DEPTH_TEST);
  else glDisable(GL_DEPTH_TEST);
  if (stateProperties[state].cull_face) glEnable(GL_CULL_FACE);
  else glDisable(GL_CULL_FACE);
  if (stateProperties[state].cull_face_cw) glFrontFace(GL_CW);
  else glFrontFace(GL_CCW);
  if (stateProperties[state].stencil) glEnable(GL_STENCIL_TEST);
  else glDisable(GL_STENCIL_TEST);
  if (stateProperties[state].fog) glEnable(GL_FOG);
  else glDisable(GL_FOG);
}


void GL_Render::drawTextRect(GLint x, GLint y, GLint width, GLint height, int texture) {

  glBindTexture(GL_TEXTURE_2D, getTextureID(texture));

  glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
  glPushMatrix(); // Store The Projection Matrix
  glLoadIdentity(); // Reset The Projection Matrix
  glOrtho(0,window_width,0,window_height,-1,1); // Set Up An Ortho Screen
  glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
  glPushMatrix(); // Store The Modelview Matrix
  glLoadIdentity(); // Reset The Modelview Matrix

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

  glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
  glPopMatrix(); // Restore The Old Projection Matrix
  glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
  glPopMatrix(); // Restore The Old Projection Matrix

}

void GL_Render::procEvent(int x, int y) {
  unsigned int ic;
  std::string selected_id;
  GLubyte i[3];
  stateChange(SELECT);
  glClear(GL_COLOR_BUFFER_BIT);
  drawScene("", true);
  y = window_height - y;
  x_pos = x;
  y_pos = y;
  glReadPixels(x, y, 1, 1, GL_RGB , GL_UNSIGNED_BYTE, &i);

  GLubyte red = i[0] >> (8 - redBits);// & redMask;
  GLubyte green = i[1] >> (8 - greenBits);// & greenMask;
  GLubyte blue = i[2] >> (8 - blueBits);// & blueMask;

  ic = 0;
  ic += red;
  ic = ic << redBits;
  ic += green;
  ic = ic << greenBits;
  ic += blue;
  selected_id = getSelectedID(ic);
  if (selected_id != activeID) {
    if (!activeID.empty()) Log::writeLog(std::string("ActiveID: ") + activeID, Log::DEFAULT);
    activeID = selected_id;
  }
  stateChange(FONT);
}

void GL_Render::extractFrustum() {
  float   proj[16];
  float   modl[16];
  float   clip[16];
  float   t;

  /* Get the current PROJECTION matrix from OpenGL */
  glGetFloatv( GL_PROJECTION_MATRIX, proj );

  /* Get the current MODELVIEW matrix from OpenGL */
  glGetFloatv( GL_MODELVIEW_MATRIX, modl );

  /* Combine the two matrices (multiply projection by modelview) */
  clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
  clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
  clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
  clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];
  clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
  clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
  clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
  clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];
  clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
  clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
  clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
  clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];
  clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
  clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
  clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
  clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

  /* Extract the numbers for the RIGHT plane */
  frustum[0][0] = clip[ 3] - clip[ 0];
  frustum[0][1] = clip[ 7] - clip[ 4];
  frustum[0][2] = clip[11] - clip[ 8];
  frustum[0][3] = clip[15] - clip[12];

  /* Normalize the result */
  t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
  frustum[0][0] /= t;
  frustum[0][1] /= t;
  frustum[0][2] /= t;
  frustum[0][3] /= t;

  /* Extract the numbers for the LEFT plane */
  frustum[1][0] = clip[ 3] + clip[ 0];
  frustum[1][1] = clip[ 7] + clip[ 4];
  frustum[1][2] = clip[11] + clip[ 8];
  frustum[1][3] = clip[15] + clip[12];

  /* Normalize the result */
  t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
  frustum[1][0] /= t;
  frustum[1][1] /= t;
  frustum[1][2] /= t;
  frustum[1][3] /= t;

  /* Extract the BOTTOM plane */
  frustum[2][0] = clip[ 3] + clip[ 1];
  frustum[2][1] = clip[ 7] + clip[ 5];
  frustum[2][2] = clip[11] + clip[ 9];
  frustum[2][3] = clip[15] + clip[13];

  /* Normalize the result */
  t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
  frustum[2][0] /= t;
  frustum[2][1] /= t;
  frustum[2][2] /= t;
  frustum[2][3] /= t;

  /* Extract the TOP plane */
  frustum[3][0] = clip[ 3] - clip[ 1];
  frustum[3][1] = clip[ 7] - clip[ 5];
  frustum[3][2] = clip[11] - clip[ 9];
  frustum[3][3] = clip[15] - clip[13];

  /* Normalize the result */
  t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
  frustum[3][0] /= t;
  frustum[3][1] /= t;
  frustum[3][2] /= t;
  frustum[3][3] /= t;

  /* Extract the FAR plane */
  frustum[4][0] = clip[ 3] - clip[ 2];
  frustum[4][1] = clip[ 7] - clip[ 6];
  frustum[4][2] = clip[11] - clip[10];
  frustum[4][3] = clip[15] - clip[14];

  /* Normalize the result */
  t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
  frustum[4][0] /= t;
  frustum[4][1] /= t;
  frustum[4][2] /= t;
  frustum[4][3] /= t;

  /* Extract the NEAR plane */
  frustum[5][0] = clip[ 3] + clip[ 2];
  frustum[5][1] = clip[ 7] + clip[ 6];
  frustum[5][2] = clip[11] + clip[10];
  frustum[5][3] = clip[15] + clip[14];

   /* Normalize the result */
   t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
   frustum[5][0] /= t;
   frustum[5][1] /= t;
   frustum[5][2] /= t;
   frustum[5][3] /= t;
}

bool GL_Render::PointInFrustum( float x, float y, float z ) {
  int p;
  for( p = 0; p < 6; p++ )
    if( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= 0 ) return false;
  return true;
} 

int GL_Render::CubeInFrustum( WorldEntity *we ) {  
  int p;
  int c;
  int c2 = 0;
  if (!we->hasBBox()) return true;
  WFMath::AxisBox<3> entity_bbox = we->getBBox();
  WFMath::Point<3> pos = we->getAbsPos();
  //Translate BBox to correct position
  WFMath::Point<3> lowCorner = WFMath::Point<3>(entity_bbox.lowCorner().x() + pos.x(), entity_bbox.lowCorner().y() + pos.y(), entity_bbox.lowCorner().z() + pos.z());
  WFMath::Point<3> highCorner = WFMath::Point<3>(entity_bbox.highCorner().x() + pos.x(), entity_bbox.highCorner().y() + pos.y(), entity_bbox.highCorner().z() + pos.z());


  WFMath::AxisBox<3> bbox = WFMath::AxisBox<3>(lowCorner, highCorner);

  for( p = 0; p < 6; p++ ) {
    c = 0;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( c == 0 ) return 0;
    if( c == 8 ) c2++;
  }
  return (c2 == 6) ? 2 : 1;
}


int GL_Render::patchInFrustum(WFMath::AxisBox<3> bbox) {  
  int p;
  int c;
  int c2 = 0;

  for( p = 0; p < 6; p++ ) {
    c = 0;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( c == 0 ) return 0;
    if( c == 8 ) c2++;
  }
  return (c2 == 6) ? 2 : 1;
}

float GL_Render::distFromNear(float x, float y, float z) {
  return (frustum[5][0] * x + frustum[5][1] * y + frustum[5][2] * z + frustum[5][3]);
}

void GL_Render::setCallyState(int i) {
  if (player_model) player_model->setState(i, 0.1f);	
}

void GL_Render::setCallyMotion(float f1, float f2, float f3) {
  float f[3];
  f[0] = f1;
  f[1] = f2;
  f[2] = f3;
  if (player_model) player_model->setMotionBlend(f, 0.1f);
}


void GL_Render::executeCallyAction(int action) {
  if (player_model) player_model->executeAction(action);
}

WFMath::AxisBox<3> GL_Render::bboxCheck(WFMath::AxisBox<3> bbox) {
  int count = 0;
  if (bbox.lowCorner().x()
    + bbox.lowCorner().y() 
    + bbox.lowCorner().z()
    + bbox.highCorner().x()
    + bbox.highCorner().y()
    + bbox.highCorner().z()
    == 0.0f) {
    // BBOX has no size!! or is equidistant sround origin!!!!!
    WFMath::Point<3> lc = WFMath::Point<3>(0.0f, 0.0f, 0.0f);
    WFMath::Point<3> hc = WFMath::Point<3>(1.0f, 1.0f, 1.0f);
    bbox = WFMath::AxisBox<3>(lc, hc);
  }
  if (bbox.highCorner().x() > bbox.lowCorner().x()) count++;
  if (bbox.highCorner().y() < bbox.lowCorner().y()) count++;
  if (bbox.highCorner().z() < bbox.lowCorner().z()) count++;
  if (count == 0 || count == 2) return bbox;
  else return WFMath::AxisBox<3>(bbox.highCorner(), bbox.lowCorner());
}

void GL_Render::drawBBox(BoundBox *bbox) {
  bool textures = checkState(RENDER_TEXTURES);
  bool lighting = checkState(RENDER_LIGHTING);
  glVertexPointer(3, GL_FLOAT, 0, bbox->getVertexData());
  glEnableClientState(GL_VERTEX_ARRAY);
  if (textures) {
    glTexCoordPointer(2, GL_FLOAT, 0, bbox->getTextureData());
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }
  if (lighting) {
    glNormalPointer(GL_FLOAT, 0, bbox->getNormalData());
    glEnableClientState(GL_NORMAL_ARRAY);
  }
  glDrawArrays(GL_QUADS, 0, bbox->getNumPoints());
  glDisableClientState(GL_VERTEX_ARRAY);
  if (lighting) glDisableClientState(GL_NORMAL_ARRAY);
  if (textures) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void GL_Render::CheckError() {
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
  Log::writeLog(msg, Log::ERROR);
}

void GL_Render::processObjectProperties(ObjectProperties *op) {
  if (!op) {
    Log::writeLog("No OP!", Log::ERROR);
    return;
  }
  glMaterialfv (GL_FRONT, GL_AMBIENT,   op->material_properties->ambient);
  glMaterialfv (GL_FRONT, GL_DIFFUSE,   op->material_properties->diffuse);
  glMaterialfv (GL_FRONT, GL_SPECULAR,  op->material_properties->specular);
  glMaterialf  (GL_FRONT, GL_SHININESS, op->material_properties->shininess);
  glMaterialfv (GL_FRONT, GL_EMISSION,  op->material_properties->emission);
}

void GL_Render::checkModelStatus(const std::string &id) {
  ModelStruct *ms = _entity_models[id];
  if (ms) {
    if (!ms->in_use) {
      Log::writeLog("Freeing entity data", Log::DEFAULT);
      if (ms->model) { // Cal3D model
        Log::writeLog("Deleting a model", Log::DEFAULT);
        ms->model->onShutdown();
	delete ms->model;
	ms->model = NULL;
      }
      if (ms->models) {
        Log::writeLog("Deleting a model", Log::DEFAULT);
        ms->models->shutdown();
	delete ms->models;
	ms->models = NULL;
      }
      _entity_models[id] = NULL;
      delete ms;
    }
  }
}

void GL_Render::setModelInUse(const std::string &id, bool use) {
  ModelStruct *ms = _entity_models[id];
  if (ms) ms->in_use = use;
}

void GL_Render::drawBillBoard(BillBoard *billboard) {
/*  bool textures = checkState(RENDER_TEXTURES);
  bool lighting = checkState(RENDER_LIGHTING);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, billboard->getVertexData());
  if (textures) {
    glTexCoordPointer(2, GL_FLOAT, 0, billboard->getTextureData());
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }
  if (lighting) {
    glNormalPointer(GL_FLOAT, 0, billboard->getNormalData());
    glEnableClientState(GL_NORMAL_ARRAY);
  }
  glDrawArrays(GL_QUADS, 0, billboard->getNumPoints());
  glDisableClientState(GL_VERTEX_ARRAY);
  if (lighting) glDisableClientState(GL_NORMAL_ARRAY);
  if (textures) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  */
}

void GL_Render::drawBBoardQueue(bool select_mode) {
  std::string type, parent;
  for (std::map<std::string, Queue>::const_iterator I = billboard_queue.begin(); I != billboard_queue.end(); I++) {
    WorldEntity *we = (WorldEntity *)*(I->second.begin());
    type = we->getType()->getName();
    parent = *we->getType()->getParentsAsSet().begin();
    bool loaded = false;
    ObjectLoader *ol = _system->getObjectLoader();
    ObjectProperties *op = NULL;
    if (!type.empty()) op = ol->getObjectProperties(type);
    if (op == NULL && !parent.empty()) op = ol->getObjectProperties(parent);
    if (op == NULL) op = ol->getObjectProperties("default"); 
    int texture_id ;
    int texture_mask_id = -1;
    texture_id = requestTexture("billboard_" + type, true);
    if (texture_id == -1) texture_id = requestTexture("billboard_" + parent, true);
    else texture_mask_id = requestTextureMask("billboard_" + type, true);
    if (texture_id != -1) { 
      loaded = true;
      if (texture_mask_id == -1) texture_mask_id = requestTextureMask("billboard_" + parent, true);
      if (select_mode) switchTexture(texture_mask_id);
      else switchTexture(texture_id);
    } else {
      strncpy(op->model_type, "bbox\0", 5);
      continue;
    }
    Models *billboard = NULL;
    ModelStruct *ms;
    ms = _entity_models[type];
    if (ms) {
      billboard = (BillBoard*)ms->models;
    } else {
      billboard = new BillBoard(op->width, op->height);
      ms = new ModelStruct();
      ms->model_name = "billboard";
      ms->in_use = true;
      if ((billboard) && billboard->init()) {
        _entity_models[type] = ms;
      } else {
        if (billboard) delete billboard;
        billboard = NULL;
        strncpy(op->model_type, "bbox\0", 5);
        continue;
      }
      ms->model = NULL;
      ms->models = billboard;
    }
    for (Queue::const_iterator J = I->second.begin(); J != I->second.end(); J++) {
      WorldEntity *we = (WorldEntity*)*J;
      if (select_mode) nextColour(we->getID());
      else glColor4fv(white);
      glPushMatrix();
      WFMath::Point<3> pos = we->getAbsPos();
      glTranslatef(pos.x(), pos.y(), pos.z() + terrain->getHeight(pos.x(), pos.y()));

      float rotation_matrix[4][4];
      WFMath::Quaternion  orient2 = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Initial Camera rotation
      orient2 /= orient;
      QuatToMatrix(orient2, rotation_matrix); //Get the rotation matrix for base rotation

      glMultMatrixf(&rotation_matrix[0][0]); //Apply rotation matrix
      if (!select_mode && we->getID() == activeID) {
	active_name = we->getName();
        stateChange(HALO);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_ALPHA_TEST);
        glColor4fv(_halo_blend_colour);
        if (checkState(RENDER_STENCIL)) {
          glEnable(GL_STENCIL_TEST);
          glStencilFunc(GL_ALWAYS, -1, 1);
          glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
          drawModel(billboard);
          glStencilFunc(GL_NOTEQUAL, -1, 1);
          drawModel(billboard);
          glDisable(GL_STENCIL_TEST);
        } else {
          switchTexture(texture_mask_id);
	  drawModel(billboard);
	  switchTexture(texture_id);
      	}
        glColor4fv(white);
        stateChange(BILLBOARD);
      } else {
         drawModel(billboard);
      }
      glPopMatrix();
    }
  }
}

void GL_Render::buildDisplayLists() {
  if (glIsList(states)) glDeleteLists(states, LAST_STATE);
  setupStates();
  states = glGenLists(LAST_STATE);
  for (State state = (State)0; state != LAST_STATE; ((int)state)++) {
    Log::writeLog("Building list for state: " + string_fmt(state), Log::DEFAULT);
    glNewList(states + state, GL_COMPILE);
      if (stateProperties[state].alpha_test) glEnable(GL_ALPHA_TEST);
      else glDisable(GL_ALPHA_TEST);
      if (stateProperties[state].blend) glEnable(GL_BLEND);
      else glDisable(GL_BLEND);
      if (stateProperties[state].lighting) glEnable(GL_LIGHTING);
      else glDisable(GL_LIGHTING);
      if (stateProperties[state].textures) glEnable(GL_TEXTURE_2D);
      else glDisable(GL_TEXTURE_2D);
      if (stateProperties[state].colour_material) glEnable(GL_COLOR_MATERIAL);
      else glDisable(GL_COLOR_MATERIAL);
      if (stateProperties[state].depth_test) glEnable(GL_DEPTH_TEST);
      else glDisable(GL_DEPTH_TEST);
      if (stateProperties[state].cull_face) glEnable(GL_CULL_FACE);
      else glDisable(GL_CULL_FACE);
      if (stateProperties[state].cull_face_cw) glFrontFace(GL_CW);
      else glFrontFace(GL_CCW);
      if (stateProperties[state].stencil) glEnable(GL_STENCIL_TEST);
      else glDisable(GL_STENCIL_TEST);
      if (stateProperties[state].fog) glEnable(GL_FOG);
      else glDisable(GL_FOG);
    glEndList();
  }
}

void GL_Render::drawImpostorQueue(bool select_mode) {
  std::string type, parent;
  for (std::map<std::string, Queue>::const_iterator I = imposter_queue.begin(); I != imposter_queue.end(); I++) {
    WorldEntity *we = (WorldEntity *)*(I->second.begin());
    type = we->getType()->getName();
    parent = *we->getType()->getParentsAsSet().begin();
    bool loaded = false;
    ObjectLoader *ol = _system->getObjectLoader();
    ObjectProperties *op = NULL;
    if (!type.empty()) op = ol->getObjectProperties(type);
    if (op == NULL && !parent.empty()) op = ol->getObjectProperties(parent);
    if (op == NULL) op = ol->getObjectProperties("default"); 
    int texture_id ;
    int texture_mask_id = -1;
    texture_id = requestTexture("impostor_" + type, true);
    if (texture_id == -1) texture_id = requestTexture("impostor_" + parent, true);
    else texture_mask_id = requestTextureMask("impostor_" + type, true);
    if (texture_id != -1) { 
      loaded = true;
      if (texture_mask_id == -1) texture_mask_id = requestTextureMask("impostor_" + parent, true);
      if (select_mode) switchTexture(texture_mask_id);
      else switchTexture(texture_id);
    } else {
      strncpy(op->model_type, "bbox\0", 5);
      continue;
    }
    Models *impostor = NULL;
    ModelStruct *ms;
    ms = _entity_models[type];
    if (ms) {
      impostor = (Impostor*)ms->models;
    } else {
      impostor = new Impostor(op->width, op->height);
      ms = new ModelStruct();
      ms->model_name = "impostor";
      ms->in_use = true;
      if ((impostor) && impostor->init()) {
        _entity_models[type] = ms;
      } else {
        if (impostor) delete impostor;
        impostor = NULL;
        strncpy(op->model_type, "bbox\0", 5);
        continue;
      }
      ms->model = NULL;
      ms->models = impostor;
    }
    for (Queue::const_iterator J = I->second.begin(); J != I->second.end(); J++) {
      WorldEntity *we = (WorldEntity*)*J;
      if (select_mode) nextColour(we->getID());
      else glColor3f(1.0f, 1.0f, 1.0f);
      glPushMatrix();
      WFMath::Point<3> pos = we->getAbsPos();
      glTranslatef(pos.x(), pos.y(), pos.z() + terrain->getHeight(pos.x(), pos.y()));
      glRotatef(pos.x() + pos.y() + pos.z(), 0.0f, 0.0f, 1.0f);
      if (!select_mode && we->getID() == activeID) {
	active_name = we->getName();
 	stateChange(HALO);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_TEXTURE_2D);
        glColor4fv(_halo_blend_colour);
	if (checkState(RENDER_STENCIL)) {
          glEnable(GL_STENCIL_TEST);
	  glStencilFunc(GL_ALWAYS, -1, 1);
	  glStencilOp(GL_REPLACE, GL_KEEP, GL_REPLACE);
	  drawModel(impostor);
	  glStencilFunc(GL_NOTEQUAL, -1, 1);
          drawModel(impostor);
          glDisable(GL_STENCIL_TEST);
	} else {
          switchTexture(texture_mask_id);
          drawModel(impostor);
          switchTexture(texture_id);
	}
      stateChange(BILLBOARD);
      glColor4fv(white);
      } else {
        drawModel(impostor);
      }
      glPopMatrix();
    }
  }
}

void GL_Render::drawImpostor(Impostor *impostor) {
/*  bool textures = checkState(RENDER_TEXTURES);
  bool lighting = checkState(RENDER_LIGHTING);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, impostor->getVertexData());
  if (textures) {
    glTexCoordPointer(2, GL_FLOAT, 0, impostor->getTextureData());
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }
  if (lighting) {
    glNormalPointer(GL_FLOAT, 0, impostor->getNormalData());
    glEnableClientState(GL_NORMAL_ARRAY);
  }
  glDrawArrays(GL_QUADS, 0, impostor->getNumPoints());
  glDisableClientState(GL_VERTEX_ARRAY);
  if (lighting) glDisableClientState(GL_NORMAL_ARRAY);
  if (textures) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  */
}

void GL_Render::readConfig() {
  std::string temp;
  Config *general = _system->getGeneral();
  Log::writeLog("Loading Renderer Config", Log::DEFAULT);
  if (!general) {
    Log::writeLog("GL_Render: Error - General config object does not exist!", Log::ERROR);
    return;
  }

  // Setup character light source
  temp = general->getAttribute(KEY_character_light_kc);
  lights[LIGHT_CHARACTER].kc = (temp.empty()) ? (DEFAULT_character_light_kc) : (atof(temp.c_str()));
  temp = general->getAttribute(KEY_character_light_kl);
  lights[LIGHT_CHARACTER].kl = (temp.empty()) ? (DEFAULT_character_light_kl) : (atof(temp.c_str()));
  temp = general->getAttribute(KEY_character_light_kq);
  lights[LIGHT_CHARACTER].kq = (temp.empty()) ? (DEFAULT_character_light_kq) : (atof(temp.c_str()));
  
  temp = general->getAttribute(KEY_character_light_ambient_red);
  lights[LIGHT_CHARACTER].ambient[0] = (temp.empty()) ? (DEFAULT_character_light_ambient_red) : (atof(temp.c_str()));
  temp = general->getAttribute(KEY_character_light_ambient_green);
  lights[LIGHT_CHARACTER].ambient[1] = (temp.empty()) ? (DEFAULT_character_light_ambient_green) : (atof(temp.c_str()));
  temp = general->getAttribute(KEY_character_light_ambient_blue);
  lights[LIGHT_CHARACTER].ambient[2] = (temp.empty()) ? (DEFAULT_character_light_ambient_blue) : (atof(temp.c_str()));
  temp = general->getAttribute(KEY_character_light_ambient_alpha);
  lights[LIGHT_CHARACTER].ambient[3] = (temp.empty()) ? (DEFAULT_character_light_ambient_alpha) : (atof(temp.c_str()));

  temp = general->getAttribute(KEY_character_light_diffuse_red);
  lights[LIGHT_CHARACTER].diffuse[0] = (temp.empty()) ? (DEFAULT_character_light_diffuse_red) : (atof(temp.c_str()));
  temp = general->getAttribute(KEY_character_light_diffuse_green);
  lights[LIGHT_CHARACTER].diffuse[1] = (temp.empty()) ? (DEFAULT_character_light_diffuse_green) : (atof(temp.c_str()));
  temp = general->getAttribute(KEY_character_light_diffuse_blue);
  lights[LIGHT_CHARACTER].diffuse[2] = (temp.empty()) ? (DEFAULT_character_light_diffuse_blue) : (atof(temp.c_str()));
  temp = general->getAttribute(KEY_character_light_diffuse_alpha);
  lights[LIGHT_CHARACTER].diffuse[3] = (temp.empty()) ? (DEFAULT_character_light_diffuse_alpha) : (atof(temp.c_str()));

  temp = general->getAttribute(KEY_character_light_specular_red);
  lights[LIGHT_CHARACTER].specular[0] = (temp.empty()) ? (DEFAULT_character_light_specular_red) : (atof(temp.c_str()));
  temp = general->getAttribute(KEY_character_light_specular_green);
  lights[LIGHT_CHARACTER].specular[1] = (temp.empty()) ? (DEFAULT_character_light_specular_green) : (atof(temp.c_str()));
  temp = general->getAttribute(KEY_character_light_specular_blue);
  lights[LIGHT_CHARACTER].specular[2] = (temp.empty()) ? (DEFAULT_character_light_specular_blue) : (atof(temp.c_str()));
  temp = general->getAttribute(KEY_character_light_specular_alpha);
  lights[LIGHT_CHARACTER].specular[3] = (temp.empty()) ? (DEFAULT_character_light_specular_alpha) : (atof(temp.c_str()));
  //Setup Sun light source
  temp = general->getAttribute(KEY_sun_light_kc);
  lights[LIGHT_SUN].kc = (temp.empty()) ? (DEFAULT_sun_light_kc) : (atof(temp.c_str()));
  temp = general->getAttribute(KEY_sun_light_kl);
  lights[LIGHT_SUN].kl = (temp.empty()) ? (DEFAULT_sun_light_kl) : (atof(temp.c_str()));
  temp = general->getAttribute(KEY_sun_light_kq);
  lights[LIGHT_SUN].kq = (temp.empty()) ? (DEFAULT_sun_light_kq) : (atof(temp.c_str()));

  // Setup render states
  temp = general->getAttribute(KEY_use_textures);
  setState(RENDER_TEXTURES, (temp.empty()) ? (DEFAULT_use_textures) : (temp == "true"));
  temp = general->getAttribute(KEY_use_lighting);
  setState(RENDER_LIGHTING, (temp.empty()) ? (DEFAULT_use_lighting) : (temp == "true"));
  temp = general->getAttribute(KEY_show_fps);
  setState(RENDER_FPS, (temp.empty()) ? (DEFAULT_show_fps) : (temp == "true"));
  temp = general->getAttribute(KEY_use_stencil);
  setState(RENDER_STENCIL, (temp.empty()) ? (DEFAULT_use_stencil) : (temp == "true"));

  // Setup frame rate detail boundaries
  temp = general->getAttribute(KEY_lower_frame_rate_bound);
  _lower_frame_rate_bound = (temp.empty()) ? (DEFAULT_lower_frame_rate_bound) : atof(temp.c_str());
  temp = general->getAttribute(KEY_upper_frame_rate_bound);
  _upper_frame_rate_bound = (temp.empty()) ? (DEFAULT_upper_frame_rate_bound) : atof(temp.c_str());

  // Setup the speech offsets
  temp = general->getAttribute(KEY_speech_offset_x);
  _speech_offset_x = (temp.empty()) ? (DEFAULT_speech_offset_x) : atof(temp.c_str());
  temp = general->getAttribute(KEY_speech_offset_y);
  _speech_offset_y = (temp.empty()) ? (DEFAULT_speech_offset_y) : atof(temp.c_str());
  temp = general->getAttribute(KEY_speech_offset_z);
  _speech_offset_z = (temp.empty()) ? (DEFAULT_speech_offset_y) : atof(temp.c_str());

  temp = general->getAttribute(KEY_fog_start);
  _fog_start = (temp.empty()) ? (DEFAULT_fog_start) : atof(temp.c_str());
  temp = general->getAttribute(KEY_fog_end);
  _fog_end = (temp.empty()) ? (DEFAULT_fog_end) : atof(temp.c_str());

  temp = general->getAttribute(KEY_far_clip_dist);
  _far_clip_dist = (temp.empty()) ? (DEFAULT_far_clip_dist) : atof(temp.c_str());
  
}  

void GL_Render::writeConfig() {
  Config *general = _system->getGeneral();
  if (!general) {
    Log::writeLog("GL_Render: Error - General config object does not exist!", Log::ERROR);
    return;
  }
  
  // Save character light source
  general->setAttribute(KEY_character_light_kc, string_fmt(lights[LIGHT_CHARACTER].kc));
  general->setAttribute(KEY_character_light_kl, string_fmt(lights[LIGHT_CHARACTER].kl));
  general->setAttribute(KEY_character_light_kq, string_fmt(lights[LIGHT_CHARACTER].kq));

  general->setAttribute(KEY_character_light_ambient_red, string_fmt(lights[LIGHT_CHARACTER].ambient[0]));
  general->setAttribute(KEY_character_light_ambient_green, string_fmt(lights[LIGHT_CHARACTER].ambient[1]));
  general->setAttribute(KEY_character_light_ambient_blue, string_fmt(lights[LIGHT_CHARACTER].ambient[2]));
  general->setAttribute(KEY_character_light_ambient_alpha, string_fmt(lights[LIGHT_CHARACTER].ambient[3]));

  general->setAttribute(KEY_character_light_diffuse_red, string_fmt(lights[LIGHT_CHARACTER].diffuse[0]));
  general->setAttribute(KEY_character_light_diffuse_green, string_fmt(lights[LIGHT_CHARACTER].diffuse[1]));
  general->setAttribute(KEY_character_light_diffuse_blue, string_fmt(lights[LIGHT_CHARACTER].diffuse[2]));
  general->setAttribute(KEY_character_light_diffuse_alpha, string_fmt(lights[LIGHT_CHARACTER].diffuse[3]));

  general->setAttribute(KEY_character_light_specular_red, string_fmt(lights[LIGHT_CHARACTER].specular[0]));
  general->setAttribute(KEY_character_light_specular_green, string_fmt(lights[LIGHT_CHARACTER].specular[1]));
  general->setAttribute(KEY_character_light_specular_blue, string_fmt(lights[LIGHT_CHARACTER].specular[2]));
  general->setAttribute(KEY_character_light_specular_alpha, string_fmt(lights[LIGHT_CHARACTER].specular[3]));
  
  // Save Sun light source
  general->setAttribute(KEY_sun_light_kc, string_fmt(lights[LIGHT_SUN].kc));
  general->setAttribute(KEY_sun_light_kl, string_fmt(lights[LIGHT_SUN].kl));
  general->setAttribute(KEY_sun_light_kq, string_fmt(lights[LIGHT_SUN].kq));

  // Save render states
  general->setAttribute(KEY_use_textures, (checkState(RENDER_TEXTURES)) ? ("true") : ("false"));
  general->setAttribute(KEY_use_lighting, (checkState(RENDER_LIGHTING)) ? ("true") : ("false"));
  general->setAttribute(KEY_show_fps, (checkState(RENDER_FPS)) ? ("true") : ("false"));
  general->setAttribute(KEY_use_stencil, (checkState(RENDER_STENCIL)) ? ("true") : ("false"));
  
  // Save frame rate detail boundaries
  general->setAttribute(KEY_lower_frame_rate_bound, string_fmt(_lower_frame_rate_bound));
  general->setAttribute(KEY_upper_frame_rate_bound, string_fmt(_upper_frame_rate_bound));

  // Save the speech offsets
  general->setAttribute(KEY_speech_offset_x, string_fmt(_speech_offset_x));
  general->setAttribute(KEY_speech_offset_y, string_fmt(_speech_offset_y));
  general->setAttribute(KEY_speech_offset_z, string_fmt(_speech_offset_z));

  general->setAttribute(KEY_fog_start, string_fmt(_fog_start));
  general->setAttribute(KEY_fog_end, string_fmt(_fog_end));
  general->setAttribute(KEY_far_clip_dist, string_fmt(_far_clip_dist));
}  

void GL_Render::stateDisplayList(GLuint list, State previous_state, State next_state) {
  glNewList(list, GL_COMPILE);
  if (stateProperties[previous_state].alpha_test != stateProperties[next_state].alpha_test) {
    if (stateProperties[next_state].alpha_test) glEnable(GL_ALPHA_TEST);
    else glDisable(GL_ALPHA_TEST);
  }
  if (stateProperties[previous_state].blend != stateProperties[next_state].blend) {
    if (stateProperties[next_state].blend) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
  }
  if (stateProperties[previous_state].lighting != stateProperties[next_state].lighting) {
    if (stateProperties[next_state].lighting) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);
  }
  if (stateProperties[previous_state].textures != stateProperties[next_state].textures) {
    if (stateProperties[next_state].textures) glEnable(GL_TEXTURE_2D);
    else glDisable(GL_TEXTURE_2D);
  }
  if (stateProperties[previous_state].colour_material != stateProperties[next_state].colour_material) {
    if (stateProperties[next_state].colour_material) glEnable(GL_COLOR_MATERIAL);
    else glDisable(GL_COLOR_MATERIAL);
  }
  if (stateProperties[previous_state].depth_test != stateProperties[next_state].depth_test) {
    if (stateProperties[next_state].depth_test) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
  }
  if (stateProperties[previous_state].cull_face != stateProperties[next_state].cull_face) {
    if (stateProperties[next_state].cull_face) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
    if (stateProperties[next_state].cull_face_cw) glFrontFace(GL_CW);
    else glFrontFace(GL_CCW);
  }
  if (stateProperties[previous_state].stencil != stateProperties[next_state].stencil) {
    if (stateProperties[next_state].stencil) glEnable(GL_STENCIL_TEST);
    else glDisable(GL_STENCIL_TEST);
  }
  if (stateProperties[previous_state].fog != stateProperties[next_state].fog) {
    if (stateProperties[next_state].fog) glEnable(GL_FOG);
    else glDisable(GL_FOG);
  }
  glEndList();
}


void GL_Render::setupStates() {
  glAlphaFunc(GL_GREATER, 0.1f);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  GLfloat fog_colour[] = {0.50f, 0.50f, 0.50f, 0.50f};
  glFogfv(GL_FOG_COLOR, fog_colour);
  glFogf(GL_FOG_START, _fog_start);
  glFogf(GL_FOG_END, _fog_end);

  stateProperties[SKYBOX].alpha_test = false;
  stateProperties[SKYBOX].blend = false;
  stateProperties[SKYBOX].lighting = false;
  stateProperties[SKYBOX].textures = checkState(RENDER_TEXTURES);
  stateProperties[SKYBOX].colour_material = false;
  stateProperties[SKYBOX].depth_test = false;
  stateProperties[SKYBOX].cull_face = false;
  stateProperties[SKYBOX].cull_face_cw = false;
  stateProperties[SKYBOX].stencil = false;
  stateProperties[SKYBOX].fog = false;
  
  stateProperties[TERRAIN].alpha_test = false;
  stateProperties[TERRAIN].blend = false;
  stateProperties[TERRAIN].lighting = checkState(RENDER_LIGHTING);
  stateProperties[TERRAIN].textures = checkState(RENDER_TEXTURES);
  stateProperties[TERRAIN].colour_material = false;//!checkState(RENDER_LIGHTING);
  stateProperties[TERRAIN].depth_test = true;
  stateProperties[TERRAIN].cull_face = true;
  stateProperties[TERRAIN].cull_face_cw = true;
  stateProperties[TERRAIN].stencil = false;
  stateProperties[TERRAIN].fog = true;
  
  stateProperties[WIREFRAME].alpha_test = false;
  stateProperties[WIREFRAME].blend = false;
  stateProperties[WIREFRAME].lighting = false;
  stateProperties[WIREFRAME].textures = false;
  stateProperties[WIREFRAME].colour_material = false;
  stateProperties[WIREFRAME].depth_test = true;
  stateProperties[WIREFRAME].cull_face = false;
  stateProperties[WIREFRAME].cull_face_cw = false;
  stateProperties[WIREFRAME].stencil = false;
  stateProperties[WIREFRAME].fog = true;

  stateProperties[CHARACTERS].alpha_test = false;
  stateProperties[CHARACTERS].blend = false;
  stateProperties[CHARACTERS].lighting = checkState(RENDER_LIGHTING);
  stateProperties[CHARACTERS].textures = false;
  stateProperties[CHARACTERS].colour_material = false;
  stateProperties[CHARACTERS].depth_test = true;
  stateProperties[CHARACTERS].cull_face = false;
  stateProperties[CHARACTERS].cull_face_cw = false;
  stateProperties[CHARACTERS].stencil = false;
  stateProperties[CHARACTERS].fog = true;

  stateProperties[MODELS].alpha_test = false;
  stateProperties[MODELS].blend = false;
  stateProperties[MODELS].lighting = checkState(RENDER_LIGHTING);
  stateProperties[MODELS].textures = checkState(RENDER_TEXTURES);
  stateProperties[MODELS].colour_material = !checkState(RENDER_LIGHTING);
  stateProperties[MODELS].depth_test = true;
  stateProperties[MODELS].cull_face = true;
  stateProperties[MODELS].cull_face_cw = true;
  stateProperties[MODELS].stencil = false;
  stateProperties[MODELS].fog = true;

  stateProperties[BILLBOARD].alpha_test = true;
  stateProperties[BILLBOARD].blend = false;
  stateProperties[BILLBOARD].lighting = checkState(RENDER_LIGHTING);
  stateProperties[BILLBOARD].textures = checkState(RENDER_TEXTURES);
  stateProperties[BILLBOARD].colour_material = !checkState(RENDER_LIGHTING);
  stateProperties[BILLBOARD].depth_test = true;
  stateProperties[BILLBOARD].cull_face = false;
  stateProperties[BILLBOARD].cull_face_cw = false;
  stateProperties[BILLBOARD].stencil = false;
  stateProperties[BILLBOARD].fog = true;

  stateProperties[FONT].alpha_test = true;
  stateProperties[FONT].blend = false;
  stateProperties[FONT].lighting = false;
  stateProperties[FONT].textures = true;
  stateProperties[FONT].colour_material = false;
  stateProperties[FONT].depth_test = false;
  stateProperties[FONT].cull_face = false;
  stateProperties[FONT].cull_face_cw = false;
  stateProperties[FONT].stencil = false;
  stateProperties[FONT].fog = false;

  stateProperties[PANEL].alpha_test = false;
  stateProperties[PANEL].blend = true;
  stateProperties[PANEL].lighting = false;
  stateProperties[PANEL].textures = checkState(RENDER_TEXTURES);
  stateProperties[PANEL].colour_material = false;
  stateProperties[PANEL].depth_test = false;
  stateProperties[PANEL].cull_face = false;
  stateProperties[PANEL].cull_face_cw = false;
  stateProperties[PANEL].stencil = false;
  stateProperties[PANEL].fog = false;

  stateProperties[SELECT].alpha_test = true;
  stateProperties[SELECT].blend = false;
  stateProperties[SELECT].lighting = false;
  stateProperties[SELECT].textures = true;
  stateProperties[SELECT].colour_material = true;
  stateProperties[SELECT].depth_test = true;
  stateProperties[SELECT].cull_face = false;
  stateProperties[SELECT].cull_face_cw = false;
  stateProperties[SELECT].stencil = false;
  stateProperties[SELECT].fog = false;

  stateProperties[HALO].alpha_test = false;
  stateProperties[HALO].blend = false;
  stateProperties[HALO].lighting = false;
  stateProperties[HALO].textures = false;
  stateProperties[HALO].colour_material = false;
  stateProperties[HALO].depth_test = true;
  stateProperties[HALO].cull_face = false;
  stateProperties[HALO].cull_face_cw = false;
  stateProperties[HALO].stencil = false;
  stateProperties[HALO].fog = true;
  
  _states = glGenLists(LAST_CHANGE);
  stateDisplayList(_states + SKYBOX_TO_TERRAIN, SKYBOX, TERRAIN);         // 0
  stateDisplayList(_states + TERRAIN_TO_WIREFRAME, TERRAIN, WIREFRAME); // 1
  
  _states = glGenLists(LAST_CHANGE);
  stateDisplayList(_states + SKYBOX_TO_TERRAIN, SKYBOX, TERRAIN);         // 0
  stateDisplayList(_states + TERRAIN_TO_WIREFRAME, TERRAIN, WIREFRAME); // 1
  stateDisplayList(_states + WIREFRAME_TO_CHARACTERS, WIREFRAME, CHARACTERS); // 1
  stateDisplayList(_states + CHARACTERS_TO_MODELS, CHARACTERS, MODELS);   // 2
  stateDisplayList(_states + MODELS_TO_BILLBOARD, MODELS, BILLBOARD);     // 3
  stateDisplayList(_states + BILLBOARD_TO_FONT, BILLBOARD, FONT);         // 4
  stateDisplayList(_states + FONT_TO_PANEL, FONT, PANEL);                 // 5
  stateDisplayList(_states + PANEL_TO_FONT, PANEL, FONT);                 // 6
  stateDisplayList(_states + FONT_TO_SKYBOX, FONT, SKYBOX);               // 7

  _current_state = FONT_TO_PANEL;
  stateChange(FONT);
}

void GL_Render::nextState(int desired_state) {
  if (desired_state == _current_state) return;
  if ((_current_state == BILLBOARD_TO_FONT) && (desired_state == FONT_TO_SKYBOX)) _current_state = PANEL_TO_FONT; // Skip states if required!
  if ((_current_state == PANEL_TO_FONT) && (desired_state == FONT_TO_PANEL)) _current_state = BILLBOARD_TO_FONT; // Skip states if required!
  _current_state++;
  if (_current_state >= LAST_CHANGE) _current_state = 0;
  if (_current_state != desired_state) {
    // Do not want to be here if at all possible
    // Force change to required state
    switch(desired_state) {
      case FONT_TO_SKYBOX: stateChange(SKYBOX); break;
      case SKYBOX_TO_TERRAIN: stateChange(TERRAIN); break;
      case TERRAIN_TO_WIREFRAME: stateChange(WIREFRAME); break;
      case WIREFRAME_TO_CHARACTERS: stateChange(CHARACTERS); break;
      case CHARACTERS_TO_MODELS: stateChange(MODELS); break;
      case MODELS_TO_BILLBOARD: stateChange(BILLBOARD); break;
      case BILLBOARD_TO_FONT: stateChange(FONT); break;
      case FONT_TO_PANEL: stateChange(PANEL); break;
      case PANEL_TO_FONT: stateChange(FONT); break;
    }
    _current_state = desired_state;  
  } else glCallList(_states + _current_state);
}

bool GL_Render::SphereInFrustum( WorldEntity *we) {
//  return true;	
  int p;
  float x, y, z, radius;
  WFMath::AxisBox<3> bbox = bboxCheck(we->getBBox());
  WFMath::Ball<3> b = bbox.boundingSphere();
  x = b.getCenter().x() + we->getAbsPos().x();
  y = b.getCenter().y() + we->getAbsPos().y();
  z = b.getCenter().z() + we->getAbsPos().z() + terrain->getHeight(we->getAbsPos().x(), we->getAbsPos().y());
  radius = b.radius();
  for( p = 0; p < 6; p++ )
    if( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= -radius )
      return false;
    return true;
}

void GL_Render::drawWireFrameQueue(bool select_mode) {
  for (std::map<std::string, Queue>::const_iterator I = wireframe_queue.begin(); I != wireframe_queue.end(); I++) {
    WorldEntity *we = (WorldEntity *)*(I->second.begin());
    std::string type = we->getType()->getName();
    for (Queue::const_iterator J = I->second.begin(); J != I->second.end(); J++) {
      WorldEntity *we = (WorldEntity*)*J;
      if (select_mode) nextColour(we->getID());
      else glColor3f(1.0f, 1.0f, 1.0f);
      glPushMatrix();
      WFMath::Point<3> pos = we->getAbsPos();
      glTranslatef(pos.x(), pos.y(), pos.z() + terrain->getHeight(pos.x(), pos.y()));
      WFMath::Quaternion q = we->getAbsOrient();
      float o[4][4];
      QuatToMatrix(q, o);
      Models *wireframe = NULL;
      ModelStruct *ms;
      ms = _entity_models[we->getID()];
      if (ms) {
        wireframe = (WireFrame*)ms->models;
      } else {
        WFMath::AxisBox<3> bbox = we->getBBox();
        if (!we->hasBBox()) {
          WFMath::Point<3> lc = WFMath::Point<3>(0.0f, 0.0f, 0.0f);
          WFMath::Point<3> hc = WFMath::Point<3>(1.0f, 1.0f, 1.0f);
          bbox = WFMath::AxisBox<3>(lc, hc);
        }
        bbox = bboxCheck(bbox);
	wireframe = new WireFrame(bbox);
        ms = new ModelStruct();
        ms->model_name = "wireframe";
        ms->in_use = true;
        if ((wireframe) && wireframe->init()) {
          _entity_models[we->getID()] = ms;
        } else {
          if (wireframe) delete wireframe;
          wireframe = NULL;
	  // CANNOT DRAW ENTITY
        }
        ms->model = NULL;
        ms->models = wireframe;
      }
      if (!select_mode && we->getID() == activeID) {
	active_name = we->getName();
	glColor4fv(_halo_blend_colour);
	if (checkState(RENDER_STENCIL)) {
          glEnable(GL_STENCIL_TEST);
          glStencilFunc(GL_ALWAYS, -1,1);
          glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
          drawModel(wireframe);
          glStencilFunc(GL_NOTEQUAL, -1,1);
          drawModel(wireframe);
          glDisable(GL_STENCIL_TEST);
	} else {
          drawModel(wireframe);
	  glColor4fv(white);
	}
      } else {
        drawModel(wireframe);
      }
      glPopMatrix();
    }
  }
}

void GL_Render::drawWireFrame(WorldEntity *we) {
  WFMath::AxisBox<3> bbox = we->getBBox();
  if (!we->hasBBox()) {
    WFMath::Point<3> lc = WFMath::Point<3>(0.0f, 0.0f, 0.0f);
    WFMath::Point<3> hc = WFMath::Point<3>(1.0f, 1.0f, 1.0f);
    bbox = WFMath::AxisBox<3>(lc, hc);
  }
  bbox = bboxCheck(bbox);
  glBegin(GL_LINES);
      glVertex3f(bbox.lowCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z());
      glVertex3f(bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z());

      glVertex3f(bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z());
      glVertex3f(bbox.highCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z());

      glVertex3f(bbox.highCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z());
      glVertex3f(bbox.highCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z());

      glVertex3f(bbox.highCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z());
      glVertex3f(bbox.lowCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z());
      //Top
      glVertex3f(bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z());
      glVertex3f(bbox.lowCorner().x(), bbox.highCorner().y(), bbox.highCorner().z());

      glVertex3f(bbox.lowCorner().x(), bbox.highCorner().y(), bbox.highCorner().z());
      glVertex3f(bbox.highCorner().x(), bbox.highCorner().y(), bbox.highCorner().z());

      glVertex3f(bbox.highCorner().x(), bbox.highCorner().y(), bbox.highCorner().z());
      glVertex3f(bbox.highCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z());

      glVertex3f(bbox.highCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z());
      glVertex3f(bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z());

      //Verticals
      glVertex3f(bbox.lowCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z());
      glVertex3f(bbox.lowCorner().x(), bbox.highCorner().y(), bbox.highCorner().z());

      glVertex3f(bbox.lowCorner().x(), bbox.highCorner().y(), bbox.highCorner().z());
      glVertex3f(bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z());

      glVertex3f(bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z());
      glVertex3f(bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z());

      glVertex3f(bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z());
      glVertex3f(bbox.lowCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z());

      
      glVertex3f(bbox.highCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z());
      glVertex3f(bbox.highCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z());
      
      glVertex3f(bbox.highCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z());
      glVertex3f(bbox.highCorner().x(), bbox.highCorner().y(), bbox.highCorner().z());
      
      glVertex3f(bbox.highCorner().x(), bbox.highCorner().y(), bbox.highCorner().z());
      glVertex3f(bbox.highCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z());
      
      glVertex3f(bbox.highCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z());
      glVertex3f(bbox.highCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z());
 

      
      glVertex3f(bbox.highCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z());
      glVertex3f(bbox.highCorner().x(), bbox.highCorner().y(), bbox.highCorner().z());
     
      glVertex3f(bbox.highCorner().x(), bbox.highCorner().y(), bbox.highCorner().z());
      glVertex3f(bbox.lowCorner().x(), bbox.highCorner().y(), bbox.highCorner().z());
     
      glVertex3f(bbox.lowCorner().x(), bbox.highCorner().y(), bbox.highCorner().z());
      glVertex3f(bbox.lowCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z());
     
      glVertex3f(bbox.lowCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z());
      glVertex3f(bbox.highCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z());


      glVertex3f(bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z());
      glVertex3f(bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z());

      glVertex3f(bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z());
      glVertex3f(bbox.highCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z());

      glVertex3f(bbox.highCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z());
      glVertex3f(bbox.highCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z());

      glVertex3f(bbox.highCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z());
      glVertex3f(bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z());
      
  glEnd();
}



void GL_Render::drawModel(Models *model) {
  bool textures = checkState(RENDER_TEXTURES);
  bool lighting = checkState(RENDER_LIGHTING);
  if (!model->hasVertexData()) return; //Error
  glVertexPointer(3, GL_FLOAT, 0, model->getVertexData());
  glEnableClientState(GL_VERTEX_ARRAY);
  if (textures && model->hasTextureData()) {
    glTexCoordPointer(2, GL_FLOAT, 0, model->getTextureData());
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }
  if (lighting && model->hasNormalData()) {
    glNormalPointer(GL_FLOAT, 0, model->getNormalData());
    glEnableClientState(GL_NORMAL_ARRAY);
  }
  switch (model->getType()) {
    case (Models::INVALID): break;
    case (Models::LINES): glDrawArrays(GL_LINES, 0, model->getNumPoints()); break;
    case (Models::QUADS): glDrawArrays(GL_QUADS, 0, model->getNumPoints()); break;
  }
  glDisableClientState(GL_VERTEX_ARRAY);
  if (lighting && model->hasNormalData()) glDisableClientState(GL_NORMAL_ARRAY);
  if (textures && model->hasTextureData()) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}



void GL_Render::readComponentConfig() {
  if (camera) camera->readConfig();
  if (terrain)terrain->readConfig();
}

} /* namespace Sear */
