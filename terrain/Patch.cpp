// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Patch.cpp,v 1.17 2003-03-07 12:50:51 simon Exp $

// Code based upon ROAM Simplistic Implementation by Bryan Turner bryan.turner@pobox.com

#include "src/System.h"
#include <math.h>

#include <Eris/World.h>
#include <Eris/Entity.h>

#include "src/WorldEntity.h"

#include "common/Utility.h"

#include "src/Graphics.h"
#include "src/Camera.h"
#include "src/Render.h"

#include "Patch.h"
#include "Landscape.h"
#include "ROAM.h"

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

#define DEF_VAL 9999.0f
  
namespace Sear {

Patch::Patch() :
  _renderer(NULL),
  _terrain(NULL),
  _landscape(NULL),
  min_height(DEF_VAL),
  max_height(-DEF_VAL)
{}

Patch::Patch(Render* render, ROAM *roam, Landscape *landscape) :
  _renderer(render),
  _terrain(roam),
  _landscape(landscape),
  min_height(DEF_VAL),
  max_height(-DEF_VAL)
{}

Patch::~Patch() {}
  
// ---------------------------------------------------------------------
// Split a single Triangle and link it into the mesh.
// Will correctly force-split diamonds.
//
void Patch::Split(TriTreeNode *tri) {
  // We are already split, no need to do it again.
  if (tri->LeftChild) return;
  // If this triangle is not in a proper diamond, force split our base neighbor
  if ( tri->BaseNeighbor && (tri->BaseNeighbor->BaseNeighbor != tri) ) Split(tri->BaseNeighbor);
  // Create children and link into mesh
  tri->LeftChild  = _landscape->AllocateTri();
  tri->RightChild = _landscape->AllocateTri();

  // If creation failed, just exit. 
  if ( !tri->LeftChild ) return;

  // Fill in the information we can get from the parent (neighbor pointers)
  tri->LeftChild->BaseNeighbor  = tri->LeftNeighbor;
  tri->LeftChild->LeftNeighbor  = tri->RightChild;

  tri->RightChild->BaseNeighbor  = tri->RightNeighbor;
  tri->RightChild->RightNeighbor = tri->LeftChild;

  // Link our Left Neighbor to the new children
  if (tri->LeftNeighbor != NULL) {
    if (tri->LeftNeighbor->BaseNeighbor == tri) tri->LeftNeighbor->BaseNeighbor = tri->LeftChild;
    else if (tri->LeftNeighbor->LeftNeighbor == tri) tri->LeftNeighbor->LeftNeighbor = tri->LeftChild;
    else if (tri->LeftNeighbor->RightNeighbor == tri) tri->LeftNeighbor->RightNeighbor = tri->LeftChild;
    else ;// Illegal Left Neighbor!
  }

  // Link our Right Neighbor to the new children
  if (tri->RightNeighbor != NULL) {
    if (tri->RightNeighbor->BaseNeighbor == tri) tri->RightNeighbor->BaseNeighbor = tri->RightChild;
    else if (tri->RightNeighbor->RightNeighbor == tri) tri->RightNeighbor->RightNeighbor = tri->RightChild;
    else if (tri->RightNeighbor->LeftNeighbor == tri) tri->RightNeighbor->LeftNeighbor = tri->RightChild;
    else ;// Illegal Right Neighbor!
  }

  // Link our Base Neighbor to the new children
  if (tri->BaseNeighbor != NULL) {
    if ( tri->BaseNeighbor->LeftChild ) {
      tri->BaseNeighbor->LeftChild->RightNeighbor = tri->RightChild;
      tri->BaseNeighbor->RightChild->LeftNeighbor = tri->LeftChild;
      tri->LeftChild->RightNeighbor = tri->BaseNeighbor->RightChild;
      tri->RightChild->LeftNeighbor = tri->BaseNeighbor->LeftChild;
    } else Split( tri->BaseNeighbor);  // Base Neighbor (in a diamond with us) was not split yet, so do that now.
  } else {  
    // An edge triangle, trivial case.
    tri->LeftChild->RightNeighbor = NULL;
    tri->RightChild->LeftNeighbor = NULL;
  }
}

// ---------------------------------------------------------------------
// Tessellate a Patch.
// Will continue to split until the variance metric is met.
//
void Patch::RecursTessellate( TriTreeNode *tri,
               int leftX,  int leftY,
               int rightX, int rightY,
               int apexX,  int apexY,
               int node ) {
  float TriVariance = 0.0f;
  int centerX = (leftX + rightX)>>1; // Compute X coordinate of center of Hypotenuse
  int centerY = (leftY + rightY)>>1; // Compute Y coord...

  static int var_depth = 1 << VARIANCE_DEPTH;
  
  if ( node < var_depth) {
    // Extremely slow distance metric (sqrt is used).
    // Replace this with a faster one! 
    WorldEntity *focus = ((WorldEntity*)Eris::World::Instance()->getFocusedEntity());
    WFMath::Point<3> v = focus->getAbsPos();
//    cout << _landscape->offset_x << endl;
//    cout << _landscape->offset_y << endl;
    float x = v.x() - (centerX  + _landscape->offset_x);
    float y = v.y() - (centerY + _landscape->offset_y);
//    float z = v.z() - _terrain->getHeight(centerX, centerY);
    assert ((centerX >= 0 && centerX <= 200) && "centerX out of bounds");
    assert ((centerY >= 0 && centerY <= 200) && "centerY out of bounds");
    float z = v.z() - _landscape->getHeight(centerX, centerY);// * _terrain->_terrain_scale;
    
    x += System::instance()->getGraphics()->getCamera()->getXPos();
    y += System::instance()->getGraphics()->getCamera()->getYPos();
    z += System::instance()->getGraphics()->getCamera()->getZPos();
    
    float height = (focus->hasBBox()) ? (focus->getBBox().highCorner().z() - focus->getBBox().lowCorner().z()) : (1.0f);
    z += height;
      
    float distance = sqrt(x * x + y * y + z * z);
	      
    // Egads!  A division too?  What's this world coming to!
    // This should also be replaced with a faster operation.
    TriVariance = ((float)m_CurrentVariance[node] * (float)ROAM::map_size * 2.0f)/distance;  // Take both distance and variance into consideration
  }
  if ( (node >= var_depth) ||  // IF we do not have variance info for this node, then we must have gotten here by splitting, so continue down to the lowest level.
  (TriVariance > _landscape->gFrameVariance)
  )  // OR if we are not below the variance tree, test for variance.
  {
    Split(tri);                            // Split this triangle.
    static int n =(int)(log(_landscape->patch_size)/log(2));
    static int rdepth = 2*n -1;
    if (tri->LeftChild &&
		    node < (1<<rdepth))
//	    ((abs(leftX - rightX) >= 3) || (abs(leftY - rightY) >= 3))) 
    {
      RecursTessellate( tri->LeftChild,   apexX,  apexY, leftX, leftY, centerX, centerY,    node<<1  );
      RecursTessellate( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY, 1+(node<<1) );
    }
  }
}

// ---------------------------------------------------------------------
// Render the tree.  Simple no-fan method.
//
void Patch::RecursRender( TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY ) {
  if ( tri->LeftChild ) {          // All non-leaf nodes have both children, so just check for one
    int centerX = (leftX + rightX)>>1;  // Compute X coordinate of center of Hypotenuse
    int centerY = (leftY + rightY)>>1;  // Compute Y coord...
    RecursRender( tri->LeftChild,  apexX,   apexY, leftX, leftY, centerX, centerY );
    RecursRender( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY );
  } else {              
    // A leaf node!  Output a triangle to be rendered.
    // Actual number of rendered triangles...
    _landscape->gNumTrisRendered++;
    float leftZ  = m_HeightMap[(leftY *(_landscape->map_size+1))+leftX ];
    float rightZ = m_HeightMap[(rightY*(_landscape->map_size+1))+rightX];
    float apexZ  = m_HeightMap[(apexY *(_landscape->map_size+1))+apexX ];

//    leftZ  -= (leftZ  < 127.0f) ? (10.0f) : (0.0f);
//    rightZ -= (rightZ < 127.0f) ? (10.0f) : (0.0f);
//    apexZ  -= (apexZ  < 127.0f) ? (10.0f) : (0.0f);

//    static float scale = _terrain->_terrain_scale;
    
//    leftZ *=  scale;
//    rightZ *=  scale;
//    apexZ *=  scale;
    // Perform polygon coloring based on a height sample

    if (_renderer->checkState(Render::RENDER_LIGHTING)) {
      static float v[3][3];
      static float out[3];
      // Create a vertex normal for this triangle.
      // NOTE: This is an extremely slow operation for illustration purposes only.
      //       You should use a texture map with the lighting pre-applied to the texture.
      v[1][0] = (float) leftX;// / MULT_SCALE_LAND;
      v[1][1] = (float) leftY;// / MULT_SCALE_LAND;
      v[1][2] = (float) leftZ;// / MULT_SCALE_HEIGHT;
      
      v[0][0] = (float) rightX;// / MULT_SCALE_LAND;
      v[0][1] = (float) rightY;// / MULT_SCALE_LAND;
      v[0][2] = (float) rightZ;// / MULT_SCALE_HEIGHT;
  
      v[2][0] = (float) apexX;// / MULT_SCALE_LAND;
      v[2][1] = (float) apexY;// / MULT_SCALE_LAND;
      v[2][2] = (float) apexZ;/// MULT_SCALE_HEIGHT;
  
      calcNormal( v, out );
  
//  std::cout << out[0] << "," << out[1] << "," << out[2] << std::endl;
      //Scale Normal so it will become normalized after scaling is applied to it
//      out[0] /= MULT_SCALE_LAND;
//      out[1] /= MULT_SCALE_LAND;
//      out[2] /= MULT_SCALE_HEIGHT;
      normal_data[n_counter].x = out[0];
      normal_data[n_counter].y = out[1];
      normal_data[n_counter++].z = out[2];
     
      normal_data[n_counter].x = out[0];
      normal_data[n_counter].y = out[1];
      normal_data[n_counter++].z = out[2];
      
      normal_data[n_counter].x = out[0];
      normal_data[n_counter].y = out[1];
      normal_data[n_counter++].z = out[2];
    }
    
//    if (_renderer->checkState(RENDER_TEXTURES)) {
      _renderer->setColour(1.0f, 1.0f, 1.0f, 1.0f);

//      float lx = leftX ; 
//      float ly = leftY;
//      float rx = rightX;
//      float ry = rightY;
//      float ax = apexX;
//      float ay = apexY;
      float lx = leftX + m_WorldX;
      float ly = leftY + m_WorldY;
      float rx = rightX + m_WorldX;
      float ry = rightY + m_WorldY;      
      float ax = apexX + m_WorldX;
      float ay = apexY + m_WorldY;

      lx = lx / _landscape->map_size;
      ly = ly / _landscape->map_size;
      rx = rx / _landscape->map_size; 
      ry = ry / _landscape->map_size;      
      ax = ax / _landscape->map_size;
      ay = ay / _landscape->map_size;
//      float lx = leftX / _landscape->map_size;
//      float ly = leftY / _landscape->map_size;
//      float rx = rightX / _landscape->map_size; 
//      float ry = rightY / _landscape->map_size;      
//      float ax = apexX / _landscape->map_size;
//      float ay = apexY / _landscape->map_size;

      
//      lx /= (float)_landscape->patch_size;
//      ly /= (float)_landscape->patch_size;
//      rx /= (float)_landscape->patch_size;
//      ry /= (float)_landscape->patch_size;
//      ax /= (float)_landscape->patch_size;
//      ay /= (float)_landscape->patch_size;

      texture_data[t_counter].s = lx;
      texture_data[t_counter++].t = ly;
    
      texture_data[t_counter].s = rx;
      texture_data[t_counter++].t = ry;
      
      texture_data[t_counter].s = ax;
      texture_data[t_counter++].t = ay;

      vertex_data[v_counter].x = leftX;
      vertex_data[v_counter].y = leftY;
      vertex_data[v_counter++].z = leftZ;

      vertex_data[v_counter].x = rightX;
      vertex_data[v_counter].y = rightY;
      vertex_data[v_counter++].z = rightZ;

      vertex_data[v_counter].x = apexX;
      vertex_data[v_counter].y = apexY;
      vertex_data[v_counter++].z = apexZ;

//cout << leftX << " " << leftY << " " << leftZ << endl;
      
//    } else {
//      float fColor = (60.0f + leftZ) / 256.0f;
//      if ( fColor > 1.0f )  fColor = 1.0f;
//      _renderer->setColour(0.0f, fColor, 0.0f, 1.0f );
//      glVertex3f((GLfloat) leftX, (GLfloat) leftY, (GLfloat)leftZ);
//      glVertex3f((GLfloat) rightX, (GLfloat) rightY, (GLfloat)rightZ);
//      glVertex3f((GLfloat) apexX, (GLfloat) apexY, (GLfloat)apexZ);
//    }
  }
}

// ---------------------------------------------------------------------
// Computes Variance over the entire tree.  Does not examine node relationships.
//
float Patch::RecursComputeVariance( int leftX,  int leftY, float leftZ,
                        int rightX, int rightY, float rightZ,
                      int apexX,  int apexY, float apexZ,
                      int node)
{
  /*
          /|\
        /  |  \
      /    |    \
    /      |      \
    ~~~~~~~*~~~~~~~  <-- Compute the X and Y coordinates of '*'
  */
  int centerX = (leftX + rightX) >>1;    // Compute X coordinate of center of Hypotenuse
  int centerY = (leftY + rightY) >>1;    // Compute Y coord...
  float myVariance;

  // Get the height value at the middle of the Hypotenuse
  float centerZ  = m_HeightMap[(centerY * (_landscape->map_size+1)) + centerX];

  // Variance of this triangle is the actual height at it's hypotenuse midpoint minus the interpolated height.
  // Use values passed on the stack instead of re-accessing the Height Field.
  myVariance = fabs(centerZ - (leftZ + rightZ / 2));

  // Since we're after speed and not perfect representations,
  //    only calculate variance down to an 8x8 block
  if ( (abs(leftX - rightX) >= 2) ||  (abs(leftY - rightY) >= 2) ) {
    // Final Variance for this node is the max of it's own variance and that of it's children.
    float a = RecursComputeVariance( apexX,   apexY,  apexZ, leftX, leftY, leftZ, centerX, centerY, centerZ,    node<<1 );

    myVariance = MAX(myVariance, a);

    a = RecursComputeVariance( rightX, rightY, rightZ, apexX, apexY, apexZ, centerX, centerY, centerZ, 1+(node<<1));

   myVariance = MAX( myVariance, a);
//	  myVariance = MAX( myVariance, RecursComputeVariance( apexX,   apexY,  apexZ, leftX, leftY, leftZ, centerX, centerY, centerZ,    node<<1 ) );
	                  myVariance = MAX( myVariance, RecursComputeVariance( rightX, rightY, rightZ, apexX, apexY, apexZ, centerX, centerY, centerZ, 1+(node<<1)) );

  }


  // Store the final variance for this node.  Note Variance is never zero.

  if (node < (1<<VARIANCE_DEPTH)) m_CurrentVariance[node] = 1 + myVariance;
  return myVariance;
}

// -------------------------------------------------------------------------------------------------
//  PATCH CLASS
// -------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------
// Initialize a patch.
//
void Patch::Init( int heightX, int heightY, int worldX, int worldY, float *hMap ) {
//  _renderer = System::instance()->getGraphics()->getRender();
//  _landscape = ((ROAM*)System::instance()->getGraphics()->getTerrain())->getLandscape();
  // Clear all the relationships
  m_BaseLeft.RightNeighbor = m_BaseLeft.LeftNeighbor = m_BaseRight.RightNeighbor = m_BaseRight.LeftNeighbor =
    m_BaseLeft.LeftChild = m_BaseLeft.RightChild = m_BaseRight.LeftChild = m_BaseLeft.LeftChild = NULL;

  // Attach the two m_Base triangles together
  m_BaseLeft.BaseNeighbor = &m_BaseRight;
  m_BaseRight.BaseNeighbor = &m_BaseLeft;

  // Store Patch offsets for the world and heightmap.
  m_WorldX = worldX;
  m_WorldY = worldY;

  // Store pointer to first byte of the height data for this patch.
  m_HeightMap = &hMap[heightY * (_landscape->map_size+1) + heightX];

  // Initialize flags
  m_VarianceDirty = 1;
  m_isVisible = 0;
}

// ---------------------------------------------------------------------
// Reset the patch.
//
void Patch::Reset() {
  // Assume patch is not visible.
  m_isVisible = 0;

  // Reset the important relationships
  m_BaseLeft.LeftChild = m_BaseLeft.RightChild = m_BaseRight.LeftChild = m_BaseLeft.LeftChild = NULL;

  // Attach the two m_Base triangles together
  m_BaseLeft.BaseNeighbor = &m_BaseRight;
  m_BaseRight.BaseNeighbor = &m_BaseLeft;

  // Clear the other relationships.
  m_BaseLeft.RightNeighbor = m_BaseLeft.LeftNeighbor = m_BaseRight.RightNeighbor = m_BaseRight.LeftNeighbor = NULL;
}

// ---------------------------------------------------------------------
// Compute the variance tree for each of the Binary Triangles in this patch.
//
void Patch::ComputeVariance() {
  // Compute variance on each of the base triangles...
  m_CurrentVariance = m_VarianceLeft;

  RecursComputeVariance(0, _landscape->patch_size, m_HeightMap[_landscape->patch_size * (_landscape->map_size+1)], _landscape->patch_size, 0, m_HeightMap[_landscape->patch_size], 0, 0, m_HeightMap[0], 1);

  m_CurrentVariance = m_VarianceRight;

  RecursComputeVariance(_landscape->patch_size, 0, m_HeightMap[ _landscape->patch_size], 0, _landscape->patch_size, m_HeightMap[ _landscape->patch_size * (_landscape->map_size+1)], _landscape->patch_size, _landscape->patch_size, m_HeightMap[(_landscape->patch_size * (_landscape->map_size+1)) + _landscape->patch_size], 1);
  // Clear the dirty flag for this patch
  m_VarianceDirty = 0;
}

// ---------------------------------------------------------------------
// Set patch's visibility flag.
//
void Patch::SetVisibility() {// int eyeX, int eyeY, int leftX, int leftY, int rightX, int rightY ) {
  int m_WorldX = this->m_WorldX + _landscape->offset_x;
  int m_WorldY = this->m_WorldY + _landscape->offset_y;
  if (min_height == DEF_VAL) {
    for (int x = this->m_WorldX; x < this->m_WorldX + _landscape->patch_size; x++) {
      for (int y = this->m_WorldY; y < this->m_WorldY + _landscape->patch_size; y++) {
        float h = _landscape->getHeight(x,y);
	if (h < min_height) min_height = h;
      }
    }
//    min_height *= _terrain->_terrain_scale;
  }
  if (max_height == -DEF_VAL) {
    for (int x = this->m_WorldX; x < this->m_WorldX + _landscape->patch_size; x++) {
      for (int y = this->m_WorldY; y < this->m_WorldY + _landscape->patch_size; y++) {
        float h = _landscape->getHeight(x,y);
	if (h > max_height) max_height = h;
      }
    }
//    max_height *= _terrain->_terrain_scale;
  }
//  WFMath::Point<3> corner1 = WFMath::Point<3>(m_WorldX, m_WorldY, _terrain->getHeight(m_WorldX, m_WorldY));
//  WFMath::Point<3> corner2 = WFMath::Point<3>(m_WorldX + _landscape->patch_size, m_WorldY+_landscape->patch_size, _terrain->getHeight(m_WorldX+_landscape->patch_size, m_WorldY+_landscape->patch_size));
  //
  //Need to determine min/max pathc height:

//  WFMath::Point<3> corner1 = WFMath::Point<3>(m_WorldX, m_WorldY, _landscape->getHeight(m_WorldX, m_WorldY) * _terrain->_terrain_scale);
//  WFMath::Point<3> corner2 = WFMath::Point<3>(m_WorldX + _landscape->patch_size, m_WorldY+_landscape->patch_size, _landscape->getHeight(m_WorldX+_landscape->patch_size, m_WorldY+_landscape->patch_size) * _terrain->_terrain_scale);
  WFMath::Point<3> corner1 = WFMath::Point<3>(m_WorldX, m_WorldY, min_height);
  WFMath::Point<3> corner2 = WFMath::Point<3>(m_WorldX + _landscape->patch_size, m_WorldY+_landscape->patch_size, max_height);
  int i = _renderer->patchInFrustum(WFMath::AxisBox<3>(corner1,corner2));//, point);

  if (i != 0) m_isVisible = 1;
  else m_isVisible = 0;
//  m_isVisible = 1;

}

// ---------------------------------------------------------------------
// Create an approximate mesh.
//
void Patch::Tessellate() {
  // Split each of the base triangles
//  int m_WorldX = this->m_WorldX + _landscape->offset_x;
//  int m_WorldY = this->m_WorldY + _landscape->offset_y;
  m_CurrentVariance = m_VarianceLeft;
  assert((m_WorldY+_landscape->patch_size > 0) && "m_WorldY+_landscape->patch_size less than 0");
  assert((m_WorldX+_landscape->patch_size > 0) && "m_WorldX+_landscape->patch_size less than 0");
  RecursTessellate (&m_BaseLeft, m_WorldX, m_WorldY+_landscape->patch_size, m_WorldX+_landscape->patch_size, m_WorldY, m_WorldX, m_WorldY, 1);
  m_CurrentVariance = m_VarianceRight;
  RecursTessellate(&m_BaseRight, m_WorldX+_landscape->patch_size, m_WorldY, m_WorldX, m_WorldY+_landscape->patch_size, m_WorldX+_landscape->patch_size, m_WorldY+_landscape->patch_size,1);
}

// ---------------------------------------------------------------------
// Render the mesh.
//
void Patch::render() {
  v_counter = n_counter = t_counter = 0;
  
  RecursRender(&m_BaseLeft, 0, _landscape->patch_size, _landscape->patch_size, 0, 0, 0);
  RecursRender(&m_BaseRight, _landscape->patch_size, 0, 0, _landscape->patch_size, _landscape->patch_size, _landscape->patch_size);

  _renderer->store();
  _renderer->translateObject(m_WorldX, m_WorldY, 0 );
  _renderer->renderArrays(Graphics::RES_TRIANGLES, 0, v_counter, &vertex_data[0], &texture_data[0], &normal_data[0], true);
  _renderer->restore();
}

void Patch::renderWater() {
  v_counter = n_counter = t_counter = 0;
  RecursRenderWater(&m_BaseLeft, 0, _landscape->patch_size, _landscape->patch_size, 0, 0, 0);
  RecursRenderWater(&m_BaseRight, _landscape->patch_size, 0, 0, _landscape->patch_size, _landscape->patch_size, _landscape->patch_size);

  _renderer->store();
  _renderer->translateObject(m_WorldX, m_WorldY, 0 );
  _renderer->renderArrays(Graphics::RES_TRIANGLES, 0, v_counter, &vertex_data[0], &texture_data[0], &normal_data[0], true);
  _renderer->restore();
}

void Patch::RecursRenderWater( TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY ) {
  if ( tri->LeftChild ) {          // All non-leaf nodes have both children, so just check for one
    int centerX = (leftX + rightX)>>1;  // Compute X coordinate of center of Hypotenuse
    int centerY = (leftY + rightY)>>1;  // Compute Y coord...
    RecursRenderWater( tri->LeftChild,  apexX,   apexY, leftX, leftY, centerX, centerY );
    RecursRenderWater( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY );
  } else {              
    // A leaf node!  Output a triangle to be rendered.
    // Actual number of rendered triangles...
    float leftZ  = m_HeightMap[(leftY *(_landscape->map_size+1))+leftX ];
    float rightZ = m_HeightMap[(rightY*(_landscape->map_size+1))+rightX];
    float apexZ  = m_HeightMap[(apexY *(_landscape->map_size+1))+apexX ];

    // Perform polygon coloring based on a height sample
    if (leftZ <=  Landscape::waterlevel || rightZ <=  Landscape::waterlevel || apexZ <=  Landscape::waterlevel) {
//	    float scale = _terrain->_terrain_scale;
//      leftZ *=  scale;
//      rightZ *=  scale;
//      apexZ *=  scale;
            
      static float v[3][3];
      static float out[3];
      // Create a vertex normal for this triangle.
      // NOTE: This is an extremely slow operation for illustration purposes only.
      //       You should use a texture map with the lighting pre-applied to the texture.
      v[1][0] = (float) leftX;// / MULT_SCALE_LAND;
      v[1][1] = (float) leftY;// / MULT_SCALE_LAND;
      v[1][2] = (float) leftZ;// / MULT_SCALE_HEIGHT;
      
      v[0][0] = (float) rightX;// / MULT_SCALE_LAND;
      v[0][1] = (float) rightY;// / MULT_SCALE_LAND;
      v[0][2] = (float) rightZ;// / MULT_SCALE_HEIGHT;
  
      v[2][0] = (float) apexX;// / MULT_SCALE_LAND;
      v[2][1] = (float) apexY;// / MULT_SCALE_LAND;
      v[2][2] = (float) apexZ;/// MULT_SCALE_HEIGHT;
  
      calcNormal( v, out );
  
//  std::cout << out[0] << "," << out[1] << "," << out[2] << std::endl;
      //Scale Normal so it will become normalized after scaling is applied to it
//      out[0] /= MULT_SCALE_LAND;
//      out[1] /= MULT_SCALE_LAND;
//      out[2] /= MULT_SCALE_HEIGHT;
      normal_data[n_counter].x = out[0];
      normal_data[n_counter].y = out[1];
      normal_data[n_counter++].z = out[2];
     
      normal_data[n_counter].x = out[0];
      normal_data[n_counter].y = out[1];
      normal_data[n_counter++].z = out[2];
      
      normal_data[n_counter].x = out[0];
      normal_data[n_counter].y = out[1];
      normal_data[n_counter++].z = out[2];
//    }
    
//    if (_renderer->checkState(RENDER_TEXTURES)) {
      _renderer->setColour(1.0f, 1.0f, 1.0f, 1.0f);

      float lx = leftX + m_WorldX;
      float ly = leftY + m_WorldY;
      float rx = rightX + m_WorldX;
      float ry = rightY + m_WorldY;      
      float ax = apexX + m_WorldX;
      float ay = apexY + m_WorldY;
      lx = lx / _landscape->map_size;
      ly = ly / _landscape->map_size;
      rx = rx / _landscape->map_size; 
      ry = ry / _landscape->map_size;      
      ax = ax / _landscape->map_size;
      ay = ay / _landscape->map_size;


//      float diff = (sin(System::instance()->getTime() / 1000.0f) - 1.0f);
//      float val = _landscape->patch_size - diff;
//
//      lx /= (float)val;
//      ly /= (float)val;
//      rx /= (float)val;
//      ry /= (float)val;
//      ax /= (float)val;
//      ay /= (float)val;
      
      texture_data[t_counter].s = lx;
      texture_data[t_counter++].t = ly;
    
      texture_data[t_counter].s = rx;
      texture_data[t_counter++].t = ry;
      
      texture_data[t_counter].s = ax;
      texture_data[t_counter++].t = ay;

      float z = (float)Landscape::waterlevel;
      vertex_data[v_counter].x = leftX;
      vertex_data[v_counter].y = leftY;
      vertex_data[v_counter++].z = z;

      vertex_data[v_counter].x = rightX;
      vertex_data[v_counter].y = rightY;
      vertex_data[v_counter++].z = z;

      vertex_data[v_counter].x = apexX;
      vertex_data[v_counter].y = apexY;
      vertex_data[v_counter++].z = z;
  }
  }
}

} /* namespace Sear */
