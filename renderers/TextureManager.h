// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: TextureManager.h,v 1.13 2004-05-19 17:52:20 simon Exp $

#ifndef SEAR_RENDER_TEXTUREMANAGER_H
#define SEAR_RENDER_TEXTUREMANAGER_H 1

#include <string>
#include <map>
#include <vector>
#include <cassert>
#include <SDL/SDL.h>
#include <sage/sage.h>
#include <sage/GL.h>

#include <sigc++/object_slot.h>
#include "interfaces/ConsoleObject.h"

#include <varconf/Config.h>

#include "RenderSystem.h"

/*
 TODO

 Add console commands
 - determine max_texture_units from gl query
 - read/write config values
 - add ability to resample image to 2^N by 2^M
 - allow more formats thanb just RGB and RGBA
 - work in image loaders
 - create default textures

  */ 
namespace Sear {

//  typedef unsigned int TextureObject; ///< OpenGL reference to texture
//  typedef int TextureID; ///< TextureManager reference to texture
/**
 * This class handles everything to do with textures. 
 * Expected usuage is to call requestTexture to initially load the texture
 * and store the returned value.
 */ 

class Console;
  
class TextureManager : public SigC::Object, public ConsoleObject {

public:
  
  typedef std::map<std::string, TextureID> TextureMap;
  typedef std::vector<GLuint> TextureVector;
  typedef std::vector<std::string> NameVector;
 
  /**
   * Default constructor
   */   
  TextureManager();

  /**
   * Destructor
   */ 
  ~TextureManager() {
    // Call shutdown if it hasn't already been
    if (m_initialised) shutdown();
  }

  /**
   * Initialise a TextureManager object
   */ 
  void init();

  void initGL();

  /**
   * Clean up the TextureManager object
   */ 
  void shutdown();

  /**
   * Reads the texture manager config from the specified config object
   * @param config Config object containing config data
   */ 
  void readConfig(varconf::Config &config);

  /**
   * Writes the texture manager config to the specified config object
   * @param config Config object to write config data to
   */ 
  void writeConfig(varconf::Config &config);
  
  /**
   * This reads in texture configuration data from the specified file
   * @param filename Filename to texture configuration file
   */ 
  void readTextureConfig(const std::string &filename);

  TextureID requestTextureID(const std::string &texture_name, bool mask) {
//  TextureID requestTextureID(std::string texture_name) {
    assert(m_initialised != false);
    std::string name = (mask) ? ("mask_" + texture_name) : (texture_name);
    TextureID id = m_texture_map[name];
    if (id == 0) {
      m_texture_map[name] = m_texture_counter;
      m_names[m_texture_counter] = name;
      id = m_texture_counter++;
      // If we have run out of space, allocate some more
      if (m_texture_counter >= (int)m_texture_map.size()) {
        // Double size allocation
        int new_size = 2 * m_texture_map.size();
        m_textures.resize(new_size);
        m_names.resize(new_size);
      }
    }
    return id;
  }

  /** 
   * Loads the requested texture. Parameters for the textures are taken from thr
   * config values accessed by texture_name. The return value should be used 
   * with getTextureObejct to get the actual ID value.
   * @param texture_name Name of texture to load
   * @return ID for texture.
   */ 
  GLuint loadTexture(const std::string &texture_name);

  /**
   * Unloads the specified texture from the OpenGL system
   * @param texture_name Name of texture to unload
   */ 
  void unloadTexture(const std::string &texture_name);

  /**
   * Unloads the specified texture from the OpenGL system
   * @param texture_object TextureObject to unload
   */ 
  void unloadTexture(GLuint texture_object);

  /**
   * This is the standard function to switch textures. Will only
   * change textures if the requested texture is different from the 
   * currently loaded one.
   * @param texture_id TextureID of the texture to load
   */ 
  void switchTexture(TextureID texture_id);

  /**
   * This function switchs the texture for a given unit.
   * @param texture_unit Texture unit to use.
   * @param texture_id TextureID of the texture
   */ 
  void switchTexture(unsigned int texture_unit, TextureID texture_id);

  void setScale(float scale) { setScale(scale, scale); }
  void setScale(float scale_x, float scale_y);

  void setScale(unsigned int texture_unit, float scale) {
    setScale(texture_unit, scale, scale);
  }
  void setScale(unsigned int texture_unit, float scale_x, float scale_y);

  int getNumTextureUnits() const { return m_texture_units; }
 
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &arguments);
  void setupGLExtensions();
 
  void invalidate();

  static GLint getFormat(const std::string &fmt);
 
private:
  bool m_initialised; ///< Flag indicating whether object has had init called
  varconf::Config m_texture_config; ///< Config object for all texture
  TextureMap m_texture_map; ///< Mapping between texture name and its TextureID
  TextureVector m_textures; ///< Used to translate a TextureID to a TextureObject
  NameVector m_names; 
  int m_texture_counter; ///< Keeps track of last allocated TextureID
  std::vector<TextureID> m_last_textures;
  int m_texture_units;
  TextureID m_default_texture;
  TextureID m_default_font;

  /**
   * This function is used to setup the required OpenGL texture extensions
   */ 
  GLuint loadTexture(const std::string &texture_name, SDL_Surface *surface, bool mask);
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);  
  void varconf_error_callback(const char *message);

  TextureID createDefaultTexture();
  TextureID createDefaultFont();

  static SDL_Surface *loadImage(const  std::string &filename);
  
  /**
   * Returns the OpenGL filter from the name
   * @param filter_name String name of the filter
   * @return The filter, or 0 is a wrong name was specified
   */ 
  static int getFilter(const std::string &filter_name);

};
  
} /* namespace Sear */

#endif /* SEAR_RENDER_TEXTUREMANAGER_H */
