// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: TextureManager.cpp,v 1.25 2004-05-23 21:28:36 jmt Exp $

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "TextureManager.h"

#include <sage/GL.h>
#include <sage/GLU.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "src/System.h"
#include "src/Console.h"

#include "src/FileHandler.h"

#include <unistd.h>

// Default texture maps
#include "default_image.xpm"
#include "default_font.xpm"


#ifdef USE_MMGR
#include "common/mmgr.h"
  
#endif

#include "Sprite.h"

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
static const std::string KEY_internal_format = "internal_format";

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
static const std::string CMD_LOAD_SPRITE_CONFIG = "load_sprites";

// Format strings
static const std::string ALPHA = "alpha";
static const std::string ALPHA4 = "alpha4";
static const std::string ALPHA8 = "alpha8";
static const std::string ALPHA12 = "alpha12";
static const std::string ALPHA16 = "alpha16";

static const std::string LUMINANCE = "luminance";
static const std::string LUMINANCE4 = "luminance4";
static const std::string LUMINANCE8 = "luminance8";
static const std::string LUMINANCE12 = "luminance12";
static const std::string LUMINANCE16 = "luminance16";

static const std::string LUMINANCE_ALPHA = "luminance_alpha";
static const std::string LUMINANCE4_ALPHA4 = "luminance4_alpha4";
static const std::string LUMINANCE6_ALPHA2 = "luminance6_alpha2";
static const std::string LUMINANCE8_ALPHA8 = "luminance8_alpha8";
static const std::string LUMINANCE12_ALPHA4 = "luminance12_alpha4";
static const std::string LUMINANCE12_ALPHA12 = "luminance12_alpha12";
static const std::string LUMINANCE16_ALPHA16 = "luminance16_alpha16";
  
static const std::string INTENSITY = "intensity";
static const std::string INTENSITY4 = "intensity4";
static const std::string INTENSITY8 = "intensity8";
static const std::string INTENSITY12 = "intensity12";
static const std::string INTENSITY16 = "intensity16";

static const std::string RGB = "rgb";
static const std::string R3_G3_B2 = "r3_g3_b2";
static const std::string RGB4 = "rgb4";
static const std::string RGB5 = "rgb5";
static const std::string RGB8 = "rgb8";
static const std::string RGB10 = "rgb10";
static const std::string RGB12 = "rgb12";
static const std::string RGB16 = "rgb16";
static const std::string RGBA = "rgba";
static const std::string RGBA2 = "rgba2";
static const std::string RGBA4 = "rgba4";
static const std::string RGB5_A1 = "rgb5_a1";
static const std::string RGBA8 = "rgba8";
static const std::string RGB10_A2 = "rgb10_a2";
static const std::string RGBA12 = "rgba12";
static const std::string RGBA16 = "rgba16";

// OpenGL Extension flags
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
  // Default size, can be bigger if required
  m_textures.resize(256);
  m_names.resize(256);

  m_initialised = true;
 
  //readConfig(System::instance()->getGeneral());
}

void TextureManager::initGL() {
  assert((m_initialised == true) && "TextureManager not initialised");
  // Determine available OpenGL extensions
  setupGLExtensions();
  // Initialise our current texture cache
  m_last_textures.resize(m_texture_units);
  for (unsigned int i = 0; i < m_last_textures.size(); m_last_textures[i++] = -1);
  // Setup default texture properties
  m_default_texture = createDefaultTexture();
  if (m_default_texture == -1) std::cerr << "Error building default texture" << std::endl;

  // create default font
  m_default_font = createDefaultFont();
  if (m_default_font == -1) std::cerr << "Error building default font" << std::endl;

}

