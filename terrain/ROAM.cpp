// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $id: $

#include <string>
#include <stdlib.h>

#include <varconf/Config.h>
#include "common/Utility.h"
#include "common/Log.h"

#include "src/Render.h"
#include "src/System.h"

#include "ROAM.h"
#include "Landscape.h"

#ifdef DEBUG
  #include "common/mmgr.h"
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

static const std::string TERRAIN = "terrain";
  
namespace Sear {
	
float ROAM::_water_level = 0.0f;

ROAM::ROAM(System *system, Render *renderer) :
  hMap(NULL),
  last_time(0.0f),
  _landscapes(NULL),
  _system(system),
  _renderer(renderer),
  _initialised(false)
{}

ROAM::~ROAM() {
  if (_initialised) shutdown();
}

bool ROAM::init() {
  if (debug) Log::writeLog("Initialising ROAM", Log::LOG_DEFAULT);
  if (_initialised) shutdown();
  readConfig();
  System::instance()->getGeneral().sigsv.connect(SigC::slot(*this, &ROAM::varconf_callback));
  _height_maps = (unsigned char **)malloc(number_of_landscapes * sizeof(unsigned char *));
  _landscapes = (Landscape*)malloc(num_landscapes * sizeof (Landscape*));
  for (unsigned int i = 0; i < num_landscapes; ++i) {
    _height_maps[i] = NULL;
    l
    std::string height_map = (std::string)System::instance()->getGeneral.getItem(TERRAIN, std::string(KEY_landscape) + string_fmt(i));
    if (!loadHeightMap(_height_maps[i], heightmap)) {
      // Error loading height map
      // TODO handle this error. create a flat terrain or summat
      continue;
    }
    _landscapes[i] = new Landscape(_renderer, this);
    _landscapes->Init(_height_maps, map_size);
  }
   
  Landscape::waterlevel = _water_level;
  _initialised = true;
  return true;
}

void ROAM::shutdown() {
  if (debug) Log::writeLog("Shutting down ROAM", Log::LOG_DEFAULT);
  writeConfig();
  if (_landscapes) {
    for (unsigned int i = 0; i < num_landscapes; ++i) if (_landscapes[i]) free (_landscapes[i]);
    free _landscapes;
    _landscapes = NULL;
  }
  if (_height_maps) {
    for (unsigned int i = 0; i < num_landscapes; ++i) if (_height_maps[i]) free (_height_maps[i]);
    free(_height_maps);
    _height_maps = NULL;
  }
  _initialised = false;
}

void ROAM::draw() {
  static float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  _renderer->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
  render();
}

void ROAM::render() {
  _landscapes->Reset();
  _landscapes->Tessellate();
  _landscapes->render();
}

int ROAM::loadHeightMap(unsigned char **h_map, const std::string &heightmap) {
  int i, x, y;
  unsigned char *hMap = *h_map;
  SDL_Surface *terrain = NULL;
  
  terrain = System::loadImage(heightmap);
  
  if (terrain == NULL) {
    Log::writeLog(std::string("Unable to load heightmap: ") + hmap + std::string(": ") + string_fmt (SDL_GetError()), Log::LOG_ERROR);
    Log::writeLog("Using flat terrain instead", Log::LOG_ERROR);
    return 0;
  }

  if (terrain->w != map_size || terrain->h != map_size) {
    // ERROR heightmap is or wrong dimensions
    // TODO shall we just ignore oversized heightmaps?
    SDL_FreeSurface(terrain);
    return 0;
  }
  hMap = (unsigned char *)malloc(map_size * map_size * sizeof(unsigned char));
  if (!hMap) {
    Log::writeLog("ROAM: Error - Unable to allocate memory for height map array", Log::LOG_ERROR);
    return 0;
  } 
  SDL_LockSurface(terrain);
  i = 0;
  for(y = 0; y < map_size; ++y) {
    for(x = 0; x < map_size; ++x) {
      hMap[i++] = System::getPixel(terrain, x, map_size - 1 - y) & 0xFF;
    }
  }
  SDL_UnlockSurface(terrain);
  SDL_FreeSurface(terrain);
  return 1;
}

void ROAM::update(float time_elapsed) {
  last_time += time_elapsed;
}

float ROAM::getHeight(float x, float y) {
  //TODO FIX ME	
//  if (_landscapes) return _landscapes->getHeight(x, y);
//  else return 0.0f;
}


void ROAM::readConfig() {
  varconf::Variable temp;
  varconf::Config &general = _system->getGeneral();
  
  hmap = general.getItem(TERRAIN, KEY_height_map);
    
  temp = general.getItem(TERRAIN, KEY_height);
  _height = (!temp.is_int()) ? (DEFAULT_height) : ((int)(temp));

  temp = general.getItem(TERRAIN, KEY_water_level);
  _water_level = (!temp.is_double()) ? (DEFAULT_water_level) : ((double)(temp));

  temp = general.getItem(TERRAIN, KEY_terrain_scale);
  _terrain_scale = (!temp.is_double()) ? (DEFAULT_terrain_scale) : ((double)(temp));
}

void ROAM::writeConfig() {
  if (debug) Log::writeLog("Writing ROAM Config", Log::LOG_DEFAULT);
  varconf::Config &general = _system->getGeneral();
  general.setItem(TERRAIN, KEY_height, _height);
  general.setItem(TERRAIN, KEY_water_level, _water_level);
  general.setItem(TERRAIN, KEY_terrain_scale, _terrain_scale);
  general.setItem(TERRAIN, KEY_height_map, hmap);
}

void ROAM::lowerDetail() { 
//  _landscapes->lowerDetail();
}

void ROAM::raiseDetail() {
  //_landscapes->raiseDetail();
}

void ROAM::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  varconf::Variable temp;

  if (section == TERRAIN) {
    if (key == KEY_height_map) {	  
      hmap = config.getItem(TERRAIN, KEY_height_map);
    }
    else if (key == KEY_height) {    
      temp = config.getItem(TERRAIN, KEY_height);
      _height = (!temp.is_int()) ? (DEFAULT_height) : ((int)(temp));
    }
    else if (key == KEY_water_level) {
      temp = config.getItem(TERRAIN, KEY_water_level);
      _water_level = (!temp.is_double()) ? (DEFAULT_water_level) : ((double)(temp));
    }
    else if (key == KEY_terrain_scale) {
      temp = config.getItem(TERRAIN, KEY_terrain_scale);
      _terrain_scale = (!temp.is_double()) ? (DEFAULT_terrain_scale) : ((double)(temp));
    }
  }
}


} /* namespace Sear */
