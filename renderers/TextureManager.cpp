// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall, University of Southampton

// $Id: TextureManager.cpp,v 1.8 2003-06-11 23:07:57 simon Exp $

#include "TextureManager.h"

#include <sage/sage.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "SDL.h"

#include "common/Utility.h"

#include "src/System.h"
#include "src/Console.h"

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
static const bool debug = true;
#endif

namespace Sear {

// Config section name
static const std::string SECTION_texture_manager = "texture_manager";
static const std::string SECTION_render = "render";

// config keys
static const std::string KEY_filename = "filename";
static const std::string KEY_path = "path";
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

static const std::string CMD_LOAD_TEXTURE_CONFIG = "load_textures";	

bool use_arb_multitexture = false;
bool use_sgis_generate_mipmap = false;
bool use_arb_texture_border_clamp = false;
bool use_ext_texture_filter_anisotropic = false;

TextureManager::TextureManager() :
  _initialised(false),
  _texture_counter(1),
  _last_textures(NULL),
  _texture_units(1)
{  
  _texture_config.sigsv.connect(SigC::slot(*this, &TextureManager::varconf_callback));
  _texture_config.sige.connect(SigC::slot(*this, &TextureManager::varconf_error_callback));
}

void TextureManager::init() {
  if (_initialised) shutdown();
  if (debug) std::cout << "Initialising TextureManager" << std::endl;
  readConfig(System::instance()->getGeneral());
  _textures.resize(256);
  _names.resize(256);
//  _texture_config = System::instance()->getTexture();
  setupGLExtensions();
  // Create _last_textures
  _last_textures = new TextureID[_texture_units];
  for (unsigned int i = 0; i < _texture_units;_last_textures[i++] = -1);
  // Setup default texture properties
  _default_texture = createDefaultTexture();
  if (_default_texture == -1) std::cerr << "Error building default texture" << std::endl;
  TextureID _default_font = createDefaultFont();
  
  if (_default_font == -1) std::cerr << "Error building default font" << std::endl;
  // setup default font properties

  // create default texture

  // create default font
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

  if (_last_textures) {
    delete [] _last_textures;
    _last_textures = NULL;
  }

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
//  varconf::Config config;
  
  _texture_config.readFromFile(filename);
}

TextureObject TextureManager::loadTexture(const std::string &texture_name) {
  assert((_initialised == true) && "TextureManager not initialised");
  std::string clean_name = std::string(texture_name);
  _texture_config.clean(clean_name);
  if (!_texture_config.find(clean_name)) {
    std::cerr << "Texture " << texture_name << " not defined." << std::endl;
    return 0;
  }
  // Check if the texture has a filename specified
  if (!_texture_config.findItem(clean_name, KEY_filename)) {
    std::cerr << "Error " << texture_name << " has no filename" << std::endl;
    return 0;
  }
  std::string filename = (std::string)_texture_config.getItem(clean_name, KEY_filename);
  std::string path = (std::string)_texture_config.getItem(clean_name, KEY_path);
  if (!path.empty()) {
    filename = path + "/" + filename;
  }
  
  // Load texture into memory
  SDL_Surface *surface = System::loadImage(filename);
  if (!surface) {
    std::cerr << "Error loading texture: " << filename << std::endl;
    return 0;
  }
  TextureObject texture_id = loadTexture(clean_name, surface);
  // Free image
  SDL_FreeSurface(surface);
  // store into texture array
//  _textures[_texture_counter] = texture_id;
  // assign name to texture array loc
//  _texture_map[texture_name] = _texture_counter;
return texture_id;
//  return _texture_counter++;
}

TextureObject TextureManager::loadTexture(const std::string &name, SDL_Surface *surface) {
  std::string texture_name(name);
  _texture_config.clean(texture_name);
  // If we have requested a mask, filter pixels
/*  bool mask = (bool)_texture_config.getItem(texture_name, KEY_mask);
  mask = false;
  if (mask) {
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
 */ 
  // Create open gl texture
  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  // Set texture filters
  std::string min_filter = (std::string)_texture_config.getItem(texture_name, KEY_min_filter);
  if (min_filter.empty()) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, DEFAULT_min_filter);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getFilter(min_filter));
  }
  std::string mag_filter = (std::string)_texture_config.getItem(texture_name, KEY_mag_filter);
  if (mag_filter.empty()) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, DEFAULT_mag_filter);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getFilter(mag_filter));
  }
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
  // Check to see if anisotropic filtering is available
  if (use_ext_texture_filter_anisotropic) {
    GLfloat largest_supported_anisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
  }
  // TODO make this support more texture formats
