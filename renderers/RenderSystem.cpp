// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

#ifdef HAVE_CONFIG
  #include "config.h"
#endif

#include <SDL/SDL.h>

#include "RenderSystem.h"
#include "TextureManager.h"
#include "StateManager.h"

#include "src/Render.h"
#include "renderers/GL.h"

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
  if (m_initialised) shutdown();
  if (debug) std::cout << "RenderSystem: Initialise" << std::endl;

  m_renderer = new GL();
  m_renderer->init();

  m_stateManager = new StateManager();
  m_stateManager->init();

  m_textureManager = new TextureManager();
  m_textureManager->init();

  m_initialised = true;
}

void RenderSystem::registerCommands(Console *console) {
  assert (m_initialised);
  dynamic_cast<GL*>(m_renderer)->registerCommands(console);
  m_textureManager->registerCommands(console);
  m_stateManager->registerCommands(console);
}

void RenderSystem::initContext() {
  assert (m_initialised);
  m_textureManager->initGL();
}

void RenderSystem::shutdown() {
  assert (m_initialised);
  if (!m_initialised) return;
  if (debug) std::cout << "RenderSystem: Shutdown" << std::endl;

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
  assert (m_initialised);
  m_textureManager->invalidate();
  m_stateManager->invalidate();
}

StateID RenderSystem::getCurrentState() {
  assert (m_initialised);
  return m_stateManager->getCurrentState();
}


void RenderSystem::createWindow(unsigned int width, unsigned int height, bool fullscreen) {
  assert (m_initialised);
  dynamic_cast<GL*>(m_renderer)->createWindow(width, height, fullscreen);
}
void RenderSystem::destroyWindow() {
  assert (m_initialised);
  dynamic_cast<GL*>(m_renderer)->destroyWindow();
}

void RenderSystem::toggleFullscreen() {
  assert (m_initialised);
  dynamic_cast<GL*>(m_renderer)->toggleFullscreen();
}

void RenderSystem::readConfig() {
  assert (m_initialised);
  m_renderer->readConfig();
//  m_textureManager->readConfig();
//  m_stateManager->readConfig();
} 

void RenderSystem::writeConfig() {
  assert (m_initialised);
  m_renderer->writeConfig();
//  m_textureManager->writeConfig();
//  m_stateManager->writeConfig();
} 

} // namespace Sear
