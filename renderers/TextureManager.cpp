// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: TextureManager.cpp,v 1.18 2004-04-26 15:32:30 simon Exp $

#include "TextureManager.h"

#include <sage/GLU.h>

#ifdef __APPLE__
  extern SDL_Surface *IMG_Load(const char *);
#else
  #include <SDL/SDL_image.h>
#endif

#include "common/Log.h"
#include "common/Utility.h"

#include "src/System.h"
#include "src/Console.h"

#include "src/FileHandler.h"

#include <unistd.h>

// Default texture maps
#include "default_image.xpm"
#include "default_font.xpm"

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

// Config section name
static const std::string SECTION_texture_manager = "texture_manager";
static const std::string SECTION_render = "render";

// config keys
static const std::string KEY_filename = "filename";
static const std::string KEY_path = "path";
static const std::string KEY_clamp = "clamp";
static const std::string KEY_clamp_s = "clamp_s";
static const std::string KEY_clamp_t = "clamp_t";
static const std::string KEY_mipmap = "mipmap";
static const std::string KEY_type = "type";
static const std::string KEY_priority = "priority";
static const std::string KEY_mask = "mask";
static const std::string KEY_min_filter = "min_filter";
static const std::string KEY_mag_filter = "mag_filter";

// config defaults
static const bool DEFAULT_clamp = false;
static const bool DEFAULT_clamp_s = DEFAULT_clamp;
static const bool DEFAULT_clamp_t = DEFAULT_clamp;
static const bool DEFAULT_mipmap = true;
static const bool DEFAULT_mask = false;
static const unsigned int DEFAULT_min_filter = GL_LINEAR;
static const unsigned int DEFAULT_mag_filter = GL_LINEAR;
//static const unsigned int DEFAULT_mag_filter = GL_NEAREST;

// Filter strings
static const std::string FILTER_NEAREST = "nearest";
static const std::string FILTER_LINEAR = "linear";
static const std::string FILTER_NEAREST_MIPMAP_NEAREST = "nearest_mipmap_nearest";
static const std::string FILTER_NEAREST_MIPMAP_LINEAR = "nearest_mipmap_linear";
static const std::string FILTER_LINEAR_MIPMAP_NEAREST = "linear_mipmap_nearest";
static const std::string FILTER_LINEAR_MIPMAP_LINEAR = "linear_mipmap_linear";

static const std::string CMD_LOAD_TEXTURE_CONFIG = "load_textures";	

bool use_arb_multitexture = false;
bool use_sgis_generate_mipmap = false;
bool use_arb_texture_border_clamp = false;
bool use_ext_texture_filter_anisotropic = false;

TextureManager::TextureManager() :
  m_initialised(false),
  m_texture_counter(1),
  m_texture_units(1)
{  
  m_texture_config.sigsv.connect(SigC::slot(*this, &TextureManager::varconf_callback));
  m_texture_config.sige.connect(SigC::slot(*this, &TextureManager::varconf_error_callback));
}

void TextureManager::init() {
  if (m_initialised) shutdown();
  if (debug) std::cout << "Initialising TextureManager" << std::endl;
  readConfig(System::instance()->getGeneral());
  m_textures.resize(256);
  m_names.resize(256);

  m_initialised = true;
}

void TextureManager::initGL() {
  setupGLExtensions();
  m_last_textures.resize(m_texture_units);
  for (unsigned int i = 0; i < m_last_textures.size(); m_last_textures[i++] = -1);
  // Setup default texture properties
  m_default_texture = createDefaultTexture();
  if (m_default_texture == -1) std::cerr << "Error building default texture" << std::endl;

  // create default font
  TextureID m_default_font = createDefaultFont();
  if (m_default_font == -1) std::cerr << "Error building default font" << std::endl;
}

void TextureManager::shutdown() {
  // Unload all textures
  for (unsigned int i = 1; i < m_texture_counter; ++i) {
    unloadTexture(m_textures[i]);
  }
  // Clear map and vector
  m_textures.clear();
  m_texture_map.clear();
  // Reset counter
  m_texture_counter = 1;

  m_last_textures.clear();

  m_initialised = false;
}

void TextureManager::readConfig(varconf::Config &config) {
  // Read in max number of textures for multitexturing
  // Read in pixel format settings

  // Read in default texture properties
}

void TextureManager::writeConfig(varconf::Config &config) {
}

