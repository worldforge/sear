// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton 

#include <string>

#include "common/Config.h"
#include "common/Utility.h"
#include "common/Log.h"

#include "Camera.h"
#include "System.h"
#include "conf.h"
#include "Console.h"

namespace Sear {

Camera::Camera() :
  _distance(0.0f),
  _rotation(0.0f),
  _elevation(0.0f),
  _zoom_dir(0),
  _rotation_dir(0),
  _elevation_dir(0),
  _zoom_speed(0.0f),
  _rotation_speed(0.0f),
  _elevation_speed(0.0f)
{ }

Camera::~Camera() {
	
}

bool Camera::init() {
  readConfig();
  // Write config now so defaults will get stored
  writeConfig();
  return true;
}

void Camera::shutdown() {
  Log::writeLog("Shutting down camera.", Log::LOG_DEFAULT);
}

void Camera::updateCameraPos(float time_elapsed) {
  _distance += _zoom_speed * _zoom_dir * time_elapsed;
  // Don't let us move the camera past the focus point
  if (_distance < _min_distance) _distance = _min_distance; 
  if (_distance > _max_distance) _distance = _max_distance; 
  _rotation  += deg_to_rad(_rotation_speed * (float)_rotation_dir * time_elapsed);
  _elevation += deg_to_rad(_elevation_speed * (float)_elevation_dir * time_elapsed);
}

void Camera::zoom  (int dir) { _zoom_dir += dir; }

void Camera::rotate (int dir) { _rotation_dir += dir; }

void Camera::elevate  (int dir) { _elevation_dir += dir; }

void Camera::readConfig() {
  std::string temp;
  Config *general = System::instance()->getGeneral();
  if (!general) {
    Log::writeLog("Camera: Error - General config object not created!", Log::LOG_ERROR);
    return;
  }
  
  temp = general->getAttribute(KEY_camera_distance);
  _distance = (temp.empty()) ? (DEFAULT_camera_distance) : atof(temp.c_str());
  temp = general->getAttribute(KEY_camera_rotation);
  _rotation = (temp.empty()) ? (DEFAULT_camera_rotation) : atof(temp.c_str());
  temp = general->getAttribute(KEY_camera_elevation);
  _elevation = (temp.empty()) ? (DEFAULT_camera_elevation) : atof(temp.c_str());
  
  temp = general->getAttribute(KEY_camera_zoom_speed);
  _zoom_speed = (temp.empty()) ? (DEFAULT_camera_zoom_speed) : atof(temp.c_str());
  temp = general->getAttribute(KEY_camera_rotation_speed);
  _rotation_speed = (temp.empty()) ? (DEFAULT_camera_rotation_speed) : atof(temp.c_str());
  temp = general->getAttribute(KEY_camera_elevation_speed);
  _elevation_speed = (temp.empty()) ? (DEFAULT_camera_elevation_speed) : atof(temp.c_str());
  
  temp = general->getAttribute(KEY_camera_min_distance);
  _min_distance = (temp.empty()) ? (DEFAULT_camera_min_distance) : atof(temp.c_str());
  temp = general->getAttribute(KEY_camera_max_distance);
  _max_distance = (temp.empty()) ? (DEFAULT_camera_max_distance) : atof(temp.c_str());
}

void Camera::writeConfig() {
  Config *general = System::instance()->getGeneral();
  if (!general) {
    Log::writeLog("Camera: Error - General config object not created!", Log::LOG_ERROR);
    return;
  }

  general->setAttribute(KEY_camera_distance, string_fmt(_distance));
  general->setAttribute(KEY_camera_rotation, string_fmt(_rotation));
  general->setAttribute(KEY_camera_elevation, string_fmt(_elevation));

  general->setAttribute(KEY_camera_zoom_speed, string_fmt(_zoom_speed));
  general->setAttribute(KEY_camera_rotation_speed, string_fmt(_rotation_speed));
  general->setAttribute(KEY_camera_elevation_speed, string_fmt(_elevation_speed));
  
  general->setAttribute(KEY_camera_min_distance, string_fmt(_min_distance));
  general->setAttribute(KEY_camera_max_distance, string_fmt(_max_distance));
}

void Camera::registerCommands(Console *console) {
  console->registerCommand(ZOOM_IN, this);
  console->registerCommand(ZOOM_OUT, this);
  console->registerCommand(ZOOM_STOP_IN, this);
  console->registerCommand(ZOOM_STOP_OUT, this);

  console->registerCommand(ROTATE_LEFT, this);
  console->registerCommand(ROTATE_RIGHT, this);
  console->registerCommand(ROTATE_STOP_LEFT, this);
  console->registerCommand(ROTATE_STOP_RIGHT, this);

  console->registerCommand(ELEVATE_UP, this);
  console->registerCommand(ELEVATE_DOWN, this);
  console->registerCommand(ELEVATE_STOP_UP, this);
  console->registerCommand(ELEVATE_STOP_DOWN, this);
}

void Camera::runCommand(const std::string &command, const std::string &args) {
  if (command == ZOOM_IN) zoom(-1);
  else if (command == ZOOM_OUT) zoom(1);
  else if (command == ZOOM_STOP_IN) zoom(1);
  else if (command == ZOOM_STOP_OUT) zoom(-1);

  else if (command == ROTATE_LEFT) rotate(-1);
  else if (command == ROTATE_RIGHT) rotate(1);
  else if (command == ROTATE_STOP_LEFT) rotate(1);
  else if (command == ROTATE_STOP_RIGHT) rotate(-1);

  else if (command == ELEVATE_UP) elevate(1);
  else if (command == ELEVATE_DOWN) elevate(-1);
  else if (command == ELEVATE_STOP_UP) elevate(-1);
  else if (command == ELEVATE_STOP_DOWN) elevate(1);
}

}
