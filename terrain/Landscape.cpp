// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// Code based upon ROAM Simplistic Implementation by Bryan Turner bryan.turner@pobox.com

#include <math.h>

#include <Eris/Entity.h>
#include <Eris/World.h>

#include "../src/Camera.h"
#include "../src/System.h"
#include "../src/Render.h"
#include "../src/ObjectLoader.h"

#include "Landscape.h"
#include "ROAM.h"

namespace Sear {

// -------------------------------------------------------------------------------------------------
//	LANDSCAPE CLASS
// -------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------
// Definition of the static member variables
//
int Landscape::m_NextTriNode;
TriTreeNode Landscape::m_TriPool[POOL_SIZE];

float Landscape::waterlevel = 0.0f;

// ---------------------------------------------------------------------
// Allocate a TriTreeNode from the pool.
//
TriTreeNode *Landscape::AllocateTri() {
  TriTreeNode *pTri;

  // IF we've run out of TriTreeNodes, just return NULL (this is handled gracefully)
  if ( m_NextTriNode >= POOL_SIZE ) return NULL;
  pTri = &(m_TriPool[m_NextTriNode++]);
  pTri->LeftChild = pTri->RightChild = NULL;
  return pTri;
}

// ---------------------------------------------------------------------
// Initialize all patches
//
void Landscape::Init(unsigned char *hMap, int size) {
  Patch *patch;
  int X, Y;
  // Store the Height Field array
  m_HeightMap = hMap;
  map_size = size;
  patch_size = map_size / NUM_PATCHES_PER_SIDE;
  // Initialize all terrain patches
  for ( Y=0; Y < NUM_PATCHES_PER_SIDE; Y++)
    for ( X=0; X < NUM_PATCHES_PER_SIDE; X++ ) {
      m_Patches[Y][X] = Patch(_renderer, _terrain, this);
      patch = &(m_Patches[Y][X]);
      patch->Init( X*patch_size, Y*patch_size, X*patch_size, Y*patch_size, hMap );
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
  for ( Y=0; Y < NUM_PATCHES_PER_SIDE; Y++ )
    for ( X=0; X < NUM_PATCHES_PER_SIDE; X++) {
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
	else patch->GetBaseRight()->LeftNeighbor = NULL; // Link to bordering Landscape here..
	if ( Y > 0 ) patch->GetBaseLeft()->RightNeighbor = m_Patches[Y-1][X].GetBaseRight();
	else patch->GetBaseLeft()->RightNeighbor = NULL; // Link to bordering Landscape here..
	if ( Y < (NUM_PATCHES_PER_SIDE-1) ) patch->GetBaseRight()->RightNeighbor = m_Patches[Y+1][X].GetBaseLeft();
        else patch->GetBaseRight()->RightNeighbor = NULL; // Link to bordering Landscape here..
      }
    }
}

// ---------------------------------------------------------------------
// Create an approximate mesh of the landscape.
//
void Landscape::Tessellate() {
  // Perform Tessellation
  int nCount;
  Patch *patch = &(m_Patches[0][0]);
  for (nCount=0; nCount < NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE; nCount++, patch++ )
    if (patch->isVisibile()) patch->Tessellate( );
}

// ---------------------------------------------------------------------
// Render each patch of the landscape & adjust the frame variance.
//
void Landscape::render() {

   //TODO: split into render land, render water
  int nCount;
  Patch *patch = &(m_Patches[0][0]);
  // Scale the terrain by the terrain scale specified at compile time.
//  glScalef(MULT_SCALE_LAND, MULT_SCALE_LAND, MULT_SCALE_HEIGHT);
  _renderer->translateObject(-100.0f, -100.0f, 0.0f);

  ObjectLoader *ol = System::instance()->getObjectLoader();
  ObjectProperties *op = ol->getObjectProperties("terrain");
  if (!op) op = ol->getObjectProperties("default");
  
//  _renderer->processObjectProperties(op);  
  waterlevel = ROAM::_water_level + sin(System::instance()->getTime() / 1000.0f);

  _renderer->setColour(1.0f, 1.0f, 1.0f, 1.0f);	
  _renderer->stateChange("terrain");
  if (_renderer->checkState(Render::RENDER_TEXTURES)) {
    _renderer->switchTexture(_renderer->requestMipMap("terrain", "grass"));
  } else {
    // Do Nothing
  }
  
  for (nCount=0; nCount < NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE; nCount++, patch++ ) if (patch->isVisibile()) patch->render();
  patch = &(m_Patches[0][0]);
  
  _renderer->setColour(1.0f, 1.0f, 1.0f, 0.6f);
  _renderer->stateChange("water");
  if (_renderer->checkState(Render::RENDER_TEXTURES)) {
    _renderer->switchTexture(_renderer->requestMipMap("water", "water"));
  } else {
    //Do Nothing
  }
  for (nCount=0; nCount < NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE; nCount++, patch++ ) if (patch->isVisibile()) patch->renderWater();
					
  // Check to see if we got close to the desired number of triangles.
  // Adjust the frame variance to a better value.
  if ( GetNextTriNode() != gDesiredTris ) gFrameVariance += ((float)GetNextTriNode() - (float)gDesiredTris) / (float)gDesiredTris;
  // Bounds checking.
  if (gFrameVariance < 0 ) gFrameVariance = 0;
}

float Landscape::getHeight(float x, float y) {
  float height;
  float mod_x = x + 100.0f;
  float mod_y = y + 100.0f;
  if (mod_x < 0 || mod_x >= 200 || mod_y < 0 || mod_y >= 200) return 0.0f;
  float h1 = m_HeightMap[(int)(mod_x) + (int)mod_y * 200];
  float h2 = m_HeightMap[(int)(mod_x + 1) +  (int)mod_y * 200];
  float h3 = m_HeightMap[(int)(mod_x + 1) + (int)(mod_y + 1) * 200];
  float h4 = m_HeightMap[(int)(mod_x)+(int)(mod_y + 1) * 200];
  float x_m = (float)x - (float)((int)x);
  float y_m = (float)y - (float)((int)y);
  float h = x_m * (h2 - h1 + h3 - h4) + y_m * (h3 - h2 + h4 - h1);
  height = h1 + (h / 2.0f);
  return height * _terrain->_terrain_scale;
}

} /* namespace Sear */
