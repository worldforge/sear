// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2008 Simon Goodall, University of Southampton

// $Id: GL.cpp,v 1.174 2008-10-07 19:20:29 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <SDL/SDL.h>

#ifdef __APPLE__
    #include <SDL_image/SDL_image.h>
#else
    #include <SDL/SDL_image.h>
#endif

#include <sigc++/object_slot.h>


#include <sage/sage.h>
#include <sage/GL.h>
#include <sage/GLU.h>

#include <unistd.h>

#include <varconf/config.h>
#include <wfmath/quaternion.h>
#include <wfmath/vector.h>
#include <Eris/Entity.h>

#include "RenderSystem.h"

#include "common/Log.h"
#include "common/Utility.h"

#include "src/Calendar.h"
#include "Camera.h"
#include "CameraSystem.h"
#include "src/Console.h"
#include "Frustum.h"
#include "Graphics.h"
#include "loaders/ModelSystem.h"
#include "loaders/Model.h"
#include "loaders/ModelHandler.h"
#include "loaders/ModelRecord.h"
#include "loaders/ObjectRecord.h"
#include "src/System.h"
#include "src/WorldEntity.h"

#include "GL.h"

#include "common/Mesh.h"

#include "src/sear_icon.xpm"


  // Consts
  static const int sleep_time = 10;

  static const std::string font_texture = "ui_font";
  static const std::string TEXTURE_splash_texture = "ui_splash_screen";
  
  // Config key strings
  
  static const std::string KEY_use_textures = "render_use_textures";
  static const std::string KEY_use_lighting = "render_use_lighting";
  static const std::string KEY_show_fps = "render_show_fps";
  static const std::string KEY_use_stencil = "render_use_stencil";
  static const std::string KEY_use_fsaa = "render_use_fsaa";
  static const std::string KEY_use_vbo = "render_use_vbo";

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
	
  static const std::string KEY_near_clip = "near_clip";
  static const std::string KEY_far_clip_dist = "far_clip_dist";
  static const std::string KEY_texture_scale = "texture_scale";
  

  static const std::string KEY_sdl_gl_red_size = "sdl_gl_red_size";
  static const std::string KEY_sdl_gl_green_size = "sdl_gl_green_size";
  static const std::string KEY_sdl_gl_blue_size = "sdl_gl_blue_size";
  static const std::string KEY_sdl_gl_alpha_size = "sdl_gl_alpha_size";

  static const std::string KEY_sdl_gl_doublebuffer = "sdl_gl_doublebuffer";
  static const std::string KEY_sdl_gl_buffer_size = "sdl_gl_buffer_size";
  static const std::string KEY_sdl_gl_depth_size = "sdl_gl_depth_size";
  static const std::string KEY_sdl_gl_stencil_size = "sdl_gl_stencil_size";
  static const std::string KEY_sdl_gl_stereo = "sdl_gl_stereo";
  static const std::string KEY_sdl_gl_swap_control = "sdl_gl_swap_control";
  static const std::string KEY_sdl_gl_accelerated_visual = "sdl_gl_accelerated_visual";

  static const std::string KEY_sdl_gl_accum_red_size = "sdl_gl_accum_red_size";
  static const std::string KEY_sdl_gl_accum_green_size = "sdl_gl_accum_green_size";
  static const std::string KEY_sdl_gl_accum_blue_size = "sdl_gl_accum_blue_size";
  static const std::string KEY_sdl_gl_accum_alpha_size = "sdl_gl_accum_alpha_size";

  static const std::string KEY_sdl_gl_multisamplesamples = "sdl_gl_multisamplesamples";
  static const std::string KEY_sdl_gl_multisamplebuffers = "sdl_gl_multisamplebuffers";

  // Default config values
  static const float DEFAULT_character_light_kc = 0.5f;
  static const float DEFAULT_character_light_kl = 0.2f;
  static const float DEFAULT_character_light_kq = 0.15f;

  static const float DEFAULT_character_light_ambient_red = 0.0f;
  static const float DEFAULT_character_light_ambient_green = 0.0f;
  static const float DEFAULT_character_light_ambient_blue = 0.0f;
  static const float DEFAULT_character_light_ambient_alpha = 0.0f;
  
  static const float DEFAULT_character_light_diffuse_red = 1.0f;
  static const float DEFAULT_character_light_diffuse_green = 1.0f;
  static const float DEFAULT_character_light_diffuse_blue = 0.9f;
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

  static const bool DEFAULT_use_textures = true;
  static const bool DEFAULT_use_lighting = true;
  static const bool DEFAULT_show_fps = true;
  static const bool DEFAULT_use_stencil = true;
  static const bool DEFAULT_use_fsaa = false;
  static const bool DEFAULT_use_vbo = true;

  static const float DEFAULT_lower_frame_rate_bound = 25.0f;
  static const float DEFAULT_upper_frame_rate_bound = 30.0f;

  static const float DEFAULT_speech_offset_x = 0.0f;
  static const float DEFAULT_speech_offset_y = 0.0f;
  static const float DEFAULT_speech_offset_z = 0.0f;

  static const float DEFAULT_fog_start = 100.0f;
  static const float DEFAULT_fog_end = 150.0f;
  static const float DEFAULT_near_clip = 0.1f;
  static const float DEFAULT_far_clip_dist = 1000.0f;
  static const float DEFAULT_texture_scale = 10.0f;

  static const int DEFAULT_sdl_gl_red_size = 5;
  static const int DEFAULT_sdl_gl_green_size = 6;
  static const int DEFAULT_sdl_gl_blue_size = 5;
  static const int DEFAULT_sdl_gl_alpha_size = 5;

  static const int DEFAULT_sdl_gl_doublebuffer = 1;
  static const int DEFAULT_sdl_gl_buffer_size = 16;
  static const int DEFAULT_sdl_gl_depth_size = 5;
  static const int DEFAULT_sdl_gl_stencil_size = 1;
  static const int DEFAULT_sdl_gl_stereo = 0;
  static const int DEFAULT_sdl_gl_swap_control = 1;
  static const int DEFAULT_sdl_gl_accelerated_visual = 1;

  static const int DEFAULT_sdl_gl_accum_red_size = 0;
  static const int DEFAULT_sdl_gl_accum_green_size = 0;
  static const int DEFAULT_sdl_gl_accum_blue_size = 0;
  static const int DEFAULT_sdl_gl_accum_alpha_size = 0;

  static const int DEFAULT_sdl_gl_multisamplesamples = 4;
  static const int DEFAULT_sdl_gl_multisamplebuffers = 1;

static bool use_ext_compiled_vertex_array = false;

static const std::string STATE_font = "font";
static const std::string STATE_splash = "splash";
static const std::string STATE_halo = "halo";
static const std::string STATE_default = "default";

static const std::string DEFAULT_FONT = "default_font";

static const std::string MASK = "_mask";
static const std::string RENDER = "render";
	
static const GLfloat halo_colour[4] = {1.0f, 0.0f, 1.0f, 1.0f};
static const GLfloat activeNameColour[] = { 1.0f, 0.75f, 0.2f, 1.0f};
static const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
//static GLfloat red[] =   { 1.0f, 0.0f, 0.0f, 1.0f };
static const GLfloat yellow[] =  { 0.0f, 1.0f, 1.0f, 1.0f };
static const GLfloat blue[] =  { 0.0f, 0.0f, 1.0f, 1.0f };
static const GLfloat blackLight[]    = { 0.0f,  0.0f, 0.0f, 1.0f };


#ifdef DEBUG
static const bool debug = true;
#else
static const bool debug = false;
#endif

namespace Sear {

bool GL::createWindow(unsigned int width, unsigned int height, bool fullscreen) {
  assert(m_screen == NULL);
  m_graphics = RenderSystem::getInstance().getGraphics();
  
  if (debug) printf("GL: Creating Window\n");
  // Set new window size etc..
  m_width = width;
  m_height = height;
  m_fullscreen = fullscreen;

  // TODO check return values
  SDL_InitSubSystem(SDL_INIT_VIDEO);
  SDL_EnableUNICODE(1);

  //Request Open GL window attributes
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, m_sdl_gl_red_size);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, m_sdl_gl_green_size);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, m_sdl_gl_blue_size);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, m_sdl_gl_alpha_size);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, m_sdl_gl_buffer_size);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, m_sdl_gl_doublebuffer);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, m_sdl_gl_depth_size);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, m_sdl_gl_accum_red_size);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, m_sdl_gl_accum_green_size);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, m_sdl_gl_accum_blue_size);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, m_sdl_gl_accum_alpha_size);
  // This should always be zero for sear...
  SDL_GL_SetAttribute(SDL_GL_STEREO, m_sdl_gl_stereo);
  SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, m_sdl_gl_swap_control);
 SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, m_sdl_gl_accelerated_visual);
