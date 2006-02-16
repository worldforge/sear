// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 Simon Goodall

#include <sigc++/bind.h>

#include <sage/sage.h>
#include <sage/GL.h>
#include <sage/GLU.h>

#include "src/WorldEntity.h"

#include "renderers/RenderSystem.h"

#include "Weather.h"

static const bool debug = false;

static const std::string ATTR_RAIN = "rain";
static const std::string ATTR_SNOW = "snow";

namespace Sear {

Weather::Weather() :
  m_initialised(false),
  m_rain(0.0f),
  m_snow(0.0f)
{}

Weather::~Weather() {
  assert(m_initialised == false);
}

int Weather::init() {
  assert(m_initialised == false);
  m_initialised = true;
  return 0;
}

void Weather::shutdown() {
  assert(m_initialised == true);

  notify_callbacks();

  m_initialised = false;
}

void Weather::setWeatherEntity(WorldEntity *we) {
  assert(m_initialised == true);
  // Get initial values
  if (we->hasAttr(ATTR_RAIN)) {
    m_rain = we->valueOfAttr(ATTR_RAIN).asNum();
  }  
  if (we->hasAttr(ATTR_SNOW)) {
    m_snow = we->valueOfAttr(ATTR_SNOW).asNum();
  }
  // Link to callback in case it ever changes.
  we->Changed.connect(sigc::bind(sigc::mem_fun(this, &Weather::weatherChanged), we));
}

void Weather::weatherChanged(const Eris::StringSet &s, Sear::WorldEntity *we) {
  Eris::Entity::AttrMap am = we->getAttributes();
  Eris::Entity::AttrMap::const_iterator I = am.begin();
  while (I != am.end()) {
    std::string attr_name = I->first;
    if (attr_name == ATTR_RAIN) {
      m_rain = we->valueOfAttr(ATTR_RAIN).asNum();
    }  
    else if (attr_name == ATTR_SNOW) {
      m_snow = we->valueOfAttr(ATTR_SNOW).asNum();
    }
    ++I;
  }
}
   
void Weather::render() {

  glColor4f(1.0f, 1.0f, 1.0f, 0.0f);

  int num_points = (int)(500.0 * m_rain);

  float buf[3 * num_points];
  int cnt = -1;
  for (int i = 0; i <  num_points; ++i) {
    buf[++cnt] =  (float)rand() / (float)RAND_MAX * 640.0;
    buf[++cnt] =  (float)rand() / (float)RAND_MAX * 480.0;
    buf[++cnt] =  (float)rand() / (float)RAND_MAX * -2.0;
  }
  float maxSize = 1.0f;
  bool texEnabled = glIsEnabled(GL_TEXTURE_2D);
  int pSize = 1;
  glGetIntegerv(GL_POINT_SIZE,  &pSize);
  if (sage_ext[GL_ARB_POINT_SPRITE]) {
    glGetFloatv(GL_POINT_SIZE_MAX_ARB, &maxSize);
    if (maxSize > 5.0f) maxSize = 5.0f;
    glEnable( GL_POINT_SPRITE_ARB );
    glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
    glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 1.0f );
    glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
    glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, maxSize );

    RenderSystem::getInstance().switchTexture(
      RenderSystem::getInstance().requestTexture("rain_drop"));
  } else {
    if (texEnabled) glDisable(GL_TEXTURE_2D);
  }
  glPointSize(maxSize);
 
  glVertexPointer(3, GL_FLOAT,0,buf);
  glDrawArrays(GL_POINTS,0,num_points);

  if (sage_ext[GL_ARB_POINT_SPRITE]) {
    glDisable( GL_POINT_SPRITE_ARB );
  } else {
    if (texEnabled) glEnable(GL_TEXTURE_2D);
  }
  glPointSize(pSize);
}

} // namespace Sear
