// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Camera.h,v 1.8 2002-11-12 23:59:22 simon Exp $

#ifndef SEAR_CAMERA_H
#define SEAR_CAMERA_H 1

/*
 * This class represents the camera. Currently defined as a chase camera
 * future development should allow for other styles, e.g. first person.
 *
 * In this class camera distance and zoom level are used interchangably
 *  and really just represent the distance between the camera origin (~player
 *  character's head) and the view plane
 */ 

#include <string>
#include <sigc++/object_slot.h>
#include "ConsoleObject.h"

namespace varconf {
  class Config;
}

namespace Sear {

class Console;
	
class Camera : public ConsoleObject, public SigC::Object {
public:
  Camera();
  ~Camera();

  // initialise camera
  bool init();

  // Shutdown camera
  void shutdown();

  // updates camera values according to current rates
  void updateCameraPos(float);

  // Change direction / rate of camera value
  void zoom(int dir) { _zoom_dir += dir; }
  void rotate(int dir) { _rotation_dir += dir; }
  void elevate(int dir) { _elevation_dir += dir; }

  // Accessor functions
  const float getRotation() const { return _rotation;  }
  const float getElevation() const { return _elevation; }
  const float getDistance() const { return _distance; }

  float getXPos() const { return _x_pos; }
  float getYPos() const { return _y_pos; }
  float getZPos() const { return _z_pos; }

  void readConfig();
  void writeConfig();
 
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
  
protected:
   void varconf_callback(const std::string &, const std::string &, varconf::Config &);
  // General key values
  const static char * const KEY_camera_distance = "camera_distance";
  const static char * const KEY_camera_rotation = "camera_rotation";
  const static char * const KEY_camera_elevation = "camera_elevation";

  const static char * const KEY_camera_zoom_speed = "camera_zoom_speed";
  const static char * const KEY_camera_rotation_speed = "camera_rotation_speed";
  const static char * const KEY_camera_elevation_speed = "camera_elevation_speed";
  
  const static char * const KEY_camera_min_distance = "camera_min_distance";
  const static char * const KEY_camera_max_distance = "camera_max_distance";
  
  // Default config values
  const static float DEFAULT_camera_distance = 5.0f;
  const static float DEFAULT_camera_rotation = 0.0f;
  const static float DEFAULT_camera_elevation = -0.1f;
  
  const static float DEFAULT_camera_zoom_speed = 20.0f;
  const static float DEFAULT_camera_rotation_speed = 40.0f;
  const static float DEFAULT_camera_elevation_speed = 40.0f;
  
  const static float DEFAULT_camera_min_distance = 5.0f;
  const static float DEFAULT_camera_max_distance = 25.0f;
  
  static const char * const ZOOM_IN = "+camera_zoom_in";
  static const char * const ZOOM_OUT = "+camera_zoom_out";
  static const char * const ZOOM_STOP_IN = "-camera_zoom_in";
  static const char * const ZOOM_STOP_OUT = "-camera_zoom_out";
  static const char * const ROTATE_LEFT = "+camera_rotate_left";
  static const char * const ROTATE_RIGHT = "+camera_rotate_right";
  static const char * const ROTATE_STOP_LEFT = "-camera_rotate_left";
  static const char * const ROTATE_STOP_RIGHT = "-camera_rotate_right";
  static const char * const ELEVATE_UP = "+camera_elevate_up";
  static const char * const ELEVATE_DOWN = "+camera_elevate_down";
  static const char * const ELEVATE_STOP_UP = "-camera_elevate_up";
  static const char * const ELEVATE_STOP_DOWN = "-camera_elevate_down";
  
  float _distance;  // distance from focus
  float _rotation;  // horizontal rotation
  float _elevation; // vertical rotation
  
  int _zoom_dir;      // Direction / rate of zoom
  int _rotation_dir;  // Direction / rate of rotation
  int _elevation_dir; // Direction / rate of elevation

  float _zoom_speed;
  float _rotation_speed;
  float _elevation_speed;

  float _min_distance; // Minimum camera distance allowed
  float _max_distance; // Maximum camera distance allowed

  float _x_pos;
  float _y_pos;
  float _z_pos;

  bool _initialised;
};

} /* namespace Sear */
#endif /* SEAR_CAMERA_H */
