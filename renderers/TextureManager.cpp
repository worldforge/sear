// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall, University of Southampton

// $Id: TextureManager.cpp,v 1.2 2003-03-11 22:44:34 simon Exp $

#include "TextureManager.h"

#ifndef __WIN32
#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include "SDL.h"

#include "common/Utility.h"

#include "src/System.h"

#ifdef __WIN32
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB  = NULL;
#endif 

#ifdef HAVE_CONFIG
#include "config.h"
#endif

#ifdef USE_MMGR
#include "common/mmgr.h"
  
#endif

#ifdef DEBUG
static const bool debug = true;
#else
static const bool debug = true;
#endif

namespace Sear {

// Config section name
static const std::string SECTION_texture_manager = "texture_manager";
static const std::string SECTION_render = "render";

// config keys
static const std::string KEY_filename = "filename";
static const std::string KEY_clamp = "clamp";
static const std::string KEY_mipmap = "mipmap";
static const std::string KEY_type = "type";
static const std::string KEY_priority = "priority";
static const std::string KEY_mask = "mask";
static const std::string KEY_min_filter = "min_filter";
static const std::string KEY_mag_filter = "mag_filter";

// config defaults
static const bool DEFAULT_clamp = false;
static const bool DEFAULT_mipmap = true;
static const bool mask = false;
static const unsigned int DEFAULT_min_filter = GL_LINEAR;
static const unsigned int DEFAULT_mag_filter = GL_NEAREST;

// Filter strings
static const std::string FILTER_NEAREST = "nearest";
static const std::string FILTER_LINEAR = "linear";
static const std::string FILTER_NEAREST_MIPMAP_NEAREST = "nearest_mipmap_nearest";
static const std::string FILTER_NEAREST_MIPMAP_LINEAR = "nearest_mipmap_linear";
static const std::string FILTER_LINEAR_MIPMAP_NEAREST = "linear_mipmap_nearest";
static const std::string FILTER_LINEAR_MIPMAP_LINEAR = "linear_mipmap_linear";
 
static const unsigned int MAX_TEXTURES = 8;
// Need to have MAX_TEXTURES of -1
#define TEXTURE_INIT { -1, -1, -1, -1, -1, -1, -1, -1 }

bool use_arb_multitexture = false;
bool use_sgis_generate_mipmap = false;
bool use_arb_texture_border_clamp = false;
bool use_ext_texture_filter_anisotropic = false;

TextureManager::TextureManager() :
  _initialised(false),
  _texture_counter(1)
{  
  _texture_config.sigsv.connect(SigC::slot(*this, &TextureManager::varconf_callback));
  _texture_config.sige.connect(SigC::slot(*this, &TextureManager::varconf_error_callback));
}

void TextureManager::init() {
  if (_initialised) shutdown();
  setupGLExtensions();
  _initialised = true;
}

void TextureManager::shutdown() {
  // Unload all textures
  for (unsigned int i = 1; i < _texture_counter; ++i) {
    unloadTexture(_textures[i]);
  }
  // Clear map and vector
  _textures.clear();
  _texture_map.clear();
  // Reset counter
  _texture_counter = 1;

  _initialised = false;
}

void TextureManager::readConfig(varconf::Config &config) {
  // Read in max number of textures for multitexturing
  // Read in pixel format settings

  // Read in default texture properties
}

void TextureManager::writeConfig(varconf::Config &config) {
}

void TextureManager::readTextureConfig(const std::string &filename) {
  assert((_initialised == true) && "TextureManager not initialised");
  varconf::Config config;
  config.readFromFile(filename);
}

int TextureManager::loadTexture(const std::string &texture_name) {
  assert((_initialised == true) && "TextureManager not initialised");
  if (!_texture_config.find(texture_name)) {
    std::cerr << "Texture " << texture_name << " not defined." << std::endl;
    return 0;
  }
  // Read texture properties
  if (!_texture_config.findItem(texture_name, KEY_filename)) {
    std::cerr << "Error " << texture_name << " has no filename" << std::endl;
    return 0;
  }
  std::string filename = (std::string)_texture_config.getItem(texture_name, KEY_filename);
  
  // Load texture into memory
  SDL_Surface *surface = System::loadImage(filename);
  if (!surface) {
    std::cerr << "Error loading texture: " << filename << std::endl;
    return 0;
  }
  // If we have requested a mask, filter pixels
  bool mask = (bool)_texture_config.getItem(texture_name, KEY_mask);
  if (mask) {
    // Set all picles to white. We let the alpha channel do the clipping
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
  
  // Create open gl texture
  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  // Set texture filters
  std::string min_filter = (std::string)_texture_config.getItem(texture_name, KEY_min_filter);
  std::string mag_filter = (std::string)_texture_config.getItem(texture_name, KEY_mag_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getFilter(mag_filter));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getFilter(min_filter));
  
  // Set clamp or repeat
  bool clamp = (bool)_texture_config.getItem(texture_name, KEY_clamp);
  if (clamp) {
    if (use_arb_texture_border_clamp) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    
    } else {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
  // Chcek to see if anisotropic filtering is available
  if (use_ext_texture_filter_anisotropic) {
    GLfloat largest_supported_anisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
  }
  // TODO make this support more texture formats
//  int type = (int)_texture_config.getItem(texture_name, KEY_type);
  unsigned int depth = surface->format->BytesPerPixel;
  switch (depth) {
    case 3: depth = GL_RGB;
    case 4: depth = GL_RGBA;
  }

  // build image - use mip mapping if requested
  bool mipmap = (bool)_texture_config.getItem(texture_name, KEY_mipmap);
  if (mipmap) {
    if (use_sgis_generate_mipmap) {
      glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
      glTexImage2D(GL_TEXTURE_2D, 0, depth, surface->w, surface->h, 0, depth, GL_UNSIGNED_BYTE, surface->pixels);
    } else {
      gluBuild2DMipmaps(GL_TEXTURE_2D, depth, surface->w, surface->h, depth, GL_UNSIGNED_BYTE, surface->pixels);
    }
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, depth, surface->w, surface->h, 0, depth, GL_UNSIGNED_BYTE, surface->pixels);
  }

  // Free image
  SDL_FreeSurface(surface);

  // Set texture priority if requested
  if (_texture_config.findItem(texture_name, KEY_priority)) {
    float priority = (double)_texture_config.getItem(texture_name, KEY_priority);
    glPrioritizeTextures(1, &texture_id, &priority);
  }

  // store into texture array
  _textures[_texture_counter] = texture_id;
  // assign name to texture array loc
  _texture_map[texture_name] = _texture_counter;

  return _texture_counter++;
}

void TextureManager::unloadTexture(const std::string &texture_name) {
  assert((_initialised == true) && "TextureManager not initialised");
  unsigned int texture_id = _texture_map[texture_name];
  unloadTexture(_textures[texture_id]);
  _textures[texture_id] = 0;
  _texture_map[texture_name] = 0;
}

void TextureManager::unloadTexture(unsigned int texture_id) {
  assert((_initialised == true) && "TextureManager not initialised");
  if (glIsTexture(texture_id)) glDeleteTextures(1, &texture_id);
}

void TextureManager::switchTexture(TextureID texture_id) {
  assert((_initialised == true) && "TextureManager not initialised");
  static TextureID last_texture_id = -1;
  if (texture_id == last_texture_id) return;
  
  glBindTexture(GL_TEXTURE_2D, _textures[texture_id]);
  last_texture_id = texture_id;  
}

void TextureManager::switchTexture(unsigned int texture_unit, TextureID texture_id) {
  assert((_initialised == true) && "TextureManager not initialised");
  // TODO make this more generic!
  static TextureID last_texture_ids[MAX_TEXTURES] = TEXTURE_INIT;
  if (!use_arb_multitexture) return switchTexture(texture_id);
  glActiveTextureARB(GL_TEXTURE0_ARB + texture_unit);
  glBindTexture(GL_TEXTURE_2D, _textures[texture_id]);
  last_texture_ids[texture_unit] = texture_id;
  // Make sure we are at texture unit 0
  glActiveTextureARB(GL_TEXTURE0_ARB);
}


void TextureManager::switchMultiTexture(TextureID *texture_ids, unsigned int num_textures) {
  assert((_initialised == true) && "TextureManager not initialised");
  assert((texture_ids != NULL) && "NULL textures");
  // TODO make this more generic!
  static TextureID last_texture_ids[MAX_TEXTURES] = TEXTURE_INIT;
  if (!use_arb_multitexture) return switchTexture(texture_ids[0]);

  for (unsigned int i = num_textures - 1; i >=0; --i) {
    // Check to see if texture is already loaded
    if (last_texture_ids[i] == texture_ids[i]) continue;
    glActiveTextureARB(GL_TEXTURE0_ARB + i);
    glBindTexture(GL_TEXTURE_2D, _textures[texture_ids[i]]);
    last_texture_ids[i] = texture_ids[i];
  }
  // Make sure we are at texture unit 0
  glActiveTextureARB(GL_TEXTURE0_ARB);
}

void TextureManager::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {

}


void TextureManager::varconf_error_callback(const char *message) {
  std::cerr << "Error reading texture config: " << message << std::endl;
}

unsigned int TextureManager::getFilter(const std::string &filter_name) {
  unsigned int filter = 0;
  if (filter_name == FILTER_NEAREST) filter = GL_NEAREST;
  else if (filter_name == FILTER_LINEAR) filter = GL_LINEAR;
  else if (filter_name == FILTER_NEAREST_MIPMAP_NEAREST) filter = GL_NEAREST_MIPMAP_NEAREST;
  else if (filter_name == FILTER_NEAREST_MIPMAP_LINEAR) filter = GL_NEAREST_MIPMAP_LINEAR;
  else if (filter_name == FILTER_LINEAR_MIPMAP_NEAREST) filter = GL_LINEAR_MIPMAP_NEAREST;
  else if (filter_name == FILTER_LINEAR_MIPMAP_LINEAR) filter = GL_LINEAR_MIPMAP_LINEAR;
  return filter;
}

void TextureManager::setupGLExtensions() {
  std::string extensions = string_fmt(glGetString(GL_EXTENSIONS));
  // Setup multitexturing extension	
  if (extensions.find("GL_ARB_multitexture") != std::string::npos) {
    use_arb_multitexture = true;
    if (debug) std::cout << "Using GL_ARB_multitexture Extension" << std::endl;
  } else {
    use_arb_multitexture = false;
  }
  #ifdef __WIN32 
  glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)SDL_GL_GetProcAddress("glActiveTextureARB");
  if (glActiveTextureARB == NULL) {
    std::cerr << "Error linking glActiveTextureARB - disabling multitexture support" << std::endl;
    use_arb_multitexture = false;
  }
  #endif