#ifdef HAVE_SDL_GL_MULTISAMPLE
  if (m_use_fsaa) {
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, m_sdl_gl_multisamplesamples);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, m_sdl_gl_multisamplebuffers);
  }
#endif

  //RenderSystem::getInstance().setState(RenderSystem::RENDER_STENCIL,false);
  // Only request stencil if specified
  if (RenderSystem::getInstance().getState(RenderSystem::RENDER_STENCIL)) {
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, m_sdl_gl_stencil_size);
   // SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1 );
  }

  const SDL_VideoInfo *info = SDL_GetVideoInfo();
  if (!info) {
    Log::writeLog("Error quering video", Log::LOG_DEFAULT);
    return false;
  }                                                                                
  /* Check is there are any modes available */
  if (debug) {
    SDL_Rect **videoModes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
    if (videoModes == 0) {
      printf("No modes available!\n");
    } else if (videoModes == (SDL_Rect **)-1) {
      printf("All resolutions available.\n");
    } else{
      /* Print valid modes */
      printf("Available Modes\n");
      for(int i=0; videoModes[i]; ++i) {
        float aspect = (float)videoModes[i]->w / (float)videoModes[i]->h;
        printf("  %d x %d -- %f\n", videoModes[i]->w, videoModes[i]->h, aspect);
      }
    }
    fflush(stdout);
  }
  //Create Window
  int flags = SDL_OPENGL | SDL_RESIZABLE;
  int bpp = info->vfmt->BitsPerPixel;
  if (m_fullscreen) flags |= SDL_FULLSCREEN;
  if (!(m_width && m_height)) {
    fprintf(stderr,"Invalid resolution: %d x %d\n", m_width, m_height);
    return false;
  }
  if (debug) printf("Setting video to %d x %d\n", m_width, m_height);

   // TODO:these are probably leaked, however freeing them often causes a segfault!
  // TODO: SDL_image still uses non-const ref
  SDL_Surface *icon = IMG_ReadXPMFromArray(const_cast<char**>(sear_icon_xpm));
  SDL_WM_SetIcon(icon, NULL);
  SDL_FreeSurface(icon);

  //Is this the correct way to free a window?
  m_screen = SDL_SetVideoMode(m_width, m_height, bpp, flags);
  if (m_screen == NULL ) {
    fprintf(stderr, "Unable to set %d x %d video: %s\n", m_width, m_height, SDL_GetError());
    return false;
  }

  // Check OpenGL flags
  if (debug) {
    int value;
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &value);
    printf("[GL] Red Size: %d\n", value);

    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &value);
    printf("[GL] Green Size: %d\n", value);

    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &value);
    printf("[GL] Blue Size: %d\n", value);

    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &value);
    printf("[GL] Depth Size: %d\n", value);

    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &value);
    printf("[GL] Stencil Size: %d\n", value);

    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &value);
    printf("[GL] Double Buffer: %d\n", value);

    SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &value);
    printf("[GL] Accelerated Visual: %d\n", value);
  }

  const GLubyte *vendor     = glGetString(GL_VENDOR);
  const GLubyte *renderer   = glGetString(GL_RENDERER);
  const GLubyte *version    = glGetString(GL_VERSION);
  const GLubyte *extensions = glGetString(GL_EXTENSIONS);

   // These will be empty if there was a problem initialising the driver
  if (vendor == 0 || renderer == 0 || version == 0 || extensions == 0) {
    std::cerr << "Error with OpenGL system" << std::endl;
    return false;
  }

  printf("[GL] GL_VENDER: %s\n", vendor);
  printf("[GL] GL_RENDERER: %s\n", renderer);
  printf("[GL] GL_VERSION: %s\n", version);
  printf("[GL] GL_EXTENSIONS: %s\n", extensions);
                                                                                
  // TODO: Check that the OpenGL version is at least 1.3

  if (contextCreated()) {
    fprintf(stderr, "Error initialising context.\n");
    SDL_FreeSurface(m_screen);
    m_screen = NULL;
    return false;
  }

  return true;
        
}                                                                       

int GL::contextCreated() {

  // Hide cursor
  SDL_ShowCursor(0);

  initLighting();
  // TODO: initialisation need to go into system?
  setupStates();
  if (debug) checkError();
  glLineWidth(4.0f);
                                                                                
  //TODO: this needs to go into the set viewport method
  //Check for divide by 0
  if (m_height == 0) m_height = 1;

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Colour used to clear window
  glClearDepth(1.0); // Enables Clearing Of The Depth Buffer
  glClear(GL_DEPTH_BUFFER_BIT);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  if (m_use_fsaa) {
    glEnable(GL_MULTISAMPLE_ARB);
  }
                                                                                
  setViewMode(PERSPECTIVE);
  if (setupExtensions()) {
    fprintf(stderr, "[GL] Error finding required extensions.\n");
    return 1;
  }

  buildColourSet();
  if (debug) std::cout << "[GL] Window created" << std::endl << std::flush;

  incrementContext();
  if (debug) printf("[GL] New Context Number: %d\n", m_context_instantiation);
  m_context_valid = true;
  RenderSystem::getInstance().ContextCreated.emit();

  return 0;
}
void GL::destroyWindow() {
  assert(m_screen != NULL);

  SDL_FreeSurface(m_screen);
  m_screen = NULL;

  SDL_QuitSubSystem(SDL_INIT_VIDEO);

}

void GL::toggleFullscreen() {
  m_fullscreen = !m_fullscreen;
  // If fullscreen fails, create a new window with the fullscreen flag (un)set
  if (!SDL_WM_ToggleFullScreen(m_screen)) {
    destroyWindow();
    contextDestroyed(false);
    createWindow(m_width, m_height, m_fullscreen);
  }
}

int GL::checkError() {
  int iserr = 0;
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
  if (!msg.empty()) {
    std::cerr << msg << std::endl;
    iserr = 1;
  }
  return iserr;
}   


int GL::setupExtensions() {
  sage_init();

  // Disable VBO's if requested.
  if (!m_use_vbo) {
    sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT] = false;  
  }

  if (sage_ext[GL_ARB_MULTITEXTURE]) {
    if (debug) std::cout << "[GL] Found arb_multitexture" << std::endl;
  } else {
    fprintf(stderr, "[GL] Error, no GL_ARB_multitexture. Sear cannot run\n");
    return 1;
  }

  if (use_ext_compiled_vertex_array) {
    if (debug) std::cout << "[GL] Using use_ext_compiled_vertex_array" << std::endl;
  }
  return 0;
}
                                                                                
void GL::nextColour(WorldEntity *we){
  assert(we != 0);
  // TODO: we are now passing the same we to this function several times per
  // frame. We might want to return the same colour each time, rather than 
  // allocate a new one.

  // Dynamically grow array as needed.
  if (m_colour_index >= m_entityArray.size()) {
    m_entityArray.resize((m_entityArray.size() + 1) * 2);
  }

  m_entityArray[m_colour_index] = we; // Store entity in array slot

  // From a 32-bit number, extract red, green and blue components so that they are suitable for use
  // in glColor3ub. We use at most 24 bits (ignore alpha - it's used for
  // transparency).
  // We break up m_colour_index as follows;
  // [unused | red bits | green bits | blue bits].
  // For colour components with less than 8 bits, we bitshift so that high order bits are used over low order bits.
  // I.e. for a 5-bit depth, 00011011 becomes 11011000
  // In procEvent, these three colour components will be re-combined to obtain the original number.
  
  // Bitshift an 8-bit mask into the correct position in the 32-bit number, extract bits, then shift contents
  // back to a 8-bit placement.
  GLubyte red = (m_colour_index & (m_redMask << m_redShift)) >> (m_redShift - (8 - m_redBits));
  GLubyte green = (m_colour_index & (m_greenMask << m_greenShift)) >> (m_greenShift - (8 - m_greenBits));
  // Blue shift will often be 0, so we could end up with negative bitshifting.
  //GLubyte blue = (m_colour_index & (m_blueMask << m_blueShift)) >> (m_blueShift - (8 - m_blueBits));
  GLubyte blue = (m_colour_index & (m_blueMask << m_blueShift)) << (8 - m_blueBits);

  // Set the colour
  glColor3ub(red, green, blue);

  ++m_colour_index; // Increment counter for next pass
}

void GL::selectTerrainColour(WorldEntity * we) {
  nextColour(we);
}

void GL::buildColourSet() {
  // Get the bits info from OpenGL
  glGetIntegerv (GL_RED_BITS, &m_redBits);
  glGetIntegerv (GL_GREEN_BITS, &m_greenBits);
  glGetIntegerv (GL_BLUE_BITS, &m_blueBits);

  // Create masks
  m_redMask = makeMask(m_redBits);
  m_greenMask = makeMask(m_greenBits);
  m_blueMask = makeMask(m_blueBits);
  // Calculate shifts
  m_redShift = m_greenBits + m_blueBits;
  m_greenShift = m_blueBits;
  m_blueShift = 0;
}

