// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: SkyBox.cpp,v 1.11 2003-03-06 23:50:38 simon Exp $

#include "common/Log.h"

#include "src/Render.h"
#include "src/Model.h"

#include "SkyBox.h"

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


	
// Setup static data items
Vertex_3 SkyBox::vertex_coords[] = VERTEX_COORDS;
Texel SkyBox::texture_coords[] = TEXTURE_COORDS;
Normal SkyBox::normal_coords[] = NORMAL_COORDS;

SkyBox::SkyBox(System *system, Render *renderer) :
  _system(system),
  _renderer(renderer),
  _initialised(false)
{}

SkyBox::~SkyBox() {
  if (_initialised) shutdown();
}

bool SkyBox::init() {
  if (_initialised) shutdown();
  if (debug) Log::writeLog("SkyBox: Initialising.", Log::LOG_DEFAULT);
  texture_id[TEXTURE_SKY_DAY_NORTH]    = _renderer->requestTexture("skybox", CONFIG_SKY_DAY_NORTH, true);
  texture_id[TEXTURE_SKY_DAY_SOUTH]    = _renderer->requestTexture("skybox", CONFIG_SKY_DAY_SOUTH, true);
  texture_id[TEXTURE_SKY_DAY_WEST]     = _renderer->requestTexture("skybox", CONFIG_SKY_DAY_WEST, true);
  texture_id[TEXTURE_SKY_DAY_EAST]     = _renderer->requestTexture("skybox", CONFIG_SKY_DAY_EAST, true);
  texture_id[TEXTURE_SKY_DAY_TOP]      = _renderer->requestTexture("skybox", CONFIG_SKY_DAY_TOP, true);
  texture_id[TEXTURE_SKY_DAY_BOTTOM]   = _renderer->requestTexture("skybox", CONFIG_SKY_DAY_BOTTOM, true);
  texture_id[TEXTURE_SKY_NIGHT_NORTH]  = _renderer->requestTexture("skybox", CONFIG_SKY_NIGHT_NORTH, true);
  texture_id[TEXTURE_SKY_NIGHT_SOUTH]  = _renderer->requestTexture("skybox", CONFIG_SKY_NIGHT_SOUTH, true);
  texture_id[TEXTURE_SKY_NIGHT_WEST]   = _renderer->requestTexture("skybox", CONFIG_SKY_NIGHT_WEST, true);
  texture_id[TEXTURE_SKY_NIGHT_EAST]   = _renderer->requestTexture("skybox", CONFIG_SKY_NIGHT_EAST, true);
  texture_id[TEXTURE_SKY_NIGHT_TOP]    = _renderer->requestTexture("skybox", CONFIG_SKY_NIGHT_TOP, true);
  texture_id[TEXTURE_SKY_NIGHT_BOTTOM] = _renderer->requestTexture("skybox", CONFIG_SKY_NIGHT_BOTTOM, true);
  _initialised = true;
  return true;
}

void SkyBox::shutdown() {
  if (debug) Log::writeLog("SkyBox: Shutting down", Log::LOG_DEFAULT);
  _initialised = false;
}

void SkyBox::draw() {
  render();
}


void SkyBox::render() {
  float val = 1.0f - _renderer->getLightLevel();
  _renderer->stateChange("sky_0");
  _renderer->setColour(1.0f, 1.0f, 1.0f, 1.0f);
  renderTextureSet(0);
//  float val = 1.0f - System::instance()->getNormalTime();
  _renderer->setColour(1.0f, 1.0f, 1.0f, val);
  _renderer->stateChange("sky_1");
  renderTextureSet(1);
}

void SkyBox::renderTextureSet(int base) {
  base *= 6; //NUM OF CUBE SIDES
  //Top
  _renderer->switchTexture(texture_id[base + TEXTURE_SKY_DAY_TOP]);
  _renderer->renderArrays(Graphics::RES_QUADS, 0, 4, &vertex_coords[0], &texture_coords[0], &normal_coords[0],false);
  //North
  _renderer->switchTexture(texture_id[base + TEXTURE_SKY_DAY_NORTH]);
  _renderer->renderArrays(Graphics::RES_QUADS, 4, 4, &vertex_coords[0], &texture_coords[0], &normal_coords[0],false);
  //South
  _renderer->switchTexture(texture_id[base + TEXTURE_SKY_DAY_SOUTH]);
  _renderer->renderArrays(Graphics::RES_QUADS, 8, 4, &vertex_coords[0], &texture_coords[0], &normal_coords[0],false);
  //East
  _renderer->switchTexture(texture_id[base + TEXTURE_SKY_DAY_EAST]);
  _renderer->renderArrays(Graphics::RES_QUADS, 12, 4, &vertex_coords[0], &texture_coords[0], &normal_coords[0],false);
  //West
  _renderer->switchTexture(texture_id[base + TEXTURE_SKY_DAY_WEST]);
  _renderer->renderArrays(Graphics::RES_QUADS, 16, 4, &vertex_coords[0], &texture_coords[0], &normal_coords[0],false);
  //Bottom
  _renderer->switchTexture(texture_id[base + TEXTURE_SKY_DAY_BOTTOM]);
  _renderer->renderArrays(Graphics::RES_QUADS, 20, 4, &vertex_coords[0], &texture_coords[0], &normal_coords[0],false);
}

} /* namespace Sear */
