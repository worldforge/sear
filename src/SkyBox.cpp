// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "SkyBox.h"
#include "Config.h"

#include <GL/gl.h>

namespace Sear {

// Setup static data items
float SkyBox::vertex_coords[] = VERTEX_COORDS;
int   SkyBox::texture_coords[] = TEXTURE_COORDS;
float SkyBox::normal_coords[] = NORMAL_COORDS;

SkyBox::SkyBox(System *system, Render *renderer) :
  _system(system),
  _renderer(renderer)
{}

SkyBox::~SkyBox() {
}

bool SkyBox::init() {
  std::cout << "SkyBox: Initialising." << std::endl;
  texture_id[TEXTURE_SKY_DAY_NORTH]    = _renderer->requestTexture(CONFIG_SKY_DAY_NORTH, true);
  texture_id[TEXTURE_SKY_DAY_SOUTH]    = _renderer->requestTexture(CONFIG_SKY_DAY_SOUTH, true);
  texture_id[TEXTURE_SKY_DAY_WEST]     = _renderer->requestTexture(CONFIG_SKY_DAY_WEST, true);
  texture_id[TEXTURE_SKY_DAY_EAST]     = _renderer->requestTexture(CONFIG_SKY_DAY_EAST, true);
  texture_id[TEXTURE_SKY_DAY_TOP]      = _renderer->requestTexture(CONFIG_SKY_DAY_TOP, true);
  texture_id[TEXTURE_SKY_DAY_BOTTOM]   = _renderer->requestTexture(CONFIG_SKY_DAY_BOTTOM, true);
  texture_id[TEXTURE_SKY_NIGHT_NORTH]  = _renderer->requestTexture(CONFIG_SKY_NIGHT_NORTH, true);
  texture_id[TEXTURE_SKY_NIGHT_SOUTH]  = _renderer->requestTexture(CONFIG_SKY_NIGHT_SOUTH, true);
  texture_id[TEXTURE_SKY_NIGHT_WEST]   = _renderer->requestTexture(CONFIG_SKY_NIGHT_WEST, true);
  texture_id[TEXTURE_SKY_NIGHT_EAST]   = _renderer->requestTexture(CONFIG_SKY_NIGHT_EAST, true);
  texture_id[TEXTURE_SKY_NIGHT_TOP]    = _renderer->requestTexture(CONFIG_SKY_NIGHT_TOP, true);
  texture_id[TEXTURE_SKY_NIGHT_BOTTOM] = _renderer->requestTexture(CONFIG_SKY_NIGHT_BOTTOM, true);
  return true;
}

void SkyBox::shutdown() {
  cout << "SkyBox: Shutting down" << endl;
}

void SkyBox::draw() {
  render();
}


void SkyBox::render() {
  float val = 1.0f - _renderer->getLightLevel();
  _renderer->setColour(1.0f, 1.0f, 1.0f, 1.0f);
  renderTextureSet(0);
  glEnable(GL_BLEND);
//  float val = 1.0f - System::instance()->getNormalTime();
  _renderer->setColour(1.0f, 1.0f, 1.0f, val);
  renderTextureSet(1);
  glDisable(GL_BLEND);
}

void SkyBox::renderTextureSet(int base) {
  base *= 6; //NUM OF CUBE SIDES
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  
  glVertexPointer(3, GL_FLOAT, 0, vertex_coords);
  glTexCoordPointer(2, GL_INT, 0, texture_coords);
  glNormalPointer(GL_FLOAT, 0, normal_coords);
 
  //Top
  _renderer->switchTexture(texture_id[base + TEXTURE_SKY_DAY_TOP]);
  glDrawArrays(GL_QUADS, 0, 4);
  //North
  _renderer->switchTexture(texture_id[base + TEXTURE_SKY_DAY_NORTH]);
  glDrawArrays(GL_QUADS, 4, 4);
  //South
  _renderer->switchTexture(texture_id[base + TEXTURE_SKY_DAY_SOUTH]);
  glDrawArrays(GL_QUADS, 8, 4);
  //East
  _renderer->switchTexture(texture_id[base + TEXTURE_SKY_DAY_EAST]);
  glDrawArrays(GL_QUADS, 12, 4);
  //West
  _renderer->switchTexture(texture_id[base + TEXTURE_SKY_DAY_WEST]);
  glDrawArrays(GL_QUADS, 16, 4);
  //Bottom
  _renderer->switchTexture(texture_id[base + TEXTURE_SKY_DAY_BOTTOM]);
  glDrawArrays(GL_QUADS, 20, 4);
  
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
}

} /* namespace Sear */