GL::GL() :
  m_fontInitialised(false),
  m_width(0), m_height(0),
  m_fullscreen(false),
  m_screen(NULL),
  m_system(System::instance()),
  m_graphics(NULL),
  m_fov(RENDER_FOV),
  m_near_clip(RENDER_NEAR_CLIP),
  m_far_clip_dist(100.0f),
  m_base(0),
  m_font_id(NO_TEXTURE_ID),
  m_splash_id(NO_TEXTURE_ID),
  m_state_font(-1),
  m_state_splash(-1),
  m_x_pos(0), m_y_pos(0),
  m_speech_offset_x(0.0f),
  m_speech_offset_y(0.0f),
  m_speech_offset_z(0.0f),
  m_fog_start(100.0f),
  m_fog_end(150.0f),
  m_light_level(1.0f),
  m_colour_index(0),
  m_redBits(0), m_greenBits(0), m_blueBits(0),
  m_redMask(0), m_greenMask(0), m_blueMask(0),
  m_redShift(0), m_greenShift(0), m_blueShift(0),
  m_use_fsaa(DEFAULT_use_fsaa),
  m_use_vbo(DEFAULT_use_vbo),
  m_initialised(false)
{
}


GL::~GL() {
  if (m_initialised) shutdown();
}

void GL::shutdown() {
  assert(m_initialised == true);

  if (debug) std::cout << "GL: Shutdown" << std::endl;

  assert(m_screen == NULL);

  if (m_font_id != NO_TEXTURE_ID) RenderSystem::getInstance().releaseTexture(m_font_id);
  if (m_splash_id != NO_TEXTURE_ID) RenderSystem::getInstance().releaseTexture(m_splash_id);
  m_font_id = NO_TEXTURE_ID;
  m_splash_id = NO_TEXTURE_ID;

  m_initialised = false;
}

void GL::init() {
  assert(m_initialised == false);

  if (debug) std::cout << "GL: Initialise" << std::endl;

  System::instance()->getGeneral().sigsv.connect(sigc::mem_fun(*this, &GL::varconf_callback));

  m_initialised = true;
}

void GL::contextDestroyed(bool check) {
  m_context_valid = false;
  // Clear font display list
  shutdownFont(check);

  RenderSystem::getInstance().ContextDestroyed.emit(check);
}

void GL::initLighting() {
  if (debug) Log::writeLog("Render: initialising lighting", Log::LOG_DEFAULT);
  float gambient[4] = {0.1f, 0.1f,0.1f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,gambient);
  // Light values and coordinates
           
  // Setup and enable light 0
  glLightfv(GL_LIGHT0, GL_AMBIENT, m_lights[LIGHT_CHARACTER].ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, m_lights[LIGHT_CHARACTER].diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, m_lights[LIGHT_CHARACTER].specular);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, m_lights[LIGHT_CHARACTER].attenuation_constant);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, m_lights[LIGHT_CHARACTER].attenuation_linear);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, m_lights[LIGHT_CHARACTER].attenuation_quadratic);
  glEnable(GL_LIGHT0);
  
  glLightfv(GL_LIGHT1, GL_AMBIENT, blackLight);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, blackLight);
  glLightfv(GL_LIGHT1, GL_SPECULAR, blackLight);
  
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, m_lights[LIGHT_SUN].attenuation_constant);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, m_lights[LIGHT_SUN].attenuation_linear);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, m_lights[LIGHT_SUN].attenuation_quadratic);
  glEnable(GL_LIGHT1);
}

