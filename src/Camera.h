// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _CAMERA_H_
#define _CAMERA_H_ 1

class Camera {
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
  void zoom(int dir);
  void rotate(int dir);
  void elevate(int dir);

  // Accessor functions
  const float getRotation()  { return _rotation;  }
  const float getElevation() { return _elevation; }
  const float getDistance() { return _distance; }

  void readConfig();
  void writeConfig();
  
protected:
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
  
  float _distance;  // distance from focus
  float _rotation;  // horizontal rotation
  float _elevation; // vertical rotation
  
  int _zoom_dir;      // Direction / rate of zoom
  int _rotation_dir;  // Direction / rate of rotation
  int _elevation_dir; // Direction / rate of elevation

  float _zoom_speed;
  float _rotation_speed;
  float _elevation_speed;

  float _min_distance;
  float _max_distance;
};

#endif /* _CAMERA_H_ */
