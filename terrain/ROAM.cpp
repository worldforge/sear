// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall, University of Southampton

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

#include <GL/gl.h>

#ifdef HAVE_CONFIG
  #include "config.h"
#endif

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif


#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

static const std::string TERRAIN = "terrain";
  
namespace Sear {
	
float ROAM::_water_level = 0.0f;
float ROAM::_water_level_base = 0.0f;
 
static const int DEFAULT_height = 128;
static const float DEFAULT_water_level = 127.0f;
static const float DEFAULT_terrain_scale = 0.01f;
static const float DEFAULT_texture_detail_scale = 1.0;

static const std::string KEY_water_level = "terrain_water_level";
static const std::string KEY_height = "terrain_height";
static const std::string KEY_height_map = "height_map";
static const std::string KEY_terrain_scale = "terrain_scale";
static const std::string KEY_num_x_landscapes = "num_x_landscapes";
static const std::string KEY_num_y_landscapes = "num_y_landscapes";
static const std::string KEY_texture_detail_scale = "texture_detail_scale";
static const std::string KEY_landscape = "landscape_";
  
  
 
ROAM::ROAM(System *system, Render *renderer) :
  _height_maps(NULL),
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
  unsigned int num_landscapes = _num_x_landscapes * _num_y_landscapes;
  _height_maps = (float **)malloc(num_landscapes * sizeof(float *));
  _landscapes = (Landscape**)malloc(num_landscapes * sizeof (Landscape*));
  memset(_landscapes, 0, num_landscapes * sizeof (Landscape*));
  for (int x = _num_x_landscapes - 1; x >= 0; --x) {
    for (int y = _num_y_landscapes - 1; y >= 0; --y) {
      unsigned int i = y * _num_x_landscapes + x;
      _height_maps[i] = NULL;
    
      std::string height_map = (std::string)System::instance()->getGeneral().getItem(TERRAIN, std::string(KEY_landscape) + string_fmt(x) + "_" + string_fmt(y));
      if (!loadHeightMap(&_height_maps[i], height_map)) {
        // Error loading height map
        // TODO handle this error. create a flat terrain or summat
        std::cerr << "Error loading heightmap" << std::endl;
        continue;
      }
      int offset_x = -((map_size * _num_x_landscapes) / 2) + x * map_size;
      int offset_y = ((map_size * _num_y_landscapes) / 2) - (y+1) * map_size;
      int right = y * _num_x_landscapes + (x + 1);
      int bottom = (y + 1) * _num_x_landscapes + x;
      Landscape *l_right = (x < (int)_num_x_landscapes - 1) ? (_landscapes[right]) : (NULL);
      Landscape *l_bottom = (y  < (int)_num_y_landscapes - 1) ? (_landscapes[bottom]) : (NULL);

      if (l_right) {
        for (unsigned int xx = 0; xx <= map_size; ++xx) {
	  int i1 = xx * (map_size+1) + (map_size);
	  int i2 = xx * (map_size+1) + 0;
        _height_maps[i][i1] =  _height_maps[right][i2];
	}
      }

      if (l_bottom) {
        for (unsigned int yy = 0; yy <= map_size; ++yy) {
	  int i1 = (map_size) * (map_size+1)+ yy;
	  int i2 = yy;
	   _height_maps[bottom][i1] = _height_maps[i][i2];
	}
      }
       
      _landscapes[i] = new Landscape(_renderer, this, l_bottom, l_right);
      _landscapes[i]->Init(_height_maps[i], map_size, offset_x, offset_y);
      _landscapes[i]->Reset();
      
    }
  }
   
  Landscape::waterlevel = _water_level;
  _initialised = true;
  return true;
}