void GL::initFont() {
  if (debug) Log::writeLog("Render: Initailising Fonts", Log::LOG_DEFAULT);
  float cx; // Holds Our X Character Coord
  float cy; // Holds Our Y Character Coord
  m_base = glGenLists(256); // Creating 256 Display Lists

  assert(m_font_id == NO_TEXTURE_ID);
  m_font_id = RenderSystem::getInstance().requestTexture(DEFAULT_FONT);
  m_state_font = RenderSystem::getInstance().requestState(STATE_font);

  RenderSystem::getInstance().switchTexture(m_font_id);
  for (int loop=0; loop<256; ++loop) {
    cx=(float)(loop%16)/16.0f; // X Position Of Current Character
    cy=(float)(loop/16)/16.0f; // Y Position Of Current Character
    glNewList(m_base+loop,GL_COMPILE); // Start Building A List
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
  m_fontInitialised = true;
}

void GL::shutdownFont(bool check) {
  if (debug) Log::writeLog("Render: Shutting down fonts", Log::LOG_DEFAULT);
  if (check) {
    if (glIsList(m_base)) {
      glDeleteLists(m_base, 256); // Delete All 256 Display Lists
    }
  }
  if (m_font_id != NO_TEXTURE_ID) {
    RenderSystem::getInstance().releaseTexture(m_font_id);
    m_font_id = NO_TEXTURE_ID;
  }

  m_fontInitialised = false;
}

void GL::print(int x, int y, const char * string, int set) {
  if (!m_fontInitialised) initFont();
  if (set > 1) set = 1;
  RenderSystem::getInstance().switchTexture(m_font_id);
  glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
  glPushMatrix();
  glLoadIdentity(); // Reset The Projection Matrix
  glOrtho(0, m_width, 0 , m_height, -1, 1); // Set Up An Ortho Screen
  glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
  glPushMatrix();
  glLoadIdentity(); // Reset The Modelview Matrix
  glTranslated(x,y,0); // Position The Text (0,0 - Bottom Left)
  glListBase(m_base-32+(128*set)); // Choose The Font Set (0 or 1)
  glCallLists(strlen(string),GL_BYTE,string); // Write The Text To The Screen
  glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
  glPopMatrix(); // Restore The Old Projection Matrix
  glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
  glPopMatrix(); // Restore The Old Projection Matrix
}

void GL::print3D(const char *string, int set) {
  if (!m_fontInitialised) initFont();
  if (set > 1) set = 1;
  RenderSystem::getInstance().switchTexture(m_font_id);
  glPushMatrix();
  glListBase(m_base-32+(128*set)); // Choose The Font Set (0 or 1)
  glCallLists(strlen(string),GL_BYTE,string); // Write The Text To The Screen
  glPopMatrix(); // Restore The Old Projection Matrix
}

inline void GL::newLine() const {
  glTranslatef(0.0f,  ( FONT_HEIGHT) , 0.0f);
}

void GL::getScreenCoords(int & x, int & y, double z_offset) const
{
  GLint viewport[4];

  GLdouble mvmatrix[16], projmatrix[16];

  GLdouble wx, wy, wz;  /*  returned window x, y, z coords  */

  glGetIntegerv (GL_VIEWPORT, viewport);
  glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

  gluProject (0, 0, z_offset, mvmatrix, projmatrix, viewport, &wx, &wy, &wz);

  x = (int)wx;
  y = (int)wy;

  // std::cout << "Got screen coords " << x << ":" << y << std::endl << std::flush;
}

void GL::drawTextRect(int x, int y, int width, int height, int texture) const {
  RenderSystem::getInstance().switchTexture(texture);
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
  // No need to perform checks until we are in the game world
  if (!m_system->checkState(SYS_IN_WORLD)) return;

  // Reset last selected entity isSelected flag
  if (m_activeEntity) {
    dynamic_cast<WorldEntity*>(m_activeEntity.get())->setIsSelected(false);
  }

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


  GLubyte i[3];
  glClear(GL_COLOR_BUFFER_BIT);
  RenderSystem::getInstance().drawScene(true, 0);
  m_x_pos = x;
  y = m_height - y;
  m_y_pos = y;
  // This is a very expensive operation!
  glReadPixels(x, y, 1, 1, GL_RGB , GL_UNSIGNED_BYTE, &i);

// TODO pre-cache 8 - bits?
 
  // Convert the RGB components back into the index from nextColour(); 

  // Shift back into low-order bits
  // I.e. for a 5-bit depth, 11011000 becomes 00011011 
  GLubyte red = i[0] >> (8 - m_redBits);// & m_redMask;
  GLubyte green = i[1] >> (8 - m_greenBits);// & m_greenMask;
  GLubyte blue = i[2] >> (8 - m_blueBits);// & m_blueMask;

  // Reconstruct original index
  unsigned int ic = red;
  ic <<= m_greenBits;
  ic += green;
  ic <<= m_blueBits;
  ic += blue;

  // Find WorldEntity associated with index.
  WorldEntity *selected_entity = getSelectedID(ic);

  // Mark entity as selected
  if (selected_entity) selected_entity->setIsSelected(true);

  // If this entity is different to the previously selected entity, update our ref.
  if (selected_entity != m_activeEntity.get()) {
    if (selected_entity != NULL ) {
      m_activeEntity = Eris::EntityRef(selected_entity);
    } else {
      m_activeEntity = Eris::EntityRef();
    }

    if (debug && m_activeEntity) Log::writeLog(std::string("ActiveID: ") + m_activeEntity->getId(), Log::LOG_DEFAULT);
  }
}

//TODO should be in general render class
void GL::readConfig(varconf::Config &config) {
  if (debug) std::cout << "GL: readConfig" << std::endl;
  if (debug) Log::writeLog("Loading Renderer Config", Log::LOG_DEFAULT);

  // Setup character light source
  m_lights[LIGHT_CHARACTER].attenuation_constant = readDoubleValue(config, RENDER, KEY_character_light_kc, DEFAULT_character_light_kc);
  m_lights[LIGHT_CHARACTER].attenuation_linear = readDoubleValue(config, RENDER, KEY_character_light_kl, DEFAULT_character_light_kl);
  m_lights[LIGHT_CHARACTER].attenuation_quadratic = readDoubleValue(config, RENDER, KEY_character_light_kq, DEFAULT_character_light_kq);


  m_lights[LIGHT_CHARACTER].ambient[0] = readDoubleValue(config, RENDER, KEY_character_light_ambient_red, DEFAULT_character_light_ambient_red);
  m_lights[LIGHT_CHARACTER].ambient[1] = readDoubleValue(config, RENDER, KEY_character_light_ambient_green, DEFAULT_character_light_ambient_green);
  m_lights[LIGHT_CHARACTER].ambient[2] = readDoubleValue(config, RENDER, KEY_character_light_ambient_blue, DEFAULT_character_light_ambient_blue);
  m_lights[LIGHT_CHARACTER].ambient[3] = readDoubleValue(config, RENDER, KEY_character_light_ambient_alpha, DEFAULT_character_light_ambient_alpha);


  m_lights[LIGHT_CHARACTER].diffuse[0] = readDoubleValue(config, RENDER, KEY_character_light_diffuse_red, DEFAULT_character_light_diffuse_red);
  m_lights[LIGHT_CHARACTER].diffuse[1] = readDoubleValue(config, RENDER, KEY_character_light_diffuse_green, DEFAULT_character_light_diffuse_green);
  m_lights[LIGHT_CHARACTER].diffuse[2] = readDoubleValue(config, RENDER, KEY_character_light_diffuse_blue, DEFAULT_character_light_diffuse_blue);
  m_lights[LIGHT_CHARACTER].diffuse[3] = readDoubleValue(config, RENDER, KEY_character_light_diffuse_alpha, DEFAULT_character_light_diffuse_alpha);

  m_lights[LIGHT_CHARACTER].specular[0] = readDoubleValue(config, RENDER, KEY_character_light_specular_red, DEFAULT_character_light_specular_red);
  m_lights[LIGHT_CHARACTER].specular[1] = readDoubleValue(config, RENDER, KEY_character_light_specular_green, DEFAULT_character_light_specular_green);
  m_lights[LIGHT_CHARACTER].specular[2] = readDoubleValue(config, RENDER, KEY_character_light_specular_blue, DEFAULT_character_light_specular_blue);
  m_lights[LIGHT_CHARACTER].specular[3] = readDoubleValue(config, RENDER, KEY_character_light_specular_alpha, DEFAULT_character_light_specular_alpha);

  //Setup Sun light source
  m_lights[LIGHT_SUN].attenuation_constant = readDoubleValue(config, RENDER, KEY_sun_light_kc, DEFAULT_sun_light_kc);
  m_lights[LIGHT_SUN].attenuation_linear = readDoubleValue(config, RENDER, KEY_sun_light_kl, DEFAULT_sun_light_kl);
  m_lights[LIGHT_SUN].attenuation_quadratic = readDoubleValue(config, RENDER, KEY_sun_light_kq, DEFAULT_sun_light_kq);


  // Setup render states
  bool b;
  b = readBoolValue(config, RENDER, KEY_use_textures, DEFAULT_use_textures);
  RenderSystem::getInstance().setState(RenderSystem::RENDER_TEXTURES, b);

  b = readBoolValue(config, RENDER, KEY_use_lighting, DEFAULT_use_lighting);
  RenderSystem::getInstance().setState(RenderSystem::RENDER_LIGHTING, b);

  b = readBoolValue(config, RENDER, KEY_use_stencil, DEFAULT_use_stencil);
  RenderSystem::getInstance().setState(RenderSystem::RENDER_STENCIL, b);

  m_use_fsaa = readBoolValue(config, RENDER, KEY_use_fsaa, DEFAULT_use_fsaa);
  m_use_vbo = readBoolValue(config, RENDER, KEY_use_vbo, DEFAULT_use_vbo);

  // Setup the speech offsets
  m_speech_offset_x = readDoubleValue(config, RENDER, KEY_speech_offset_x, DEFAULT_speech_offset_x);
  m_speech_offset_z = readDoubleValue(config, RENDER, KEY_speech_offset_y, DEFAULT_speech_offset_y);
  m_speech_offset_y = readDoubleValue(config, RENDER, KEY_speech_offset_z, DEFAULT_speech_offset_z);

  m_fog_start = readDoubleValue(config, RENDER, KEY_fog_start, DEFAULT_fog_start);
  m_fog_end = readDoubleValue(config, RENDER, KEY_fog_end, DEFAULT_fog_end);

  m_near_clip = readDoubleValue(config, RENDER, KEY_near_clip, DEFAULT_near_clip);
  m_far_clip_dist = readDoubleValue(config, RENDER, KEY_far_clip_dist, DEFAULT_far_clip_dist);


  m_sdl_gl_red_size = readIntValue(config, RENDER, KEY_sdl_gl_red_size, DEFAULT_sdl_gl_red_size);
  m_sdl_gl_green_size = readIntValue(config, RENDER, KEY_sdl_gl_green_size, DEFAULT_sdl_gl_green_size);
  m_sdl_gl_blue_size = readIntValue(config, RENDER, KEY_sdl_gl_blue_size, DEFAULT_sdl_gl_blue_size);
  m_sdl_gl_alpha_size = readIntValue(config, RENDER, KEY_sdl_gl_alpha_size, DEFAULT_sdl_gl_alpha_size);

  m_sdl_gl_accum_red_size = readIntValue(config, RENDER, KEY_sdl_gl_accum_red_size, DEFAULT_sdl_gl_accum_red_size);
  m_sdl_gl_accum_green_size = readIntValue(config, RENDER, KEY_sdl_gl_accum_green_size, DEFAULT_sdl_gl_accum_green_size);
  m_sdl_gl_accum_blue_size = readIntValue(config, RENDER, KEY_sdl_gl_accum_blue_size, DEFAULT_sdl_gl_accum_blue_size);
  m_sdl_gl_accum_alpha_size = readIntValue(config, RENDER, KEY_sdl_gl_accum_alpha_size, DEFAULT_sdl_gl_accum_alpha_size);

  m_sdl_gl_doublebuffer = readIntValue(config, RENDER, KEY_sdl_gl_doublebuffer, DEFAULT_sdl_gl_doublebuffer);
  m_sdl_gl_buffer_size = readIntValue(config, RENDER, KEY_sdl_gl_buffer_size, DEFAULT_sdl_gl_buffer_size);
  m_sdl_gl_depth_size = readIntValue(config, RENDER, KEY_sdl_gl_depth_size, DEFAULT_sdl_gl_depth_size);
  m_sdl_gl_stencil_size = readIntValue(config, RENDER, KEY_sdl_gl_stencil_size, DEFAULT_sdl_gl_stencil_size);
  m_sdl_gl_stereo = readIntValue(config, RENDER, KEY_sdl_gl_stereo, DEFAULT_sdl_gl_stereo);
  m_sdl_gl_swap_control = readIntValue(config, RENDER, KEY_sdl_gl_swap_control, DEFAULT_sdl_gl_swap_control);
  m_sdl_gl_accelerated_visual = readIntValue(config, RENDER, KEY_sdl_gl_accelerated_visual, DEFAULT_sdl_gl_accelerated_visual);

  m_sdl_gl_multisamplesamples = readIntValue(config, RENDER, KEY_sdl_gl_multisamplesamples, DEFAULT_sdl_gl_multisamplesamples);
  m_sdl_gl_multisamplebuffers = readIntValue(config, RENDER, KEY_sdl_gl_multisamplebuffers, DEFAULT_sdl_gl_multisamplebuffers);
}

void GL::writeConfig(varconf::Config &config) {
  
  // Save character light source
  config.setItem(RENDER, KEY_character_light_kc, m_lights[LIGHT_CHARACTER].attenuation_constant);
  config.setItem(RENDER, KEY_character_light_kl, m_lights[LIGHT_CHARACTER].attenuation_linear);
  config.setItem(RENDER, KEY_character_light_kq, m_lights[LIGHT_CHARACTER].attenuation_quadratic);

  config.setItem(RENDER, KEY_character_light_ambient_red, m_lights[LIGHT_CHARACTER].ambient[0]);
  config.setItem(RENDER, KEY_character_light_ambient_green, m_lights[LIGHT_CHARACTER].ambient[1]);
  config.setItem(RENDER, KEY_character_light_ambient_blue, m_lights[LIGHT_CHARACTER].ambient[2]);
  config.setItem(RENDER, KEY_character_light_ambient_alpha, m_lights[LIGHT_CHARACTER].ambient[3]);

  config.setItem(RENDER, KEY_character_light_diffuse_red, m_lights[LIGHT_CHARACTER].diffuse[0]);
  config.setItem(RENDER, KEY_character_light_diffuse_green, m_lights[LIGHT_CHARACTER].diffuse[1]);
  config.setItem(RENDER, KEY_character_light_diffuse_blue, m_lights[LIGHT_CHARACTER].diffuse[2]);
  config.setItem(RENDER, KEY_character_light_diffuse_alpha, m_lights[LIGHT_CHARACTER].diffuse[3]);

  config.setItem(RENDER, KEY_character_light_specular_red, m_lights[LIGHT_CHARACTER].specular[0]);
  config.setItem(RENDER, KEY_character_light_specular_green, m_lights[LIGHT_CHARACTER].specular[1]);
  config.setItem(RENDER, KEY_character_light_specular_blue, m_lights[LIGHT_CHARACTER].specular[2]);
  config.setItem(RENDER, KEY_character_light_specular_alpha, m_lights[LIGHT_CHARACTER].specular[3]);
  
  // Save Sun light source
  config.setItem(RENDER, KEY_sun_light_kc, m_lights[LIGHT_SUN].attenuation_constant);
  config.setItem(RENDER, KEY_sun_light_kl, m_lights[LIGHT_SUN].attenuation_linear);
  config.setItem(RENDER, KEY_sun_light_kq, m_lights[LIGHT_SUN].attenuation_quadratic);

  // Save render states
  config.setItem(RENDER, KEY_use_textures, RenderSystem::getInstance().getState(RenderSystem::RENDER_TEXTURES));
  config.setItem(RENDER, KEY_use_lighting, RenderSystem::getInstance().getState(RenderSystem::RENDER_LIGHTING));
  config.setItem(RENDER, KEY_use_stencil, RenderSystem::getInstance().getState(RenderSystem::RENDER_STENCIL));
  
  // Save the speech offsets
  config.setItem(RENDER, KEY_speech_offset_x, m_speech_offset_x);
  config.setItem(RENDER, KEY_speech_offset_y, m_speech_offset_y);
  config.setItem(RENDER, KEY_speech_offset_z, m_speech_offset_z);

  config.setItem(RENDER, KEY_fog_start, m_fog_start);
  config.setItem(RENDER, KEY_fog_end, m_fog_end);
  config.setItem(RENDER, KEY_near_clip, m_near_clip);
  config.setItem(RENDER, KEY_far_clip_dist, m_far_clip_dist);

  config.setItem(RENDER, KEY_sdl_gl_red_size, m_sdl_gl_red_size);
  config.setItem(RENDER, KEY_sdl_gl_green_size, m_sdl_gl_green_size);
  config.setItem(RENDER, KEY_sdl_gl_blue_size, m_sdl_gl_blue_size);
  config.setItem(RENDER, KEY_sdl_gl_alpha_size, m_sdl_gl_alpha_size);

  config.setItem(RENDER, KEY_sdl_gl_accum_red_size, m_sdl_gl_accum_red_size);
  config.setItem(RENDER, KEY_sdl_gl_accum_green_size, m_sdl_gl_accum_green_size);
  config.setItem(RENDER, KEY_sdl_gl_accum_blue_size, m_sdl_gl_accum_blue_size);
  config.setItem(RENDER, KEY_sdl_gl_accum_alpha_size, m_sdl_gl_accum_alpha_size);

  config.setItem(RENDER, KEY_sdl_gl_doublebuffer, m_sdl_gl_doublebuffer);
  config.setItem(RENDER, KEY_sdl_gl_buffer_size, m_sdl_gl_buffer_size);
  config.setItem(RENDER, KEY_sdl_gl_depth_size, m_sdl_gl_depth_size);
  config.setItem(RENDER, KEY_sdl_gl_stencil_size, m_sdl_gl_stencil_size);
  config.setItem(RENDER, KEY_sdl_gl_stereo, m_sdl_gl_stereo);
  config.setItem(RENDER, KEY_sdl_gl_swap_control, m_sdl_gl_swap_control);
  config.setItem(RENDER, KEY_sdl_gl_accelerated_visual, m_sdl_gl_accelerated_visual);

  config.setItem(RENDER, KEY_sdl_gl_multisamplesamples, m_sdl_gl_multisamplesamples);
  config.setItem(RENDER, KEY_sdl_gl_multisamplebuffers, m_sdl_gl_multisamplebuffers);
}  

void GL::setupStates() {
  // TODO: should this be in the init?
  glDepthFunc(GL_LEQUAL);
  glAlphaFunc(GL_GREATER, 0.1f);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  GLfloat fog_colour[] = {0.50f, 0.50f, 0.50f, 0.50f};
  glFogfv(GL_FOG_COLOR, fog_colour);
  glFogf(GL_FOG_START, m_fog_start);
  glFogf(GL_FOG_END, m_fog_end);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Initial GL states
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_AUTO_NORMAL);
  glDisable(GL_BLEND);
  //TODO: More of these!
  glDisable(GL_CLIP_PLANE0);


  glDisable(GL_COLOR_LOGIC_OP);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_COLOR_TABLE);
  glDisable(GL_CONVOLUTION_1D);
  glDisable(GL_CONVOLUTION_2D);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_DITHER);
  glDisable(GL_FOG);
  glDisable(GL_HISTOGRAM);
  glDisable(GL_INDEX_LOGIC_OP);

   // TODO More of these
  glDisable(GL_LIGHT0);

  glDisable(GL_LIGHTING);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_LINE_STIPPLE);
  glDisable(GL_MAP1_COLOR_4);
  glDisable(GL_MAP1_INDEX);
  glDisable(GL_MAP1_NORMAL);
  glDisable(GL_MAP1_TEXTURE_COORD_1);
  glDisable(GL_MAP1_TEXTURE_COORD_2);
  glDisable(GL_MAP1_TEXTURE_COORD_3);
  glDisable(GL_MAP1_TEXTURE_COORD_4);
  glDisable(GL_MAP1_VERTEX_3);
  glDisable(GL_MAP1_VERTEX_4);
  glDisable(GL_MAP2_COLOR_4);
  glDisable(GL_MAP2_INDEX);
  glDisable(GL_MAP2_NORMAL);
  glDisable(GL_MAP2_TEXTURE_COORD_1);
  glDisable(GL_MAP2_TEXTURE_COORD_2);
  glDisable(GL_MAP2_TEXTURE_COORD_3);
  glDisable(GL_MAP2_TEXTURE_COORD_4);
  glDisable(GL_MAP2_VERTEX_3);
  glDisable(GL_MAP2_VERTEX_4);
  glDisable(GL_MINMAX);
  glDisable(GL_NORMALIZE);
  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_POLYGON_OFFSET_FILL);
  glDisable(GL_POLYGON_OFFSET_LINE);
  glDisable(GL_POLYGON_OFFSET_POINT);
  glDisable(GL_POLYGON_SMOOTH);
  glDisable(GL_POLYGON_STIPPLE);
  glDisable(GL_POST_COLOR_MATRIX_COLOR_TABLE);
  glDisable(GL_POST_CONVOLUTION_COLOR_TABLE);
  glDisable(GL_RESCALE_NORMAL);
  glDisable(GL_SEPARABLE_2D);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_TEXTURE_3D);
  glDisable(GL_TEXTURE_GEN_Q);
  glDisable(GL_TEXTURE_GEN_R);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);

  // Initial client states
  glEnableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_EDGE_FLAG_ARRAY);
  glDisableClientState(GL_INDEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

inline void GL::translateObject(float x, float y, float z) const {
  glTranslatef(x, y, z);
}

inline void GL::rotate(float angle, float x, float y, float z) const {
  glRotatef(angle, x, y, z);
}

void GL::rotateObject(ObjectRecord* object_record, ModelRecord* model_record) const {
  WorldEntity *we = dynamic_cast<WorldEntity*>(object_record->entity.get());
  assert(we != 0);

  switch (model_record->rotation_style) {
    case ROS_NONE: return; break;
    case ROS_POSITION: {
       const WFMath::Point<3> &pos = object_record->position;
       assert(pos.isValid());
       glRotatef(pos.x() + pos.y() + pos.z(), 0.0f, 0.0f, 1.0f);
       break;
    }       
    case ROS_NORMAL: {
      applyQuaternion(we->getAbsOrient().inverse());
      break;
    }
    case ROS_BILLBOARD: // Same as STATE_halo, but does not rotate with camera elevation
    case ROS_HALO: {
      float rotation_matrix[4][4];
      WFMath::Quaternion orient2(1.0f, 0.0f, 0.0f, 0.0f); // Initial Camera rotation
      orient2 *= m_graphics->getCameraOrientation();
      QuatToMatrix(orient2, rotation_matrix); //Get the rotation matrix for base rotation
      glMultMatrixf(&rotation_matrix[0][0]); //Apply rotation matrix
      break;
    }
  }
}

inline void GL::scaleObject(float scale) const {
  glScalef(scale, scale, scale);
}

void GL::setViewMode(int type) const {
  Camera *cam = RenderSystem::getInstance().getCameraSystem()->getCurrentCamera();
  if (type == CAMERA) {
    if (cam->getType() == Camera::CAMERA_ISOMETRIC) 
      type = ISOMETRIC;
    else type = PERSPECTIVE;
  }
//  Perspective
  glViewport(0, 0, m_width, m_height);
  switch (type) {
    case ISOMETRIC: {
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity(); // Reset The Projection Matrix
  
      // Calculate The Aspect Ratio Of The Window
      double aspect =  (double)m_width / (double)m_height;
      double region = cam->getDistance();
      double far_clip = 100.0;
      double near_clip = -100.0;
      glOrtho(-region * aspect, region * aspect, -region, region, near_clip, far_clip);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      break;
    }
    case PERSPECTIVE: {
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity(); // Reset The Projection Matrix
  
      // Calculate The Aspect Ratio Of The Window
      gluPerspective(m_fov,(GLfloat)m_width/(GLfloat)m_height, m_near_clip, m_far_clip_dist);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      break;
    }
    case ORTHOGRAPHIC: {
      glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
      glLoadIdentity(); // Reset The Projection Matrix
      glOrtho(0, m_width, 0 , m_height, -1, 1); // Set Up An Ortho Screen
      glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
      glLoadIdentity();
      break;
    }			    
  }	
}

// TODO put into material manager and use display lists to retrieve them
void GL::setMaterial(float *ambient, float *diffuse, float *specular, float shininess, float *emissive) const {
  // TODO: set up missing values
  if (ambient)           glMaterialfv (GL_FRONT, GL_AMBIENT,   ambient);
  if (diffuse)           glMaterialfv (GL_FRONT, GL_DIFFUSE,   diffuse);
  if (specular)          glMaterialfv (GL_FRONT, GL_SPECULAR,  specular);
  if (shininess >= 0.0f) glMaterialf  (GL_FRONT, GL_SHININESS, shininess);
  if (emissive)          glMaterialfv (GL_FRONT, GL_EMISSION,  emissive);
  else                   glMaterialfv (GL_FRONT, GL_EMISSION,  black);
}

void GL::renderArrays(unsigned int type, unsigned int offset, unsigned int number_of_points, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data, bool multitexture) const {
 
  if (!vertex_data) {
    Log::writeLog("No Vertex Data", Log::LOG_ERROR);
    return;
  }

  // TODO: Reduce ClientState switches
  bool textures = RenderSystem::getInstance().getState(RenderSystem::RENDER_TEXTURES);
  bool lighting = RenderSystem::getInstance().getState(RenderSystem::RENDER_LIGHTING);

  glVertexPointer(3, GL_FLOAT, 0, (float*)vertex_data);
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
    case (RES_INVALID): Log::writeLog("Trying to render INVALID type", Log::LOG_ERROR); break;
    case (RES_POINT): glDrawArrays(GL_POINT, offset, number_of_points); break;
    case (RES_LINES): glDrawArrays(GL_LINES, offset, number_of_points); break;
    case (RES_TRIANGLES): glDrawArrays(GL_TRIANGLES, offset, number_of_points); break;
    case (RES_QUADS): glDrawArrays(GL_QUADS, offset, number_of_points); break;
    case (RES_TRIANGLE_FAN): glDrawArrays(GL_TRIANGLE_FAN, offset, number_of_points); break;
    case (RES_TRIANGLE_STRIP): glDrawArrays(GL_TRIANGLE_STRIP, offset, number_of_points); break;
    case (RES_QUAD_STRIP): glDrawArrays(GL_QUAD_STRIP, offset, number_of_points); break;
    default: Log::writeLog("Unknown type", Log::LOG_ERROR); break;
  }
 
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

void GL::drawQueue(QueueMap &queue, bool select_mode) {
  QueueMap::const_iterator I = queue.begin();
  QueueMap::const_iterator Iend = queue.end();
  for (; I != Iend; ++I) {
    // Change state for this queue
    RenderSystem::getInstance().switchState(I->first);
    Queue::const_iterator J = I->second.begin();
    Queue::const_iterator Jend = I->second.end();
    for (;J != Jend; ++J) {

      ObjectRecord* object_record = J->first;
      ModelRecord* model_record = J->second;
      SPtr<Model> model = model_record->model;
      assert(model);

      WorldEntity *we = dynamic_cast<WorldEntity*>(object_record->entity.get());

      glPushMatrix();

      // 1) Apply Object transforms
      const WFMath::Point<3> &pos = we->getAbsPos();
      assert(pos.isValid());
      glTranslatef(pos.x(), pos.y(), pos.z() );

      rotateObject(object_record, model_record);

      // 2) Apply Model Transforms

      // Scale Object
      float scale = model_record->scale;
      // Do not perform scaling if it is to zero or has no effect
      if (scale != 0.0f && scale != 1.0f) glScalef(scale, scale, scale);

      glTranslatef(model_record->offset_x, model_record->offset_y, model_record->offset_z);

      glRotatef(model_record->rotate_z, 0.0f, 0.0f, 1.0f);   

      // 3) Apply final scaling once model is in place

      // Scale model by all bounding box axis
      if (model_record->scale_bbox && we->hasBBox()) {
        const WFMath::AxisBox<3> &bbox = we->getBBox();
        float x_scale = bbox.highCorner().x() - bbox.lowCorner().x();
        float y_scale = bbox.highCorner().y() - bbox.lowCorner().y();
        float z_scale = bbox.highCorner().z() - bbox.lowCorner().z();

        glScalef(x_scale, y_scale, z_scale);
      }
      // Scale model by bounding box height
      else if (model_record->scaleByHeight && we->hasBBox()) {
        const WFMath::AxisBox<3> &bbox = we->getBBox();
        float z_scale = fabs(bbox.highCorner().z() - bbox.lowCorner().z());
        glScalef(z_scale, z_scale, z_scale);
      }

      // Draw Model
      if (select_mode) {
        nextColour(we);
        model->render(true);
      } else {
        if (we->isSelectedEntity()) {
          m_active_name = object_record->entity->getName();
          drawOutline(model_record);
        } else {
          GLboolean blend_enabled = true;
          GLboolean cmat_enabled = true;
          if (we->getFade() < 1.0f) {
            glGetBooleanv(GL_BLEND, &blend_enabled);
            glGetBooleanv(GL_COLOR_MATERIAL, &cmat_enabled);
            if (!blend_enabled) glEnable(GL_BLEND);
            if (!cmat_enabled) glEnable(GL_COLOR_MATERIAL);
          }
          glColor4f(1.0f, 1.0f, 1.0f, we->getFade());

          model->render(false);

          if (!blend_enabled) glDisable(GL_BLEND);
          if (!cmat_enabled)  glDisable(GL_COLOR_MATERIAL);
        }
      }

      glPopMatrix();
    }
  }
}

/**
 * Render the list of entity names in the world.
 */
void GL::drawNameQueue(MessageList &list) {
  glColor4fv(blue);
  RenderSystem::getInstance().switchState(m_state_font);
  MessageList::const_iterator I = list.begin();
  MessageList::const_iterator Iend = list.end();
  for (; I != Iend; ++I) {
    WorldEntity *we = *I;
    glPushMatrix();
    const WFMath::Point<3> &pos = we->getAbsPos();
    assert(pos.isValid());
    glTranslatef(pos.x(), pos.y(), pos.z());
    WFMath::Quaternion orient2(1.0f, 0.0f, 0.0f, 0.0f); // Initial Camera rotation
    orient2 *= m_graphics->getCameraOrientation(); 
    applyQuaternion(orient2);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(0.025f, 0.025f, 0.025f);
    glTranslatef(m_speech_offset_x, m_speech_offset_y, m_speech_offset_z);
    print3D(we->getName().c_str(), 0);
    glPopMatrix();
  }
}

void GL::drawMessageQueue(MessageList &list) {
  glColor4fv(yellow);
  RenderSystem::getInstance().switchState(m_state_font);
  MessageList::const_iterator I = list.begin();
  MessageList::const_iterator Iend = list.end();
  for (; I != Iend; ++I) {
    WorldEntity *we = *I;
    glPushMatrix();
    const WFMath::Point<3> &pos = we->getAbsPos();
    assert(pos.isValid());
    glTranslatef(pos.x(), pos.y(), pos.z());
    if (we->screenCoordsRequest() > 0) {
      double height = 2;
      getScreenCoords(we->screenX(), we->screenY(), height);
    }
    glPopMatrix();
  }
}
 
inline float GL::distFromNear(float x, float y, float z) const {
  return Frustum::distFromNear(m_frustum, x, y, z);
}

inline int GL::axisBoxInFrustum(const WFMath::AxisBox<3> &bbox) const {
  return Frustum::axisBoxInFrustum(m_frustum, bbox);
}

void GL::drawOutline(ModelRecord* model_record) {
  StateID cur_state = RenderSystem::getInstance().getCurrentState();
  SPtr<Model> model = model_record->model;
  assert(model);
  bool use_stencil = RenderSystem::getInstance().getState(RenderSystem::RENDER_STENCIL) && model_record->outline;
  if (use_stencil) { // Using Stencil Buffer
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, -1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glPushMatrix();
    model->render(false);
    glPopMatrix();
    //TODO hard code halo in static const variable
    RenderSystem::getInstance().switchState(model_record->select_state);
    glStencilFunc(GL_NOTEQUAL, -1, 1);
    glColor4fv(halo_colour);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    model->render(true);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_STENCIL_TEST);
    glColor4fv(white);
  } else { // Just use solid colour on object 
    RenderSystem::getInstance().switchState(model_record->select_state);
    glColor4fv(halo_colour);  
    model->render(true);
    glColor4fv(white);
  }
  RenderSystem::getInstance().switchState(cur_state); // Restore state
}

