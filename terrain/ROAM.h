// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: ROAM.h,v 1.12 2002-12-07 14:13:41 simon Exp $

#ifndef SEAR_ROAM_H
#define SEAR_ROAM_H 1

#include "src/Terrain.h"

//#include <stdlib.h>
//#include "Landscape.h"

#include <sigc++/object_slot.h>

namespace varconf {
class Config;
}

namespace Sear {

class Landscape;
class System;
class Render;

class ROAM : public Terrain, public SigC::Object {

public:
  ROAM(System *system, Render *renderer);
  ~ROAM();

  static const unsigned int map_size = 200;
//  static const unsigned int map_size = 512;

  bool init();
  void shutdown();
  void draw();
  void update(float time_elapsed);

//  int getMapWidth()  { return hMapWidth;  }
//  int getMapHeight() { return hMapHeight; }
  //Assuming square map
//  int getMapSize() { return hMapWidth; }

//  Landscape *getLandscape() { return gLand; }
  
  float getHeight(float, float);

  void lowerDetail();
  void raiseDetail();
  
  static float _water_level;
  void readConfig();
  void writeConfig();

  float _terrain_scale;
  
protected:
  
  static const int DEFAULT_height = 128;
  static const float DEFAULT_water_level = 127.0f;
  static const float DEFAULT_terrain_scale = 0.01f;

  static const char * const KEY_water_level = "terrain_water_level";
  static const char * const KEY_height = "terrain_height";
  
  static const char * const KEY_height_map = "height_map";
  static const char * const KEY_terrain_scale = "terrain_scale";
 
  static const char * const KEY_num_x_landscapes = "num_x_landscapes";
  static const char * const KEY_num_y_landscapes = "num_y_landscapes";
  static const char * const KEY_landscape = "landscape_";
  
  
  void render();
  static int loadHeightMap(unsigned char **, const std::string&);
 
//  unsigned int num_landscapes;
  unsigned int _num_x_landscapes;
  unsigned int _num_y_landscapes;
  unsigned char **_height_maps;
  float last_time;
  Landscape **_landscapes;
  
  System *_system;
  Render *_renderer;
  bool _initialised;
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
};

} /* namespace Sear */
#endif /* SEAR_ROAM_H */

