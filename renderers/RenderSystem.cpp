// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

#include <SDL/SDL.h>

#include "RenderSystem.h"
#include "TextureManager.h"
#include "StateManager.h"
#include "src/Render.h"
#include "renderers/GL.h"

namespace Sear {

RenderSystem RenderSystem::m_instance;

void RenderSystem::init() {
  if (m_initialised) shutdown();

  m_renderer = new GL();
  m_renderer->init();

  m_stateManager = new StateManager();
  m_stateManager->init();

  m_textureManager = new TextureManager();
  m_textureManager->init();

  m_initialised = true;
}

void RenderSystem::registerCommands(Console *console) {
  dynamic_cast<GL*>(m_renderer)->registerCommands(console);
  m_textureManager->registerCommands(console);
  m_stateManager->registerCommands(console);
}

void RenderSystem::initContext() {
  m_textureManager->initGL();
}

void RenderSystem::shutdown() {
  if (!m_initialised) return;

  m_textureManager->shutdown();
  delete m_textureManager;
  m_textureManager = NULL;

  m_stateManager->shutdown();
  delete m_stateManager;
  m_stateManager = NULL;

  m_initialised = false;
}

TextureID RenderSystem::requestTexture(const std::string &textureName, bool mask) {
  return m_textureManager->requestTextureID(textureName, mask);
}

void RenderSystem::switchTexture(TextureID to) {
  m_textureManager->switchTexture(to);
}

void RenderSystem::switchTexture(unsigned int texUnit, TextureID to) {
  m_textureManager->switchTexture(texUnit, to);
}

StateID RenderSystem::requestState(const std::string &state) {
  return m_stateManager->getState(state);
}

void RenderSystem::switchState(StateID state) {
  m_stateManager->stateChange(state);
}

void RenderSystem::invalidate() {
  m_textureManager->invalidate();
  m_stateManager->invalidate();
}

StateID RenderSystem::getCurrentState() {
  return m_stateManager->getCurrentState();
}


void RenderSystem::createWindow(unsigned int width, unsigned int height, bool fullscreen) {
  dynamic_cast<GL*>(m_renderer)->createWindow(width, height, fullscreen);
}
void RenderSystem::destroyWindow() {
  dynamic_cast<GL*>(m_renderer)->destroyWindow();
}

void RenderSystem::toggleFullscreen() {
  dynamic_cast<GL*>(m_renderer)->toggleFullscreen();
}

void RenderSystem::readConfig() {
  m_renderer->readConfig();
//  m_textureManager->readConfig();
//  m_stateManager->readConfig();
} 

void RenderSystem::writeConfig() {
  m_renderer->writeConfig();
//  m_textureManager->writeConfig();
//  m_stateManager->writeConfig();
} 

} // namespace Sear
