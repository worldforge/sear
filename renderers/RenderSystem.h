// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H 1

#include <string>

namespace Sear {

typedef int TextureID;
typedef int StateID;

class TextureManager;
class StateManager;
class Console;

class RenderSystem {
public:
  static RenderSystem &getInstance() { return m_instance; }

  RenderSystem() :
    m_initialised(false),
    m_stateManager(NULL),
    m_textureManager(NULL)
  { }
  virtual ~RenderSystem() {}

  void init();
  void shutdown();

  TextureID requestTexture(const std::string &textureName, bool mask = false);
  void switchTexture(TextureID to);
  void switchTexture(unsigned int texUnit, TextureID to);

  StateID requestState(const std::string &state);
  void switchState(StateID state);

  StateID getCurrentState();

  void invalidate();

  void registerCommands(Console *con);

private:
  static RenderSystem m_instance;

  bool m_initialised;

  StateManager *m_stateManager;
  TextureManager *m_textureManager;
};

} // namespace Sear

#endif
