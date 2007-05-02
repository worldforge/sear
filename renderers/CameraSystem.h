// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

#ifndef SEAR_RENDERERS_CAMERASYSTEM_H
#define SEAR_RENDERERS_CAMERASYSTEM_H 1

#include <string>
#include <sigc++/trackable.h>

#include "common/SPtr.h"
#include "interfaces/ConsoleObject.h"

namespace varconf {
  class Config;
}

namespace Sear {

class Camera;
class Console;

class CameraSystem : public ConsoleObject, public sigc::trackable {
public:

  typedef std::vector<SPtr<Camera> > CameraVector;

  CameraSystem();
  ~CameraSystem();

  int init();
  int shutdown();
  bool isInitialised() const { return m_initialised; }
  
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);

  // Add a new camera to the system. CameraSystem takes ownership
  int addCamera(Camera *camera) {
    int pos = m_cameras.size();
    m_cameras.push_back(SPtr<Camera>(camera));
    return pos;
  }

  // Remove a camera from the system
  void removeCamera(int pos);

  void update(double time_elapsed);

  // Get a camera
  const Camera *getCamera(int pos) const { return m_cameras[pos].get(); }

  // Get the camera currently in use
  const Camera *getCurrentCamera() const { return m_cameras[m_current].get(); }

  Camera *getCurrentCamera() { return m_cameras[m_current].get(); }

  // Select a camera for use
  void setCurrentCamera(unsigned int i) {
    // Check bounds
    if (i >= m_cameras.size()) return;
    m_current = i; 
  }

  void readConfig(varconf::Config &config);
  void writeConfig(varconf::Config &config);

private:
  void varconf_callback(const std::string &section,
                        const std::string &key,
                        varconf::Config &config);


  bool m_initialised;

  CameraVector m_cameras;
  int m_current; 

};

} /* namespace Sear */

#endif /* SEAR_RENDERERS_CAMERASYSTEM_H */
