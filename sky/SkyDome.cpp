// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall

// $Id: SkyDome.cpp,v 1.4 2003-03-06 23:50:38 simon Exp $

#include "SkyDome.h"

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

