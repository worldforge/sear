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
class Render;

class RenderSystem {
public:
  typedef enum {
    RENDER_UNKNOWN = 0,
    RENDER_LIGHTING,
    RENDER_TEXTURES,
    RENDER_SHADOWS,
    RENDER_STENCIL,
    RENDER_LAST_STATE
  } RenderState;



  static RenderSystem &getInstance() { return m_instance; }

  RenderSystem() :
    m_initialised(false),
    m_stateManager(NULL),
    m_textureManager(NULL),
    m_renderer(NULL)
  { }
  virtual ~RenderSystem() {}

  void init();
  void initContext();
  void shutdown();

  // Texture Manager Functions
  TextureID requestTexture(const std::string &textureName, bool mask = false);
  void switchTexture(TextureID to);
  void switchTexture(unsigned int texUnit, TextureID to);

  // State Manager functions
  StateID requestState(const std::string &state);
  void switchState(StateID state);
  StateID getCurrentState();

  void invalidate();


  TextureManager *getTextureManager() const { return m_textureManager; }
  StateManager *getStateManager() const { return m_stateManager; }
  Render *getRenderer() const { return m_renderer; }
 
  // Renderer Functions
  bool createWindow(unsigned int width, unsigned int height, bool fullscreen);
  void destroyWindow();
  void toggleFullscreen();

  void registerCommands(Console *console);
  void runCommand(const std::string &command) {}

  void readConfig();
  void writeConfig();

  void setState(RenderState state, bool value) {
    m_renderState[state] = value;
  }
  bool getState(RenderState state) const {
    return m_renderState[state];
  }

  void resize(int width, int height);

private:
  static RenderSystem m_instance;

  bool m_initialised;

  StateManager *m_stateManager;
  TextureManager *m_textureManager;
  Render *m_renderer;
  bool  m_renderState[RENDER_LAST_STATE];
};

} // namespace Sear

#endif
