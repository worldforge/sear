// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: StateManager.h,v 1.16 2006-12-03 13:38:48 simon Exp $

#ifndef SEAR_RENDER_STATEMANAGER_H
#define SEAR_RENDER_STATEMANAGER_H 1

#include <string>
#include <map>
#include <vector>

#include <sigc++/trackable.h>

#include "interfaces/ConsoleObject.h"
#include "common/SPtr.h"

#include "RenderTypes.h"

namespace varconf {
  class Config;
}

namespace Sear {

class Console;
	
// This would be better obtained elsewhere ie from a query to
// OpenGL, but simpler this way
#define MAX_UNITS 2

typedef struct {
  std::string state;
  bool alpha_test;
  bool blend;
  bool lighting;
  bool two_sided_lighting;
  bool textures[MAX_UNITS];
  bool colour_material;
  bool depth_test;
  bool depth_write;
  bool cull_face;
  bool cull_face_cw;
  bool stencil;
  bool fog;
  bool rescale_normals;
  bool normalise;
  float alpha_value;
  int alpha_function;
  int blend_src_function;
  int blend_dest_function;
} StateProperties;

//typedef int StateID;
typedef std::map<std::string, StateID> StateNameMap;
typedef std::vector<std::string> NameStateVector;
typedef std::vector<SPtr<StateProperties> > StateVector;
typedef std::vector<std::vector<unsigned int> > StateChangeVector;

class StateManager : public sigc::trackable, public ConsoleObject {
public:
  StateManager();
  ~StateManager();

  int init();
  int shutdown();
  bool isInitialised() const { return m_initialised; }

  void readFiles(const std::string &);

  StateID requestState(const std::string &state_name);

  void stateChange(StateID state);
  void forceStateChange(StateID state) {
    m_current_state = -1;
    stateChange(state);
  }
//  std::map<std::string, StateProperties*> getMap() const { return _state_properties; }

//  StateProperties *getStateProperties(const std::string &state) {
//    StateProperties *sp = _state_properties[state];
//    return ((sp) ? (sp) : (_state_properties["default"]));
//  }

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &arguments);
  
  StateID getCurrentState() const { return m_current_state; }

  void contextCreated();
  void contextDestroyed(bool check);

private:
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
  void varconf_error_callback(const char *message);

  static int getAlphaFunction(const std::string &alpha_function);
  static int getBlendFunction(const std::string &blend_function);
 
  void buildStateChange(unsigned int &list, SPtr<StateProperties> previous_state, SPtr<StateProperties> next_state);
  
  bool m_initialised;
  StateVector m_states; ///< This stores all the state records
  StateChangeVector m_state_change_vector; ///< This keeps a display list for every state change
  StateNameMap m_state_name_map; ///< This maps a state name to its id
  NameStateVector m_name_state_vector; ///< This will map a stateid back to its name

  StateID m_current_state;
  unsigned int m_state_counter;

};

} /* namespace Sear */

#endif /* SEAR_RENDER_STATEMANAGER_H */
