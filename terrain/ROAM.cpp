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

namespace Sear {

float ROAM::_water_level = 0.0f;

ROAM::ROAM(System *system, Render *renderer) :
  hMap(NULL),
  last_time(0.0f),
  gLand(NULL),
  _system(system),
  _renderer(renderer),
  _initialised(false)
{}

ROAM::~ROAM() {
  if (_initialised) shutdown();
}

bool ROAM::init() {
  if (_initialised) shutdown();
  readConfig();
  loadHeightMap();
  gLand = new Landscape(_renderer, this);
  if (hMap) {
    // height map array was sucessfully created!
    gLand->Init(hMap, hMapWidth);
  }
  else {
    Log::writeLog("ROAM: Error creating height map array", Log::LOG_ERROR);
    return false;
  }
  Landscape::waterlevel = _water_level;
  _initialised = true;
  return true;
}

void ROAM::shutdown() {
  Log::writeLog("Shutting down ROAM", Log::LOG_DEFAULT);
  writeConfig();
  Log::writeLog("Deleting ROAM", Log::LOG_DEFAULT);
  if (gLand) {
    delete gLand;
    gLand = NULL;
  }
  Log::writeLog("Freeing HeightMap", Log::LOG_DEFAULT);
  if (hMap) {
    free(hMap);
    hMap = NULL;
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
  gLand->Reset();
  gLand->Tessellate();
  gLand->render();
}

void ROAM::loadHeightMap() {
  int i, x, y;
  if (hMap) {
    free(hMap);
    hMap = NULL;
  }
  SDL_Surface *terrain = NULL;
  
  terrain = IMG_Load(hmap.c_str());
  if (terrain == NULL) {
    Log::writeLog(std::string("Unable to load heightmap: ") + hmap + std::string(": ") + string_fmt (SDL_GetError()), Log::LOG_ERROR);
    Log::writeLog("Using flat terrain instead", Log::LOG_ERROR);
    hMapHeight = DEFAULT_map_height;
    hMapWidth = DEFAULT_map_width;
    hMap = (unsigned char *)malloc(hMapWidth * hMapHeight * sizeof(unsigned char));
    if (hMap) {
      memset(hMap, DEFAULT_height, hMapHeight * hMapWidth * sizeof(unsigned char));
    } else {
      Log::writeLog("ROAM: Error - Unable to allocate memory for height map array", Log::LOG_ERROR);
    }
    return;
  }
  SDL_LockSurface(terrain);

  hMapWidth = terrain->w;
  hMapHeight = terrain->h;
  hMap = (unsigned char *)malloc(hMapWidth * hMapHeight * sizeof(unsigned char));
  if (!hMap) {
    Log::writeLog("ROAM: Error - Unable to allocate memory for height map array", Log::LOG_ERROR);
    return;
  }
  i = 0;
  for(y = 0; y < hMapHeight; ++y) {
    for(x = 0; x < hMapWidth; ++x) {
      hMap[i++] = System::getPixel(terrain, x, hMapHeight - 1 - y) & 0xFF;
    }
  }
  SDL_UnlockSurface(terrain);
  free(terrain);
}

void ROAM::update(float time_elapsed) {
   last_time += time_elapsed;
}

float ROAM::getHeight(float x, float y) {
  if (gLand) return gLand->getHeight(x, y);
  else return 0.0f;
}


void ROAM::readConfig() {
  varconf::Variable temp;
  varconf::Config *general = _system->getGeneral();
  if (!general) {
    Log::writeLog("ROAM: General config object not created!", Log::LOG_ERROR);
    return;
  }
  
  hmap = _system->getGeneral()->getItem("terrain", KEY_height_map);
    
  temp = general->getItem("terrain", KEY_height);
  _height = (!temp.is_int()) ? (DEFAULT_height) : ((int)(temp));

  temp = general->getItem("terrain", KEY_water_level);
  _water_level = (!temp.is_double()) ? (DEFAULT_water_level) : ((double)(temp));

  temp = general->getItem("terrain", KEY_terrain_scale);
  _terrain_scale = (!temp.is_double()) ? (DEFAULT_terrain_scale) : ((double)(temp));
}

void ROAM::writeConfig() {
  Log::writeLog("Writing ROAM Config", Log::LOG_DEFAULT);
  varconf::Config *general = _system->getGeneral();
  if (!general) {
    Log::writeLog("ROAM: General config object not created!", Log::LOG_ERROR);
    return;
  }
  general->setItem("terrain", KEY_height, _height);
  general->setItem("terrain", KEY_water_level, _water_level);
  general->setItem("terrain", KEY_terrain_scale, _terrain_scale);
  general->setItem("terrain", KEY_height_map, hmap);
  
}

void ROAM::lowerDetail() { 
  gLand->lowerDetail();
}

void ROAM::raiseDetail() {
  gLand->raiseDetail();
}
} /* namespace Sear */
