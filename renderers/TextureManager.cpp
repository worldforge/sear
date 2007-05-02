// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall, University of Southampton

// $Id: TextureManager.cpp,v 1.52 2007-05-02 20:47:56 simon Exp $

#include <unistd.h>

#include <sigc++/object_slot.h>

#include <sage/sage.h>
#include <sage/GL.h>
#include <sage/GLU.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "src/System.h"
#include "src/Console.h"

#include "src/FileHandler.h"

#include "TextureManager.h"

#include "src/MediaManager.h"

#ifdef WINDOWS
    
int ilogb(double x)
{
    return static_cast<int>(_logb(x));
}
    
#endif

//#define WFUT_TEST

// Default texture maps
#include "default_image.xpm"
#include "default_font.h"
#include "cursors.h"

#include "Sprite.h"
#include "ImageUtils.h"

#ifdef DEBUG
static const bool debug = true;
#else
static const bool debug = false;
#endif
namespace Sear {

// Find the next largest power of 2 to i, but no bigger than the max texture
// size we are allowed
inline int scaleDimension(int i) {
  GLint texSize;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
  int n = 2;
  while (n < i && n <= texSize) {
    n <<= 1;
  }
  if (n > texSize) return texSize;
  else return n;
}

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
static const std::string KEY_base_texture_level = "base_texture_level";

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
static const std::string CMD_SET_TEXTURE_BASE_LEVEL = "set_texture_detail";
static const std::string CMD_dump_reference_count = "dump_reference_count";
static const std::string CMD_reload_config_textures = "reload_config_textures";
static const std::string CMD_reload_config_sprites = "reload_config_sprites";

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
  m_initGL(false),
  m_texture_counter(1),
  m_texture_units(1),
  m_baseMipmapLevel(0)
{  
  varconf::Config &cfg = System::instance()->getGeneral();
  cfg.sigsv.connect(sigc::mem_fun(this, &TextureManager::generalConfigChanged));
}

TextureManager::~TextureManager() {
  if (m_initialised) shutdown();
}

void TextureManager::init()
{
  assert(m_initialised == false);
  if (debug) std::cout << "Initialising TextureManager" << std::endl;

  m_texture_counter = 1;
  m_textures.resize(1); // we need to leave texture ID zero free
  m_names.resize(1); // ditto
  // Set some values to help with debugging!
  m_names[0] = "null_texture";
  m_textures[0] = 0;

  m_texture_map.clear();
 
  m_texture_config.sige.connect(sigc::mem_fun(this, &TextureManager::varconf_error_callback));
 
  m_initialised = true;
}

void TextureManager::contextCreated() {
  assert((m_initialised == true) && "TextureManager not initialised");
  assert(m_initGL == false);
  
  // Determine available OpenGL extensions
  setupGLExtensions();
  
  // Initialise our current texture cache
  m_last_textures.resize(m_texture_units);
  for (size_t i = 0; i < m_last_textures.size(); m_last_textures[i++] = -1);

  // Setup default texture properties
  m_default_texture = createDefaultTexture();
  if (m_default_texture == -1) std::cerr << "Error building default texture" << std::endl;

  // create default font
  m_default_font = createDefaultFont();
  if (m_default_font == -1) std::cerr << "Error building default font" << std::endl;

  m_cursor_ids.push_back(createCursor("cursor_default", arrow));
  m_cursor_ids.push_back(createCursor("cursor_pickup", pickup));
  m_cursor_ids.push_back(createCursor("cursor_touch", touch));
  m_cursor_ids.push_back(createCursor("cursor_use", use));
  m_cursor_ids.push_back(createCursor("cursor_attack", attack));

  m_initGL = true;
}

void TextureManager::shutdown()
{
  if (!m_initialised) return;
  if (debug) std::cout << "TextureManager: Shutdown" << std::endl;

  releaseTextureID(m_default_texture);
  releaseTextureID(m_default_font);

  for (size_t i = 0; i < m_cursor_ids.size(); ++i) {
    releaseTextureID(m_cursor_ids[i]);
  }

  ReferenceCounter::const_iterator I = m_ref_counter.begin();
  ReferenceCounter::const_iterator Iend = m_ref_counter.end();
  while (I != Iend) {
    printf("%s (%d) -> %d\n", getTextureName(I->first).c_str(), I->first,I->second);
    ++I;
  }

  assert(m_ref_counter.empty());

  // Unload all textures
  for (size_t i = 1; i < m_textures.size(); ++i) {
    unloadTexture(m_textures[i]);
  }

  // Clean up sprites
  while (!m_sprites.empty()) {
    delete m_sprites.begin()->second;
    m_sprites.erase(m_sprites.begin());
  }

  m_last_textures.clear();
  m_initialised = false;
}

void TextureManager::readTextureConfig(const std::string &filename) {
  assert((m_initialised == true) && "TextureManager not initialised");
  
  m_texture_config.readFromFile(filename);
}

GLuint TextureManager::loadTexture(const std::string &texture_name) {
  assert((m_initialised == true) && "TextureManager not initialised");
  // TODO: names should be cleaned by this point already!
  std::string clean_name(texture_name);
  
  bool mask = false;
  if (clean_name.substr(0, 5) == "mask_") {
    mask = true;
    clean_name = clean_name.substr(5);
  }
 
  m_texture_config.clean(clean_name);
  // Check texture is defined
  if (!m_texture_config.find(clean_name)) {
    fprintf(stderr, "Texture %s (%s) not defined.\n", texture_name.c_str(), clean_name.c_str());
    return 0;
  }
 
  if (!m_texture_config.findItem(clean_name, KEY_filename)) {
    fprintf(stderr, "Texture %s has no filename(clean name).\n", clean_name.c_str());
    return 0;
  }

  std::string filename = (std::string)m_texture_config.getItem(clean_name, KEY_filename);
#ifdef WFUT_TEST
  std::string fname = filename.substr(16);
  MediaManager::MediaStatus status = System::instance()->getMediaManager()->checkFile(fname, MediaManager::MEDIA_TEXTURE);

  switch (status) {
    case MediaManager::STATUS_OK:
    {
      printf("Local file is up-to-date: %s\n", filename.c_str());
      break;
    }
    case MediaManager::STATUS_USE_OLD:
    case MediaManager::STATUS_USE_DEFAULT:
    {
      printf("File is being updated, using existing version for now\n");
      if (m_pending_updates.find(filename) == m_pending_updates.end()) {
        printf("Adding to pending update: %s\n", fname.c_str());
        m_pending_updates[filename] = 0;
      }
      // Return default texture
      return 0;
      break;
    }
    case MediaManager::STATUS_UNKNOWN_FILE:    
    default:
    {
      printf("Status default or unknown file\n");
    }
  }
  printf(">>>>>>>>> %s\n", texture_name.c_str());
#endif
  // Perhaps this stage should be in a different function?
  System::instance()->getFileHandler()->getFilePath(filename);
  SDL_Surface* image = loadImageFromPath(filename);
  if (!image) return 0;
  
  GLuint texture_id = loadTexture(clean_name, image, mask);
  SDL_FreeSurface(image);
  return texture_id;
}

GLuint TextureManager::loadTexture(const std::string &name, SDL_Surface *surface, bool mask)
{
  assert((m_initialised == true) && "TextureManager not initialised");
  // Copy name so we can change it
  // TODO: SHould already be cleaned!
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
      for (int i = 0; i < surface->w * surface->h * surface->format->BytesPerPixel; i += surface->format->BytesPerPixel) {
        for (int j = 0; j < surface->format->BytesPerPixel; ++j) {
          ((unsigned char *)surface->pixels)[i + j] = (unsigned char)0xff;
        }
      }
    }
  }


