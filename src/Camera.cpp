// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton 

// $Id: Camera.cpp,v 1.9 2002-10-20 15:50:27 simon Exp $

#include <string>

#include <varconf/Config.h>

#include "common/Utility.h"
#include "common/Log.h"

#include "Camera.h"
#include "System.h"
#include "conf.h"
#include "Console.h"

namespace Sear {

#ifdef DEBUG
static const bool debug = true;
#else
static const bool debug = false;
#endif
	
static std::string CAMERA = "camera";
	
Camera::Camera() :
  _distance(0.0f),
  _rotation(0.0f),
  _elevation(0.0f),
  _zoom_dir(0),
  _rotation_dir(0),
  _elevation_dir(0),
  _zoom_speed(0.0f),
  _rotation_speed(0.0f),
  _elevation_speed(0.0f),
  _initialised(false)
{ }

Camera::~Camera() {
  if (_initialised) shutdown();
}

bool Camera::init() {
  if (_initialised) shutdown();
  readConfig();
  // Write config now so defaults will get stored
  writeConfig();
  _initialised = true;
  return true;
}

void Camera::shutdown() {
  if (debug) Log::writeLog("Shutting down camera.", Log::LOG_DEFAULT);
  _initialised = false;
}

void Camera::updateCameraPos(float time_elapsed) {
  _distance += _zoom_speed * _zoom_dir * time_elapsed;
  // Don't let us move the camera past the focus point
  if (_distance < _min_distance) _distance = _min_distance; 
  if (_distance > _max_distance) _distance = _max_distance; 
  _rotation  += deg_to_rad(_rotation_speed * (float)_rotation_dir * time_elapsed);
  _elevation += deg_to_rad(_elevation_speed * (float)_elevation_dir * time_elapsed);
}

void Camera::readConfig() {
  varconf::Variable temp;
  varconf::Config *general = System::instance()->getGeneral();
  if (!general) {
    Log::writeLog("Camera: Error - General config object not created!", Log::LOG_ERROR);
    return;
  }
  
  temp = general->getItem(CAMERA, KEY_camera_distance);
  _distance = (!temp.is_double()) ? (DEFAULT_camera_distance) : ((double)(temp));
  temp = general->getItem(CAMERA, KEY_camera_rotation);
  _rotation = (!temp.is_double()) ? (DEFAULT_camera_rotation) : ((double)(temp));
  temp = general->getItem(CAMERA, KEY_camera_elevation);
  _elevation = (!temp.is_double()) ? (DEFAULT_camera_elevation) : ((double)(temp));
  
  temp = general->getItem(CAMERA, KEY_camera_zoom_speed);
  _zoom_speed = (!temp.is_double()) ? (DEFAULT_camera_zoom_speed) : ((double)(temp));
  temp = general->getItem(CAMERA, KEY_camera_rotation_speed);
  _rotation_speed = (!temp.is_double()) ? (DEFAULT_camera_rotation_speed) : ((double)(temp));
  temp = general->getItem(CAMERA, KEY_camera_elevation_speed);
  _elevation_speed = (!temp.is_double()) ? (DEFAULT_camera_elevation_speed) : ((double)(temp));
  
  temp = general->getItem(CAMERA, KEY_camera_min_distance);
  _min_distance = (!temp.is_double()) ? (DEFAULT_camera_min_distance) : ((double)(temp));
  temp = general->getItem(CAMERA, KEY_camera_max_distance);
  _max_distance = (!temp.is_double()) ? (DEFAULT_camera_max_distance) : ((double)(temp));
}

void Camera::writeConfig() {
  varconf::Config *general = System::instance()->getGeneral();
  if (!general) {
    Log::writeLog("Camera: Error - General config object not created!", Log::LOG_ERROR);
    return;
  }

  general->setItem(CAMERA, KEY_camera_distance, _distance);
  general->setItem(CAMERA, KEY_camera_rotation, _rotation);
  general->setItem(CAMERA, KEY_camera_elevation, _elevation);

  general->setItem(CAMERA, KEY_camera_zoom_speed, _zoom_speed);
  general->setItem(CAMERA, KEY_camera_rotation_speed, _rotation_speed);
  general->setItem(CAMERA, KEY_camera_elevation_speed, _elevation_speed);
  
  general->setItem(CAMERA, KEY_camera_min_distance, _min_distance);
  general->setItem(CAMERA, KEY_camera_max_distance, _max_distance);
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

} /* namespace Sear */
