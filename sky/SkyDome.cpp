// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: SkyDome.cpp,v 1.3 2002-10-21 20:12:04 simon Exp $

#include "SkyDome.h"

#ifdef DEBUG
  #include "common/mmgr.h"
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

SkyDome::SkyDome() :
  vertex_coords(NULL),
  texture_coords(NULL),
  normal_coords(NULL),
  _initialised(false)
{

}

SkyDome::~SkyDome() {
  if (_initialised) shutdown();
}
  
bool SkyDome::init() {
  if (_initialised) shutdown();
  _initialised = true;
  return true;
}

void SkyDome::shutdown() {
  _initialised = false;
}
 
void SkyDome::update(float time_elapsed) {

}

} /* namespace Sear */

