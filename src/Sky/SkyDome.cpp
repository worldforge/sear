// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall
 
#include "SkyDome.h"

namespace Sear {

SkyDome::SkyDome() :
  vertex_coords(NULL),
  texture_coords(NULL),
  normal_coords(NULL)
{

}

SkyDome::~SkyDome() {

}
  
bool SkyDome::init() {
  return true;
}

void SkyDome::shutdown() {

}
 
void SkyDome::update(float time_elapsed) {

}

int SkyDome::getHRes() {
  return 0;
}

int SkyDome::getVRes() {
  return 0;
}
  
float *SkyDome::getVertexData() {
  return NULL;
}

float *SkyDome::getTextureData() {
  return NULL;
}

float *SkyDome::getNormalData() {
  return NULL;
}
  
bool SkyDome::hasVertexData() {
  return true;
}

bool SkyDome::hasTextureData() {
  return true;
}

bool SkyDome::hasNormalData() {
  return true;
}

}