void TextureManager::readTextureConfig(const std::string &filename) {
  assert((m_initialised == true) && "TextureManager not initialised");
//  varconf::Config config;
  
  m_texture_config.readFromFile(filename);
}

GLuint TextureManager::loadTexture(const std::string &texture_name) {
  assert((m_initialised == true) && "TextureManager not initialised");
std::cout << "Loading Texture: " << texture_name << std::endl;
  std::string clean_name = std::string(texture_name);
  
  bool mask = false;
//  std::cout << clean_name << " - " << clean_name.substr(0,5) << " - " << clean_name.substr(5) <<  std::endl;
  if (clean_name.substr(0, 5) == "mask_") {
    mask = true;
    clean_name = clean_name.substr(5);
  }
 
  m_texture_config.clean(clean_name);
  if (!m_texture_config.find(clean_name)) {
    std::cerr << "Texture " << texture_name << " not defined." << std::endl;
    return 0;
  }
  // Check if the texture has a filename specified
  if (!m_texture_config.findItem(clean_name, KEY_filename)) {
    std::cerr << "Error " << texture_name << " has no filename" << std::endl;
    return 0;
  }
  std::string filename = (std::string)m_texture_config.getItem(clean_name, KEY_filename);
   
  std::string path = (std::string)m_texture_config.getItem(clean_name, KEY_path);
  if (!path.empty()) {
    filename = path + "/" + filename;
  }
 
  System::instance()->getFileHandler()->expandString(filename);
  // Load texture into memory
  SDL_Surface *surface = loadImage(filename);
  if (!surface) {
    std::cerr << "Error loading texture: " << filename << std::endl;
    return 0;
  }
  GLuint texture_id = loadTexture(clean_name, surface, mask);
  // Free image
  SDL_FreeSurface(surface);

  return texture_id;
}

