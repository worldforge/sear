// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

/*TODO
 * Allow texture unloading
 * Allow priority textures
 *
 *
 */ 


#include "GL.h"

#include <GL/glu.h>

#include "../src/Camera.h"
#include "../src/System.h"
#include "../src/Config.h"
#include "../src/Terrain.h"
#include "../src/Sky.h"
#include "../src/Camera.h"
#include "../src/WorldEntity.h"
#include "../src/Console.h"
#include "../src/Utility.h"
#include <Eris/Entity.h>
#include <Eris/World.h>
#include <wfmath/quaternion.h>
#include <wfmath/vector.h>
#include "../src/Character.h"
#include "../src/ObjectLoader.h"

#include "../src/Frustum.h"

#include "../src/Models.h"
#include <unistd.h>

#include "../src/ModelHandler.h"

#include "../src/Log.h"

#include "../terrain/ROAM.h"
#include "../sky/SkyBox.h"

namespace Sear {

float GL::_halo_blend_colour[4] = {1.0f, 0.0f, 1.0f, 0.4f};
float GL::_halo_colour[3] = {1.0f, 0.0f, 1.0f};

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
  
void GL::buildColourSet() {
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

GL *GL::_instance = NULL;

GL::GL() :
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

GL::GL(System *system) :
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

GL::~GL() {
  writeConfig();
  shutdownFont();

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

void GL::initWindow(int width, int height) {
  Log::writeLog("Render: Initilising Renderer", Log::DEFAULT);
  // TODO: put this into an info method 
  Log::writeLog(std::string("GL_VENDER: ") + string_fmt(glGetString(GL_VENDOR)), Log::DEFAULT);
  Log::writeLog(std::string("GL_RENDERER: ") + string_fmt(glGetString(GL_RENDERER)), Log::DEFAULT);
  Log::writeLog(std::string("GL_VERSION: ") + string_fmt(glGetString(GL_VERSION)), Log::DEFAULT);
  Log::writeLog(std::string("GL_EXTENSIONS: ") + string_fmt(glGetString(GL_EXTENSIONS)), Log::DEFAULT);
  
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
}
  
void GL::init() {
  // Most of this should be elsewhere
  readConfig();
  splash_id = requestTexture(splash_texture);
  initFont();
  // TODO: initialisation need to go into system
  Log::writeLog("Initialising Terrain", Log::DEFAULT);
  terrain = new ROAM(_system, this);
  if (!terrain->init()) {
    Log::writeLog("Error initialising Terrain. Suggest Restart!", Log::ERROR);
  }
  skybox = new SkyBox(_system, this);
  if (!skybox->init()) {
    Log::writeLog("Render: Error - Could not initialise Sky Box", Log::ERROR);
  }
  camera = new Camera();
  camera->init();
  camera->registerCommands(_system->getConsole());
//  CheckError();
  initLighting();
  mh = _system->getModelHandler();

  _state_loader = _system->getStateLoader();
  setupStates();

}

void GL::initLighting() {
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

void GL::initFont() {
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

void GL::shutdownFont() {
  Log::writeLog("Render: Shutting down fonts", Log::DEFAULT);
  glDeleteLists(base,256); // Delete All 256 Display Lists
}

void GL::print(GLint x, GLint y, const char * string, int set) {
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

void GL::print3D(const char *string, int set) {
  if (set > 1) set = 1;
  glBindTexture(GL_TEXTURE_2D, getTextureID(font_id)); // Select Our Font Texture
  glPushMatrix(); // Store The Projection Matrix
  glListBase(base-32+(128*set)); // Choose The Font Set (0 or 1)
  glCallLists(strlen(string),GL_BYTE,string); // Write The Text To The Screen
  glPopMatrix(); // Restore The Old Projection Matrix
}

void GL::newLine() {
  glTranslatef(0.0f,  ( FONT_HEIGHT) , 0.0f);
}

int GL::requestTexture(const std::string &texture_name, bool clamp) {
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

int GL::requestMipMap(const std::string &texture_name, bool clamp) {
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

int GL::requestTextureMask(const std::string &texture_name, bool clamp) {
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

void GL::createTexture(SDL_Surface *surface, unsigned int texture, bool clamp) {
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
  glTexImage2D(GL_TEXTURE_2D, 0, (surface->format->BytesPerPixel == 3) ? GL_RGB : GL_RGBA, surface->w, surface->h, 0, (surface->format->BytesPerPixel == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
}
 
void GL::createMipMap(SDL_Surface *surface, unsigned int texture, bool clamp)  {
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

void GL::createTextureMask(SDL_Surface *surface, unsigned int texture, bool clamp) {
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

inline GLuint GL::getTextureID(int texture_id) {
  int i;
  std::list<GLuint>::const_iterator I = textureList.begin();
  for (i = 1; i < texture_id; i++, I++);
  return *I;
}

void GL::drawScene(const std::string& command, bool select_mode) {
	
  // TODO: Move to generic render class
//select_mode = true;
  if (select_mode) resetColors();
  active_name = "";
  // This clears the currently loaded texture
  if (select_mode) glBindTexture(GL_TEXTURE_2D, 0);
 // Calculate in system 
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
  if (select_mode) stateChange("select"); //SELECT
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
//          nextState(FONT_TO_SKYBOX);
//	  stateChange("skybox");
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

//      if (_entity_models[id]) player_model = _entity_models[id]->model;
      float  proj[16];
      float  modl[16];
      /* Get the current PROJECTION matrix from OpenGL */
      glGetFloatv( GL_PROJECTION_MATRIX, proj );
                              
      /* Get the current MODELVIEW matrix from OpenGL */
      glGetFloatv( GL_MODELVIEW_MATRIX, modl );
      
      Frustum::getFrustum(frustum, proj, modl); 
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
//        nextState(SKYBOX_TO_TERRAIN);
//	  stateChange("terrain");
        terrain->draw();
      glPopMatrix();
    }
// Draw known entities
    WorldEntity *root = NULL; 
    if ((root = (WorldEntity *)world->getRootEntity())) {
      if (_character) _character->updateLocals(false);
      render_queue = std::map<std::string, Queue>();
      buildQueues(root, 0);
      drawQueue(render_queue, select_mode, time_elapsed);
      // TODO: make into rasieDetail and lowerDetail methods
      if (!select_mode) {
        if (frame_rate < _lower_frame_rate_bound) {
          model_detail -= 0.1f;
          if (model_detail < 0.0f) model_detail = 0.0f;
          terrain->lowerDetail();
        } else if (frame_rate > _upper_frame_rate_bound) {
          model_detail += 0.05f;
          if (model_detail > 1.0f) model_detail = 1.0f;
          terrain->raiseDetail();
        }
      }
    }
  } else {
    #ifndef _WIN32
      // TODO Need to find a win32 version
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
  stateChange("console");
if (!select_mode) _system->getConsole()->draw(command);
//  nextState(PANEL_TO_FONT);
//  Calc FPS
  stateChange("font");
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
  
  if (!select_mode) SDL_GL_SwapBuffers();

//  CheckError();
}

void GL::buildQueues(WorldEntity *we, int depth) {
  if (depth == 0 || we->isVisible()) {
    if (we->getType() != NULL) {
      ObjectProperties *op = we->getObjectProperties();
      if (!op) {
        std::string type = we->type();
        std::string parent = we->parent();
        ObjectLoader *ol = _system->getObjectLoader();
        if (!type.empty()) op = ol->getObjectProperties(type);
        if (op == NULL && !parent.empty()) op = ol->getObjectProperties(parent);
        if (op == NULL) op = ol->getObjectProperties("default");
	we->setObjectProperties(op);
      }
      
      std::string model_type = std::string(op->model_type);

      if (op->draw_self && Frustum::sphereInFrustum(frustum, we, terrain)) render_queue[op->state].push_back(we);
      if (op->draw_members) {
        for (unsigned int i = 0; i < we->getNumMembers(); i++) {
          buildQueues((WorldEntity*)we->getMember(i), depth + 1);
        }
      }
    }
  }
}

void GL::stateChange(const std::string &state) {
  stateChange(_state_loader->getStateProperties(state));
}

void GL::stateChange(StateProperties *sp) {
  if (!sp) {
    return;
    // throw Exception("StateProperties is NULL");
  }
  if (_cur_state == sp) return;
  if (_cur_state) {
    std::string change = std::string(_cur_state->state) + std::string("_to_") + std::string(sp->state);
    GLuint list = _state_map[change];
    if (!glIsList(list)) stateDisplayList(list, _cur_state, sp);
    glCallList(list);
    _cur_state = sp;
  } else { 
    if (sp->alpha_test) glEnable(GL_ALPHA_TEST);
    else glDisable(GL_ALPHA_TEST);
    if (sp->blend) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
    if (sp->lighting && checkState(RENDER_LIGHTING)) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);
    if (sp->textures && checkState(RENDER_TEXTURES)) glEnable(GL_TEXTURE_2D);
    else glDisable(GL_TEXTURE_2D);
    if (sp->colour_material) glEnable(GL_COLOR_MATERIAL);
    else glDisable(GL_COLOR_MATERIAL);
    if (sp->depth_test) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
    if (sp->cull_face) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
    if (sp->cull_face_cw) glFrontFace(GL_CW);
    else glFrontFace(GL_CCW);
    if (sp->stencil) glEnable(GL_STENCIL_TEST);
    else glDisable(GL_STENCIL_TEST);
    if (sp->fog) glEnable(GL_FOG);
    else glDisable(GL_FOG);
  }
  _current_state = std::string(sp->state);
  
}


void GL::drawTextRect(GLint x, GLint y, GLint width, GLint height, int texture) {
  // TODO should use switchTexture
  glBindTexture(GL_TEXTURE_2D, getTextureID(texture));

  glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
  glPushMatrix(); // Store The Projection Matrix
  glLoadIdentity(); // Reset The Projection Matrix
  glOrtho(0,window_width,0,window_height,-1,1); // Set Up An Ortho Screen
  glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
  glPushMatrix(); // Store The Modelview Matrix
  glLoadIdentity(); // Reset The Modelview Matrix
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

  glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
  glPopMatrix(); // Restore The Old Projection Matrix
  glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
  glPopMatrix(); // Restore The Old Projection Matrix

}

void GL::procEvent(int x, int y) {
  unsigned int ic;
  std::string selected_id;
  GLubyte i[3];
  stateChange("select"); // SELECT
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
  stateChange("font"); // FONT
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
  Log::writeLog(msg, Log::ERROR);
}

//TODO should be in general render class
void GL::readConfig() {
  std::string temp;
  Config *general = _system->getGeneral();
  Log::writeLog("Loading Renderer Config", Log::DEFAULT);
  if (!general) {
    Log::writeLog("GL: Error - General config object does not exist!", Log::ERROR);
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

void GL::writeConfig() {
  Config *general = _system->getGeneral();
  if (!general) {
    Log::writeLog("GL: Error - General config object does not exist!", Log::ERROR);
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


// THIS BUILDS A STATE TRANSITION LIST
void GL::stateDisplayList(GLuint &list, StateProperties *previous_state, StateProperties *next_state) {
  // TODO: if textures are disabled then disable them here too
  glNewList(list, GL_COMPILE);
  if (previous_state->alpha_test != next_state->alpha_test) {
    if (next_state->alpha_test) glEnable(GL_ALPHA_TEST);
    else glDisable(GL_ALPHA_TEST);
  }
  if (previous_state->blend != next_state->blend) {
    if (next_state->blend) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
  }
  if (previous_state->lighting != next_state->lighting) {
    if (next_state->lighting && checkState(RENDER_LIGHTING)) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);
  }
  if (previous_state->textures != next_state->textures) {
    if (next_state->textures && checkState(RENDER_TEXTURES)) glEnable(GL_TEXTURE_2D);
    else glDisable(GL_TEXTURE_2D);
  }
  if (previous_state->colour_material != next_state->colour_material) {
    if (next_state->colour_material) glEnable(GL_COLOR_MATERIAL);
    else glDisable(GL_COLOR_MATERIAL);
  }
  if (previous_state->depth_test != next_state->depth_test) {
    if (next_state->depth_test) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
  }
  if (previous_state->cull_face != next_state->cull_face) {
    if (next_state->cull_face) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
    if (next_state->cull_face_cw) glFrontFace(GL_CW);
    else glFrontFace(GL_CCW);
  }
  if (previous_state->stencil != next_state->stencil) {
    if (next_state->stencil) glEnable(GL_STENCIL_TEST);
    else glDisable(GL_STENCIL_TEST);
  }
  if (previous_state->fog != next_state->fog) {
    if (next_state->fog) glEnable(GL_FOG);
    else glDisable(GL_FOG);
  }
  glEndList();
}


void GL::setupStates() {
  // TODO: should this be in the init?
  glAlphaFunc(GL_GREATER, 0.1f);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  GLfloat fog_colour[] = {0.50f, 0.50f, 0.50f, 0.50f};
  glFogfv(GL_FOG_COLOR, fog_colour);
  glFogf(GL_FOG_START, _fog_start);
  glFogf(GL_FOG_END, _fog_end);

//  stateChange("font"); //font
}

void GL::readComponentConfig() {
  if (camera) camera->readConfig();
  if (terrain) terrain->readConfig();
}

void GL::writeComponentConfig() {
  if (camera) camera->writeConfig();
  if (terrain) terrain->writeConfig();
}
void GL::translateObject(float x, float y, float z) {
  glTranslatef(x, y, z);
}

void GL::rotateObject(WorldEntity *we, int type) {
  if (!we) return; // THROW ERROR;
  switch (type) {
    case Models::NONE: return; break;
    case Models::POSITION: {
       WFMath::Point<3> pos = we->getPosition();
       glRotatef(pos.x() + pos.y() + pos.z(), 0.0f, 0.0f, 1.0f);
       break;
    }       
    case Models::NORMAL: {
      WFMath::Quaternion q = we->getAbsOrient();
      float rotation_matrix[4][4];
      QuatToMatrix(q, rotation_matrix);
      glMultMatrixf(&rotation_matrix[0][0]);
      break;
    }
    case Models::BILLBOARD: // Same as HALO, but does not rotate with camera elevation
    case Models::HALO: {
      float rotation_matrix[4][4];
      WFMath::Quaternion  orient2 = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Initial Camera rotation
      orient2 /= orient;
      QuatToMatrix(orient2, rotation_matrix); //Get the rotation matrix for base rotation
      glMultMatrixf(&rotation_matrix[0][0]); //Apply rotation matrix
      break;
    }
  }
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
      break;
    }
    case ORTHOGRAPHIC: {
      glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
      glLoadIdentity(); // Reset The Projection Matrix
      glOrtho(0, window_width, 0 , window_height, -1, 1); // Set Up An Ortho Screen
      glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
      break;
    }			    
    case ISOMETRIC: {
      break;
    }			    
  }	
}

void GL::setMaterial(float *ambient, float *diffuse, float *specular, float shininess, float *emissive) {
  // TODO: set up missing values
  if (ambient)           glMaterialfv (GL_FRONT, GL_AMBIENT,   ambient);
  if (diffuse)           glMaterialfv (GL_FRONT, GL_DIFFUSE,   diffuse);
  if (specular)          glMaterialfv (GL_FRONT, GL_SPECULAR,  specular);
  if (shininess >= 0.0f) glMaterialf  (GL_FRONT, GL_SHININESS, shininess);
  if (emissive)          glMaterialfv (GL_FRONT, GL_EMISSION,  emissive);
  else                   glMaterialfv (GL_FRONT, GL_EMISSION,  black);
}

void GL::renderArrays(unsigned int type, unsigned int offset, unsigned int number_of_points, float *vertex_data, float *texture_data, float *normal_data) {
  // TODO: Reduce ClientState switches
  bool textures = checkState(RENDER_TEXTURES);
  bool lighting = checkState(RENDER_LIGHTING);
 
  if (!vertex_data) {
	  Log::writeLog("No Vertex Data", Log::ERROR);
	  return; //throw Exception(""); 
  }
  glVertexPointer(3, GL_FLOAT, 0, vertex_data);
  glEnableClientState(GL_VERTEX_ARRAY);
  if (textures && texture_data) {
    glTexCoordPointer(2, GL_FLOAT, 0, texture_data);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }
  if (lighting && normal_data) {
    glNormalPointer(GL_FLOAT, 0, normal_data);
    glEnableClientState(GL_NORMAL_ARRAY);
  }

  switch (type) {
    case (Models::INVALID): Log::writeLog("Trying to render INVALID type", Log::ERROR); break;
    case (Models::POINT): glDrawArrays(GL_POINT, offset, number_of_points); break;
    case (Models::LINES): glDrawArrays(GL_LINES, offset, number_of_points); break;
    case (Models::TRIANGLES): glDrawArrays(GL_TRIANGLES, offset, number_of_points); break;
    case (Models::QUADS): glDrawArrays(GL_QUADS, offset, number_of_points); break;
    case (Models::TRIANGLE_FAN): glDrawArrays(GL_TRIANGLE_FAN, offset, number_of_points); break;
    case (Models::TRIANGLE_STRIP): glDrawArrays(GL_TRIANGLE_STRIP, offset, number_of_points); break;
    case (Models::QUAD_STRIP): glDrawArrays(GL_QUAD_STRIP, offset, number_of_points); break;
    default: Log::writeLog("Unknown type", Log::ERROR); break;
  }
 
  glDisableClientState(GL_VERTEX_ARRAY);
  if (lighting && normal_data) glDisableClientState(GL_NORMAL_ARRAY);
  if (textures && texture_data) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void GL::renderElements(unsigned int type, unsigned int number_of_points, int *faces_data, float *vertex_data, float *texture_data, float *normal_data) {
  // TODO: Reduce ClientState switches
  bool textures = checkState(RENDER_TEXTURES);
  bool lighting = checkState(RENDER_LIGHTING);
 
  if (!vertex_data) return; //throw Exception(""); 
  glVertexPointer(3, GL_FLOAT, 0, vertex_data);
  glEnableClientState(GL_VERTEX_ARRAY);
  if (textures && texture_data) {
    glTexCoordPointer(2, GL_FLOAT, 0, texture_data);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }
  if (lighting && normal_data) {
    glNormalPointer(GL_FLOAT, 0, normal_data);
    glEnableClientState(GL_NORMAL_ARRAY);
  }

  switch (type) {
    case (Models::INVALID): Log::writeLog("Trying to render INVALID type", Log::ERROR); break;
    case (Models::POINT): glDrawElements(GL_POINT, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Models::LINES): glDrawElements(GL_LINES, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Models::TRIANGLES): glDrawElements(GL_TRIANGLES, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Models::QUADS): glDrawElements(GL_QUADS, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Models::TRIANGLE_FAN): glDrawElements(GL_TRIANGLE_FAN, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Models::TRIANGLE_STRIP): glDrawElements(GL_TRIANGLE_STRIP, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Models::QUAD_STRIP): glDrawElements(GL_QUAD_STRIP, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    default: Log::writeLog("Unknown type", Log::ERROR); break;
  }
 
  glDisableClientState(GL_VERTEX_ARRAY);
  if (lighting && normal_data) glDisableClientState(GL_NORMAL_ARRAY);
  if (textures && texture_data) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

unsigned int GL::createTexture(unsigned int width, unsigned int height, unsigned int depth, unsigned char *data, bool clamp) {
  unsigned int texture = 0;
 glGenTextures(1, &texture);
  // TODO: Check for valid texture generation and return error
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
  glTexImage2D(GL_TEXTURE_2D, 0, (depth == 3) ? GL_RGB : GL_RGBA, width, height, 0, (depth == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
  return texture;
}

void GL::drawQueue(std::map<std::string, Queue> queue, bool select_mode, float time_elapsed) {
  if (select_mode) stateChange("select");
  for (std::map<std::string, Queue>::const_iterator I = queue.begin(); I != queue.end(); I++) {
    // Change staet for this queue
    if (!select_mode) stateChange(_state_loader->getStateProperties((std::string)I->first));
    for (Queue::const_iterator J = I->second.begin(); J != I->second.end(); J++) {

      WorldEntity *we = (WorldEntity *)*J;
      std::string type = we->type();
      // Get model
      Models *model = mh->getModel(we);
      if (!model) {  // ERROR GETTING MODEL
	Log::writeLog("Trying to render NULL model", Log::ERROR);
        continue;
      }
      
      glPushMatrix();

      // Translate Model
      WFMath::Point<3> pos = we->getAbsPos();
      translateObject(pos.x(), pos.y(), pos.z() + terrain->getHeight(pos.x(), pos.y()));
     
      // Rotate Model
      // TODO: Is this broken or is it to do with the server?
      if (model->rotationStyle()) rotateObject(we, model->rotationStyle());

      // Scale Object
      float scale = model->getScale();
      glScalef(scale, scale, scale);

      // Update Model
      model->update(time_elapsed);
      
      // Draw Model
      if (select_mode) {
        nextColour(we->getID());
	model->render(true);
      } else {
        if (we->getID() == activeID) drawOutline(we, model, checkState(RENDER_STENCIL) && model->getFlag("outline"));
	else model->render(false);
      }
      
      glPopMatrix();
    }
  }
}

 
float GL::distFromNear(float x, float y, float z) {
  return Frustum::distFromNear(frustum, x, y, z);
}
	
int GL::patchInFrustum(WFMath::AxisBox<3> bbox) {
  return Frustum::patchInFrustum(frustum, bbox);
}

void GL::drawOutline(WorldEntity *we, Models *model, bool use_stencil) {
  active_name = we->getName();
  StateProperties *sp = _cur_state;
  if (use_stencil) {
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, -1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glPushMatrix();
    model->render(false);
    glPopMatrix();
    stateChange("halo");
    glStencilFunc(GL_NOTEQUAL, -1, 1);
    glColor4fv(_halo_blend_colour);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    model->render(true);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_STENCIL_TEST);
    glColor4fv(white);
  } else {
    glColor4fv(_halo_blend_colour);  
    model->render(true);
    glColor4fv(white);
  }
  stateChange(sp);
}
  
} /* namespace Sear */
