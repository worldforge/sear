// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Landscape.cpp,v 1.14 2002-12-07 17:34:53 simon Exp $

// Code based upon ROAM Simplistic Implementation by Bryan Turner bryan.turner@pobox.com

#include "src/System.h"
#include <math.h>

#include <Eris/Entity.h>
#include <Eris/World.h>

#include "src/Camera.h"
#include "src/Render.h"

#include "Landscape.h"
#include "ROAM.h"

#ifdef DEBUG
  #include "common/mmgr.h"
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

static const std::string TERRAIN = "terrain";
static const std::string GRASS = "grass";
static const std::string WATER = "water";

#define DEF_VAL 9999.0f

// -------------------------------------------------------------------------------------------------
//	LANDSCAPE CLASS
// -------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------
// Definition of the static member variables
//
//int Landscape::m_NextTriNode;
//TriTreeNode Landscape::m_TriPool[POOL_SIZE];

float Landscape::waterlevel = 0.0f;

  unsigned char Landscape::getHeight(unsigned int x, unsigned int y)  {
    return m_HeightMap[x + y * (ROAM::map_size + 1)];
  }
// ---------------------------------------------------------------------
// Allocate a TriTreeNode from the pool.
//
TriTreeNode *Landscape::AllocateTri() {
  TriTreeNode *pTri;

  // IF we've run out of TriTreeNodes, just return NULL (this is handled gracefully)
  if ( m_NextTriNode >= POOL_SIZE ) return NULL;
  pTri = &(m_TriPool[m_NextTriNode++]);
  pTri->LeftChild = pTri->RightChild = NULL;

  pTri->l = this;
  return pTri;
}

// ---------------------------------------------------------------------
// Initialize all patches
//
void Landscape::Init(unsigned char *hMap, int size, int offsetx, int offsety) {
  Patch *patch;
  int X, Y;
  // Store the Height Field array
  m_HeightMap = hMap;
  map_size = size;
  offset_x = offsetx;
  offset_y = offsety;
  patch_size = map_size / NUM_PATCHES_PER_SIDE;
  // Initialize all terrain patches
  for ( Y=0; Y < NUM_PATCHES_PER_SIDE; ++Y)
    for ( X=0; X < NUM_PATCHES_PER_SIDE; ++X) {
      m_Patches[Y][X] = Patch(_renderer, _terrain, this);
      patch = &(m_Patches[Y][X]);
      patch->Init(X*patch_size, Y*patch_size, X*patch_size, Y*patch_size, hMap );
      patch->ComputeVariance();
    }  
}

// ---------------------------------------------------------------------
// Reset all patches, recompute variance if needed
//
void Landscape::Reset() {
  //
  // Perform simple visibility culling on entire patches.
  //   - Define a triangle set back from the camera by one patch size, following
  //     the angle of the frustum.
  //   - A patch is visible if it's center point is included in the angle: Left,Eye,Right
  //   - This visibility test is only accurate if the camera cannot look up or down significantly.
  //
  int X, Y;
  Patch *patch;

  // Set the next free triangle pointer back to the beginning
  SetNextTriNode(0);

  // Reset rendered triangle count.
  gNumTrisRendered = 0;

  // Go through the patches performing resets, compute variances, and linking.
  for ( Y=0; Y < NUM_PATCHES_PER_SIDE; ++Y)
    for ( X=0; X < NUM_PATCHES_PER_SIDE; ++X) {
      patch = &(m_Patches[Y][X]);
      // Reset the patch
      patch->Reset();
      patch->SetVisibility();
      // Check to see if this patch has been deformed since last frame.
      // If so, recompute the varience tree for it.
      if ( patch->isDirty() ) patch->ComputeVariance();
      if ( patch->isVisibile() ) {
        // Link all the patches together.
	if ( X > 0 ) patch->GetBaseLeft()->LeftNeighbor = m_Patches[Y][X-1].GetBaseRight();
	else patch->GetBaseLeft()->LeftNeighbor = NULL;	// Link to bordering Landscape here..
  
  	if ( X < (NUM_PATCHES_PER_SIDE-1) ) patch->GetBaseRight()->LeftNeighbor = m_Patches[Y][X+1].GetBaseLeft();
	else if (_right) {
          Patch *p = _right->getPatch(0, Y);
          if (p) {
            patch->GetBaseRight()->LeftNeighbor = p->GetBaseLeft();
	    if (p->GetBaseLeft()) p->GetBaseLeft()->LeftNeighbor = patch->GetBaseRight();
          }
          else patch->GetBaseRight()->LeftNeighbor = NULL;
        } else {
          patch->GetBaseRight()->LeftNeighbor = NULL; // Link to bordering Landscape here..
	}
	if ( Y > 0 ) patch->GetBaseLeft()->RightNeighbor = m_Patches[Y-1][X].GetBaseRight();
	else patch->GetBaseLeft()->RightNeighbor = NULL; // Link to bordering Landscape here..
	if ( Y < (NUM_PATCHES_PER_SIDE-1) ) patch->GetBaseRight()->RightNeighbor = m_Patches[Y+1][X].GetBaseLeft();
	else if (_bottom) {
          Patch *p = _bottom->getPatch(X, 0);
          if (p) {
	    patch->GetBaseRight()->RightNeighbor = p->GetBaseLeft();
	    if (p->GetBaseLeft()) p->GetBaseLeft()->RightNeighbor = patch->GetBaseRight();
	  }
          else patch->GetBaseRight()->RightNeighbor = NULL; // Link to bordering Landscape here..
        } else {
          patch->GetBaseRight()->RightNeighbor = NULL; // Link to bordering Landscape here..
	}
      }
    }
  SetVisibility();
}