GLuint TextureManager::loadTexture(const std::string &name, SDL_Surface *surface, bool mask) {
  std::string texture_name(name);
  m_texture_config.clean(texture_name);
  // If we have requested a mask, filter pixels
/*  bool mask = (bool)m_texture_config.getItem(texture_name, KEY_mask);
  mask = false;
*/  if (mask) {
    // Set all pixels to white. We let the alpha channel do the clipping
    // TODO perhaps define a transparent pixel or threshold to do this
    if (surface->format->BytesPerPixel == 4) {
      for (int i = 0; i < surface->w * surface->h * 4; i += 4) {
        ((unsigned char *)surface->pixels)[i + 0] = (unsigned char)0xff;
        ((unsigned char *)surface->pixels)[i + 1] = (unsigned char)0xff;
        ((unsigned char *)surface->pixels)[i + 2] = (unsigned char)0xff;
      }
    } else {
      for (int i = 0; i < surface->w * surface->h * 3; i += 3) {
        ((unsigned char *)surface->pixels)[i + 0] = (unsigned char)0xff;
        ((unsigned char *)surface->pixels)[i + 1] = (unsigned char)0xff;
        ((unsigned char *)surface->pixels)[i + 2] = (unsigned char)0xff;
      }
    }
  }
// */ 
  // Create open gl texture
  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  // Set texture filters
  std::string min_filter = (std::string)m_texture_config.getItem(texture_name, KEY_min_filter);
  if (min_filter.empty()) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, DEFAULT_min_filter);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getFilter(min_filter));
  }
  std::string mag_filter = (std::string)m_texture_config.getItem(texture_name, KEY_mag_filter);
  if (mag_filter.empty()) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, DEFAULT_mag_filter);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getFilter(mag_filter));
  }
  // Set clamp or repeat
  bool clamp = DEFAULT_clamp;
  bool clamp_s = DEFAULT_clamp_s;
  bool clamp_t = DEFAULT_clamp_t;
  if (m_texture_config.findItem(texture_name, KEY_clamp)) {
    clamp_s = clamp_t = clamp = (bool)m_texture_config.getItem(texture_name, KEY_clamp);
  }
  if (m_texture_config.findItem(texture_name, KEY_clamp_s)) {
    clamp_s = (bool)m_texture_config.getItem(texture_name, KEY_clamp_s);
  }
  if (m_texture_config.findItem(texture_name, KEY_clamp_t)) {
    clamp_t = (bool)m_texture_config.getItem(texture_name, KEY_clamp_t);
  }

  if (clamp_s) {
    if (use_arb_texture_border_clamp) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    } else {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    }
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  }

  if (clamp_t) {
    if (use_arb_texture_border_clamp) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    
    } else {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
  // Check to see if anisotropic filtering is available
  if (use_ext_texture_filter_anisotropic) {
    GLfloat largest_supported_anisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
  }


  // Taken from Apogee
  int format, fmt;
  int bpp = surface->format->BitsPerPixel;
                                                                                
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    if (surface->format->Rshift > surface->format->Bshift) {
#else // SDL_BYTEORDER == SDL_LIL_ENDIAN
    if (surface->format->Rshift < surface->format->Bshift) {
#endif // SDL_BYTEORDER == SDL_LIL_ENDIAN
        format = (bpp == 24) ? GL_BGR_EXT : GL_BGRA_EXT;
    } else {
        format = (bpp == 24) ? GL_RGB : GL_RGBA;
    }
//    fmt = (bpp == 24) ? 3 : 4;
    fmt = (bpp == 24) ? GL_RGB5 : GL_RGB5_A1;
/*
  // TODO make this support more texture formats
//  int type = (int)m_texture_config.getItem(texture_name, KEY_type);
  int depth = surface->format->BytesPerPixel;
  switch (depth) {
    case 1: depth = GL_ALPHA; break;
    case 2: depth = GL_ALPHA; break;
    case 3: depth = 3;break;//GL_RGB; break;
    case 4: depth = 4;break;//GL_RGBA; break;
    default: depth = GL_RGBA; break;
  }
*/
  // build image - use mip mapping if requested
  bool mipmap = (bool)m_texture_config.getItem(texture_name, KEY_mipmap);
  if (mipmap) {
    if (use_sgis_generate_mipmap) {
      glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
      glTexImage2D(GL_TEXTURE_2D, 0, fmt, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
    } else {
      gluBuild2DMipmaps(GL_TEXTURE_2D, fmt, surface->w, surface->h, format, GL_UNSIGNED_BYTE, surface->pixels);
    }
  } else {
   glTexImage2D(GL_TEXTURE_2D, 0, fmt, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
  }

  // Set texture priority if requested
  if (m_texture_config.findItem(texture_name, KEY_priority)) {
    float priority = (double)m_texture_config.getItem(texture_name, KEY_priority);
    glPrioritizeTextures(1, &texture_id, &priority);
  }
  return texture_id;
}


void TextureManager::unloadTexture(const std::string &texture_name) {
  assert((m_initialised == true) && "TextureManager not initialised");
  unsigned int texture_id = m_texture_map[texture_name];
  unloadTexture(m_textures[texture_id]);
  m_textures[texture_id] = 0;
  m_texture_map[texture_name] = 0;
}

void TextureManager::unloadTexture(unsigned int texture_id) {
  assert((m_initialised == true) && "TextureManager not initialised");
  if (glIsTexture(texture_id)) glDeleteTextures(1, &texture_id);
}

void TextureManager::switchTexture( TextureID texture_id) {
  assert((m_initialised == true) && "TextureManager not initialised");
  if (texture_id == m_last_textures[0]) return;
  GLuint to = m_textures[texture_id];
  if (to == 0) {
    m_texture_config.clean(m_names[texture_id]);
    to = loadTexture(m_names[texture_id]);
    if (to == 0) {
      std::cerr << "Cannot find " << m_names[texture_id] << " id " << texture_id <<  std::endl;
      to = m_textures[m_default_texture];
    }
    m_textures[texture_id] = to;
  }
  glBindTexture(GL_TEXTURE_2D, to);
  m_last_textures[0] = texture_id;  
}

void TextureManager::switchTexture(unsigned int texture_unit,  TextureID texture_id) {
  assert((m_initialised == true) && "TextureManager not initialised");
  if (texture_id == m_last_textures[texture_unit]) return;
//  if (texture_id == -1) texture_id = m_default_texture;
  if (!use_arb_multitexture) return switchTexture(texture_id);
  if (texture_unit >= m_texture_units) return; // Check we have enough texture units
  GLuint to = (texture_id == -1) ? (m_textures[m_default_texture]) : (m_textures[texture_id]);
  if (to == 0) {
    to = loadTexture(m_names[texture_id]);
    if (to == 0) {
      std::cerr << "Cannot find " << m_names[texture_id] << std::endl;
      to = m_textures[m_default_texture];
    }
    m_textures[texture_id] = to;
  }
  glActiveTextureARB(GL_TEXTURE0_ARB + texture_unit);
  glBindTexture(GL_TEXTURE_2D, to);
  m_last_textures[texture_unit] = to;
  // Make sure we are at texture unit 0
  glActiveTextureARB(GL_TEXTURE0_ARB);
}

void TextureManager::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
//  if (!config.findItem(section, KEY_path)) {
//    if (debug) std::cout << "New Texture: " << section << std::endl;
//    char cwd[256];
//    memset(cwd, '\0', 256);
//    getcwd(cwd, 355);
//    config.setItem(section, KEY_path, cwd);
//  }
}

void TextureManager::varconf_error_callback(const char *message) {
  std::cerr << "Error reading texture config: " << message << std::endl;
}

int TextureManager::getFilter(const std::string &filter_name) {
  int filter = -1;
  if (filter_name == FILTER_NEAREST) filter = GL_NEAREST;
  else if (filter_name == FILTER_LINEAR) filter = GL_LINEAR;
  else if (filter_name == FILTER_NEAREST_MIPMAP_NEAREST) filter = GL_NEAREST_MIPMAP_NEAREST;
  else if (filter_name == FILTER_NEAREST_MIPMAP_LINEAR) filter = GL_NEAREST_MIPMAP_LINEAR;
  else if (filter_name == FILTER_LINEAR_MIPMAP_NEAREST) filter = GL_LINEAR_MIPMAP_NEAREST;
  else if (filter_name == FILTER_LINEAR_MIPMAP_LINEAR) filter = GL_LINEAR_MIPMAP_LINEAR;
  assert (filter != -1);
  return filter;
}

void TextureManager::setupGLExtensions() {
  // Get number of texture units
  // We write to a tempory as m_texture_units is a uint
  int tex_units;
  glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &tex_units);
  m_texture_units = tex_units;
  std::cout << "We have " << m_texture_units << " texture units" << std::endl;
  use_arb_multitexture = sage_ext[GL_ARB_MULTITEXTURE];
  use_sgis_generate_mipmap = sage_ext[GL_SGIS_GENERATE_MIPMAP];
  use_ext_texture_filter_anisotropic = sage_ext[GL_EXT_TEXTURE_FILTER_ANISOTROPIC];
  use_arb_texture_border_clamp = true;//sage_ext[GL_ARB_TEXTURE_BORDER_CLAMP];
}

void TextureManager::setScale(float scale_x, float scale_y) {
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(scale_x, scale_y, 1.0f);
  glMatrixMode(GL_MODELVIEW);
}

void TextureManager::setScale(unsigned int texture_unit, float scale_x, float scale_y) {
  glActiveTextureARB(GL_TEXTURE0_ARB + texture_unit);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(scale_x, scale_y, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glActiveTextureARB(GL_TEXTURE0_ARB);
}

TextureID TextureManager::createDefaultTexture() {
  std::string texture_name = "default_texture";
  // Load texture into memory
  /*
  m_texture_config.setItem(texture_name, KEY_mask, false);
  m_texture_config.setItem(texture_name, KEY_min_filter, FILTER_LINEAR);
  m_texture_config.setItem(texture_name, KEY_mag_filter, FILTER_LINEAR);
  m_texture_config.setItem(texture_name, KEY_clamp, false);
  m_texture_config.setItem(texture_name, KEY_mipmap, false);
  
  SDL_Surface *surface = IMG_ReadXPMFromArray(default_image_xpm);
 
  if (!surface) {
    std::cerr << "Error loading default texture" << std::endl;
    return -1;
  }
  TextureObject texture = loadTexture(texture_name, surface);
  //  Free image
  SDL_FreeSurface(surface);
  */
  unsigned int width, height;
  GLuint texture;
  glGenTextures(1, &texture);
  unsigned char *data = xpm_to_image((const char**)default_image_xpm, width, height);

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  delete []data;


  // store into texture array
  m_textures[m_texture_counter] = texture;
  // assign name to texture array loc
  m_texture_map[texture_name] = m_texture_counter;
  return m_texture_counter++;
}

TextureID TextureManager::createDefaultFont() {
//  assert((m_initialised == true) && "TextureManager not initialised");
  
  std::string texture_name = "default_font";
/*
  m_texture_config.setItem(texture_name, KEY_mask, false);
  m_texture_config.setItem(texture_name, KEY_min_filter, FILTER_LINEAR);
  m_texture_config.setItem(texture_name, KEY_mag_filter, FILTER_LINEAR);
  m_texture_config.setItem(texture_name, KEY_clamp, true);
  m_texture_config.setItem(texture_name, KEY_mipmap, false);
 
  // Load texture into memory
  SDL_Surface *surface = IMG_ReadXPMFromArray(default_font_xpm);
  if (!surface) {
    std::cerr << "Error loading default font" << std::endl;
    return -1;
  }
  TextureObject texture = loadTexture(texture_name, surface);
  // Free image
  SDL_FreeSurface(surface);

*/

  GLuint texture;

  unsigned int width, height;

  unsigned char *data = xpm_to_image((const char**)default_font_xpm, width, height);
 glGenTextures(1, &texture);

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  GLfloat priority = 1.0f;
  glPrioritizeTextures(1, &texture, &priority);
  
  delete [] data;


  // store into texture array
  m_textures[m_texture_counter] = texture;
  // assign name to texture array loc
  m_texture_map[texture_name] = m_texture_counter;
  return m_texture_counter++;
}


void TextureManager::registerCommands(Console *console) {
  if (debug) std::cout << "Registering commands" << std::endl;
  console->registerCommand(CMD_LOAD_TEXTURE_CONFIG, this);
}

void TextureManager::runCommand(const std::string &command, const std::string &arguments) {
  if (command == CMD_LOAD_TEXTURE_CONFIG) {
    std::string a = arguments;
    System::instance()->getFileHandler()->expandString(a);
    readTextureConfig(a);
  }
}

void TextureManager::invalidate() {
  // TODO unload textures first.
  for (unsigned int i = 0; i < m_textures.size(); i++) {
    // Unload texture if its still valid
    if (glIsTexture(m_textures[i])) {
      glDeleteTextures(1, &m_textures[i]);
    }
    m_textures[i] = 0;
  }

  for (unsigned int U = 0; U < m_texture_units; ++U){
    m_last_textures[U] = -1;
  }
  setupGLExtensions();
  // Backup texture counter
  int texCount = m_texture_counter;
  // reset so default textures are assigned correctly
  m_texture_counter = 1;
  m_default_texture = createDefaultTexture();
  if (m_default_texture == -1) std::cerr << "Error building default texture" << std::endl;
  TextureID m_default_font = createDefaultFont();
  
  if (m_default_font == -1) std::cerr << "Error building default font" << std::endl;
  // restore texture counter
  m_texture_counter = texCount;
 
}

SDL_Surface *TextureManager::loadImage(const  std::string &filename) {
  Uint8 *rowhi, *rowlo;
  Uint8 *tmpbuf /*, tmpch*/;
  SDL_Surface *image;
  int i/*, j*/;
  image = IMG_Load(filename.c_str());
  if ( image == NULL ) {
    Log::writeLog(std::string("Unable to load ") + filename + std::string(": ") + string_fmt( SDL_GetError()), Log::LOG_ERROR);
    return(NULL);
  }
  /* GL surfaces are upsidedown and RGB, not BGR :-) */
  tmpbuf = (Uint8 *)malloc(image->pitch);
  if ( tmpbuf == NULL ) {
    Log::writeLog("Out of memory", Log::LOG_ERROR);
    return(NULL);
  }
  rowhi = (Uint8 *)image->pixels;
  rowlo = rowhi + (image->h * image->pitch) - image->pitch;
  for ( i=0; i<image->h/2; ++i ) {
    //commented out for use with png
/*
     for ( j=0; j<image->w; ++j ) {
       tmpch = rowhi[j*3];
       rowhi[j*3] = rowhi[j*3+2];
      rowhi[j*3+2] = tmpch;
       tmpch = rowlo[j*3];
      rowlo[j*3] = rowlo[j*3+2];
       rowlo[j*3+2] = tmpch;
     }
*/
    memcpy(tmpbuf, rowhi, image->pitch);
    memcpy(rowhi, rowlo, image->pitch);
    memcpy(rowlo, tmpbuf, image->pitch);
    rowhi += image->pitch;
    rowlo -= image->pitch;
  }
  free(tmpbuf);
  return(image);
}


} /* namespace Sear */