// */ 
  // Create open gl texture
  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  
  // build image - use mip mapping if requested
  bool mipmap = true;
  if (m_texture_config.findItem(texture_name, KEY_mipmap)) {
    mipmap = (bool)m_texture_config.getItem(texture_name, KEY_mipmap);
  }

  // Set texture filters
  int minFilter = GL_LINEAR;
  if (m_texture_config.findItem(texture_name, KEY_min_filter)) {
    minFilter = getFilter((std::string)m_texture_config.getItem(texture_name, KEY_min_filter));
  } else if (mipmap) {
    minFilter = GL_LINEAR_MIPMAP_LINEAR; // trilinear
  }
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

  if (m_texture_config.findItem(texture_name, KEY_mag_filter)) {
    int filter = getFilter((std::string) m_texture_config.getItem(texture_name, KEY_mag_filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, DEFAULT_mag_filter);
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
      if (sage_ext[GL_EXT_bgra]) {
        format = (bpp == 24) ? GL_BGR_EXT : GL_BGRA_EXT;
      } else {
        fprintf(stderr, "Unsupported sprite format bgr. No GL_EXT_bgra extension\n");
      }
    } else {
        format = (bpp == 24) ? GL_RGB : GL_RGBA;
    }
    
    if (bpp == 8) format = GL_ALPHA;
    
    if (m_texture_config.findItem(texture_name, KEY_internal_format)) {
      fmt = getFormat((std::string)m_texture_config.getItem(texture_name, KEY_internal_format));
    } else {
        switch (bpp) {
        case 32:    fmt = GL_RGB5_A1; break;
        case 24:    fmt = GL_RGB5; break;
        case 8:     fmt = GL_ALPHA; break; // GL_LUMINANCE?
        default:
            std::cerr << "unsupported image depth" << std::endl;
            fmt = GL_RGB5_A1; break; // Set default
        }
    }
// TODO Create this in a new SDL_surface, and assign in surface so the mipmap stuff will work below 
  int width = scaleDimension(surface->w);
  int height = scaleDimension(surface->h);

  // Scale the image to a 2^N x 2^M size that is within the size allowed by GL
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  bool free_surface = false;
  if  (width != surface->w || height != surface->h) {

    SDL_Surface *newSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

    gluScaleImage(format, surface->w, surface->h, GL_UNSIGNED_BYTE, surface->pixels, newSurface->w, newSurface->h, GL_UNSIGNED_BYTE, newSurface->pixels);

//    SDL_FreeSurface(surface);
    surface = newSurface;
    free_surface = true;
  }

  if (mipmap) {
    if (m_baseMipmapLevel == 0)
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
    
    SDL_Surface* mip = surface;
    int max = ilogb(std::max(surface->w, surface->h));
    
    /* mipmap generation loop. This is pretty straight forward, one 
    complication is we need to free previous mipmaps, but not free the
    original surface (level 0), becuase that will be freed by the caller. */
    for (int level = 1; level <= max; ++level) {
        SDL_Surface* newMip = mipmapSurface(mip);
        if (!newMip) {
            std::cerr << "failed to created mipmap at level " << (level + 1) << std::endl;
            break;
        }
        
        if (mip != surface) SDL_FreeSurface(mip);
        mip = newMip;
        
        if (level >= m_baseMipmapLevel) {
            /* the -m_baseMipmapLevel term here is because ATI drivers seem
            to break if level zero isn't defined. So we shift all the mipmaps
            down if baseMipmapLevel > 0 */
            glTexImage2D(GL_TEXTURE_2D, level - m_baseMipmapLevel, fmt, mip->w, mip->h, 0, format, GL_UNSIGNED_BYTE, mip->pixels);
            GLenum er;
            if ((er = glGetError()) != 0) {
                std::cerr << "Texture \"" << texture_name
                          << "\" failed to load with error: "
                          << gluErrorString(er)
                          << std::endl << std::flush;
            }

        }
    } // of mipmap generation loop
    
    // don't leak the final mipmap
    if (mip != surface) SDL_FreeSurface(mip);
  } else {
    //glTexImage2D(GL_TEXTURE_2D, 0, fmt, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
  }

  // Set texture priority if requested
  if (m_texture_config.findItem(texture_name, KEY_priority)) {
    float priority = (double)m_texture_config.getItem(texture_name, KEY_priority);
    glPrioritizeTextures(1, &texture_id, &priority);
  }
  
  if (free_surface) SDL_FreeSurface(surface);
  return texture_id;
}


