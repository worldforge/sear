// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: RenderSystem.cpp,v 1.12 2005-04-15 15:47:19 simon Exp $

#include <SDL/SDL.h>

#include "src/System.h"

#include "RenderSystem.h"
#include "TextureManager.h"
#include "StateManager.h"
#include "Graphics.h"
#include "Camera.h"
#include "CameraSystem.h"

#include "Render.h"
#include "GL.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

RenderSystem RenderSystem::m_instance;

void RenderSystem::init() {
  assert (m_initialised == false);
  
  if (debug) std::cout << "RenderSystem: Initialise" << std::endl;

  m_renderer = new GL();
  m_renderer->init();

  m_stateManager = new StateManager();
  m_stateManager->init();

  m_textureManager = new TextureManager();
  m_textureManager->init();

  m_graphics = new Graphics(System::instance());
  m_graphics->init();
  m_graphics->setRenderer(m_renderer);

  m_cameraSystem = new CameraSystem();
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

  m_initialised = true;
}

void RenderSystem::registerCommands(Console *console) {
  assert (m_initialised);
  dynamic_cast<GL*>(m_renderer)->registerCommands(console);
  m_textureManager->registerCommands(console);
  m_stateManager->registerCommands(console);
  m_graphics->registerCommands(console);
  m_cameraSystem->registerCommands(console);
}

void RenderSystem::initContext() {
  assert (m_initialised);
  m_textureManager->initGL();
}

void RenderSystem::shutdown() {
  assert (m_initialised);
  
  if (debug) std::cout << "RenderSystem: Shutdown" << std::endl;

  m_cameraSystem->shutdown();
  delete m_cameraSystem;
  m_cameraSystem = NULL;

  m_graphics->shutdown();
  delete m_graphics;
  m_graphics = NULL;

  m_textureManager->shutdown();
  delete m_textureManager;
  m_textureManager = NULL;

  m_stateManager->shutdown();
  delete m_stateManager;
  m_stateManager = NULL;

  m_renderer->shutdown();
  delete m_renderer;
  m_renderer = NULL;

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
  assert (m_initialised);
  return m_stateManager->getState(state);
}

void RenderSystem::switchState(StateID state) {
  assert (m_initialised);
  m_stateManager->stateChange(state);
}

void RenderSystem::invalidate() {
  assert (m_initialised == true);
  dynamic_cast<GL*>(m_renderer)->invalidate();
  m_textureManager->invalidate();
  m_stateManager->invalidate();
//  m_graphics->invalidate();
  
  dynamic_cast<GL*>(m_renderer)->invalidate();
}

StateID RenderSystem::getCurrentState() {
  assert (m_initialised);
  return m_stateManager->getCurrentState();
}


bool RenderSystem::createWindow(unsigned int width, unsigned int height, bool fullscreen) {
  assert (m_initialised);
  return dynamic_cast<GL*>(m_renderer)->createWindow(width, height, fullscreen);
}
void RenderSystem::destroyWindow() {
  assert (m_initialised);
  dynamic_cast<GL*>(m_renderer)->destroyWindow();
}

void RenderSystem::toggleFullscreen() {
  assert (m_initialised);
  dynamic_cast<GL*>(m_renderer)->toggleFullscreen();
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
//  m_textureManager->writeConfig();
//  m_stateManager->writeConfig();
  m_graphics->writeConfig(config);
  m_cameraSystem->writeConfig(config);
} 

void RenderSystem::resize(int width, int height) {
  assert(m_initialised);
  dynamic_cast<GL*>(m_renderer)->resize(width, height);
}
} // namespace Sear
