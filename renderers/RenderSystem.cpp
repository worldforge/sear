// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

#include "RenderSystem.h"
#include "TextureManager.h"
#include "StateManager.h"

#include "src/System.h"

namespace Sear {

RenderSystem RenderSystem::m_instance;

void RenderSystem::init() {
  if (m_initialised) shutdown();

  m_stateManager = new StateManager();
  m_stateManager->init();

  m_textureManager = new TextureManager();
  m_textureManager->init();

  m_textureManager->registerCommands(System::instance()->getConsole());
  m_stateManager->registerCommands(System::instance()->getConsole());
  m_initialised = true;
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

void RenderSystem::registerCommands(Console *con) {
  m_textureManager->registerCommands(con);
  m_stateManager->registerCommands(con);
}

} // namespace Sear