inline void GL::beginFrame() {
  // TODO into display list
  m_active_name = "";
  if (RenderSystem::getInstance().getState(RenderSystem::RENDER_STENCIL)) {
    glClearStencil(1);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear The Screen And The Depth Buffer
  } else {
    glClear(GL_DEPTH_BUFFER_BIT);
  }
// TODO remove -- can't! it is required for the skybox
  glClear(GL_COLOR_BUFFER_BIT);
  setViewMode(CAMERA);
//  glLoadIdentity(); // Reset The View
  //Rotate Coordinate System so Z points upwards and Y points into the screen. 
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
}

inline void GL::endFrame(bool select_mode) {
//  glFlush();
  if (!select_mode) SDL_GL_SwapBuffers();
  if (debug) checkError();
}
  
void GL::drawSplashScreen() {

  if (m_splash_id == NO_TEXTURE_ID) {
    m_state_splash = RenderSystem::getInstance().requestState(STATE_splash);
    m_splash_id = RenderSystem::getInstance().requestTexture(TEXTURE_splash_texture);
  }

  RenderSystem::getInstance().switchState(m_state_splash);

  // We don't need super fast rendering for the splash screen
  SDL_Delay(sleep_time);

  setViewMode(ORTHOGRAPHIC);
  
  glColor4fv(white);

  RenderSystem::getInstance().switchTexture(m_splash_id);

  // TODO into vertex array?
  glBegin(GL_QUADS); 
    glTexCoord2i(0, 0); glVertex2f(0.0f, 0.0f);
    glTexCoord2i(0, 1); glVertex2f(0.0f, m_height);
    glTexCoord2i(1, 1); glVertex2f(m_width, m_height);
    glTexCoord2i(1, 0); glVertex2f(m_width, 0.0f);
  glEnd(); 
  setViewMode(PERSPECTIVE);
}
  
