// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: RenderSystem.cpp,v 1.19 2006-12-03 13:38:48 simon Exp $

#include <SDL/SDL.h>

#include <sigc++/object_slot.h>

#include "src/Console.h"
#include "src/System.h"
#include "src/WorldEntity.h"

#include "RenderSystem.h"
#include "TextureManager.h"
#include "StateManager.h"
#include "Graphics.h"
#include "Camera.h"
#include "CameraSystem.h"

#include "Render.h"
#include "GL.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

RenderSystem RenderSystem::m_instance;

static const std::string CMD_TOGGLE_FULLSCREEN = "toggle_fullscreen";

RenderSystem::RenderSystem() :
  m_initialised(false),
  m_mouseCurState(0),
  m_mouseVisible(true)
{ }

RenderSystem::~RenderSystem() {}

void RenderSystem::init() {
  assert (m_initialised == false);
  
  if (debug) std::cout << "RenderSystem: Initialise" << std::endl;

  m_stateManager = SPtrShutdown<StateManager>(new StateManager());
  m_stateManager->init();

  m_textureManager = SPtrShutdown<TextureManager>(new TextureManager());
  m_textureManager->init();

  m_renderer = SPtrShutdown<Render>(new GL());
  m_renderer->init();

  m_graphics = SPtrShutdown<Graphics>(new Graphics(System::instance()));
  m_graphics->init();
  m_graphics->setRenderer(m_renderer.get());

  m_cameraSystem = SPtrShutdown<CameraSystem>(new CameraSystem());
  m_cameraSystem->init();
  // Create default cameras
  // Chase camera
  Camera *cam = new Camera();
  cam->init();
  cam->setType(Camera::CAMERA_CHASE);
  int pos = m_cameraSystem->addCamera(cam);
  // First person camera
  cam = new Camera();
  cam->init();
  cam->setType(Camera::CAMERA_FIRST);
  m_cameraSystem->addCamera(cam);
  // Set default to chase cam
  m_cameraSystem->setCurrentCamera(pos);

  // Get ID's for cursor textures
  m_mouseState[CURSOR_DEFAULT] = m_textureManager->requestTextureID("cursor_default",false);
  m_mouseState[CURSOR_TOUCH] = m_textureManager->requestTextureID("cursor_touch", false);
  m_mouseState[CURSOR_PICKUP] = m_textureManager->requestTextureID("cursor_pickup", false);
  m_mouseState[CURSOR_USE] = m_textureManager->requestTextureID("cursor_use", false);
  m_mouseState[CURSOR_ATTACK] = m_textureManager->requestTextureID("cursor_attack", false);

  ContextCreated.connect(SigC::slot(*this, &RenderSystem::contextCreated));
  ContextDestroyed.connect(SigC::slot(*this, &RenderSystem::contextDestroyed));

  m_initialised = true;
}

void RenderSystem::registerCommands(Console *con) {
  assert (m_initialised);
  assert (con != NULL);

  con->registerCommand(CMD_TOGGLE_FULLSCREEN, this);

  dynamic_cast<GL*>(m_renderer.get())->registerCommands(con);
  m_textureManager->registerCommands(con);
  m_stateManager->registerCommands(con);
  m_graphics->registerCommands(con);
  m_cameraSystem->registerCommands(con);
}

void RenderSystem::runCommand(const std::string &command, const std::string &args) {
  assert(m_initialised);
  if (command == CMD_TOGGLE_FULLSCREEN) toggleFullscreen();
}

void RenderSystem::shutdown() {
  assert (m_initialised);
  
  if (debug) std::cout << "RenderSystem: Shutdown" << std::endl;

  m_cameraSystem.release();
  m_graphics.release();
  m_textureManager.release();
  m_stateManager.release();
  m_renderer.release();

  m_initialised = false;
}

TextureID RenderSystem::requestTexture(const std::string &textureName, bool mask) {
  assert (m_initialised);
  return m_textureManager->requestTextureID(textureName, mask);
}

void RenderSystem::switchTexture(TextureID to) {
  assert (m_initialised);
  m_textureManager->switchTexture(to);
}

void RenderSystem::switchTexture(unsigned int texUnit, TextureID to) {
  assert (m_initialised);
  m_textureManager->switchTexture(texUnit, to);
}

StateID RenderSystem::requestState(const std::string &state) {
  assert (m_stateManager.isValid());
  return m_stateManager->requestState(state);
}

void RenderSystem::switchState(StateID state) {
  assert (m_initialised);
  m_stateManager->stateChange(state);
}

void RenderSystem::forceState(StateID state) {
  assert (m_initialised);
  m_stateManager->forceStateChange(state);
}

void RenderSystem::contextCreated() {
  assert (m_initialised == true);

  m_textureManager->contextCreated();
  m_stateManager->contextCreated();
}

void RenderSystem::contextDestroyed(bool check) {
  assert (m_initialised == true);

  m_textureManager->contextDestroyed(check);
  m_stateManager->contextDestroyed(check);
}

StateID RenderSystem::getCurrentState() {
  assert (m_stateManager.isValid());
  return m_stateManager->getCurrentState();
}


bool RenderSystem::createWindow(unsigned int width, unsigned int height, bool fullscreen) {
  assert (m_initialised);
  return dynamic_cast<GL*>(m_renderer.get())->createWindow(width, height, fullscreen);
}
void RenderSystem::destroyWindow() {
  assert (m_initialised);
  dynamic_cast<GL*>(m_renderer.get())->destroyWindow();
}

void RenderSystem::toggleFullscreen() {
  assert (m_initialised);
  dynamic_cast<GL*>(m_renderer.get())->toggleFullscreen();
}

void RenderSystem::drawScene(bool select_mode, float time_elapsed) {
  m_graphics->drawScene(select_mode, time_elapsed);
}

void RenderSystem::readConfig(varconf::Config &config) {
  assert (m_initialised);
  m_renderer->readConfig(config);
//  m_textureManager->readConfig();
//  m_stateManager->readConfig();
  m_graphics->readConfig(config);
  m_cameraSystem->readConfig(config);
} 

void RenderSystem::writeConfig(varconf::Config &config) {
  assert (m_initialised);
  m_renderer->writeConfig(config);
  m_graphics->writeConfig(config);
  m_cameraSystem->writeConfig(config);
} 

void RenderSystem::resize(int width, int height) {
  assert(m_initialised);
  dynamic_cast<GL*>(m_renderer.get())->resize(width, height);
}

void RenderSystem::processMouseClick(int x, int y) {
  assert(m_initialised == true);
  m_renderer->procEvent(x, y);
}

bool RenderSystem::getWorldCoords(int x, int y, float &wx, float &wy, float &wz) {
  assert(m_initialised);
  return dynamic_cast<GL*>(m_renderer.get())->getWorldCoords(x, y, wx, wy, wz);
}

WorldEntity *RenderSystem::getActiveEntity() const {
  assert(m_initialised);
  return m_renderer->getActiveEntity();
}

std::string RenderSystem::getActiveEntityID() const {
  assert(m_initialised);
  return m_renderer->getActiveID();
}

} // namespace Sear
