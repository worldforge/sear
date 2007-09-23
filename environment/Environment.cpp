// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

#include <sigc++/object_slot.h>

#include <Atlas/Objects/Operation.h>

#include "renderers/RenderSystem.h"

#include "src/System.h"

#include "Weather.h"
#include "Environment.h"
#include "TerrainRenderer.h"
#include "SkyDome.h"
#include "Stars.h"

namespace Sear {

Environment  Environment::instance;
Environment::Environment() :
  m_initialised(false)
{}

Environment::~Environment() {
  if (m_initialised) shutdown();
}

int Environment::init() {
  assert(m_initialised == false);
  m_terrain = std::auto_ptr<TerrainRenderer>(new TerrainRenderer());
  m_skyDome = std::auto_ptr<SkyDome>(new SkyDome(1.0f, 20, 20));
  m_stars   = std::auto_ptr<Stars>(new Stars());
  m_weather = std::auto_ptr<Weather>(new Weather());
  m_weather->init();

 
  RenderSystem::getInstance().ContextCreated.connect(SigC::slot(*this, &Environment::contextCreated));
  RenderSystem::getInstance().ContextDestroyed.connect(SigC::slot(*this, &Environment::contextDestroyed));

  // Clean up terrain data when leaving game world
  System::instance()->LeftWorld.connect(SigC::slot(*this, &Environment::resetWorld));
 
  m_initialised = true;

  return 0;
}

int Environment::reinit() {


  return 0;
}

void Environment::shutdown() {
  assert(m_initialised == true);

  m_terrain.release();
  m_skyDome.release();
  m_stars.release();
  m_weather.release();

  notify_callbacks();
 
  m_initialised = false;
}

float Environment::getHeight(float x, float y) {
  assert(m_initialised == true);
  WFMath::Vector<3> n;
  float z = 0.0f;
  m_terrain->m_terrain.getHeightAndNormal(x,y,z,n);
  return z;
}

void Environment::setBasePoint(int x, int y, float z) {
  assert(m_initialised == true);
  m_terrain->m_terrain.setBasePoint(x, y, z);
}

void Environment::setSurface(const std::string &name, const std::string &pattern, const Mercator::Shader::Parameters &params) {
  assert(m_initialised == true);
  m_terrain->setSurface(name, pattern, params);
}
void Environment::renderSky() {
  assert(m_initialised == true);
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState("stars"));
  m_stars->render();
  
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState("sky_0"));
  m_skyDome->render();
}

void Environment::renderTerrain(const WFMath::Point<3> &pos, bool select_mode) {
  assert(m_initialised == true);
  m_terrain->render(pos, select_mode);
}

void Environment::renderWeather() {
  assert(m_initialised == true);
  m_weather->render();
}
void Environment::renderSea() {
  assert(m_initialised == true);
  m_terrain->renderSea();
}

void Environment::contextCreated() {
  assert(m_initialised == true);
  m_terrain->contextCreated();
  m_skyDome->contextCreated();
}

void Environment::contextDestroyed(bool check) {
  assert(m_initialised == true);
  m_terrain->contextDestroyed(check);
  m_skyDome->contextDestroyed(check);
}

void Environment::registerArea(Mercator::Area* ar)
{
    assert(ar);
    m_terrain->m_terrain.addArea(ar);
}

void Environment::registerTerrainShader(Mercator::Shader* shade, const std::string& texId)
{
  assert(m_initialised == true);
    assert(shade);
    m_terrain->registerShader(shade, texId);
}

void Environment::resetWorld() {
  assert(m_initialised == true);
  m_terrain->reset();
}

void Environment::setWeatherEntity(WorldEntity *we) {
  assert(m_initialised == true);
  m_weather->setWeatherEntity(we);
}

void Environment::registerCommands(Console *con) {
  assert(m_initialised == true);
  m_weather->registerCommands(con);
}

float Environment::getVisibility() const {
  assert(m_initialised == true);
  return m_weather->getVisibility();
}

void Environment::update(float time_elapsed) {
  assert(m_initialised == true);
  m_weather->update(time_elapsed);

}
} // namespace Sear
