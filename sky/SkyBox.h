// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: SkyBox.h,v 1.5 2003-03-06 23:50:38 simon Exp $

#ifndef SEAR_SKYBOX_H
#define SEAR_SKYBOX_H 1

#include "common/types.h"

#include "src/System.h"

#define NUM_SKY_TEXTURES 12

// defines for Config key values

#define CONFIG_SKYDIST "SkyDist"
#define CONFIG_SKY_DAY_NORTH   "day_north"
#define CONFIG_SKY_DAY_EAST    "day_east"
#define CONFIG_SKY_DAY_SOUTH   "day_south"
#define CONFIG_SKY_DAY_WEST    "day_west"
#define CONFIG_SKY_DAY_TOP     "day_top"
#define CONFIG_SKY_DAY_BOTTOM  "day_bottom"

#define CONFIG_SKY_NIGHT_NORTH   "night_north"
#define CONFIG_SKY_NIGHT_EAST    "night_east"
#define CONFIG_SKY_NIGHT_SOUTH   "night_south"
#define CONFIG_SKY_NIGHT_WEST    "night_west"
#define CONFIG_SKY_NIGHT_TOP     "night_top"
#define CONFIG_SKY_NIGHT_BOTTOM  "night_bottom"

// Define texture locations 

#define TEXTURE_SKY_DAY_NORTH  0
#define TEXTURE_SKY_DAY_SOUTH  1
#define TEXTURE_SKY_DAY_EAST   2
#define TEXTURE_SKY_DAY_WEST   3
#define TEXTURE_SKY_DAY_TOP    4
#define TEXTURE_SKY_DAY_BOTTOM 5

#define TEXTURE_SKY_NIGHT_NORTH  6
#define TEXTURE_SKY_NIGHT_SOUTH  7
#define TEXTURE_SKY_NIGHT_EAST   8
#define TEXTURE_SKY_NIGHT_WEST   9
#define TEXTURE_SKY_NIGHT_TOP    10
#define TEXTURE_SKY_NIGHT_BOTTOM 11


#define VERTEX_COORDS { \
  {-1.0f,  1.0f,  1.0f},  \
  { -1.0f, -1.0f,  1.0f},  \
  {  1.0f, -1.0f,  1.0f},  \
  { 1.0f,  1.0f,  1.0f},  \
                        \
  {-1.0f,  1.0f, -1.0f},  \
  {-1.0f,  1.0f,  1.0f},  \
  { 1.0f,  1.0f,  1.0f},  \
  { 1.0f,  1.0f, -1.0f},  \
                        \
  { 1.0f, -1.0f, -1.0f},  \
  { 1.0f, -1.0f,  1.0f},  \
  {-1.0f, -1.0f,  1.0f},  \
  {-1.0f, -1.0f, -1.0f},  \
                        \
  { 1.0f,  1.0f, -1.0f},  \
  { 1.0f,  1.0f,  1.0f},  \
  { 1.0f, -1.0f,  1.0f},  \
  { 1.0f, -1.0f, -1.0f},  \
                        \
  {-1.0f ,-1.0f, -1.0f},  \
  {-1.0f, -1.0f,  1.0f},  \
  {-1.0f,  1.0f,  1.0f},  \
  {-1.0f,  1.0f, -1.0f},  \
                        \
  {-1.0f, -1.0f, -1.0f},  \
  {-1.0f,  1.0f, -1.0f},  \
  { 1.0f,  1.0f, -1.0f},  \
  { 1.0f, -1.0f, -1.0f} }

#define TEXTURE_COORDS {  \
  { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, \
  { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, \
  { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, \
  { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, \
  { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, \
  { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f } }

#define NORMAL_COORDS { \
  { 0.0f,  0.0f, -1.0f}, \
  { 0.0f, -1.0f,  0.0f}, \
  { 0.0f,  1.0f,  0.0f}, \
  {-1.0f,  0.0f,  0.0f}, \
  { 1.0f,  0.0f,  0.0f}, \
  { 0.0f,  0.0f,  1.0f} }

#include "src/Sky.h"
   
namespace Sear {
// Forward Decls
class Render;
  
class SkyBox : public Sky{
public:
  SkyBox(System *, Render*);
  ~SkyBox();
  
  bool init();
  void shutdown();
  
  void draw();

protected:
  /**
    This method will draw the SkyBox
  */
  void render();
  void renderTextureSet(int);

  static Vertex_3 vertex_coords[];
  static Texel texture_coords[];
  static Normal normal_coords[];
 
  int texture_id[NUM_SKY_TEXTURES];
 
  System *_system; 
  Render *_renderer;
  bool _initialised;
};

} /* namespace Sear */
#endif /* SEAR_SKYBOX_H */