void ROAM::shutdown() {
  if (debug) Log::writeLog("Shutting down ROAM", Log::LOG_DEFAULT);
  writeConfig();
  unsigned int num_landscapes = _num_x_landscapes * _num_y_landscapes;
  if (_landscapes) {
    for (unsigned int i = 0; i < num_landscapes; ++i) if (_landscapes[i]) delete (_landscapes[i]);
    free (_landscapes);
    _landscapes = NULL;
  }
  if (_height_maps) {
    for (unsigned int i = 0; i < num_landscapes; ++i) if (_height_maps[i]) delete [] _height_maps[i];
    free(_height_maps);
    _height_maps = NULL;
  }
  _initialised = false;
}

void ROAM::draw() {
  static float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float specular[] = { 0.0f, 0.0f, 0.0f, 0.0f };
  static float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  _renderer->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
  render();
}

void ROAM::render() {
  for (unsigned int i = 0; i < _num_x_landscapes * _num_y_landscapes; ++i) {
    _landscapes[i]->Reset();
    _landscapes[i]->Tessellate();
    _renderer->store();
    _landscapes[i]->render();
    _renderer->restore();
  }
}

int ROAM::loadHeightMap(float **h_map, const std::string &heightmap) {
  unsigned int i, x, y;
//  unsigned char *hMap = NULL;
  float *hMap = NULL;
  SDL_Surface *terrain = NULL;

  //hMap = (float *)malloc((1+ map_size) * (1+map_size) * sizeof(float));
  hMap = new float[(1+ map_size) * (1+map_size)];
  if (!hMap) {
    Log::writeLog("ROAM: Error - Unable to allocate memory for height map array", Log::LOG_ERROR);
    return 0;
  } 
  //This a good idea on floats?
  memset(hMap, 0, (1+ map_size) * (1+map_size) * sizeof(float));
 
  terrain = System::loadImage(heightmap);
  
  if (terrain == NULL) {
    Log::writeLog(std::string("Unable to load heightmap: ") + heightmap + std::string(": ") + string_fmt (SDL_GetError()), Log::LOG_ERROR);
    Log::writeLog("Using flat terrain instead", Log::LOG_ERROR);
//    return 0;
  } else {
    if (terrain->w != (int)map_size || terrain->h != (int)map_size) {
      // ERROR heightmap is or wrong dimensions
      // TODO shall we just ignore oversized heightmaps?
//      SDL_FreeSurface(terrain);
      std::cerr << "Error height map has wrong dimensions - " << heightmap << std::endl;
//      return 0;
    } else {
      SDL_LockSurface(terrain);
      i = 0;
      for(y = 0; y < map_size; ++y) {
        for(x = 0; x < map_size; ++x) {
          //hMap[i++] = System::getPixel(terrain, x, map_size - 1 - y) & 0xFF;
          hMap[i++] = (float)(System::getPixel(terrain, x, y) & 0xFF) * _terrain_scale;
        }
        i++;
      }
      SDL_UnlockSurface(terrain);
    }
    SDL_FreeSurface(terrain);
  }
  *h_map = hMap;
  return 1;
}

void ROAM::update(float time_elapsed) {
  last_time += time_elapsed;
}

float ROAM::getHeight(float x, float y) {
  float half_x = map_size * _num_x_landscapes / 2.0f;
  float half_y = map_size * _num_y_landscapes / 2.0f;
	  
  if (x <= -half_x || x >= half_x) { return 0.0f; }
  if (y <= -half_y || y >= half_y) { return 0.0f; }
  
  x += half_x;  
  y += half_y;
  int index_x = 0;
  int index_y = _num_y_landscapes - 1;
  while (x >= map_size) {
    ++index_x;
    x -= map_size;
  }
  while (y >= map_size) {
    --index_y;
    y -= map_size;
  }
  unsigned int index = index_x + index_y * _num_x_landscapes; 
  float h1 =_height_maps[index][(int)x + (int)y * (map_size+1)];
  float h2 =_height_maps[index][(int)(x+1) + (int)y * (map_size+1)];
  float h3 =_height_maps[index][(int)(x+1) + (int)(y+1) * (map_size+1)];
  float h4 =_height_maps[index][(int)x + (int)(y+1) * (map_size+1)];
  float x_m = (float)x - (float)((int)x);
  float y_m = (float)y - (float)((int)y);
  float h = x_m * (h2 - h1 + h3 - h4) + y_m * (h3 - h2 + h4 - h1);
  float height = h1 + (h / 2.0f);
  return height;// * _terrain_scale;
}


