// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "Terrain.h"
#include "Render.h"
#include "System.h"
#include "Config.h"
#include <string>
#include <iostream.h>
#include "Utility.h"

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
    std::cerr << "Terrain: Error creating height map array" << std::endl;
    return false;
  }
  Landscape::waterlevel = _water_level;
  return true;
}

void Terrain::shutdown() {
  std::cout << "Shutting down Terrain" << std::endl;
  writeConfig();
  std::cout << "Deleting Terrain" << std::endl;
  if (gLand) {
    delete gLand;
  }
  std::cout << "Freeing HeightMap" << std::endl;
  if (hMap) free(hMap);
  
  std::cout << "Finished Shutting down Terrain" << std::endl;
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
//    std::cerr << "Error loading heightmap!" << std::endl;
    std::cerr << "Unable to load heightmap: " << hmap << ": " <<  SDL_GetError() << std::endl;
    std::cerr << "Using flat terrain instead" << std::endl;
//    exit(1);
//    return;
    hMapHeight = DEFAULT_map_height;
    hMapWidth = DEFAULT_map_width;
    hMap = (unsigned char *)malloc(hMapWidth * hMapHeight * sizeof(unsigned char));
    if (hMap) {
      memset(hMap, DEFAULT_height, hMapHeight * hMapWidth * sizeof(unsigned char));
    } else {
      std::cerr << "Terrain: Error - Unable to allocate memory for height map array" << std::endl;
    }
    return;
  }
  SDL_LockSurface(terrain);

  hMapWidth = terrain->w;
  hMapHeight = terrain->h;
  hMap = (unsigned char *)malloc(hMapWidth * hMapHeight * sizeof(unsigned char));
  if (!hMap) {
    std::cerr << "Terrain: Error - Unable to allocate memory for height map array" << std::endl;
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
    std::cerr << "Terrain: General config object not created!" << std::endl;
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
  std::cout << "Writing Terrain Config" << std::endl;
  Config *general = System::instance()->getGeneral();
  if (!general) {
    std::cerr << "Terrain: General config object not created!" << std::endl;
    return;
  }
  general->setAttribute(KEY_height, string_fmt(_height));
  general->setAttribute(KEY_water_level, string_fmt(_water_level));
  general->setAttribute(KEY_terrain_scale, string_fmt(_terrain_scale));
}

} /* namespace Sear */