inline void GL::applyQuaternion(const WFMath::Quaternion & quaternion) const {
  assert(quaternion.isValid());
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
  assert(calendar != NULL);
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
      m_light_level = tim / ((day_time - dawn_time) * modifier);
      float pos_mod = tim / ((night_time - dawn_time) * modifier);
      m_lights[LIGHT_SUN].position[0] = -200.0f * (pos_mod - 0.5f);
      break;
    }
    case Calendar::DAY: {
      m_light_level = 1.0f;
      float pos_mod = tim / ((night_time - dawn_time) * modifier);
      m_lights[LIGHT_SUN].position[0] = -200.0f * (pos_mod - 0.5f);
      break;
    }
    case Calendar::DUSK: {
      m_light_level = 1.0f - (tim / ((night_time - dusk_time) * modifier));
      float pos_mod = tim / ((night_time - dawn_time) * modifier);
      m_lights[LIGHT_SUN].position[0] = -200.0f * (pos_mod - 0.5f);
      break;
    }
    case Calendar::NIGHT: {
      m_light_level = 0.0f;
      break;
    }
  }
  if (m_light_level < 0.15f) m_light_level = 0.15f; 
  fog_colour[0] = fog_colour[1] = fog_colour[2] = fog_colour[3] = 0.5f * m_light_level;
