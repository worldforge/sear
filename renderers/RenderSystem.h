// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H 1

#include <string>

namespace varconf {
  class Config;
}

namespace Sear {

typedef int TextureID;
typedef int StateID;

class TextureManager;
class StateManager;
class Console;
class Render;
class Graphics;
class CameraSystem;

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

  typedef enum {
    CURSOR_DEFAULT = 0,
    CURSOR_TOUCH,
    CURSOR_PICKUP,
    CURSOR_USE,
    CURSOR_LAST_STATE
  } CursorState;

  static RenderSystem &getInstance() { return m_instance; }

  RenderSystem() :
    m_initialised(false),
    m_stateManager(NULL),
    m_textureManager(NULL),
    m_renderer(NULL),
    m_graphics(NULL),
    m_mouseCurState(0),
    m_mouseVisible(true)
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
  Graphics *getGraphics() const { return m_graphics; }
  CameraSystem *getCameraSystem() const { return m_cameraSystem; }
 
  // Renderer Functions
  bool createWindow(unsigned int width, unsigned int height, bool fullscreen);
  void destroyWindow();
  void toggleFullscreen();

  void drawScene(bool select_mode, float time_elapsed);

  void registerCommands(Console *console);
  void runCommand(const std::string &command) {}

  void readConfig(varconf::Config &config);
  void writeConfig(varconf::Config &config);

  void setState(RenderState state, bool value) {
    m_renderState[state] = value;
  }
  bool getState(RenderState state) const {
    return m_renderState[state];
  }

  void resize(int width, int height);
  int getMouseState() const { return m_mouseCurState; } 
  void setMouseState(int state) { m_mouseCurState = state; }
  int getMouseCursor() const { return m_mouseState[m_mouseCurState]; }
  bool isMouseVisible() const { return m_mouseVisible; }
  void setMouseVisible(bool v) { m_mouseVisible= v; }

private:
  static RenderSystem m_instance;

  bool m_initialised;

  StateManager *m_stateManager;
  TextureManager *m_textureManager;
  Render *m_renderer;
  Graphics *m_graphics;
  CameraSystem *m_cameraSystem;

  bool  m_renderState[RENDER_LAST_STATE];

  int m_mouseState[CURSOR_LAST_STATE];
  int m_mouseCurState;
  bool m_mouseVisible;
};

} // namespace Sear

#endif
