// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: LightManager.h,v 1.1 2002-12-24 16:17:15 simon Exp $

#ifndef SEAR_LIGHTMANAGER_H
#define SEAR_LIGHTMANAGER_H 1

#include <sigc++/object_slot.h>
#include <varconf/Config.h>

#include <vector>
#include <stack>
#include <queue>
namespace Sear {

class Light;
	
class LightManager : public SigC::Object {
public:
  typedef std::priority_queue<std::vector<Light*> > LightQueue;
//  typedef std::priority_queue<std::vector<Light*>, std::greater<Light*> > LightQueue;
	
  LightManager();
  ~LightManager();

  void init();
  void shutdown();

  void readConfig();
  void writeConfig();

  void config_update(const std::string &section, const std::string &key, varconf::Config &config);

  void update(float time_elapsed);
 
  LightQueue &getLights();
  
  unsigned int getNumberOfLights() const { return _number_of_lights; }
  void setNumberOfLights(unsigned int number_of_lights);
  
  
private:
  bool _initialised;
  SigC::Connection _config_connection;
  
  unsigned int _number_of_lights;  

  LightQueue _lights;
};
	
} /* namespace Sear */

#endif /* SEAR_LIGHTMANAGER_H */