void ROAM::readConfig() {
  varconf::Variable temp;
  varconf::Config &general = _system->getGeneral();
  
  temp = general.getItem(TERRAIN, KEY_water_level);
  _water_level_base = (!temp.is_double()) ? (DEFAULT_water_level) : ((double)(temp));
  _water_level = _water_level_base * _terrain_scale;

  temp = general.getItem(TERRAIN, KEY_terrain_scale);
  _terrain_scale = (!temp.is_double()) ? (DEFAULT_terrain_scale) : ((double)(temp));

  _water_level = _water_level_base * _terrain_scale;
  
  temp = general.getItem(TERRAIN, KEY_num_x_landscapes);
  _num_x_landscapes = (!temp.is_int()) ? (1) : ((int)(temp));

  temp = general.getItem(TERRAIN, KEY_num_y_landscapes);
  _num_y_landscapes = (!temp.is_int()) ? (1) : ((int)(temp));

  temp = general.getItem(TERRAIN, KEY_texture_detail_scale);
  _detail_scale = (!temp.is_double()) ? (DEFAULT_texture_detail_scale) : ((double)(temp));

}

void ROAM::writeConfig() {
  if (debug) Log::writeLog("Writing ROAM Config", Log::LOG_DEFAULT);
  varconf::Config &general = _system->getGeneral();
//  general.setItem(TERRAIN, KEY_height, _height);
  general.setItem(TERRAIN, KEY_water_level, _water_level_base);
  general.setItem(TERRAIN, KEY_terrain_scale, _terrain_scale);
//  general.setItem(TERRAIN, KEY_height_map, hmap);
  general.setItem(TERRAIN, KEY_texture_detail_scale, (double)_detail_scale);
}

void ROAM::lowerDetail() { 
  for (unsigned int i = 0; i < _num_x_landscapes * _num_y_landscapes; ++i) {
//    _landscapes[i]->lowerDetail(); 
  }
}

void ROAM::raiseDetail() {
  for (unsigned int i = 0; i < _num_x_landscapes * _num_y_landscapes; ++i) {
//    _landscapes[i]->raiseDetail();
  }
}

void ROAM::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  varconf::Variable temp;

  if (section == TERRAIN) {
//    if (key == KEY_height_map) {	  
//      hmap = config.getItem(TERRAIN, KEY_height_map);
//    }
//    else if (key == KEY_height) {    
//      temp = config.getItem(TERRAIN, KEY_height);
//      _height = (!temp.is_int()) ? (DEFAULT_height) : ((int)(temp));
//    }
//    else
	    if (key == KEY_water_level) {
      temp = config.getItem(TERRAIN, KEY_water_level);
      _water_level_base = (!temp.is_double()) ? (DEFAULT_water_level) : ((double)(temp));
  _water_level = _water_level_base * _terrain_scale;
    }
    else if (key == KEY_terrain_scale) {
      temp = config.getItem(TERRAIN, KEY_terrain_scale);
      _terrain_scale = (!temp.is_double()) ? (DEFAULT_terrain_scale) : ((double)(temp));
      _water_level = _water_level_base * _terrain_scale;
    } else if (key == KEY_texture_detail_scale) {
      temp = config.getItem(TERRAIN, KEY_texture_detail_scale);
      _detail_scale = (!temp.is_double()) ? (DEFAULT_texture_detail_scale) : ((double)(temp));
    }
  }
}


} /* namespace Sear */