//  glFogfv(GL_FOG_COLOR, fog_colour);
  float sun_pos[] = {m_lights[LIGHT_SUN].position[0], 0.0f, 100.0f, 1.0f};
  m_lights[LIGHT_SUN].ambient[0] = m_lights[LIGHT_SUN].ambient[1] = m_lights[LIGHT_SUN].ambient[2] = m_light_level * 0.5f;
  m_lights[LIGHT_SUN].diffuse[0] = m_lights[LIGHT_SUN].diffuse[1] = m_lights[LIGHT_SUN].diffuse[2] = m_light_level;
  glLightfv(GL_LIGHT1,GL_POSITION,sun_pos);
  glLightfv(GL_LIGHT1, GL_AMBIENT, m_lights[LIGHT_SUN].ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, m_lights[LIGHT_SUN].diffuse);
}

inline void GL::resetSelection() {
  resetColours();
}

inline void GL::renderActiveName() {
  if (m_active_name.empty()) return;

  glColor4fv(activeNameColour);
  RenderSystem::getInstance().switchState(m_state_font);
  print(m_x_pos, m_y_pos, m_active_name.c_str(), 1);
}

inline void GL::getFrustum(float frust[6][4]) {
  static float  proj[16];
  static float  modl[16];
  /* Get the current PROJECTION matrix from OpenGraphics */
  glGetFloatv(GL_PROJECTION_MATRIX, proj );
  /* Get the current MODELVIEW matrix from OpenGraphics */
  glGetFloatv(GL_MODELVIEW_MATRIX, modl );
  Frustum::getFrustum(frust, proj, modl);
  // Copy m_frustum - local copy plus one from graphics object
  for (int i = 0; i < 6; ++i) {
    for (int j = 0; j < 4; ++j) {
      m_frustum[i][j] = frust[i][j];
    }
  }
}

void GL::getModelviewMatrix(float m[4][4])
{
    glGetFloatv(GL_MODELVIEW_MATRIX, (float*) m);
}

