// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton 

// $Id: Camera.cpp,v 1.1 2005-01-06 12:46:54 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif
#include <string>

#include <varconf/Config.h>

#include "common/Utility.h"
#include "common/Log.h"

#include "Camera.h"
#include "src/System.h"
#include "src/conf.h"
#include "src/Console.h"


#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {
	
static const std::string CAMERA = "camera";
  // General key values
  const static std::string KEY_camera_distance = "camera_distance";
  const static std::string KEY_camera_rotation = "camera_rotation";
  const static std::string KEY_camera_elevation = "camera_elevation";

  const static std::string KEY_camera_zoom_speed = "camera_zoom_speed";
  const static std::string KEY_camera_rotation_speed = "camera_rotation_speed";
  const static std::string KEY_camera_elevation_speed = "camera_elevation_speed";
  
  const static std::string KEY_camera_min_distance = "camera_min_distance";
  const static std::string KEY_camera_max_distance = "camera_max_distance";

  const static std::string KEY_save_camera_position = "save_camera_position";
  
  // Default config values
  const static float DEFAULT_camera_distance = 5.0f;
  const static float DEFAULT_camera_rotation = 0.0f;
  const static float DEFAULT_camera_elevation = -0.1f;
  
  const static float DEFAULT_camera_zoom_speed = 20.0f;
  const static float DEFAULT_camera_rotation_speed = 40.0f;
  const static float DEFAULT_camera_elevation_speed = 40.0f;
  
  const static float DEFAULT_camera_min_distance = 5.0f;
  const static float DEFAULT_camera_max_distance = 25.0f;

  const static bool DEFAULT_save_camera_position = false;
  
  static const std::string ZOOM_IN = "+camera_zoom_in";
  static const std::string ZOOM_OUT = "+camera_zoom_out";
  static const std::string ZOOM_STOP_IN = "-camera_zoom_in";
  static const std::string ZOOM_STOP_OUT = "-camera_zoom_out";
  static const std::string ROTATE_LEFT = "+camera_rotate_left";
  static const std::string ROTATE_RIGHT = "+camera_rotate_right";
  static const std::string ROTATE_STOP_LEFT = "-camera_rotate_left";
  static const std::string ROTATE_STOP_RIGHT = "-camera_rotate_right";
  static const std::string ELEVATE_UP = "+camera_elevate_up";
  static const std::string ELEVATE_DOWN = "+camera_elevate_down";
  static const std::string ELEVATE_STOP_UP = "-camera_elevate_up";
  static const std::string ELEVATE_STOP_DOWN = "-camera_elevate_down";
 
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
  // Read camera config from file
  readConfig();
  // Store initial euclidean camera values
  float dist_sqr = _distance * _distance;
  _x_pos = dist_sqr * cos(_elevation) * cos(_rotation);
  _y_pos = dist_sqr * cos(_elevation) * sin(_rotation);
  _z_pos = _distance * sin(_elevation);
  // Connect callback to check for updates
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
  assert ((_initialised == true) && "Camera not initialised");	
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

void Camera::rotateImmediate(float rot) {
  _rotation  += deg_to_rad(rot);
  float dist_sqr = _distance * _distance;
  _x_pos = dist_sqr * cos(_elevation) * cos(_rotation);
  _y_pos = dist_sqr * cos(_elevation) * sin(_rotation);
  _z_pos = _distance * sin(_elevation);
}

void Camera::elevateImmediate(float elev) {
  _elevation += deg_to_rad(elev);
  float dist_sqr = _distance * _distance;
  _x_pos = dist_sqr * cos(_elevation) * cos(_rotation);
  _y_pos = dist_sqr * cos(_elevation) * sin(_rotation);
  _z_pos = _distance * sin(_elevation);
}

void Camera::readConfig() {
  varconf::Variable temp;
  varconf::Config &general = System::instance()->getGeneral();
  
  if (general.findItem(CAMERA, KEY_camera_distance)) {
    temp = general.getItem(CAMERA, KEY_camera_distance);
    _distance = (!temp.is_double()) ? (DEFAULT_camera_distance) : ((double)(temp));
  } else {
    _distance = DEFAULT_camera_distance;
  }
  if (general.findItem(CAMERA, KEY_camera_rotation)) {
    temp = general.getItem(CAMERA, KEY_camera_rotation);
    _rotation = (!temp.is_double()) ? (DEFAULT_camera_rotation) : ((double)(temp));
  } else {
    _rotation = DEFAULT_camera_rotation;
  }
  if (general.findItem(CAMERA, KEY_camera_elevation)) {
    temp = general.getItem(CAMERA, KEY_camera_elevation);
    _elevation = (!temp.is_double()) ? (DEFAULT_camera_elevation) : ((double)(temp));
  } else {
    _elevation = DEFAULT_camera_elevation;
  }
  
  if (general.findItem(CAMERA, KEY_camera_zoom_speed)) {
  temp = general.getItem(CAMERA, KEY_camera_zoom_speed);
  _zoom_speed = (!temp.is_double()) ? (DEFAULT_camera_zoom_speed) : ((double)(temp));
  } else {
  _zoom_speed = DEFAULT_camera_zoom_speed;
  }
  if (general.findItem(CAMERA, KEY_camera_rotation_speed)) {
    temp = general.getItem(CAMERA, KEY_camera_rotation_speed);
    _rotation_speed = (!temp.is_double()) ? (DEFAULT_camera_rotation_speed) : ((double)(temp));
  } else {
    _rotation_speed = DEFAULT_camera_rotation_speed;
  }
  if (general.findItem(CAMERA, KEY_camera_elevation_speed)) {
    temp = general.getItem(CAMERA, KEY_camera_elevation_speed);
    _elevation_speed = (!temp.is_double()) ? (DEFAULT_camera_elevation_speed) : ((double)(temp));
  } else {
    _elevation_speed = DEFAULT_camera_elevation_speed;
  }
  
  if (general.findItem(CAMERA, KEY_camera_min_distance)) {
    temp = general.getItem(CAMERA, KEY_camera_min_distance);
    _min_distance = (!temp.is_double()) ? (DEFAULT_camera_min_distance) : ((double)(temp));
  } else {
    _min_distance = DEFAULT_camera_min_distance;
  }
  if (general.findItem(CAMERA, KEY_camera_max_distance)) {
    temp = general.getItem(CAMERA, KEY_camera_max_distance);
    _max_distance = (!temp.is_double()) ? (DEFAULT_camera_max_distance) : ((double)(temp));
  } else {
    _max_distance = DEFAULT_camera_max_distance;
  }
  if (general.findItem(CAMERA, KEY_save_camera_position)) {
    temp = general.getItem(CAMERA, KEY_save_camera_position);
    _save_camera_position = (!temp.is_bool()) ? (DEFAULT_save_camera_position) : ((bool)(temp));
  } else {
    _save_camera_position = DEFAULT_save_camera_position;
  }
}

void Camera::writeConfig() {
  assert ((_initialised == true) && "Camera not initialised");	
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
  assert ((_initialised == true) && "Camera not initialised");
  assert ((console != NULL) && "console is NULL");
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
  assert ((_initialised == true) && "Camera not initialised");	
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
  assert ((_initialised == true) && "Camera not initialised");	
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