void TextureManager::shutdown() {
  if (!m_initialised) return;
  if (debug) std::cout << "TextureManager: Shutdown" << std::endl;

  // Unload all textures
  for (int i = 1; i < m_texture_counter; ++i) {
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
  assert((m_initialised == true) && "TextureManager not initialised");
  
  // Read in max number of textures for multitexturing
  // Read in pixel format settings

  // Read in default texture properties
}

void TextureManager::writeConfig(varconf::Config &config) {
  assert((m_initialised == true) && "TextureManager not initialised");
  // Nothing to write here
}

void TextureManager::readTextureConfig(const std::string &filename) {
  assert((m_initialised == true) && "TextureManager not initialised");
  
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
  // Check texture is defined
  if (!m_texture_config.find(clean_name)) {
    std::cerr << "Texture " << texture_name << " not defined." << std::endl;
    return 0;
  }
  // Check if the texture has a filename specified
  if (!m_texture_config.findItem(clean_name, KEY_filename)) {
    std::cerr << "Error " << texture_name << " has no filename" << std::endl;
    return 0;
  }
  // Get filename of texture
  std::string filename = (std::string)m_texture_config.getItem(clean_name, KEY_filename);
  // Get path to prefix to filename 
  if (m_texture_config.findItem(clean_name, KEY_path)) {
    std::string path = (std::string)m_texture_config.getItem(clean_name, KEY_path);
    if (!path.empty()) {
      filename = path + "/" + filename;
    }
  }
  // Expand variables in filename
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
  assert((m_initialised == true) && "TextureManager not initialised");
  // Copy name so we can change it
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
    if (m_texture_config.findItem(texture_name, KEY_internal_format)) {
      fmt = getFormat((std::string)m_texture_config.getItem(texture_name, KEY_internal_format));
    } else {
      fmt = (bpp == 24) ? GL_RGB5 : GL_RGB5_A1;
    }
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

void TextureManager::unloadTexture(GLuint texture_id) {
  assert((m_initialised == true) && "TextureManager not initialised");
  if (glIsTexture(texture_id)) glDeleteTextures(1, &texture_id);
}

void TextureManager::switchTexture(TextureID texture_id) {
  assert((m_initialised == true) && "TextureManager not initialised");
  if (texture_id == m_last_textures[0]) return;
  GLuint to = m_textures[texture_id];
  if (to == 0) {
    m_texture_config.clean(m_names[texture_id]);
    to = loadTexture(m_names[texture_id]);
    if (to == 0) {
      std::cerr << "Cannot find " << m_names[texture_id] << " ID " << texture_id <<  std::endl;
      to = m_textures[m_default_texture];
    }
    m_textures[texture_id] = to;
  }
  glBindTexture(GL_TEXTURE_2D, to);
  m_last_textures[0] = texture_id;  
}

void TextureManager::switchTexture(unsigned int texture_unit, TextureID texture_id) {
  assert((m_initialised == true) && "TextureManager not initialised");
  if (texture_id == m_last_textures[texture_unit]) return;
//  if (texture_id == -1) texture_id = m_default_texture;
  if (!use_arb_multitexture) return switchTexture(texture_id);
  if ((int)texture_unit >= m_texture_units) return; // Check we have enough texture units
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
  assert((m_initialised == true) && "TextureManager not initialised");
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
  assert((m_initialised == true) && "TextureManager not initialised");
  
  // Get number of texture units
  // We write to a tempory as m_texture_units is a uint
  GLint tex_units;
  glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &tex_units);
  m_texture_units = tex_units;
  
  if (debug) std::cout << m_texture_units << " texture units" << std::endl;
  
  use_arb_multitexture = sage_ext[GL_ARB_MULTITEXTURE];
  use_sgis_generate_mipmap = sage_ext[GL_SGIS_GENERATE_MIPMAP];
  use_ext_texture_filter_anisotropic = sage_ext[GL_EXT_TEXTURE_FILTER_ANISOTROPIC];
  use_arb_texture_border_clamp = true;//sage_ext[GL_ARB_TEXTURE_BORDER_CLAMP];
}

void TextureManager::setScale(float scale_x, float scale_y) {
  assert((m_initialised == true) && "TextureManager not initialised");
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(scale_x, scale_y, 1.0f);
  glMatrixMode(GL_MODELVIEW);
}

void TextureManager::setScale(unsigned int texture_unit, float scale_x, float scale_y) {
  assert((m_initialised == true) && "TextureManager not initialised");
  glActiveTextureARB(GL_TEXTURE0_ARB + texture_unit);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(scale_x, scale_y, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glActiveTextureARB(GL_TEXTURE0_ARB);
}

TextureID TextureManager::createDefaultTexture() {
  assert((m_initialised == true) && "TextureManager not initialised");
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
  assert((m_initialised == true) && "TextureManager not initialised");
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
  assert((m_initialised == true) && "TextureManager not initialised");
  if (debug) std::cout << "Registering commands" << std::endl;
  console->registerCommand(CMD_LOAD_TEXTURE_CONFIG, this);
  console->registerCommand(CMD_LOAD_SPRITE_CONFIG, this);
}

void TextureManager::runCommand(const std::string &command, const std::string &arguments) {
  assert((m_initialised == true) && "TextureManager not initialised");
  if (command == CMD_LOAD_TEXTURE_CONFIG) {
    std::string a = arguments;
    System::instance()->getFileHandler()->expandString(a);
    readTextureConfig(a);
  }
  
  if (command == CMD_LOAD_SPRITE_CONFIG) {
    std::string a = arguments;
    System::instance()->getFileHandler()->expandString(a);
    std::cout << "reading sprite config at " << a << std::endl;
    m_spriteConfig.readFromFile(a);
  }

}

void TextureManager::invalidate() {
  assert((m_initialised == true) && "TextureManager not initialised");
  // TODO unload textures first.
  for (unsigned int i = 0; i < m_textures.size(); i++) {
    // Unload texture if its still valid
    if (glIsTexture(m_textures[i])) {
      glDeleteTextures(1, &m_textures[i]);
    }
    m_textures[i] = 0;
  }

  for (int U = m_last_textures.size() - 1; U >= 0; --U){
    m_last_textures[U] = -1;
  }
  
  for (SpriteInstanceMap::iterator S=m_sprites.begin(); S != m_sprites.end(); ++S) {
    S->second->invalidate();
  }
  
  setupGLExtensions();
  // Backup texture counter
  int texCount = m_texture_counter;
  // reset so default textures are assigned correctly
  m_texture_counter = m_default_texture;
  m_default_texture = createDefaultTexture();
  if (m_default_texture == -1) std::cerr << "Error building default texture" << std::endl;
  
  m_texture_counter = m_default_font;
  m_default_font = createDefaultFont();
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


GLint TextureManager::getFormat(const std::string &fmt) {
  if (fmt == ALPHA) return GL_ALPHA;
  if (fmt == ALPHA4) return GL_ALPHA4;
  if (fmt == ALPHA8) return GL_ALPHA8;
  if (fmt == ALPHA12) return GL_ALPHA12;
  if (fmt == ALPHA16) return GL_ALPHA16;

  if (fmt == LUMINANCE) return GL_LUMINANCE;
  if (fmt == LUMINANCE4) return GL_LUMINANCE4;
  if (fmt == LUMINANCE8) return GL_LUMINANCE8;
  if (fmt == LUMINANCE12) return GL_LUMINANCE12;
  if (fmt == LUMINANCE16) return GL_LUMINANCE16;

  if (fmt == LUMINANCE_ALPHA) return GL_LUMINANCE_ALPHA;
  if (fmt == LUMINANCE4_ALPHA4) return GL_LUMINANCE4_ALPHA4;
  if (fmt == LUMINANCE6_ALPHA2) return GL_LUMINANCE6_ALPHA2;
  if (fmt == LUMINANCE8_ALPHA8) return GL_LUMINANCE8_ALPHA8;
  if (fmt == LUMINANCE12_ALPHA4) return GL_LUMINANCE12_ALPHA4;
  if (fmt == LUMINANCE12_ALPHA12) return GL_LUMINANCE12_ALPHA12;
  if (fmt == LUMINANCE16_ALPHA16) return GL_LUMINANCE16_ALPHA16;
  
  if (fmt == INTENSITY) return GL_INTENSITY;
  if (fmt == INTENSITY4) return GL_INTENSITY4;
  if (fmt == INTENSITY8) return GL_INTENSITY8;
  if (fmt == INTENSITY12) return GL_INTENSITY12;
  if (fmt == INTENSITY16) return GL_INTENSITY16;

  if (fmt == RGB) return GL_RGB;
  if (fmt == R3_G3_B2) return GL_R3_G3_B2;
  if (fmt == RGB4) return GL_RGB4;
  if (fmt == RGB5) return GL_RGB5;
  if (fmt == RGB8) return GL_RGB8;
  if (fmt == RGB10) return GL_RGB10;
  if (fmt == RGB12) return GL_RGB12;
  if (fmt == RGB16) return GL_RGB16;
  if (fmt == RGBA) return GL_RGBA;
  if (fmt == RGBA2) return GL_RGBA2;
  if (fmt == RGBA4) return GL_RGBA4;
  if (fmt == RGB5_A1) return GL_RGB5_A1;
  if (fmt == RGBA8) return GL_RGBA8;
  if (fmt == RGB10_A2) return GL_RGB10_A2;
  if (fmt == RGBA12) return GL_RGBA12;
  if (fmt == RGBA16) return GL_RGBA16;
  assert(0);
  return 0;
}

SpriteData* TextureManager::getSpriteData(const std::string& name)
{
    assert(m_initialised);
    assert(!name.empty());
    
    SpriteInstanceMap::iterator S = m_sprites.find(name);
    if (S == m_sprites.end())
    {
        SpriteData* sd = new SpriteData(name);
        S = m_sprites.insert(S, SpriteInstanceMap::value_type(name, sd));
    }
    
    return S->second;
}

void TextureManager::clearLastTexture(unsigned int index)
{
    m_last_textures[index] = -1;
}

} /* namespace Sear */