void TextureManager::unloadTexture(const std::string &texture_name)
{
  assert((m_initialised == true) && "TextureManager not initialised");
  unsigned int texture_id = m_texture_map[texture_name];
  unloadTexture(m_textures[texture_id]);
  m_textures[texture_id] = 0;
//  m_texture_map[texture_name] = 0;
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
    const std::string &tex_name = m_names[texture_id];
#ifdef WFUT_TEST
    UpdatesMap::const_iterator I = m_pending_updates.find(tex_name);
    if (I != m_pending_updates.end()) {
      to = I->second;
    } else {
#endif
      to = loadTexture(tex_name);
      if (to == 0) {
        fprintf(stderr,"Cannot find %s ID %d\n", tex_name.c_str(), texture_id);
        to = m_textures[m_default_texture];
      }
      m_textures[texture_id] = to;
#ifdef WFUT_TEST
    }
#endif
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
  glActiveTextureARB(GL_TEXTURE0_ARB + texture_unit);

  switchTexture(texture_id);
/*
  GLuint to = (texture_id == -1) ? (m_textures[m_default_texture]) : (m_textures[texture_id]);
  if (to == 0) {
    to = loadTexture(m_names[texture_id]);
    if (to == 0) {
      std::cerr << "Cannot find " << m_names[texture_id] << std::endl;
      to = m_textures[m_default_texture];
    }
    m_textures[texture_id] = to;
  }
  glBindTexture(GL_TEXTURE_2D, to);
  m_last_textures[texture_unit] = to;

*/
  // Make sure we are at texture unit 0
  glActiveTextureARB(GL_TEXTURE0_ARB);
}