  if (extensions.find("GL_SGIS_generate_mipmap") != std::string::npos) {
    use_sgis_generate_mipmap = true;
    if (debug) std::cout << "Using GL_SGIS_generate_mipmap Extension" << std::endl;
  } else {
    use_sgis_generate_mipmap = false;
  }

  if (extensions.find("GL_EXT_texture_filter_anisotropic") != std::string::npos) {
    use_ext_texture_filter_anisotropic = true;
    if (debug) std::cout << "Using GL_EXT_texture_filter_anisotropic Extension" << std::endl;
  } else {
    use_ext_texture_filter_anisotropic = false;
  }
  
  if (extensions.find("GL_ARB_texture_border_clamp") != std::string::npos) {
    use_arb_texture_border_clamp = true;
    if (debug) std::cout << "Using GL_ARB_texture_border_clamp Extension" << std::endl;
  } else {
    use_arb_texture_border_clamp = false;
  }
}

void TextureManager::setScale(float scale) {
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(scale, scale, 1.0f);
  glMatrixMode(GL_MODELVIEW);
}

void TextureManager::setScale(float scale_x, float scale_y) {
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(scale_x, scale_y, 1.0f);
  glMatrixMode(GL_MODELVIEW);
}

void TextureManager::setScale(unsigned int texture_unit, float scale) {
  glActiveTextureARB(GL_TEXTURE0_ARB + texture_unit);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(scale, scale, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glActiveTextureARB(GL_TEXTURE0_ARB);
}

void TextureManager::setScale(unsigned int texture_unit, float scale_x, float scale_y) {
  glActiveTextureARB(GL_TEXTURE0_ARB + texture_unit);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(scale_x, scale_y, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glActiveTextureARB(GL_TEXTURE0_ARB);
}


} /* namespace Sear */

#if(0)

Uint32 System::getPixel(SDL_Surface *surface, int x, int y) {
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
  switch(bpp) {
    case 1:
      return *p;
    case 2:
      return *(Uint16 *)p;
    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        return p[0] << 16 | p[1] << 8 | p[2];
      else
        return p[0] | p[1] << 8 | p[2] << 16;
     case 4:
       return *(Uint32 *)p;
    default:
       return 0;       /* shouldn't happen, but avoids warnings */
  }
}


SDL_Surface *System::loadImage(const  std::string &filename) {
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


unsigned char *xpm_to_image(const char *image[], unsigned int &width, unsigned int &height) {
  unsigned int i, row, col;
  unsigned int num_colours = 0;
  unsigned int unknown = 0;
  unsigned int index = 0;
  sscanf(&image[0][0], "%u %u %u %u", &width, &height, &num_colours, &unknown);
  std::map<char, unsigned int> colour_map;
  for (index = 1; index <= num_colours; index++) {
    char code = image[index][0];
    std::string colour_name = std::string(image[index]).substr(4);
    unsigned int colour = 0;
    if (colour_name == "None") colour = 0x00000000;
    else {
      sscanf(colour_name.c_str(), "#%x", &colour);
      colour <<= 8;
      colour |= 0xFF;
    }
    colour_map[code] = colour;
  }
//  unsigned char *data = (unsigned char *)malloc(width * height * 4 * sizeof(char));
  unsigned char *data = new unsigned char [width * height * 4];
  i = 0;
  for ( row=0; row < height; ++row ) {
    for ( col=0; col < width; ++col ) {
      unsigned int colour = colour_map[image[height - row - 1 + index][col]];
      data[i++] = (colour & 0xFF000000) >> 24;
      data[i++] = (colour & 0x00FF0000) >> 16;
      data[i++] = (colour & 0x0000FF00) >> 8;
      data[i++] = (colour & 0x000000FF);
    }	
  } 
  return data;  
}
#endif
