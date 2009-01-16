// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: LightManager.cpp,v 1.5 2007-05-02 20:47:55 simon Exp $

#include <sage/GL.h>

#include "LightManager.h"
#include "Light.h"

#include "src/System.h"

#include <cassert>

namespace Sear {

LightManager::LightManager() :
  m_initialised(false),
  m_totalLights(0),
  m_lightCounter(0)
{}

LightManager::~LightManager() {
  if (m_initialised) shutdown();
}
  
int LightManager::init() {
  assert (m_initialised == false);

  m_initialised = true;
  return 0;
}

void LightManager::shutdown() {
  assert (m_initialised == true);

  m_initialised = false;
}

void LightManager::reset() {
  // Reset counter
  m_lightCounter = 0;
  // Reset lights
  glDisable(GL_LIGHT2);
  glDisable(GL_LIGHT3);
  glDisable(GL_LIGHT4);
  glDisable(GL_LIGHT5);
  glDisable(GL_LIGHT6);
  glDisable(GL_LIGHT7);
}

void LightManager::applyLight(const Light &light) {
  // Don't do anything if the light is disabled
  if (!light.enabled) return;

  // Increment counter so we know how many lights we have used this frame
  ++m_lightCounter;

  // Select light number
  GLenum lightNum = GL_LIGHT2;
  switch (m_lightCounter) {
    case (0) : lightNum = GL_LIGHT2; break;
    case (1) : lightNum = GL_LIGHT3; break;
    case (2) : lightNum = GL_LIGHT4; break;
    case (3) : lightNum = GL_LIGHT5; break;
    case (4) : lightNum = GL_LIGHT6; break;
    case (5) : lightNum = GL_LIGHT7; break;
    default: return; // Number out of allowed range
  }

  // Turn on light
  glEnable(lightNum);

  // Set position
  float pos[] = {light.position.x(), light.position.y(), light.position.z(), 1.0f}; // 1.0f for positional light
  glLightfv(lightNum, GL_POSITION, pos);

  // Set attenuation levels
  glLightf(lightNum, GL_CONSTANT_ATTENUATION, light.attenuation_constant);
  glLightf(lightNum, GL_LINEAR_ATTENUATION, light.attenuation_linear);
  glLightf(lightNum, GL_QUADRATIC_ATTENUATION, light.attenuation_quadratic);

  // Set light colour
  glLightfv(lightNum, GL_AMBIENT, light.ambient);
  glLightfv(lightNum, GL_DIFFUSE, light.diffuse);
  glLightfv(lightNum, GL_SPECULAR, light.specular);
}

} /* namespace Sear */