//  int type = (int)_texture_config.getItem(texture_name, KEY_type);
  int depth = surface->format->BytesPerPixel;
  switch (depth) {
    case 1: depth = GL_ALPHA; break;
    case 2: depth = GL_ALPHA; break;
    case 3: depth = GL_RGB; break;
    case 4: depth = GL_RGBA; break;
    default: depth = GL_RGBA; break;
  }

  // build image - use mip mapping if requested
  bool mipmap = (bool)_texture_config.getItem(texture_name, KEY_mipmap);
  if (mipmap) {
    if (use_sgis_generate_mipmap) {
      glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
      glTexImage2D(GL_TEXTURE_2D, 0, depth, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    } else {
      gluBuild2DMipmaps(GL_TEXTURE_2D, depth, surface->w, surface->h, depth, GL_UNSIGNED_BYTE, surface->pixels);
    }
  } else {
   glTexImage2D(GL_TEXTURE_2D, 0, depth, surface->w, surface->h, 0, depth, GL_UNSIGNED_BYTE, surface->pixels);
  }

  // Set texture priority if requested
  if (_texture_config.findItem(texture_name, KEY_priority)) {
    float priority = (double)_texture_config.getItem(texture_name, KEY_priority);
    glPrioritizeTextures(1, &texture_id, &priority);
  }
  return texture_id;
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
  if (texture_id == _last_textures[0]) return;
  TextureObject to = _textures[texture_id];
  if (to == 0) {
    _texture_config.clean(_names[texture_id]);
    to = loadTexture(_names[texture_id]);
    if (to == 0) {
      std::cerr << "Cannot find " << _names[texture_id] << std::endl;
      to = _textures[_default_texture];
    }
    _textures[texture_id] = to;
  }
  glBindTexture(GL_TEXTURE_2D, to);
  _last_textures[0] = texture_id;  
}

void TextureManager::switchTexture(unsigned int texture_unit, TextureID texture_id) {
  assert((_initialised == true) && "TextureManager not initialised");
  if (texture_id == _last_textures[texture_unit]) return;
  if (texture_id == -1) texture_id = _default_texture;
  if (!use_arb_multitexture) return switchTexture(texture_id);
  if (texture_unit >= _texture_units) return; // Check we have enough texture units
  TextureObject to = _textures[texture_id];
  if (to == 0) {
    to = loadTexture(_names[texture_id]);
    if (to == 0) {
      std::cerr << "Cannot find " << _names[texture_id] << std::endl;
      to = _textures[_default_texture];
    }
    _textures[texture_id] = to;
  }
  glActiveTextureARB(GL_TEXTURE0_ARB + texture_unit);
  glBindTexture(GL_TEXTURE_2D, to);
  _last_textures[texture_unit] = to;
  // Make sure we are at texture unit 0
  glActiveTextureARB(GL_TEXTURE0_ARB);
}

void TextureManager::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  if (!config.findItem(section, KEY_path)) {
    if (debug) std::cout << "New Texture: " << section << std::endl;
    char cwd[256];
    memset(cwd, '\0', 256);
    getcwd(cwd, 355);
    config.setItem(section, KEY_path, cwd);
  }
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
  // We write to a tempory as _texture_units is a uint
  int tex_units;
  glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &tex_units);
  _texture_units = tex_units;
  std::cout << "We have " << _texture_units << " texture units" << std::endl;
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

TextureID TextureManager::createDefaultTexture() {
  std::string texture_name = "default_texture";
  // Load texture into memory
  /*
  _texture_config.setItem(texture_name, KEY_mask, false);
  _texture_config.setItem(texture_name, KEY_min_filter, FILTER_LINEAR);
  _texture_config.setItem(texture_name, KEY_mag_filter, FILTER_LINEAR);
  _texture_config.setItem(texture_name, KEY_clamp, false);
  _texture_config.setItem(texture_name, KEY_mipmap, false);
  
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
TextureObject texture;
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
  _textures[_texture_counter] = texture;
  // assign name to texture array loc
  _texture_map[texture_name] = _texture_counter;
  return _texture_counter++;
}

TextureID TextureManager::createDefaultFont() {
//  assert((_initialised == true) && "TextureManager not initialised");
  
  std::string texture_name = "default_font";
/*
  _texture_config.setItem(texture_name, KEY_mask, false);
  _texture_config.setItem(texture_name, KEY_min_filter, FILTER_LINEAR);
  _texture_config.setItem(texture_name, KEY_mag_filter, FILTER_LINEAR);
  _texture_config.setItem(texture_name, KEY_clamp, true);
  _texture_config.setItem(texture_name, KEY_mipmap, false);
 
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

  TextureObject texture;

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
  _textures[_texture_counter] = texture;
  // assign name to texture array loc
  _texture_map[texture_name] = _texture_counter;
  return _texture_counter++;
}


void TextureManager::registerCommands(Console *console) {
  if (debug) std::cout << "Registering commands" << std::endl;
  console->registerCommand(CMD_LOAD_TEXTURE_CONFIG, this);
}

void TextureManager::runCommand(const std::string &command, const std::string &arguments) {
  if (command == CMD_LOAD_TEXTURE_CONFIG) {
    readTextureConfig(arguments);
  }
}
} /* namespace Sear */