void GL::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  if (section == RENDER) {
    if (key == KEY_character_light_kc) {
      m_lights[LIGHT_CHARACTER].attenuation_constant = readDoubleValue(config, section, key, DEFAULT_character_light_kc);
    }
    else if (key ==  KEY_character_light_kl) {
      m_lights[LIGHT_CHARACTER].attenuation_linear = readDoubleValue(config, section, key, DEFAULT_character_light_kl);
    }
    else if (key == KEY_character_light_kq) {
      m_lights[LIGHT_CHARACTER].attenuation_quadratic = readDoubleValue(config, section, key, DEFAULT_character_light_kq);
    }
    else if (key == KEY_character_light_ambient_red) {
      m_lights[LIGHT_CHARACTER].ambient[0] = readDoubleValue(config, section, key, DEFAULT_character_light_ambient_red);
    }
    else if (key == KEY_character_light_ambient_green) {
      m_lights[LIGHT_CHARACTER].ambient[1] = readDoubleValue(config, section, key, DEFAULT_character_light_ambient_green);
    }
    else if (key == KEY_character_light_ambient_blue) {
      m_lights[LIGHT_CHARACTER].ambient[2] = readDoubleValue(config, section, key, DEFAULT_character_light_ambient_blue);
    }
    else if (key == KEY_character_light_ambient_alpha) {
      m_lights[LIGHT_CHARACTER].ambient[3] = readDoubleValue(config, section, key, DEFAULT_character_light_ambient_alpha);
    }
    else if (key == KEY_character_light_diffuse_red) {
      m_lights[LIGHT_CHARACTER].diffuse[0] = readDoubleValue(config, section, key, DEFAULT_character_light_diffuse_red);
    }
    else if (key == KEY_character_light_diffuse_green) {
      m_lights[LIGHT_CHARACTER].diffuse[1] = readDoubleValue(config, section, key, DEFAULT_character_light_diffuse_green);
    }
    else if (key == KEY_character_light_diffuse_blue) {
      m_lights[LIGHT_CHARACTER].diffuse[2] = readDoubleValue(config, section, key, DEFAULT_character_light_diffuse_blue);
    }
    else if (key == KEY_character_light_diffuse_alpha) {
      m_lights[LIGHT_CHARACTER].diffuse[3] = readDoubleValue(config, section, key, DEFAULT_character_light_diffuse_alpha);
    }
    else if (key == KEY_character_light_specular_red) {
      m_lights[LIGHT_CHARACTER].specular[0] = readDoubleValue(config, section, key, DEFAULT_character_light_specular_red);
    }
    else if (key == KEY_character_light_specular_green) {
      m_lights[LIGHT_CHARACTER].specular[1] = readDoubleValue(config, section, key, DEFAULT_character_light_specular_green);
    }
    else if (key == KEY_character_light_specular_blue) {
      m_lights[LIGHT_CHARACTER].specular[2] = readDoubleValue(config, section, key, DEFAULT_character_light_specular_blue);
    }
    else if (key == KEY_character_light_specular_alpha) {
      m_lights[LIGHT_CHARACTER].specular[3] = readDoubleValue(config, section, key, DEFAULT_character_light_specular_alpha);
    }
    //Setup Sun light source
    else if (key == KEY_sun_light_kc) {
      m_lights[LIGHT_SUN].attenuation_constant = readDoubleValue(config, section, key, DEFAULT_sun_light_kc);
    }
    else if (key == KEY_sun_light_kl) {
      m_lights[LIGHT_SUN].attenuation_linear = readDoubleValue(config, section, key, DEFAULT_sun_light_kl);
    }
    else if (key == KEY_sun_light_kq) {
      m_lights[LIGHT_SUN].attenuation_quadratic = readDoubleValue(config, section, key, DEFAULT_sun_light_kq);
    }
    // Setup render states
    else if (key == KEY_use_textures) {
      bool b = readBoolValue(config, section, key, DEFAULT_use_textures);
      RenderSystem::getInstance().setState(RenderSystem::RENDER_TEXTURES, b);
    }
    else if (key == KEY_use_lighting) {
      bool b = readBoolValue(config, section, key, DEFAULT_use_lighting);
      RenderSystem::getInstance().setState(RenderSystem::RENDER_LIGHTING, b);
    }
    else if (key == KEY_use_stencil) {
      bool b = readBoolValue(config, section, key, DEFAULT_use_stencil);
      RenderSystem::getInstance().setState(RenderSystem::RENDER_STENCIL, b);
    }
    // Setup the speech offsets
    else if (key == KEY_speech_offset_x) {
      m_speech_offset_x = readDoubleValue(config, section, key, DEFAULT_speech_offset_x);
    }
    else if (key == KEY_speech_offset_y) {
      m_speech_offset_y = readDoubleValue(config, section, key, DEFAULT_speech_offset_y);
    }
    else if (key == KEY_speech_offset_z) {
      m_speech_offset_z = readDoubleValue(config, section, key, DEFAULT_speech_offset_z);
    }
    else if (key == KEY_fog_start) {
      m_fog_start = readDoubleValue(config, section, key, DEFAULT_fog_start);
    }
    else if (key == KEY_fog_end) {
      m_fog_end = readDoubleValue(config, section, key, DEFAULT_fog_end);
    }
    else if (key == KEY_near_clip) {
      m_near_clip = readDoubleValue(config, section, key, DEFAULT_near_clip);
    }
    else if (key == KEY_far_clip_dist) {
      m_far_clip_dist = readDoubleValue(config, section, key, DEFAULT_far_clip_dist);
    }
  }
}

std::string GL::getActiveID() const {
  return (m_activeEntity) ? (m_activeEntity->getId()) : ("");
}

WorldEntity *GL::getActiveEntity() const {
  return dynamic_cast<WorldEntity*>(m_activeEntity.get());
} 

void GL::resize(int width, int height) {
  m_width = width;
  m_height = height;
  int flags = SDL_OPENGL | SDL_RESIZABLE;
  const SDL_VideoInfo *info = SDL_GetVideoInfo();
  if (!info) {
    Log::writeLog("Error quering video", Log::LOG_DEFAULT);
    return;// false;
  }
  int bpp = info->vfmt->BitsPerPixel;
  if (m_fullscreen) flags |= SDL_FULLSCREEN;
  if (debug) std::cout << "Setting video to " << m_width << " x " << m_height << std::endl;

  // This is probly being leaked.
  m_screen = SDL_SetVideoMode(m_width, m_height, bpp, flags);
  if (m_screen == NULL ) {
    std::cerr << "Unable to set " << m_width << " x " << m_height << " video: " << SDL_GetError() << std::endl;
    // Need a better error condition
    return;
  }

  // Have the textures been destroyed?
  if (!glIsTexture(1)) {
    printf("Warning, context lost after resize!\n");
    // TODO: What is going on in this situation
    // The window has been resized, however the GL context needs to be
    // re-created. Do we need to destroy the whole window?
    contextDestroyed(false);
    contextCreated();
  }
  // Update view port
  setViewMode(PERSPECTIVE);
}

bool GL::getWorldCoords(int x, int y, float &wx, float &wy, float &wz) {
 // Taken from equator
  y = m_height - y;
  GLint viewport[4];
  GLdouble mvmatrix[16], projmatrix[16];

  float z = 1.f;
  glReadPixels (x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
//  if (debug) printf("Screen Depth: %f\n", z);
  if (!(z < 1.f)) {
      return false;
  }

  setViewMode(CAMERA);

  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  m_graphics->setCameraTransform();
//  if (debug) printf("Screen Coord: %d %d %f\n", x, y, z);
  glGetIntegerv (GL_VIEWPORT, viewport);
  glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

  double tx, ty, tz;
  gluUnProject(x, y, z, mvmatrix, projmatrix, viewport, &tx, &ty, &tz);
  wx = tx;
  wy = ty;
  wz = tz;

//  if (debug) printf("World Coord: %f %f %f\n", wx, wy, wz);
  return true;
}

void GL::drawQueue(const QueueStaticObjectMap &object_map,
	           const QueueMatrixMap &matrix_map,
                   const QueueStateMap &state_map,
                   bool select_mode) {

  QueueStaticObjectMap::const_iterator I = object_map.begin();
  QueueStaticObjectMap::const_iterator Iend = object_map.end();
  while (I != Iend) {
    // Get object id 
    const std::string &key = I->first;

    // Store ref to object list
    const StaticObjectList &objects = I->second;

    // Get ref to matrix list
    assert(matrix_map.find(key) != matrix_map.end());
    const MatrixEntityList &matrices = matrix_map.find(key)->second;

    // Switch to the appropriate render list.
    assert(state_map.find(key) != state_map.end());
    RenderSystem::getInstance().switchState(state_map.find(key)->second);

    StaticObjectList::const_iterator J = objects.begin();
    StaticObjectList::const_iterator Jend = objects.end();
    while (J != Jend) {
      (*J++)->render(select_mode, matrices);
    }
    ++I;
  }
}

void GL::drawQueue(const QueueDynamicObjectMap &object_map,
	           const QueueMatrixMap &matrix_map,
                   const QueueStateMap &state_map,
                   bool select_mode) {

  QueueDynamicObjectMap::const_iterator I = object_map.begin();
  QueueDynamicObjectMap::const_iterator Iend = object_map.end();
  while (I != Iend) {
    // Get object id 
    const std::string &key = I->first;

    // Store ref to object list
    const DynamicObjectList &objects = I->second;

    // Get ref to matrix list
    assert(matrix_map.find(key) != matrix_map.end());
    const MatrixEntityList &matrices = matrix_map.find(key)->second;

    // Switch to the appropriate render list.
    assert(state_map.find(key) != state_map.end());
    RenderSystem::getInstance().switchState(state_map.find(key)->second);

    MatrixEntityList::const_iterator K = matrices.begin();
    MatrixEntityList::const_iterator Kend = matrices.end();
    while (K != Kend) {
      const Matrix &mx = K->first;
      WorldEntity *we = K->second;
      glPushMatrix();
      glMultMatrixf(mx.getMatrix());

      DynamicObjectList::const_iterator J = objects.begin();
      DynamicObjectList::const_iterator Jend = objects.end();
      while (J != Jend) {
  //      (*J++)->render(select_mode, matrices);
        (*J++)->render(select_mode, we);
      } 
      glPopMatrix();
      ++K;
    }
    ++I;
  }
}

} // namespace Sear
