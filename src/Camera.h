// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall, University of Southampton

// $Id: Camera.h,v 1.11 2003-03-06 23:50:38 simon Exp $

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
	
/**
 * This class represents the camera in the game world.
 */ 
class Camera : public ConsoleObject, public SigC::Object {
public:
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

  /**
   * updates camera values according to current rates
   * @param time_elapsed Time elapsed in seconds since last update
   */ 
  void updateCameraPos(float time_elapsed);

  /**
   * Change zoom state
   * @param dir Direction of zoom. -1 is move closer, 0 is stationary, 1 is move away
   */ 
  void zoom(int dir) { _zoom_dir += dir; }

  /**
   * Change rotate state
   * @param dir Direction of rotation. -1 is rotate --, 0 is stationary, 1 is rotate --
   */ 
  void rotate(int dir) { _rotation_dir += dir; }

  /**
   * Change elevation state
   * @param dir Direction of elevation. -1 is elevate --, 0 is stationary, 1 is elevate --
   */ 
  void elevate(int dir) { _elevation_dir += dir; }

  /**
   * Get current rotation angle
   * @return Current rotation angle (radians)
   */ 
  const float getRotation() const { return _rotation;  }
  /**
   * Get current elevation angle
   * @return Current elevation angle (radians)
   */ 
  const float getElevation() const { return _elevation; }
  /**
   * Get current zoom distance
   * @return Current zoom distance (meters)
   */ 
  const float getDistance() const { return _distance; }

  /**
   * Get camera X position
   * @return X position
   */ 
  float getXPos() const { return _x_pos; }

  /**
   * Get camera Y position
   * @return Y position
   */ 
  float getYPos() const { return _y_pos; }

  /**
   * Get camera Z position
   * @return Z position
   */ 
  float getZPos() const { return _z_pos; }

  /**
   * Read camera config data
   */ 
  void readConfig();

  /**
   * Write camera config data
   */ 
  void writeConfig();
 
  /**
   * Register console commands
   * @param console Console object to register with
   */ 
  void registerCommands(Console *console);

  /**
   * Runs a console command
   * @param command Console command
   * @param args Console command arguments
   */ 
  void runCommand(const std::string &command, const std::string &args);
  
protected:
  /**
   * Callback used when config data changes
   * @param section Name of section containing changed data
   * @param key Name of key with changed data
   * @param config Config object that has been changed
   */ 
   void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);

  float _distance;  ///< distance from focus (meters)
  float _rotation;  ///< horizontal rotation (radians)
  float _elevation; ///< vertical rotation (radians)
  
  int _zoom_dir;      ///< Direction / rate of zoom
  int _rotation_dir;  ///< Direction / rate of rotation
  int _elevation_dir; ///< Direction / rate of elevation - negative values point downwards

  float _zoom_speed;      ///< Speed of zoom changes
  float _rotation_speed;  ///< Speed of rotation changes
  float _elevation_speed; ///< Speed of elevation changes

  float _min_distance; ///< Minimum camera distance allowed
  float _max_distance; ///< Maximum camera distance allowed

  float _x_pos; ///< X position of camera
  float _y_pos; ///< Y position of camera
  float _z_pos; ///< Z position of camera

  bool _initialised; ///< Camera initialisation state 
  bool _save_camera_position; ///< Flag for whether camera state should be saved
};

} /* namespace Sear */
#endif /* SEAR_CAMERA_H */
