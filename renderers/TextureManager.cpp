// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall, University of Southampton

// $Id: TextureManager.cpp,v 1.1 2003-03-11 20:58:59 simon Exp $

#include "TextureManager.h"

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

// condig keys
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


TextureManager::TextureManager() :
  _initialised(false),
  _texture_counter(1)
{  
  _texture_config.sigsv.connect(SigC::slot(*this, &TextureManager::varconf_callback));
  _texture_config.sige.connect(SigC::slot(*this, &TextureManager::varconf_error_callback));
}

void TextureManager::init() {
  if (_initialiased) shutdown();

  _initialised = true;
}

void TextureManager::shutdown() {
  // Unload all textures
  for (unsigned int i = 1; i < _texture_counter; ++i) {
    unloadTexture(_textures[i]);
  }
  _textures.clear();
  _texture_map.clear();
  _texture_counter = 1;
  _initialised = false;
}

void TextureManager::readConfig(varconf::Config &config) {
}

void TextureManager::readTextureConfig(const std::string &filename) {
  assert((_initialised == true) && "TextureManager not initialised");
  varconf::Config config;
  config.readFromFile(filename);
}

unsigned int TextureManager::loadTexture(const std::string &texture_name) {
  assert((_initialised == true) && "TextureManager not initialised");
  if (!_texture_config.find[texture_name]) {
    std::cerr << "Texture " << texture_name << " not defined." << std::endl;
    return 0;
  }
  // Read texture properties
  varconf::Variable v;
  if (!_texture_config.findItem(texture_name, KEY_filename)) {
    std::cerr << "Error " << texture_name << " has no filename" << std::endl;
    return 0;
  }
  std::string filename = (std::string)_texture_config.getItem(texture_name, KEY_filename);
  bool clamp = (bool)_texture_config.getItem(texture_name, KEY_clamp);
  bool mipmap = (bool)_texture_config.getItem(texture_name, KEY_mipmap);
  int type = (int)_texture_config.getItem(texture_name, KEY_type);
  int priority (int)_texture_config.getItem(texture_name, KEY_priority);
  bool mask = (bool)_texture_config.getItem(texture_name, KEY_mask);
  std::string min_filter = (std::string)config.getItem(texture_name, KEY_min_filter);
  std::string mag_filter = (std::string)config.getItem(texture_name, KEY_mag_filter);
  
  // Load texture into memory
  SDL_surface *tmp = NULL;
  tmp = System::loadImage(filename);
  if (!tmp) {
    std::cerr << "Error loading texture: " << filename << std::endl;
    return 0;
  }
  // If we have requested a mask, filter pixels
  if (mask) {
    // Set all picles to white. We let the alpha channel do the clipping
    // TODO perhaps define a transparent pixel or threshold to do this
    if (surface->format->BytesPerPixel == 4) {
      for (i = 0; i < surface->w * surface->h * 4; i += 4) {
        ((unsigned char *)surface->pixels)[i + 0] = (unsigned char)0xff;
        ((unsigned char *)surface->pixels)[i + 1] = (unsigned char)0xff;
        ((unsigned char *)surface->pixels)[i + 2] = (unsigned char)0xff;
      }
    } else {
      for (i = 0; i < surface->w * surface->h * 3; i += 3) {
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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getFilter(mag_filter));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getFilter(min_filter));
  
  // Set clamp or repeat
  if (clamp) {
    if (use_clamp_to_edge) {
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
    glGetFloatv(MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
    glTexParameterfv(GL_TEXTURE_2D, TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
  }
  // build image - use mip mapping if requested
  unsigned int depth = surface->format->BytesPerPixel;
  switch (depth) {
    case 3: depth = GL_RGB;
    case 4: depth = GL_RGBA;
  }
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
  SDL_FreeSurface(tmp);
  if (priority != 0.0f) {
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
  if (glIsTexture(texture_id)) {
    glDeleteTextures(1, &texture_id);
  }
}

void TextureManager::switchTexture(unsigned int 

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
  
} /* namespace Sear */
