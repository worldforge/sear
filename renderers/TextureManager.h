// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall, University of Southampton

// $Id: TextureManager.h,v 1.1 2003-03-11 20:58:59 simon Exp $

#ifndef SEAR_RENDER_TEXTUREMANAGER_H
#define SEAR_RENDER_TEXTUREMANAGER_H 1

namespace Sear {

/**
 * This class handles everything to do with textures. 
 * Expected usuage is to call requestTexture to initially load the texture
 * and store the returned value.
 */ 
	
class TextureManager : public SigC::Object {

public:
  typedef unsigned int TextureObject; ///< OpenGL reference to texture
  typedef int TextureID; ///< TextureManager reference to texture
  
  typedef std::map<std::string, TextureID> TextureMap;
  typedef std::vector<TextureObject> TextureVector;

	
  TextureManager();
  ~TextureManger() {
    if (_initialised) shutdown();
  }

  void init();
  void shutdown();

  void readConfig(varconf::Config &config);
  void writeConfig(varconf::Config &config);
  
  void readTextureConfig(const std::string &filename);

  /** 
   * Loads the requested texture. Parameters for the texturte are taken from thr
   * config values accessed by texture_name. The return value should be used 
   * with getTextureObejct to get the actual ID value.
   * @param texture_name Name of texture to load
   * @return ID for texture.
   */ 
 TextureID loadTexture(const std::string &texture_name);
  
  void unloadTexture(const std::string &texture_name);

  void unloadTexture(TextureObject texture_object);

  TextureID getTextureID(const std::string &texture_name) {
    return _texture_map[texture_name];
  }
 
  TextureObject getTexture(TextureID texture) {
    return _textures[texture];
  }

private:
  bool _initialised;
  varconf::Config _texture_config;
  TextureMap _texture_map;
  TextureVector _textures;
  unsigned int _texture_counter;

  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);	  
  void varconf_error_callback(const char *message);
  
  /**
   * Returns the OpenGL filter from the name
   * @param filter_name String name of the filter
   * @return The filter, or 0 is a wrong name was specified
   */ 
  static unsigned int getFilter(const std::string &filter_name);
  
} /* namespace Sear */
#endif /* SEAR_RENDER_TEXTUREMANAGER_H */
