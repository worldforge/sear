// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: ROAM.h,v 1.14 2003-02-25 22:34:25 simon Exp $

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
  
  static float _water_level_base;
  static float _water_level;
  void readConfig();
  void writeConfig();

  float _terrain_scale;
  float _detail_scale;
  
protected:
  
  void render();
  int loadHeightMap(float **, const std::string&);
 
//  unsigned int num_landscapes;
  unsigned int _num_x_landscapes;
  unsigned int _num_y_landscapes;
  float **_height_maps;
  float last_time;
  Landscape **_landscapes;
  
  System *_system;
  Render *_renderer;
  bool _initialised;
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
};

} /* namespace Sear */
#endif /* SEAR_ROAM_H */

