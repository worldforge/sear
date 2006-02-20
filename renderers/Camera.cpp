// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton 

// $Id: Camera.cpp,v 1.8 2006-02-20 20:36:23 simon Exp $

#include <string>

#include <sigc++/object_slot.h>

#include <varconf/Config.h>

#include "common/Utility.h"
#include "common/Log.h"

#include "Camera.h"
#include "src/System.h"
#include "src/Console.h"

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
  
Camera::Camera() :
  m_initialised(false),
  m_distance(0.0f),
  m_rotation(0.0f),
  m_elevation(0.0f),
  m_zoom_dir(0),
  m_rotation_dir(0),
  m_elevation_dir(0),
  m_zoom_speed(0.0f),
  m_rotation_speed(0.0f),
  m_elevation_speed(0.0f),
  m_save_camera_position(false),
  m_type(CAMERA_CHASE)
{ }

Camera::~Camera() {
  assert(m_initialised == false);
}

bool Camera::init() {
  assert(m_initialised == false);
  if (debug) std::cout << "[Camera] Init" << std::endl;

  // Store initial euclidean camera values
  updateValues();
//  m_x_pos = m_distance * cos(m_elevation) * cos(m_rotation);
//  m_y_pos = m_distance * cos(m_elevation) * sin(m_rotation);
//  m_z_pos = m_distance * sin(m_elevation);

  // Connect callback to check for updates
  System::instance()->getGeneral().sigsv.connect(SigC::slot(*this, &Camera::varconf_callback));

  m_initialised = true;

  return true;
}

void Camera::shutdown() {
  assert(m_initialised == true);
  if (debug) std::cout << "Camera: Shutdown" << std::endl;

  m_initialised = false;
}

void Camera::updateCameraPos(float time_elapsed) {
  assert ((m_initialised == true) && "Camera not initialised");	
  bool changed = false;
  //Only perform calculations if required
  if (m_zoom_dir != 0.0f) {
    m_distance += m_zoom_speed * m_zoom_dir * time_elapsed;
    // Don't let us move the camera past the focus point
    if (m_distance < m_min_distance) m_distance = m_min_distance; 
    if (m_distance > m_max_distance) m_distance = m_max_distance; 
    changed = true;
  }
  if (m_rotation_dir != 0.0f) {
    m_rotation  += deg_to_rad(m_rotation_speed * (float)m_rotation_dir * time_elapsed);
    changed = true;
  }
  if (m_elevation_dir != 0.0f) {
    m_elevation += deg_to_rad(m_elevation_speed * (float)m_elevation_dir * time_elapsed);
    changed = true;
  }
  if (changed) {
    updateValues(); 
  }
}

void Camera::updateValues() {
  m_orient.rotation(2, m_rotation);
  m_orient.rotate(WFMath::Quaternion(0, m_elevation));

  WFMath::Vector<3> v(-m_distance, 0.0f, 0.0f);
  v.rotate(m_orient);
  m_pos = WFMath::Point<3>(0,0,0);
  m_pos += v;
//  m_pos.rotate(m_orient, v);
}

void Camera::rotateImmediate(float rot) {
  m_rotation  += deg_to_rad(rot);
  updateValues();
//  m_x_pos = m_distance * cos(m_elevation) * cos(m_rotation);
//  m_y_pos = m_distance * cos(m_elevation) * sin(m_rotation);
//  m_z_pos = m_distance * sin(m_elevation);
}

void Camera::elevateImmediate(float elev) {
  m_elevation += deg_to_rad(elev);
  updateValues();
//  m_x_pos = m_distance * cos(m_elevation) * cos(m_rotation);
//  m_y_pos = m_distance * cos(m_elevation) * sin(m_rotation);
//  m_z_pos = m_distance * sin(m_elevation);
}

