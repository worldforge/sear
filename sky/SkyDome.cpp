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

} /* namespace Sear */