void TextureManager::generalConfigChanged(const std::string &section, const std::string &key, varconf::Config &config)
{
    assert((m_initialised == true) && "TextureManager not initialised");

    if (section != "graphics") return;
    
    if (key == KEY_base_texture_level) {
        m_baseMipmapLevel = config.getItem(section, key);
        std::cout << "set base mipmap level to " << m_baseMipmapLevel << std::endl;
        
        // restart stuff as necessary
        if (m_initGL) {
            contextDestroyed(true);
            contextCreated();
        }
    }
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

void TextureManager::setupGLExtensions()
{
  assert((m_initialised == true) && "TextureManager not initialised");
  
  // Get number of texture units
  // We write to a tempory as m_texture_units is a uint
  GLint tex_units;
  glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &tex_units);
  m_texture_units = tex_units;
  
  if (debug) std::cout << m_texture_units << " texture units" << std::endl;
  
  use_arb_multitexture = sage_ext[GL_ARB_MULTITEXTURE];
  if (debug && use_arb_multitexture) std::cout << "Using arb_multitexture" << std::endl;
  
  use_sgis_generate_mipmap = sage_ext[GL_SGIS_GENERATE_MIPMAP];
  if (debug && use_sgis_generate_mipmap) std::cout << "Using GL_SGIS_GENERATE_MIPMAP" << std::endl;
  
  use_ext_texture_filter_anisotropic = sage_ext[GL_EXT_TEXTURE_FILTER_ANISOTROPIC];
  use_arb_texture_border_clamp = sage_ext[GL_ARB_TEXTURE_BORDER_CLAMP];

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

  TextureID texId = requestTextureID(texture_name, false);
  m_textures[texId] = texture;
  return texId;
}

TextureID TextureManager::createDefaultFont() {
  assert((m_initialised == true) && "TextureManager not initialised");
  std::string texture_name = "default_font";

  GLuint texture;

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, texture_font_internalFormat,
               texture_font_width, texture_font_height, 0,
               texture_font_format, GL_UNSIGNED_BYTE, texture_font_pixels);
  if (glGetError() != 0) {
      std::cerr << "Failed to load font texture" << std::endl << std::flush;
  }
  GLfloat priority = 1.0f;
  glPrioritizeTextures(1, &texture, &priority);

  TextureID texId = requestTextureID(texture_name, false);
  m_textures[texId] = texture;
  return texId;
}

TextureID TextureManager::createCursor(const std::string &texture_name, const char **xpm) {
  assert((m_initialised == true) && "TextureManager not initialised");
  unsigned int width, height;
  GLuint texture;

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  unsigned char *data = xpm_to_image((const char**)xpm, width, height);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  delete [] data;

  if (glGetError() != 0) {
      std::cerr << "Failed to cursor touch texture" << std::endl << std::flush;
  }
  GLfloat priority = 1.0f;
  glPrioritizeTextures(1, &texture, &priority);

  TextureID texId = requestTextureID(texture_name, false);
  m_textures[texId] = texture;

  return texId;
}


