// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: ROAM.h,v 1.7 2002-09-08 00:24:54 simon Exp $

#ifndef SEAR_ROAM_H
#define SEAR_ROAM_H 1

#include "src/Terrain.h"

//#include <stdlib.h>
//#include "Landscape.h"

namespace Sear {

class Landscape;
class System;
class Render;

class ROAM : public Terrain {

public:
  ROAM(System *system, Render *renderer);
  ~ROAM();

  bool init();
  void shutdown();
  void draw();
  void update(float time_elapsed);

  int getMapWidth()  { return hMapWidth;  }
  int getMapHeight() { return hMapHeight; }
  //Assuming square map
  int getMapSize() { return hMapWidth; }

  Landscape *getLandscape() { return gLand; }
  
  float getHeight(float, float);

  void lowerDetail();
  void raiseDetail();
  
  static float _water_level;
  void readConfig();
  void writeConfig();

  float _terrain_scale;
  
protected:
  
  static const int DEFAULT_map_height = 200;
  static const int DEFAULT_map_width = 200;
  
  static const int DEFAULT_height = 128;
  static const float DEFAULT_water_level = 127.0f;
  static const float DEFAULT_terrain_scale = 0.01f;

  static const char * const KEY_water_level = "terrain_water_level";
  static const char * const KEY_height = "terrain_height";
  
  static const char * const KEY_height_map = "height_map";
  static const char * const KEY_terrain_scale = "terrain_scale";
  
  void render();
  void loadHeightMap();

  int _height;
  int hMapWidth, hMapHeight;
  std::string hmap;
  unsigned char *hMap;
  float last_time;
  Landscape *gLand;
  
  System *_system;
  Render *_renderer;
  bool _initialised;
};

} /* namespace Sear */
#endif /* SEAR_ROAM_H */

