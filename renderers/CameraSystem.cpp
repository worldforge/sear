// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Simon Goodall

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <varconf/Config.h>

#include <cassert>

#include "common/Utility.h"
#include "src/Console.h"

#include "Camera.h"
#include "CameraSystem.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

static const std::string CMD_SWITCH_CAMERA = "switch_camera";
static const std::string CMD_ZOOM_IN = "+camera_zoom_in";
static const std::string CMD_ZOOM_OUT = "+camera_zoom_out";
static const std::string CMD_ZOOM_STOP_IN = "-camera_zoom_in";
static const std::string CMD_ZOOM_STOP_OUT = "-camera_zoom_out";
static const std::string CMD_ROTATE_LEFT = "+camera_rotate_left";
static const std::string CMD_ROTATE_RIGHT = "+camera_rotate_right";
static const std::string CMD_ROTATE_STOP_LEFT = "-camera_rotate_left";
static const std::string CMD_ROTATE_STOP_RIGHT = "-camera_rotate_right";
static const std::string CMD_ELEVATE_UP = "+camera_elevate_up";
static const std::string CMD_ELEVATE_DOWN = "+camera_elevate_down";
static const std::string CMD_ELEVATE_STOP_UP = "-camera_elevate_up";
static const std::string CMD_ELEVATE_STOP_DOWN = "-camera_elevate_down";

CameraSystem::CameraSystem() :
  m_initialised(false),
  m_current(-1)
{}

CameraSystem::~CameraSystem() {
  assert(m_initialised == false);
  if (m_initialised) shutdown();
}

int CameraSystem::init() {
  assert(m_initialised == false);

  m_initialised = true;
  return 0;
}

int CameraSystem::shutdown() {
  assert(m_initialised == true);
  // Clean up cameras
  while (!m_cameras.empty())  {
    Camera *cam = *m_cameras.begin();
    cam->shutdown();
    delete cam;
    m_cameras.erase(m_cameras.begin());
  }

  m_initialised = false;
  return 0;
}
  
void CameraSystem::registerCommands(Console *console) {
  assert(console);

  console->registerCommand(CMD_SWITCH_CAMERA, this);

  console->registerCommand(CMD_ZOOM_IN, this);
  console->registerCommand(CMD_ZOOM_OUT, this);
  console->registerCommand(CMD_ZOOM_STOP_IN, this);
  console->registerCommand(CMD_ZOOM_STOP_OUT, this);

  console->registerCommand(CMD_ROTATE_LEFT, this);
  console->registerCommand(CMD_ROTATE_RIGHT, this);
  console->registerCommand(CMD_ROTATE_STOP_LEFT, this);
  console->registerCommand(CMD_ROTATE_STOP_RIGHT, this);

  console->registerCommand(CMD_ELEVATE_UP, this);
  console->registerCommand(CMD_ELEVATE_DOWN, this);
  console->registerCommand(CMD_ELEVATE_STOP_UP, this);
  console->registerCommand(CMD_ELEVATE_STOP_DOWN, this);
}

void CameraSystem::runCommand(const std::string &command, const std::string &args) {
  assert(m_initialised == true);
  if (command == CMD_SWITCH_CAMERA) {
    Tokeniser tok;
    tok.initTokens(args);
    std::string cam = tok.nextToken();
    int i;
    cast_stream(cam, i);
    setCurrentCamera(i);
  }
  else if (m_current > -1){
    assert (m_cameras[m_current]);

    if (command == CMD_ZOOM_IN) m_cameras[m_current]->zoom(-1);
    else if (command == CMD_ZOOM_OUT) m_cameras[m_current]->zoom(1);
    else if (command == CMD_ZOOM_STOP_IN) m_cameras[m_current]->zoom(1);
    else if (command == CMD_ZOOM_STOP_OUT) m_cameras[m_current]->zoom(-1);

    else if (command == CMD_ROTATE_LEFT) m_cameras[m_current]->rotate(-1);
    else if (command == CMD_ROTATE_RIGHT) m_cameras[m_current]->rotate(1);
    else if (command == CMD_ROTATE_STOP_LEFT) m_cameras[m_current]->rotate(1);
    else if (command == CMD_ROTATE_STOP_RIGHT) m_cameras[m_current]->rotate(-1);

    else if (command == CMD_ELEVATE_UP) m_cameras[m_current]->elevate(1);
    else if (command == CMD_ELEVATE_DOWN) m_cameras[m_current]->elevate(-1);
    else if (command == CMD_ELEVATE_STOP_UP) m_cameras[m_current]->elevate(-1);
    else if (command == CMD_ELEVATE_STOP_DOWN) m_cameras[m_current]->elevate(1);
  }

}

// Remove a camera from the system
void CameraSystem::removeCamera(int pos) {
  assert(m_initialised == true);
}

void CameraSystem::update(double time_elapsed) {
  assert(m_initialised == true);
  if (m_current > -1) {
    assert (m_cameras[m_current]);
    m_cameras[m_current]->updateCameraPos(time_elapsed);
  }
}

void CameraSystem::readConfig(const varconf::Config &config) {
  assert(m_initialised == true);

}

void CameraSystem::writeConfig(varconf::Config &config) {
  assert(m_initialised == true);

}

void CameraSystem::varconf_callback(const std::string &section,
                      const std::string &key,
                      varconf::Config &config) {

}

} /* namespace Sear */