void TextureManager::registerCommands(Console *console) {
  assert(m_initialised == true);
  assert(console);

  console->registerCommand(CMD_LOAD_TEXTURE_CONFIG, this);
  console->registerCommand(CMD_LOAD_SPRITE_CONFIG, this);
  console->registerCommand(CMD_SET_TEXTURE_BASE_LEVEL, this);
  console->registerCommand(CMD_dump_reference_count, this);
  console->registerCommand(CMD_reload_config_textures, this);
  console->registerCommand(CMD_reload_config_sprites, this);
}

void TextureManager::runCommand(const std::string &command, const std::string &arguments) {
  assert((m_initialised == true) && "TextureManager not initialised");
  if (command == CMD_LOAD_TEXTURE_CONFIG) {
    std::string a = arguments;
    m_texture_configs.push_back(a);
    System::instance()->getFileHandler()->getFilePath(a);
    readTextureConfig(a);
  }
  else 
  if (command == CMD_LOAD_SPRITE_CONFIG) {
    std::string a = arguments;
    m_sprite_configs.push_back(a);
    System::instance()->getFileHandler()->getFilePath(a);
    if (debug) std::cout << "reading sprite config at " << a << std::endl;
    m_spriteConfig.readFromFile(a);
  }
  
  else 
  if (command == CMD_SET_TEXTURE_BASE_LEVEL) {
    int level = strtol(arguments.c_str(), NULL, 0);
    if ((level < 0) || (level > 10)) {
        std::cerr << CMD_SET_TEXTURE_BASE_LEVEL << " specified bad level: " << level << std::endl;
        return;
    }
    
    varconf::Config& cfg(System::instance()->getGeneral());
    cfg.setItem("graphics", KEY_base_texture_level, level);
  }
  else 
  if (command == CMD_dump_reference_count) {
    ReferenceCounter::const_iterator I = m_ref_counter.begin();
    ReferenceCounter::const_iterator Iend = m_ref_counter.end();
    while (I != Iend) {
      printf("Texture: %s Count: %d\n", getTextureName(I->first).c_str(), I->second);
      ++I;
    }
  }
  else 
  if (command == CMD_reload_config_textures) {
    m_texture_config = varconf::Config();
    m_texture_config.sige.connect(sigc::mem_fun(this, &TextureManager::varconf_error_callback));
    std::list<std::string>::const_iterator I = m_texture_configs.begin();
    std::list<std::string>::const_iterator Iend = m_texture_configs.end();
    while (I != Iend) {
      std::string a = *I++;
      System::instance()->getFileHandler()->getFilePath(a);
      m_texture_config.readFromFile(a);
    }
  }
  else 
  if (command == CMD_reload_config_sprites) {
    m_spriteConfig = varconf::Config();
    std::list<std::string>::const_iterator I = m_sprite_configs.begin();
    std::list<std::string>::const_iterator Iend = m_sprite_configs.end();
    while (I != Iend) {
      std::string a = *I++;
      System::instance()->getFileHandler()->getFilePath(a);
      m_spriteConfig.readFromFile(a);
    }
  }
}

void TextureManager::contextDestroyed(bool check)
{
  assert((m_initialised == true) && "TextureManager not initialised");
  assert(m_initGL);

  for (size_t i = 0; i < m_cursor_ids.size(); ++i) {
    releaseTextureID(m_cursor_ids[i]);
  }
  
  // unload textures first.
  for (unsigned int i = 0; i < m_textures.size(); ++i) {
    // Unload texture if its still valid
    if (check  && glIsTexture(m_textures[i])) {
      glDeleteTextures(1, &m_textures[i]);
    }
    m_textures[i] = 0;
  }

  SpriteInstanceMap::iterator S = m_sprites.begin();
  SpriteInstanceMap::const_iterator Send = m_sprites.end();

  for (; S != Send; ++S) {
    S->second->contextDestroyed(check);
  }
  m_initGL = false;
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

void TextureManager::varconf_error_callback(const char *error) {
  printf("Varconf Error: %s\n ", error);
}


} /* namespace Sear */
