// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton 

// $Id: Camera.cpp,v 1.12 2002-11-13 19:39:27 simon Exp $

#include <string>

#include <varconf/Config.h>

#include "common/Utility.h"
#include "common/Log.h"

#include "Camera.h"
#include "System.h"
#include "conf.h"
#include "Console.h"

#ifdef DEBUG
  #include "common/mmgr.h"
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {
	
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
  _initialised(false),
  _save_camera_position(false)
{ }

Camera::~Camera() {
  if (_initialised) shutdown();
}

bool Camera::init() {
  if (_initialised) shutdown();
  readConfig();
  float dist_sqr = _distance * _distance;
  _x_pos = dist_sqr * cos(_elevation) * cos(_rotation);
  _y_pos = dist_sqr * cos(_elevation) * sin(_rotation);
  _z_pos = _distance * sin(_elevation);
  System::instance()->getGeneral().sigsv.connect(SigC::slot(*this, &Camera::varconf_callback));
  _initialised = true;
  return true;
}

void Camera::shutdown() {
  if (debug) Log::writeLog("Shutting down camera.", Log::LOG_DEFAULT);
  writeConfig();
  _initialised = false;
}

void Camera::updateCameraPos(float time_elapsed) {
  bool changed = false;
  //Only perform calculations if required
  if (_zoom_speed != 0.0f) {
    _distance += _zoom_speed * _zoom_dir * time_elapsed;
    // Don't let us move the camera past the focus point
    if (_distance < _min_distance) _distance = _min_distance; 
    if (_distance > _max_distance) _distance = _max_distance; 
    changed = true;
  }
  if (_rotation_speed != 0.0f) {
    _rotation  += deg_to_rad(_rotation_speed * (float)_rotation_dir * time_elapsed);
    changed = true;
  }
  if (_elevation_speed != 0.0f) {
    _elevation += deg_to_rad(_elevation_speed * (float)_elevation_dir * time_elapsed);
    changed = true;
  }
  if (changed) {
    float dist_sqr = _distance * _distance;
    _x_pos = dist_sqr * cos(_elevation) * cos(_rotation);
    _y_pos = dist_sqr * cos(_elevation) * sin(_rotation);
    _z_pos = _distance * sin(_elevation);
  }
}

void Camera::readConfig() {
  varconf::Variable temp;
  varconf::Config &general = System::instance()->getGeneral();
  
  temp = general.getItem(CAMERA, KEY_camera_distance);
  _distance = (!temp.is_double()) ? (DEFAULT_camera_distance) : ((double)(temp));
  temp = general.getItem(CAMERA, KEY_camera_rotation);
  _rotation = (!temp.is_double()) ? (DEFAULT_camera_rotation) : ((double)(temp));
  temp = general.getItem(CAMERA, KEY_camera_elevation);
  _elevation = (!temp.is_double()) ? (DEFAULT_camera_elevation) : ((double)(temp));
  
  temp = general.getItem(CAMERA, KEY_camera_zoom_speed);
  _zoom_speed = (!temp.is_double()) ? (DEFAULT_camera_zoom_speed) : ((double)(temp));
  temp = general.getItem(CAMERA, KEY_camera_rotation_speed);
  _rotation_speed = (!temp.is_double()) ? (DEFAULT_camera_rotation_speed) : ((double)(temp));
  temp = general.getItem(CAMERA, KEY_camera_elevation_speed);
  _elevation_speed = (!temp.is_double()) ? (DEFAULT_camera_elevation_speed) : ((double)(temp));
  
  temp = general.getItem(CAMERA, KEY_camera_min_distance);
  _min_distance = (!temp.is_double()) ? (DEFAULT_camera_min_distance) : ((double)(temp));
  temp = general.getItem(CAMERA, KEY_camera_max_distance);
  _max_distance = (!temp.is_double()) ? (DEFAULT_camera_max_distance) : ((double)(temp));

  temp = general.getItem(CAMERA, KEY_save_camera_position);
  _save_camera_position = (!temp.is_bool()) ? (DEFAULT_save_camera_position) : ((bool)(temp));
}

void Camera::writeConfig() {
  varconf::Config &general = System::instance()->getGeneral();
  
  if (_save_camera_position) {
    general.setItem(CAMERA, KEY_camera_distance, _distance);
    general.setItem(CAMERA, KEY_camera_rotation, _rotation);
    general.setItem(CAMERA, KEY_camera_elevation, _elevation);
  }

  general.setItem(CAMERA, KEY_camera_zoom_speed, _zoom_speed);
  general.setItem(CAMERA, KEY_camera_rotation_speed, _rotation_speed);
  general.setItem(CAMERA, KEY_camera_elevation_speed, _elevation_speed);
  
  general.setItem(CAMERA, KEY_camera_min_distance, _min_distance);
  general.setItem(CAMERA, KEY_camera_max_distance, _max_distance);

  general.setItem(CAMERA, KEY_save_camera_position, _save_camera_position);
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

void Camera::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  varconf::Variable temp;
  if (section == CAMERA) {
    if (key == KEY_camera_distance) {
      temp = config.getItem(CAMERA, KEY_camera_distance);
      _distance = (!temp.is_double()) ? (DEFAULT_camera_distance) : ((double)(temp));
    }
    else if (key == KEY_camera_rotation) {
      temp = config.getItem(CAMERA, KEY_camera_rotation);
      _rotation = (!temp.is_double()) ? (DEFAULT_camera_rotation) : ((double)(temp));
    }
    else if (key == KEY_camera_elevation) {
      temp = config.getItem(CAMERA, KEY_camera_elevation);
      _elevation = (!temp.is_double()) ? (DEFAULT_camera_elevation) : ((double)(temp));
    }
    else if (key == KEY_camera_zoom_speed) {
      temp = config.getItem(CAMERA, KEY_camera_zoom_speed);
      _zoom_speed = (!temp.is_double()) ? (DEFAULT_camera_zoom_speed) : ((double)(temp));
    }
    else if (key == KEY_camera_rotation_speed) {
      temp = config.getItem(CAMERA, KEY_camera_rotation_speed);
      _rotation_speed = (!temp.is_double()) ? (DEFAULT_camera_rotation_speed) : ((double)(temp));
    }
    else if (key == KEY_camera_elevation_speed) {
      temp = config.getItem(CAMERA, KEY_camera_elevation_speed);
      _elevation_speed = (!temp.is_double()) ? (DEFAULT_camera_elevation_speed) : ((double)(temp));
    }
    else if (key == KEY_camera_min_distance) {
      temp = config.getItem(CAMERA, KEY_camera_min_distance);
      _min_distance = (!temp.is_double()) ? (DEFAULT_camera_min_distance) : ((double)(temp));
    }
    else if (key == KEY_camera_max_distance) {
      temp = config.getItem(CAMERA, KEY_camera_max_distance);
      _max_distance = (!temp.is_double()) ? (DEFAULT_camera_max_distance) : ((double)(temp));
    }
    else if (key == KEY_save_camera_position) {
      temp = config.getItem(CAMERA, KEY_save_camera_position);
      _save_camera_position = (!temp.is_bool()) ? (DEFAULT_save_camera_position) : ((bool)(temp));
    }
  }
}

} /* namespace Sear */
