// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// Code based upon ROAM Simplistic Implementation by Bryan Turner bryan.turner@pobox.com

#include <math.h>
#include <GL/gl.h>		// OpenGL

#include <Eris/World.h>
#include <Eris/Entity.h>

#include "System.h"
#include "Patch.h"
#include "Landscape.h"
#include "Terrain.h"
#include "Camera.h"
#include "Render.h"
#include "Utility.h"
// -------------------------------------------------------------------------------------------------
//	PATCH CLASS
// -------------------------------------------------------------------------------------------------

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
  tri->LeftChild  = Landscape::AllocateTri();
  tri->RightChild = Landscape::AllocateTri();

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
							 int node )
{
  float TriVariance = 0.0f;
  int centerX = (leftX + rightX)>>1; // Compute X coordinate of center of Hypotenuse
  int centerY = (leftY + rightY)>>1; // Compute Y coord...

  if ( node < (1<<VARIANCE_DEPTH) ) {
    // Extremely slow distance metric (sqrt is used).
    // Replace this with a faster one!
	 
    float distance = _renderer->distFromNear(centerX - 100, centerY - 100, _landscape->getHeight(centerX - 100, centerY - 100));

    // Egads!  A division too?  What's this world coming to!
    // This should also be replaced with a faster operation.
    TriVariance = ((float)m_CurrentVariance[node] * _landscape->map_size * 2)/distance;	// Take both distance and variance into consideration
  }
  if ( (node >= (1<<VARIANCE_DEPTH)) ||	// IF we do not have variance info for this node, then we must have gotten here by splitting, so continue down to the lowest level.
  (TriVariance > _landscape->gFrameVariance))	// OR if we are not below the variance tree, test for variance.
  {
    Split(tri);														// Split this triangle.
    int n =(int)(log(_landscape->patch_size)/log(2));
    int rdepth = 2*n -1;
    if (tri->LeftChild && node < (1<<rdepth))
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
  if ( tri->LeftChild ) {					// All non-leaf nodes have both children, so just check for one
    int centerX = (leftX + rightX)>>1;	// Compute X coordinate of center of Hypotenuse
    int centerY = (leftY + rightY)>>1;	// Compute Y coord...
    RecursRender( tri->LeftChild,  apexX,   apexY, leftX, leftY, centerX, centerY );
    RecursRender( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY );
  } else {									// A leaf node!  Output a triangle to be rendered.
    // Actual number of rendered triangles...
    _landscape->gNumTrisRendered++;

    GLfloat leftZ  = m_HeightMap[(leftY *_landscape->map_size)+leftX ];
    GLfloat rightZ = m_HeightMap[(rightY*_landscape->map_size)+rightX];
    GLfloat apexZ  = m_HeightMap[(apexY *_landscape->map_size)+apexX ];

    leftZ  -= (leftZ  < 127.0f) ? (10.0f) : (0.0f);
    rightZ -= (rightZ < 127.0f) ? (10.0f) : (0.0f);
    apexZ  -= (apexZ  < 127.0f) ? (10.0f) : (0.0f);

    float scale = _renderer->getTerrain()->_terrain_scale;
    
    leftZ *=  scale;
    rightZ *=  scale;
    apexZ *=  scale;
    // Perform polygon coloring based on a height sample

    if (_renderer->checkState(RENDER_LIGHTING)) {
      float v[3][3];
      float out[3];
      // Create a vertex normal for this triangle.
      // NOTE: This is an extremely slow operation for illustration purposes only.
      //       You should use a texture map with the lighting pre-applied to the texture.
      v[1][0] = (GLfloat) leftX;// / MULT_SCALE_LAND;
      v[1][1] = (GLfloat) leftY;// / MULT_SCALE_LAND;
      v[1][2] = (GLfloat) leftZ;// / MULT_SCALE_HEIGHT;
      
      v[0][0] = (GLfloat) rightX;// / MULT_SCALE_LAND;
      v[0][1] = (GLfloat) rightY;// / MULT_SCALE_LAND;
      v[0][2] = (GLfloat) rightZ;// / MULT_SCALE_HEIGHT;
  
      v[2][0] = (GLfloat) apexX;// / MULT_SCALE_LAND;
      v[2][1] = (GLfloat) apexY;// / MULT_SCALE_LAND;
      v[2][2] = (GLfloat) apexZ;/// MULT_SCALE_HEIGHT;
  
      calcNormal( v, out );
  
//  std::cout << out[0] << "," << out[1] << "," << out[2] << std::endl;
      //Scale Normal so it will become normalized after scaling is applied to it
//      out[0] /= MULT_SCALE_LAND;
//      out[1] /= MULT_SCALE_LAND;
//      out[2] /= MULT_SCALE_HEIGHT;
      glNormal3fv( out );
    }
    
//    if (_renderer->checkState(RENDER_TEXTURES)) {
      _renderer->setColour(1.0f, 1.0f, 1.0f, 1.0f);

      float lx = leftX;
      float ly = leftY;
      float rx = rightX; 
      float ry = rightY;	    
      float ax = apexX;
      float ay = apexY;
      lx /= (float)_landscape->patch_size;
      ly /= (float)_landscape->patch_size;
      rx /= (float)_landscape->patch_size;
      ry /= (float)_landscape->patch_size;
      ax /= (float)_landscape->patch_size;
      ay /= (float)_landscape->patch_size;

      glTexCoord2f(lx, ly);
      glVertex3f((GLfloat) leftX, (GLfloat) leftY, (GLfloat)leftZ);
      glTexCoord2f(rx, ry);
      glVertex3f((GLfloat) rightX, (GLfloat) rightY, (GLfloat)rightZ);
      glTexCoord2f(ax, ay);
      glVertex3f((GLfloat) apexX, (GLfloat) apexY, (GLfloat)apexZ);
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
unsigned char Patch::RecursComputeVariance( int leftX,  int leftY,  unsigned char leftZ,
										    int rightX, int rightY, unsigned char rightZ,
											int apexX,  int apexY,  unsigned char apexZ,
											int node)
{
  /*
          /|\
        /  |  \
      /    |    \
    /      |      \
    ~~~~~~~*~~~~~~~  <-- Compute the X and Y coordinates of '*'
  */
  int centerX = (leftX + rightX) >>1;		// Compute X coordinate of center of Hypotenuse
  int centerY = (leftY + rightY) >>1;		// Compute Y coord...
  unsigned char myVariance;

  // Get the height value at the middle of the Hypotenuse
  unsigned char centerZ  = m_HeightMap[(centerY * _landscape->map_size) + centerX];

  // Variance of this triangle is the actual height at it's hypotenuse midpoint minus the interpolated height.
  // Use values passed on the stack instead of re-accessing the Height Field.
  myVariance = abs((int)centerZ - (((int)leftZ + (int)rightZ)>>1));

  // Since we're after speed and not perfect representations,
  //    only calculate variance down to an 8x8 block
  if ( (abs(leftX - rightX) >= 8) ||  (abs(leftY - rightY) >= 8) ) {
    // Final Variance for this node is the max of it's own variance and that of it's children.
    unsigned char a = RecursComputeVariance( apexX,   apexY,  apexZ, leftX, leftY, leftZ, centerX, centerY, centerZ,    node<<1 );
    myVariance = MAX(myVariance, a);
    a = RecursComputeVariance( rightX, rightY, rightZ, apexX, apexY, apexZ, centerX, centerY, centerZ, 1+(node<<1));
    myVariance = MAX( myVariance, a);
  }


  // Store the final variance for this node.  Note Variance is never zero.

  if (node < (1<<VARIANCE_DEPTH)) m_CurrentVariance[node] = 1 + myVariance;
  return myVariance;
}

// -------------------------------------------------------------------------------------------------
//	PATCH CLASS
// -------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------
// Initialize a patch.
//
void Patch::Init( int heightX, int heightY, int worldX, int worldY, unsigned char *hMap ) {
//  _renderer = System::instance()->getRenderer();
  _landscape = _renderer->getTerrain()->getLandscape();
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
  m_HeightMap = &hMap[heightY * _landscape->map_size + heightX];

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
  RecursComputeVariance(0, _landscape->patch_size, m_HeightMap[_landscape->patch_size * _landscape->map_size], _landscape->patch_size, 0, m_HeightMap[_landscape->patch_size], 0, 0, m_HeightMap[0], 1);
  m_CurrentVariance = m_VarianceRight;
  RecursComputeVariance(_landscape->patch_size, 0, m_HeightMap[ _landscape->patch_size], 0, _landscape->patch_size, m_HeightMap[ _landscape->patch_size * _landscape->map_size], _landscape->patch_size, _landscape->patch_size, m_HeightMap[(_landscape->patch_size * _landscape->map_size) + _landscape->patch_size], 1);
  // Clear the dirty flag for this patch
  m_VarianceDirty = 0;
}

// ---------------------------------------------------------------------
// Discover the orientation of a triangle's points:
//
// Taken from "Programming Principles in Computer Graphics", L. Ammeraal (Wiley)
//
inline int orientation( int pX, int pY, int qX, int qY, int rX, int rY ) {
  int aX, aY, bX, bY;
  float d;

  aX = qX - pX;
  aY = qY - pY;

  bX = rX - pX;
  bY = rY - pY;

  d = (float)aX * (float)bY - (float)aY * (float)bX;
  return (d < 0) ? (-1) : (d > 0);
}

// ---------------------------------------------------------------------
// Set patch's visibility flag.
//
void Patch::SetVisibility() {// int eyeX, int eyeY, int leftX, int leftY, int rightX, int rightY ) {

  WFMath::Point<3> corner1 = WFMath::Point<3>(m_WorldX -100, m_WorldY-100, _landscape->getHeight(m_WorldX-100, m_WorldY -100));
  WFMath::Point<3> corner2 = WFMath::Point<3>(m_WorldX + _landscape->patch_size - 100, m_WorldY+_landscape->patch_size - 100, _landscape->getHeight(m_WorldX+_landscape->patch_size-100, m_WorldY+_landscape->patch_size-100));
  int i = _renderer->patchInFrustum(WFMath::AxisBox<3>(corner1,corner2));//, point);

  if (i != 0) m_isVisible = 1;
  else m_isVisible = 0;

}

// ---------------------------------------------------------------------
// Create an approximate mesh.
//
void Patch::Tessellate() {
  // Split each of the base triangles
  m_CurrentVariance = m_VarianceLeft;
  RecursTessellate (&m_BaseLeft, m_WorldX, m_WorldY+_landscape->patch_size, m_WorldX+_landscape->patch_size, m_WorldY, m_WorldX, m_WorldY, 1);
  m_CurrentVariance = m_VarianceRight;
  RecursTessellate(&m_BaseRight, m_WorldX+_landscape->patch_size, m_WorldY, m_WorldX, m_WorldY+_landscape->patch_size, m_WorldX+_landscape->patch_size, m_WorldY+_landscape->patch_size,1);
}

// ---------------------------------------------------------------------
// Render the mesh.
//
void Patch::render()
{
  // Store old matrix
  glPushMatrix();
  // Translate the patch to the proper world coordinates
  glTranslatef( (GLfloat)m_WorldX, (GLfloat)m_WorldY, 0 );
  glBegin(GL_TRIANGLES);
    RecursRender(&m_BaseLeft, 0, _landscape->patch_size, _landscape->patch_size, 0, 0, 0);
    RecursRender(&m_BaseRight, _landscape->patch_size, 0, 0, _landscape->patch_size, _landscape->patch_size, _landscape->patch_size);
  glEnd();
  // Restore the matrix
  glPopMatrix();
}

void Patch::renderWater() {
//	return;
   // Store old matrix
   glPushMatrix();
   // Translate the patch to the proper world coordinates
   glTranslatef( (GLfloat)m_WorldX, (GLfloat)m_WorldY, 0 );
   glBegin(GL_TRIANGLES);
    RecursRenderWater(&m_BaseLeft, 0, _landscape->patch_size, _landscape->patch_size, 0, 0, 0);
    RecursRenderWater(&m_BaseRight, _landscape->patch_size, 0, 0, _landscape->patch_size, _landscape->patch_size, _landscape->patch_size);
  glEnd();
  // Restore the matrix
  glPopMatrix();
}

void Patch::RecursRenderWater( TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY) {
  if ( tri->LeftChild ) { // All non-leaf nodes have both children, so just check for one
    int centerX = (leftX + rightX)>>1;      // Compute X coordinate of center of Hypotenuse
    int centerY = (leftY + rightY)>>1;      // Compute Y coord...
    RecursRenderWater( tri->LeftChild,  apexX,   apexY, leftX, leftY, centerX, centerY );
    RecursRenderWater( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY );
  } else { // A leaf node!  Output a triangle to be rendered.
    // Actual number of rendered triangles...
    GLfloat leftZ  = m_HeightMap[(leftY *_landscape->map_size)+leftX ];
    GLfloat rightZ = m_HeightMap[(rightY*_landscape->map_size)+rightX];
    GLfloat apexZ  = m_HeightMap[(apexY *_landscape->map_size)+apexX ];

    if (leftZ <=  Landscape::waterlevel || rightZ <=  Landscape::waterlevel || apexZ <=  Landscape::waterlevel) {
      leftZ *=  MULT_SCALE_HEIGHT;
      rightZ *=  MULT_SCALE_HEIGHT;
      apexZ *=  MULT_SCALE_HEIGHT;
      // Output the LEFT VERTEX for the triangle
//      if (_renderer->checkState(RENDER_LIGHTING)) {
        float v[3][3];
        float out[3];
        // Create a vertex normal for this triangle.
        // NOTE: This is an extremely slow operation for illustration purposes only.
        //       You should use a texture map with the lighting pre-applied to the texture.
        v[1][0] = (GLfloat) leftX;// / MULT_SCALE_LAND;
        v[1][1] = (GLfloat) leftY;// / MULT_SCALE_LAND;
        v[1][2] = (GLfloat) leftZ;// / MULT_SCALE_HEIGHT;
      
        v[0][0] = (GLfloat) rightX;// / MULT_SCALE_LAND;
        v[0][1] = (GLfloat) rightY;// / MULT_SCALE_LAND;
        v[0][2] = (GLfloat) rightZ;// / MULT_SCALE_HEIGHT;
    
        v[2][0] = (GLfloat) apexX;// / MULT_SCALE_LAND;
        v[2][1] = (GLfloat) apexY;// / MULT_SCALE_LAND;
        v[2][2] = (GLfloat) apexZ;// / MULT_SCALE_HEIGHT;
 
       
        calcNormal( v, out );
        //Scale Normal so it will become normalized after scaling is applied to it
//        out[0] /= MULT_SCALE_LAND;
//        out[1] /= MULT_SCALE_LAND;
//        out[2] /= MULT_SCALE_HEIGHT;
  
        glNormal3fv( out );
//      }
    
      if (_renderer->checkState(RENDER_TEXTURES)) {
//        _renderer->setColour(1.0f, 1.0f, 1.0f, 1.0f);

        float lx = leftX;
        float ly = leftY;
        float rx = rightX; 
        float ry = rightY;	    
        float ax = apexX;
        float ay = apexY;

	float diff = (sin(System::instance()->getTime() / 1000.0f) - 1.0f)/ 1.0f;
        float val = _landscape->patch_size - diff;
	
        lx /= (float)val;
        ly /= (float)val;
        rx /= (float)val;
        ry /= (float)val;
        ax /= (float)val;
	ay /= (float)val;


//        lx /= (float)_landscape->patch_size;
//        ly /= (float)_landscape->patch_size;
//        rx /= (float)_landscape->patch_size;
//        ry /= (float)_landscape->patch_size;
//        ax /= (float)_landscape->patch_size;
//        ay /= (float)_landscape->patch_size;
	
//	lx -= diff;
//	ly -= diff;
//	rx -= diff;
//	ry -= diff;
//	ax -= diff;
//	ay -= diff;
	

        glTexCoord2f(lx, ly);
        glVertex3f((GLfloat) leftX, (GLfloat) leftY,  (GLfloat)Landscape::waterlevel * MULT_SCALE_HEIGHT);
        glTexCoord2f(rx, ry);
        glVertex3f((GLfloat) rightX, (GLfloat) rightY,  (GLfloat)Landscape::waterlevel * MULT_SCALE_HEIGHT);
        glTexCoord2f(ax, ay);
        glVertex3f((GLfloat) apexX, (GLfloat) apexY,  (GLfloat)Landscape::waterlevel * MULT_SCALE_HEIGHT);
      } else {
        glVertex3f((GLfloat) leftX, (GLfloat) leftY,  (GLfloat)Landscape::waterlevel * MULT_SCALE_HEIGHT);
        glVertex3f((GLfloat) rightX, (GLfloat) rightY,  (GLfloat)Landscape::waterlevel * MULT_SCALE_HEIGHT);
        glVertex3f((GLfloat) apexX, (GLfloat) apexY,  (GLfloat)Landscape::waterlevel * MULT_SCALE_HEIGHT);
      }
    }
  }
}
