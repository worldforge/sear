// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006

// $Id: Camera.h,v 1.7 2006-02-20 20:36:23 simon Exp $

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
#include <sigc++/trackable.h>
#include <wfmath/quaternion.h>
#include <wfmath/point.h>

namespace varconf {
  class Config;
}

namespace Sear {

/**
 * This class represents the camera in the game world.
 */ 
class Camera : public sigc::trackable {
public:
  typedef enum {
    CAMERA_CHASE = 0,
    CAMERA_FIRST,
    CAMERA_LAST
  } CameraType;


  /**
   * Default constructor
   */ 
  Camera();

  /**
   * Destructor
   */ 
  ~Camera();

  /**
   * Initialise camera
   * @return True on success, false on failure
   */ 
  bool init();

  /**
   * Shutdown camera
   */ 
  void shutdown();
  bool isInitialised() const { return m_initialised; }
  /**
   * updates camera values according to current rates
   * @param time_elapsed Time elapsed in seconds since last update
   */ 
  void updateCameraPos(float time_elapsed);

  /**
   * Change zoom state
   * @param dir Direction of zoom. -1 is move closer, 0 is stationary, 1 is move away
   */ 
  void zoom(int dir) { m_zoom_dir += dir; }

  /**
   * Change rotate state
   * @param dir Direction of rotation. -1 is rotate --, 0 is stationary, 1 is rotate --
   */ 
  void rotate(int dir) { m_rotation_dir += dir; }

  /**
   * Change elevation state
   * @param dir Direction of elevation. -1 is elevate --, 0 is stationary, 1 is elevate --
   */ 
  void elevate(int dir) { m_elevation_dir += dir; }

  /**
   * Make an immediate change to the rotation
   * @param rot Ammount of change in rotation.
   */
  void rotateImmediate(float rot);

  /**
   * Make an immediate change to the elevation
   * @param elev Ammount of change in elevation.
   */
  void elevateImmediate(float elev);

  /**
   * Set the rotate state
   * @param dir Direction and scale of rotation,
   */
  void setRotationSpeed(float dir) { m_rotation_dir = dir; }

  /**
   * Set the elevation state
   * @param dir Direction and scale of elevation,
   */
  void setElevationSpeed(float dir) { m_elevation_dir = dir; }

  /**
   * Get current rotation angle
   * @return Current rotation angle (radians)
   */ 
  const float getRotation() const { return m_rotation;  }
  /**
   * Get current elevation angle
   * @return Current elevation angle (radians)
   */ 
  const float getElevation() const { return m_elevation; }
  /**
   * Get current zoom distance
   * @return Current zoom distance (meters)
   */ 
  const float getDistance() const { return m_distance; }

  /**
   * Read camera config data
   */ 
  void readConfig(varconf::Config &config);

  /**
   * Write camera config data
   */ 
  void writeConfig(varconf::Config &config);

  CameraType getType() const { return m_type; }
  void setType(CameraType type) { m_type = type; }

  void setMinDistance(float m) { m_min_distance = m; }
  void setMaxDistance(float m) { m_max_distance = m; }

  float getMinDistance() const { return m_min_distance; }
  float getMaxDistance() const { return m_max_distance; }
 
  void reset();

  const WFMath::Quaternion &getOrientation() const { return m_orient; }
  const WFMath::Point<3> &getPosition() const { return m_pos; }
 
protected:
  /**
   * Callback used when config data changes
   * @param section Name of section containing changed data
   * @param key Name of key with changed data
   * @param config Config object that has been changed
   */ 
   void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);

  // Update orientation and position based on current elevation, rotation and 
  //distance.
  void updateValues();

  bool m_initialised; ///< Camera initialisation state 

  float m_distance;  ///< distance from focus (meters)
  float m_rotation;  ///< horizontal rotation (radians)
  float m_elevation; ///< vertical rotation (radians)
  
  float m_zoom_dir;      ///< Direction / rate of zoom
  float m_rotation_dir;  ///< Direction / rate of rotation
  float m_elevation_dir; ///< Direction / rate of elevation - negative values point downwards

  float m_zoom_speed;      ///< Speed of zoom changes
  float m_rotation_speed;  ///< Speed of rotation changes
  float m_elevation_speed; ///< Speed of elevation changes

  float m_min_distance; ///< Minimum camera distance allowed
  float m_max_distance; ///< Maximum camera distance allowed

  bool m_save_camera_position; ///< Flag for whether camera state should be saved
  CameraType m_type;

  WFMath::Quaternion m_orient;
  WFMath::Point<3> m_pos;
};

} /* namespace Sear */
#endif /* SEAR_CAMERA_H */