void Camera::readConfig(varconf::Config &config) {
  varconf::Variable temp;
  
  if (config.findItem(CAMERA, KEY_camera_distance)) {
    temp = config.getItem(CAMERA, KEY_camera_distance);
    m_distance = (!temp.is_double()) ? (DEFAULT_camera_distance) : ((double)(temp));
  } else {
    m_distance = DEFAULT_camera_distance;
  }
  if (config.findItem(CAMERA, KEY_camera_rotation)) {
    temp = config.getItem(CAMERA, KEY_camera_rotation);
    m_rotation = (!temp.is_double()) ? (DEFAULT_camera_rotation) : ((double)(temp));
  } else {
    m_rotation = DEFAULT_camera_rotation;
  }
  if (config.findItem(CAMERA, KEY_camera_elevation)) {
    temp = config.getItem(CAMERA, KEY_camera_elevation);
    m_elevation = (!temp.is_double()) ? (DEFAULT_camera_elevation) : ((double)(temp));
  } else {
    m_elevation = DEFAULT_camera_elevation;
  }
  
  if (config.findItem(CAMERA, KEY_camera_zoom_speed)) {
    temp = config.getItem(CAMERA, KEY_camera_zoom_speed);
    m_zoom_speed = (!temp.is_double()) ? (DEFAULT_camera_zoom_speed) : ((double)(temp));
  } else {
    m_zoom_speed = DEFAULT_camera_zoom_speed;
  }
  if (config.findItem(CAMERA, KEY_camera_rotation_speed)) {
    temp = config.getItem(CAMERA, KEY_camera_rotation_speed);
    m_rotation_speed = (!temp.is_double()) ? (DEFAULT_camera_rotation_speed) : ((double)(temp));
  } else {
    m_rotation_speed = DEFAULT_camera_rotation_speed;
  }
  if (config.findItem(CAMERA, KEY_camera_elevation_speed)) {
    temp = config.getItem(CAMERA, KEY_camera_elevation_speed);
    m_elevation_speed = (!temp.is_double()) ? (DEFAULT_camera_elevation_speed) : ((double)(temp));
  } else {
    m_elevation_speed = DEFAULT_camera_elevation_speed;
  }
  
  if (config.findItem(CAMERA, KEY_camera_min_distance)) {
    temp = config.getItem(CAMERA, KEY_camera_min_distance);
    m_min_distance = (!temp.is_double()) ? (DEFAULT_camera_min_distance) : ((double)(temp));
  } else {
    m_min_distance = DEFAULT_camera_min_distance;
  }
  if (config.findItem(CAMERA, KEY_camera_max_distance)) {
    temp = config.getItem(CAMERA, KEY_camera_max_distance);
    m_max_distance = (!temp.is_double()) ? (DEFAULT_camera_max_distance) : ((double)(temp));
  } else {
    m_max_distance = DEFAULT_camera_max_distance;
  }
  if (config.findItem(CAMERA, KEY_save_camera_position)) {
    temp = config.getItem(CAMERA, KEY_save_camera_position);
    m_save_camera_position = (!temp.is_bool()) ? (DEFAULT_save_camera_position) : ((bool)(temp));
  } else {
    m_save_camera_position = DEFAULT_save_camera_position;
  }
}

void Camera::writeConfig(varconf::Config &config) {
  assert ((m_initialised == true) && "Camera not initialised");	
  
  if (m_save_camera_position) {
    config.setItem(CAMERA, KEY_camera_distance, m_distance);
    config.setItem(CAMERA, KEY_camera_rotation, m_rotation);
    config.setItem(CAMERA, KEY_camera_elevation, m_elevation);
  }

  config.setItem(CAMERA, KEY_camera_zoom_speed, m_zoom_speed);
  config.setItem(CAMERA, KEY_camera_rotation_speed, m_rotation_speed);
  config.setItem(CAMERA, KEY_camera_elevation_speed, m_elevation_speed);
  
  config.setItem(CAMERA, KEY_camera_min_distance, m_min_distance);
  config.setItem(CAMERA, KEY_camera_max_distance, m_max_distance);

  config.setItem(CAMERA, KEY_save_camera_position, m_save_camera_position);
}

void Camera::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  assert ((m_initialised == true) && "Camera not initialised");	
  varconf::Variable temp;
  if (section == CAMERA) {
    if (key == KEY_camera_distance) {
      temp = config.getItem(CAMERA, KEY_camera_distance);
      m_distance = (!temp.is_double()) ? (DEFAULT_camera_distance) : ((double)(temp));
    }
    else if (key == KEY_camera_rotation) {
      temp = config.getItem(CAMERA, KEY_camera_rotation);
      m_rotation = (!temp.is_double()) ? (DEFAULT_camera_rotation) : ((double)(temp));
    }
    else if (key == KEY_camera_elevation) {
      temp = config.getItem(CAMERA, KEY_camera_elevation);
      m_elevation = (!temp.is_double()) ? (DEFAULT_camera_elevation) : ((double)(temp));
    }
    else if (key == KEY_camera_zoom_speed) {
      temp = config.getItem(CAMERA, KEY_camera_zoom_speed);
      m_zoom_speed = (!temp.is_double()) ? (DEFAULT_camera_zoom_speed) : ((double)(temp));
    }
    else if (key == KEY_camera_rotation_speed) {
      temp = config.getItem(CAMERA, KEY_camera_rotation_speed);
      m_rotation_speed = (!temp.is_double()) ? (DEFAULT_camera_rotation_speed) : ((double)(temp));
    }
    else if (key == KEY_camera_elevation_speed) {
      temp = config.getItem(CAMERA, KEY_camera_elevation_speed);
      m_elevation_speed = (!temp.is_double()) ? (DEFAULT_camera_elevation_speed) : ((double)(temp));
    }
    else if (key == KEY_camera_min_distance) {
      temp = config.getItem(CAMERA, KEY_camera_min_distance);
      m_min_distance = (!temp.is_double()) ? (DEFAULT_camera_min_distance) : ((double)(temp));
    }
    else if (key == KEY_camera_max_distance) {
      temp = config.getItem(CAMERA, KEY_camera_max_distance);
      m_max_distance = (!temp.is_double()) ? (DEFAULT_camera_max_distance) : ((double)(temp));
    }
    else if (key == KEY_save_camera_position) {
      temp = config.getItem(CAMERA, KEY_save_camera_position);
      m_save_camera_position = (!temp.is_bool()) ? (DEFAULT_save_camera_position) : ((bool)(temp));
    }
  }
  updateValues();
}

void Camera::reset() {
  readConfig(System::instance()->getGeneral());
}

} /* namespace Sear */
