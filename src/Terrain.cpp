// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "Terrain.h"
#include "Render.h"
#include "System.h"
#include "Config.h"
#include <string>
#include "Utility.h"
#include "Log.h"

namespace Sear {

float Terrain::_water_level = 0.0f;

bool Terrain::init() {
  readConfig();
  std::string groundTexture = System::instance()->getTexture()->getAttribute(KEY_ground_texture);
  ground_id = _renderer->requestMipMap(groundTexture);
  loadHeightMap();
  gLand = new Landscape(_renderer, this);
  if (hMap) {
    // height map array was sucessfully created!
    gLand->Init(hMap, hMapWidth);
  }
  else {
    Log::writeLog("Terrain: Error creating height map array", Log::ERROR);
    return false;
  }
  Landscape::waterlevel = _water_level;
  return true;
}

void Terrain::shutdown() {
  Log::writeLog("Shutting down Terrain", Log::DEFAULT);
  writeConfig();
  Log::writeLog("Deleting Terrain", Log::DEFAULT);
  if (gLand) {
    delete gLand;
  }
  Log::writeLog("Freeing HeightMap", Log::DEFAULT);
  if (hMap) free(hMap);
}

void Terrain::draw() {
//  _renderer->stateChange(Render::TERRAIN);
  render();
}

void Terrain::render() {
//  glPolygonMode(GL_FRONT, GL_LINE);
  gLand->Reset();
  gLand->Tessellate();
  gLand->render();
//  glPolygonMode(GL_FRONT, GL_FILL);
}

void Terrain::loadHeightMap() {
  int i, x, y;
  hMap = NULL;
  SDL_Surface *terrain = NULL;
  std::string hmap = _system->getGeneral()->getAttribute(KEY_height_map);
  
  terrain = IMG_Load(hmap.c_str());
  if (terrain == NULL) {
    Log::writeLog(std::string("Unable to load heightmap: ") + hmap + std::string(": ") + string_fmt (SDL_GetError()), Log::ERROR);
    Log::writeLog("Using flat terrain instead", Log::ERROR);
    hMapHeight = DEFAULT_map_height;
    hMapWidth = DEFAULT_map_width;
    hMap = (unsigned char *)malloc(hMapWidth * hMapHeight * sizeof(unsigned char));
    if (hMap) {
      memset(hMap, DEFAULT_height, hMapHeight * hMapWidth * sizeof(unsigned char));
    } else {
      Log::writeLog("Terrain: Error - Unable to allocate memory for height map array", Log::ERROR);
    }
    return;
  }
  SDL_LockSurface(terrain);

  hMapWidth = terrain->w;
  hMapHeight = terrain->h;
  hMap = (unsigned char *)malloc(hMapWidth * hMapHeight * sizeof(unsigned char));
  if (!hMap) {
    Log::writeLog("Terrain: Error - Unable to allocate memory for height map array", Log::ERROR);
    return;
  }
  i = 0;
  for(y = 0; y < hMapHeight; y++) {
    for(x = 0; x < hMapWidth; x++) {
      hMap[i++] = System::getPixel(terrain, x, hMapHeight - 1 - y) & 0xFF;
    }
  }
  SDL_UnlockSurface(terrain);
  free(terrain);
}

void Terrain::update(float time_elapsed) {
   last_time += time_elapsed;
}

float Terrain::getHeight(float x, float y) {
  if (gLand) return gLand->getHeight(x, y);
  else return 0.0f;
}


void Terrain::readConfig() {
  std::string temp;
  Config *general = System::instance()->getGeneral();
  if (!general) {
    Log::writeLog("Terrain: General config object not created!", Log::ERROR);
    return;
  }
  temp = general->getAttribute(KEY_height);
  _height = (temp.empty()) ? (DEFAULT_height) : (atoi(temp.c_str()));

  temp = general->getAttribute(KEY_water_level);
  _water_level = (temp.empty()) ? (DEFAULT_water_level) : (atof(temp.c_str()));

  temp = general->getAttribute(KEY_terrain_scale);
  _terrain_scale = (temp.empty()) ? (DEFAULT_terrain_scale) : (atof(temp.c_str()));
}

void Terrain::writeConfig() {
  Log::writeLog("Writing Terrain Config", Log::DEFAULT);
  Config *general = System::instance()->getGeneral();
  if (!general) {
    Log::writeLog("Terrain: General config object not created!", Log::ERROR);
    return;
  }
  general->setAttribute(KEY_height, string_fmt(_height));
  general->setAttribute(KEY_water_level, string_fmt(_water_level));
  general->setAttribute(KEY_terrain_scale, string_fmt(_terrain_scale));
}

} /* namespace Sear */
