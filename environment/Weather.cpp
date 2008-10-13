// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 - 2008  Simon Goodall

/*
 * This class is meant to model weather in Sear.
 * Currently it renders rain to the screen. ideally, the rain drop would be
 * differing sizes, but the attenutation stuff in opengl is distance from the
 * eye which in 2D mode, appears to be from the bottom left corner.
 * Raining should also affect the brightness, i.e. make it darker when it rains
 *
 */

#include <sigc++/bind.h>

#include <sage/sage.h>
#include <sage/GL.h>
#include <sage/GLU.h>

#include "common/Utility.h"

#include "src/WorldEntity.h"
#include "src/Console.h"

#include "renderers/RenderSystem.h"

#include "Weather.h"

static const bool debug = false;

static const std::string SECTION_weather = "weather";

static const std::string KEY_draw_rain = "draw_rain";

static const bool DEFAULT_draw_rain = true;

static const std::string ATTR_RAIN = "rain";
static const std::string ATTR_SNOW = "snow";
static const std::string ATTR_VISIBILITY = "visibility";

static const std::string CMD_SET_RAIN = "set_rain";
static const std::string CMD_SET_SNOW = "set_snow";
static const std::string CMD_SET_VISIBILITY = "set_visibility";

namespace Sear {

Weather::Weather() :
  m_initialised(false),
  m_rain(0.0f),
  m_snow(0.0f),
  m_visibility(500.0f),
  m_current_visibility(10.0f)
{}

Weather::~Weather() {
  if (m_initialised) shutdown();
}

int Weather::init() {
  assert(m_initialised == false);

  m_rain_drop_id = RenderSystem::getInstance().requestTexture("rain_drop");

  m_initialised = true;
  return 0;
}

void Weather::shutdown() {
  assert(m_initialised == true);

  RenderSystem::getInstance().releaseTexture(m_rain_drop_id);

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
  if (we->hasAttr(ATTR_VISIBILITY)) {
    m_visibility = we->valueOfAttr(ATTR_VISIBILITY).asNum();
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
    else if (attr_name == ATTR_VISIBILITY) {
      m_visibility = we->valueOfAttr(ATTR_VISIBILITY).asNum();
    }
    ++I;
  }
}
   
void Weather::render() {
  if (!m_draw_rain || m_rain==0.0f) return;
  // Render Rain effects.
  // TODO, check possible values for m_rain.
  // Make buf permanent.

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  const int max_points = 1000;

  // Use this value as a weighting for the number of rain drops to show
  int area = RenderSystem::getInstance().getWindowWidth() * RenderSystem::getInstance().getWindowHeight() / 1000;

  int num_points = (int)((float)area * m_rain); // This should also be proportional to screen area, otherwise small windows will have lots of rain, larger ones will have less.

  if (num_points > max_points) num_points = max_points;

  float w = RenderSystem::getInstance().getWindowWidth();
  float h = RenderSystem::getInstance().getWindowHeight();

  static float buf[2 * max_points];
  int cnt = -1;
  // Random positions are fine according to some studies.
  // The human eye is unable to track rain drops/
  for (int i = 0; i <  num_points; ++i) {
    buf[++cnt] =  (float)rand() / (float)RAND_MAX * w;
    buf[++cnt] =  (float)rand() / (float)RAND_MAX * h;
  }
  float maxSize = 1.0f;
  bool texEnabled = glIsEnabled(GL_TEXTURE_2D);
  GLint pSize = 1;
  glGetIntegerv(GL_POINT_SIZE,  &pSize);
  if (sage_ext[GL_ARB_POINT_SPRITE]) {
    glGetFloatv(GL_POINT_SIZE_MAX_ARB, &maxSize);
    if (maxSize > 20.0f) maxSize = 20.0f;
    glEnable( GL_POINT_SPRITE_ARB );
    // Tell OpenGL to  generate its own texture coords for the point sprite
    glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
//    glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 1.0f );
//    glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
//    glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, maxSize );

    RenderSystem::getInstance().switchTexture(m_rain_drop_id);
  } else {
    if (texEnabled) glDisable(GL_TEXTURE_2D);
  }
  glPointSize(maxSize);
 
  glVertexPointer(2, GL_FLOAT,0,buf);
  glDrawArrays(GL_POINTS,0,num_points);

  if (sage_ext[GL_ARB_POINT_SPRITE]) {
    glDisable( GL_POINT_SPRITE_ARB );
  } else {
    if (texEnabled) glEnable(GL_TEXTURE_2D);
  }
  glPointSize(pSize);
}

void Weather::registerCommands(Console *con) {
  con->registerCommand(CMD_SET_RAIN, this);
//  con->registerCommand(CMD_SET_SNOW, this);
  con->registerCommand(CMD_SET_VISIBILITY, this);
}

void Weather::runCommand(const std::string &cmd, const std::string &args) {
  if (cmd == CMD_SET_RAIN) {
    float f;
    cast_stream(args, f);
    if (f < 0.0f) f = 0.0f;
    if (f > 1.0f) f = 1.0f;
    m_rain = f;
  }
  else
  if (cmd == CMD_SET_SNOW) {
    float f;
    cast_stream(args, f);
    if (f < 0.0f) f = 0.0f;
    if (f > 1.0f) f = 1.0f;
    m_snow = f;
  }
  else
  if (cmd == CMD_SET_VISIBILITY) {
    float f;
    cast_stream(args, f);
    m_visibility = f;
  }
}

void Weather::update(float time_elapsed) {
  // Adjust fog change speed based on how far it has to catch up
  float speed = fabs(m_visibility - m_current_visibility);
  time_elapsed *= speed; 
  // Clamp maximum speed
  if (time_elapsed > 3.0f) time_elapsed = 3.0f;

  if (m_current_visibility > m_visibility) {
    m_current_visibility -= time_elapsed;
    if (m_current_visibility < m_visibility)
       m_current_visibility = m_visibility;
  }
  else if (m_current_visibility < m_visibility) {
    m_current_visibility += time_elapsed;
    if (m_current_visibility > m_visibility)
       m_current_visibility = m_visibility;
  }
}

void Weather::readConfig(const varconf::Config &config) {
  m_draw_rain = readBoolValue(config, SECTION_weather, KEY_draw_rain, DEFAULT_draw_rain);
}

void Weather::writeConfig(varconf::Config &config) const {
}


} // namespace Sear