// ---------------------------------------------------------------------
// Create an approximate mesh of the landscape.
//
void Landscape::Tessellate() {
  // Perform Tessellation
  int nCount;
  Patch *patch = &(m_Patches[0][0]);
  for (nCount=0; nCount < NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE; ++nCount, ++patch)
    if (patch->isVisibile()) patch->Tessellate( );
}

// ---------------------------------------------------------------------
// Render each patch of the landscape & adjust the frame variance.
//
void Landscape::render() {
  if (!m_isVisible) return;
    //cout << gFrameVariance << endl;
   //TODO: split into render land, render water
  int nCount;
  Patch *patch = &(m_Patches[0][0]);
  // Scale the terrain by the terrain scale specified at compile time.
//  glScalef(MULT_SCALE_LAND, MULT_SCALE_LAND, MULT_SCALE_HEIGHT);
  _renderer->translateObject(offset_x, offset_y, 0.0f);

  waterlevel = ROAM::_water_level + sin(System::instance()->getTime() / 1000.0f);

  _renderer->setColour(1.0f, 1.0f, 1.0f, 1.0f);	
  _renderer->stateChange(TERRAIN);
  if (_renderer->checkState(Render::RENDER_TEXTURES)) {
    _renderer->switchTexture(_renderer->requestMipMap(TERRAIN, GRASS));
  } else {
    // Do Nothing
  }
  
  for (nCount=0; nCount < NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE; ++nCount, ++patch) if (patch->isVisibile()) patch->render();

  patch = &(m_Patches[0][0]);
  
//  _renderer->setColour(1.0f, 1.0f, 1.0f, 0.6f);
  _renderer->stateChange(WATER);
  if (_renderer->checkState(Render::RENDER_TEXTURES)) {
    _renderer->switchTexture(_renderer->requestMipMap(WATER, WATER));
  } else {
//    //Do Nothing
  }
  for (nCount=0; nCount < NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE; ++nCount, ++patch) if (patch->isVisibile()) patch->renderWater();
					
  // Check to see if we got close to the desired number of triangles.
  // Adjust the frame variance to a better value.
  float change = 0.0f;
  if ( GetNextTriNode() != gDesiredTris ) change= ((float)GetNextTriNode() - (float)gDesiredTris) / (float)gDesiredTris;
  if (change > 0.05f) change = 0.05f;
  gFrameVariance += change;
  //if ( GetNextTriNode() != gDesiredTris ) gFrameVariance += ((float)GetNextTriNode() - (float)gDesiredTris) / (float)gDesiredTris;
  // Bounds checking.
  if (gFrameVariance < 0 ) gFrameVariance = 0;
}
//

void Landscape::SetVisibility() {// int eyeX, int eyeY, int leftX, int leftY, int rightX, int rightY ) {
  if (min_height == DEF_VAL) {
    for (int x = 0; x < map_size; x++) {
      for (int y = 0; y < map_size; y++) {
        float h = getHeight(x,y);
	if (h < min_height) min_height = h;
	if (h > max_height) max_height = h;
      }
    }
    min_height *= _terrain->_terrain_scale;
    max_height *= _terrain->_terrain_scale;
  }
  WFMath::Point<3> corner1 = WFMath::Point<3>(offset_x, offset_y, min_height);
  WFMath::Point<3> corner2 = WFMath::Point<3>(offset_x + map_size, offset_y + map_size, max_height);
  int i = _renderer->patchInFrustum(WFMath::AxisBox<3>(corner1,corner2));//, point);

  if (i != 0) m_isVisible = 1;
  else m_isVisible = 0;
//  m_isVisible = 1;

}
/*
float Landscape::getHeight(float x, float y) {
  float height;
  float h1 = m_HeightMap[(int)(x) + (int)y * ROAM::map_size];
  float h2 = m_HeightMap[(int)(x + 1) +  (int)y * ROAM::map_size];
  float h3 = m_HeightMap[(int)(x + 1) + (int)(y + 1) * ROAM::map_size];
  float h4 = m_HeightMap[(int)(x)+(int)(y + 1) * ROAM::map_size];
  float x_m = (float)x - (float)((int)x);
  float y_m = (float)y - (float)((int)y);
  float h = x_m * (h2 - h1 + h3 - h4) + y_m * (h3 - h2 + h4 - h1);
  height = h1 + (h / 2.0f);
  return height ;// * _terrain->_terrain_scale;
}
*/
} /* namespace Sear */
