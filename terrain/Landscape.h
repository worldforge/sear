// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Landscape.h,v 1.7 2002-12-07 17:34:53 simon Exp $

// Code based upon ROAM Simplistic Implementation by Bryan Turner bryan.turner@pobox.com
#ifndef SEAR_LANDSCAPE_H
#define SEAR_LANDSCAPE_H 1

#include "Patch.h"

namespace Sear {

class ROAM;
class Render;

// ---------------------------------------------------------------------
// Various Pre-Defined map sizes & their #define counterparts:

// ------- 1024x1024 MAP -------
//#define MAP_SIZE (200)
#define NUM_PATCHES_PER_SIDE (8)


// ---------------------------------------------------------------------
// Scale of the terrain ie: 1 unit of the height map == how many world units (meters)?
// 1.0f == 1 meter resolution
// 0.5f == 1/2 meter resolution
// 0.25f == 1/4 meter resolution
// etc..
//#define MULT_SCALE_HEIGHT (0.01f)
//#define MULT_SCALE_HEIGHT (0.01f)
#define MULT_SCALE_LAND   (1.0f)

// How many TriTreeNodes should be allocated?
#define POOL_SIZE (50000)

// Some more definitions
//#define PATCH_SIZE (MAP_SIZE/NUM_PATCHES_PER_SIDE)
#define TEXTURE_SIZE (1024)

#define SQR(x) ((x) * (x))
#define MAX(a,b) ((a < b) ? (b) : (a))

//
// Landscape Class
// Holds all the information to render an entire landscape.
//


class Landscape {
protected:

  unsigned char *m_HeightMap;										// HeightMap of the Landscape
  Patch m_Patches[NUM_PATCHES_PER_SIDE][NUM_PATCHES_PER_SIDE];	// Array of patches
  
//  static 
	  int	m_NextTriNode;										// Index to next free TriTreeNode
//  static 
	  TriTreeNode m_TriPool[POOL_SIZE];						// Pool of TriTree nodes for splitting


//  static
	  int GetNextTriNode() { return m_NextTriNode; }

//  static
	  void SetNextTriNode( int nNextNode ) { m_NextTriNode = nNextNode; }

  
public:
  Landscape(Render *renderer, ROAM *terrain, Landscape *right, Landscape *bottom) :
    gFrameVariance(50.0f),
    gNumTrisRendered(0),
    gDesiredTris(10000),
    _renderer(renderer),
    _terrain(terrain),
    _right(right),
    _bottom(bottom),
    min_height(9999.0f),
    max_height(9999.0f)
  {}
  virtual ~Landscape() {}
//  static
	  TriTreeNode *AllocateTri();
 
  virtual void Init(unsigned char *hMap, int size, int offset_x, int offset_y);
  virtual void Reset();
  virtual void Tessellate();
  virtual void render();

  //static Landscape *_instance;
  
  float gFrameVariance;
  int gNumTrisRendered;
  int gDesiredTris;

  static float waterlevel;

  Render *_renderer;
  ROAM *_terrain;

  Landscape *_right,  *_bottom;
  int map_size;
  int patch_size;
 
  int offset_x;
  int offset_y;
  
//  float getHeight(float x, float y);
  unsigned char getHeight(unsigned int x, unsigned int y) ;

  void raiseDetail() {
     gDesiredTris += 50;
     if (gDesiredTris > 50000) gDesiredTris = 50000;
  }
  void lowerDetail() {
    gDesiredTris -= 100;
    if (gDesiredTris < 5000) gDesiredTris = 5000;
  }

  Patch *getPatch(unsigned int x, unsigned int y) {
    if (x < NUM_PATCHES_PER_SIDE && y < NUM_PATCHES_PER_SIDE) return &m_Patches[x][y];
    else return NULL;
  }

  void SetVisibility();
  bool m_isVisible;
  float min_height, max_height;
};

} /* namespace Sear */
#endif /* SEAR_LANDSCAPE_H */
