// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "common/Utility.h"

#include "System.h"
#include "Terrain.h"
#include "WorldEntity.h"
#include "Frustum.h"

namespace Sear {

void Frustum::getFrustum(float frustum[6][4], float proj[16], float modl[16]) {
  float   clip[16];
  float   t;
  
  /* Combine the two matrices (multiply projection by modelview) */
  clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
  clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
  clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
  clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];
  clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
  clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
  clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
  clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];
  clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
  clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
  clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
  clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];
  clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
  clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
  clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
  clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

  /* Extract the numbers for the RIGHT plane */
  frustum[0][0] = clip[ 3] - clip[ 0];
  frustum[0][1] = clip[ 7] - clip[ 4];
  frustum[0][2] = clip[11] - clip[ 8];
  frustum[0][3] = clip[15] - clip[12];

  /* Normalize the result */
  t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
  frustum[0][0] /= t;
  frustum[0][1] /= t;
  frustum[0][2] /= t;
  frustum[0][3] /= t;

  /* Extract the numbers for the LEFT plane */
  frustum[1][0] = clip[ 3] + clip[ 0];
  frustum[1][1] = clip[ 7] + clip[ 4];
  frustum[1][2] = clip[11] + clip[ 8];
  frustum[1][3] = clip[15] + clip[12];

  /* Normalize the result */
  t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
  frustum[1][0] /= t;
  frustum[1][1] /= t;
  frustum[1][2] /= t;
  frustum[1][3] /= t;

  /* Extract the BOTTOM plane */
  frustum[2][0] = clip[ 3] + clip[ 1];
  frustum[2][1] = clip[ 7] + clip[ 5];
  frustum[2][2] = clip[11] + clip[ 9];
  frustum[2][3] = clip[15] + clip[13];

  /* Normalize the result */
  t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
  frustum[2][0] /= t;
  frustum[2][1] /= t;
  frustum[2][2] /= t;
  frustum[2][3] /= t;

  /* Extract the TOP plane */
  frustum[3][0] = clip[ 3] - clip[ 1];
  frustum[3][1] = clip[ 7] - clip[ 5];
  frustum[3][2] = clip[11] - clip[ 9];
  frustum[3][3] = clip[15] - clip[13];

  /* Normalize the result */
  t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
  frustum[3][0] /= t;
  frustum[3][1] /= t;
  frustum[3][2] /= t;
  frustum[3][3] /= t;

  /* Extract the FAR plane */
  frustum[4][0] = clip[ 3] - clip[ 2];
  frustum[4][1] = clip[ 7] - clip[ 6];
  frustum[4][2] = clip[11] - clip[10];
  frustum[4][3] = clip[15] - clip[14];

  /* Normalize the result */
  t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
  frustum[4][0] /= t;
  frustum[4][1] /= t;
  frustum[4][2] /= t;
  frustum[4][3] /= t;

  /* Extract the NEAR plane */
  frustum[5][0] = clip[ 3] + clip[ 2];
  frustum[5][1] = clip[ 7] + clip[ 6];
  frustum[5][2] = clip[11] + clip[10];
  frustum[5][3] = clip[15] + clip[14];

   /* Normalize the result */
   t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
   frustum[5][0] /= t;
   frustum[5][1] /= t;
   frustum[5][2] /= t;
   frustum[5][3] /= t;
}

bool Frustum::pointInFrustum(float frustum[6][4], float x, float y, float z ) {
  int p;
  for( p = 0; p < 6; p++ )
    if( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= 0 ) return false;
  return true;
} 

int Frustum::cubeInFrustum(float frustum[6][4], WorldEntity *we ) {  
  int p;
  int c;
  int c2 = 0;
  if (!we->hasBBox()) return true;
  WFMath::AxisBox<3> entity_bbox = we->getBBox();
  WFMath::Point<3> pos = we->getAbsPos();
  //Translate BBox to correct position
  WFMath::Point<3> lowCorner = WFMath::Point<3>(entity_bbox.lowCorner().x() + pos.x(), entity_bbox.lowCorner().y() + pos.y(), entity_bbox.lowCorner().z() + pos.z());
  WFMath::Point<3> highCorner = WFMath::Point<3>(entity_bbox.highCorner().x() + pos.x(), entity_bbox.highCorner().y() + pos.y(), entity_bbox.highCorner().z() + pos.z());


  WFMath::AxisBox<3> bbox = WFMath::AxisBox<3>(lowCorner, highCorner);

  for( p = 0; p < 6; p++ ) {
    c = 0;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( c == 0 ) return 0;
    if( c == 8 ) c2++;
  }
  return (c2 == 6) ? 2 : 1;
}


int Frustum::patchInFrustum(float frustum[6][4], WFMath::AxisBox<3> bbox) {  
  int p;
  int c;
  int c2 = 0;

  for( p = 0; p < 6; p++ ) {
    c = 0;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.lowCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.lowCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.lowCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( frustum[p][0] * (bbox.highCorner().x()) + frustum[p][1] * (bbox.highCorner().y()) + frustum[p][2] * (bbox.highCorner().z()) + frustum[p][3] > 0 ) c++;
    if( c == 0 ) return 0;
    if( c == 8 ) c2++;
  }
  return (c2 == 6) ? 2 : 1;
}

float Frustum::distFromNear(float frustum[6][4], float x, float y, float z) {
  return (frustum[5][0] * x + frustum[5][1] * y + frustum[5][2] * z + frustum[5][3]);
}

bool Frustum::sphereInFrustum(float frustum[6][4], WorldEntity *we, Terrain *terrain) {
//  return true;	
  int p;
  float x, y, z, radius;
  WFMath::AxisBox<3> bbox = bboxCheck(we->getBBox());
  WFMath::Ball<3> b = bbox.boundingSphere();
  x = b.getCenter().x() + we->getAbsPos().x();
  y = b.getCenter().y() + we->getAbsPos().y();
  z = b.getCenter().z() + we->getAbsPos().z() + terrain->getHeight(we->getAbsPos().x(), we->getAbsPos().y());
  radius = b.radius();
  for( p = 0; p < 6; p++ )
    if( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= -radius )
      return false;
    return true;
}


} /* namespace Sear */
