// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall, University of Southampton

#ifndef SEAR_RENDERSYSTEM_H
#define SEAR_RENDERSYSTEM_H 1

#include <string>

#include <sigc++/signal.h>
#include <sigc++/trackable.h>

#include "interfaces/ConsoleObject.h"

#include "RenderTypes.h"

namespace varconf {
  class Config;
}

namespace Sear {

class TextureManager;
class StateManager;
class Console;
class Render;
class Graphics;
class CameraSystem;
class WorldEntity;

class RenderSystem : public sigc::trackable, public ConsoleObject {
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
    CURSOR_ATTACK,
    CURSOR_LAST_STATE
  } CursorState;

  static RenderSystem &getInstance() { return m_instance; }

  RenderSystem();
  ~RenderSystem();

  void init();
  void shutdown();
  bool isInitialised() const { return m_initialised; }

  // Texture Manager Functions
  TextureID requestTexture(const std::string &textureName, bool mask = false);
  void releaseTexture(TextureID id);
  void switchTexture(TextureID to);
  void switchTexture(unsigned int texUnit, TextureID to);

  // State Manager functions
  StateID requestState(const std::string &state);
  void switchState(StateID state);
  void forceState(StateID state);
  StateID getCurrentState();

  void contextCreated();
  void contextDestroyed(bool check);

  TextureManager *getTextureManager() { return m_textureManager.get(); }
//  StateManager *getStateManager() { return m_stateManager.get(); }
  Render *getRenderer() { return m_renderer.get(); }
  Graphics *getGraphics() { return m_graphics.get(); }
  CameraSystem *getCameraSystem() { return m_cameraSystem.get(); }
 
  // Renderer Functions
  bool createWindow(unsigned int width, unsigned int height, bool fullscreen);
  void destroyWindow();
  void toggleFullscreen();

  void drawScene(bool select_mode, float time_elapsed);

  void registerCommands(Console *con);
  void runCommand(const std::string &command, const std::string &args);

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

  WorldEntity *getActiveEntity() const;
  std::string getActiveEntityID() const;

  void processMouseClick(int x, int y);
  bool getWorldCoords(int x, int y, float &wx, float &wy, float &wz);

  sigc::signal<void> ContextCreated;
  sigc::signal<void, bool> ContextDestroyed;

  int getWindowWidth() const;
  int getWindowHeight() const;

private:
  static RenderSystem m_instance;

  bool m_initialised;

  std::auto_ptr<StateManager> m_stateManager;
  std::auto_ptr<TextureManager> m_textureManager;
  std::auto_ptr<Render> m_renderer;
  std::auto_ptr<Graphics> m_graphics;
  std::auto_ptr<CameraSystem> m_cameraSystem;

  bool  m_renderState[RENDER_LAST_STATE];

  int m_mouseState[CURSOR_LAST_STATE];
  int m_mouseCurState;
  bool m_mouseVisible;
};

} // namespace Sear

#endif /* SEAR_RENDER_SYSTEM_H */
