// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall

// $Id: StateManager.h,v 1.3 2003-05-29 18:37:17 simon Exp $

#ifndef SEAR_RENDER_STATEMANAGER_H
#define SEAR_RENDER_STATEMANAGER_H 1

#include <string>
#include <map>
#include <vector>

#include <sigc++/object_slot.h>
#include "interfaces/ConsoleObject.h"



namespace varconf {
  class Config;
}

namespace Sear {

class Console;
	
// This would be better obtained elsewhere ie from a query to
// OpenGL, but simpler this way
#define MAX_UNITS 6

typedef struct {
  std::string state;
  bool alpha_test;
  bool blend;
  bool lighting;
  bool two_sided_lighting;
  bool textures[MAX_UNITS];
  bool colour_material;
  bool depth_test;
  bool cull_face;
  bool cull_face_cw;
  bool stencil;
  bool fog;
  bool rescale_normals;
  float alpha_value;
  int alpha_function;
  int blend_src_function;
  int blend_dest_function;
} StateProperties;

typedef int StateID;
typedef std::map<std::string, StateID> StateNameMap;
typedef std::vector<std::string> NameStateVector;
typedef std::vector<StateProperties*> StateVector;
typedef std::vector<std::vector<unsigned int> > StateChangeVector;

class StateManager : public SigC::Object, public ConsoleObject {
public:
  StateManager();
  ~StateManager();

  void init();
  void shutdown();
  void readFiles(const std::string &);

  StateID getState(const std::string &state_name) {
    return _state_name_map[state_name];
  }

  void stateChange(StateID state);
//  std::map<std::string, StateProperties*> getMap() const { return _state_properties; }

//  StateProperties *getStateProperties(const std::string &state) {
//    StateProperties *sp = _state_properties[state];
//    return ((sp) ? (sp) : (_state_properties["default"]));
//  }

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &arguments);
  
  StateID getCurrentState() const { return _current_state; }
private:
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
  void varconf_error_callback(const char *message);

  int getAlphaFunction(const std::string &alpha_function);
  int getBlendFunction(const std::string &blend_function);
 
  void buildStateChange(unsigned int &list, StateProperties *previous_state, StateProperties *next_state);
  
  bool _initialised;
  StateVector _states; ///< This stores all the state records
  StateChangeVector _state_change_vector; ///< This keeps a display list for every state change
  StateNameMap _state_name_map; ///< This maps a state name to its id
  NameStateVector _name_state_vector; ///< This will map a stateid back to its name

  StateID _current_state;
  unsigned int _state_counter;

};

} /* namespace Sear */

#endif /* SEAR_RENDER_STATEMANAGER_H */
