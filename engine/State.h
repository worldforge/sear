// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: State.h,v 1.1.2.1 2003-01-05 14:22:01 simon Exp $

#ifndef SEAR_ENGINE_STATE_H
#define SEAR_ENGINE_STATE_H 1

namespace Sear {

class State {
public:
  typedef enum {
    CONNECTED_TO_SERVER = 0, // Set when connected to a server
    LOGGED_IN_TO_SERVER, // Set we we login to a server
    CHARACTER_IN_WORLD, // Set when we enter the world
    FULLSCREEN, // Set when we are in fullscreen mode
    LAST_STATE
  } States;
  
  State() {
    for (unsigned int i = 0; i < LAST_STATE; ++i) _states[(States)i] = false;
  }

  ~State() {}

  /**
   * Sets a state to the given vale
   * \param state Reference state
   * \param value New value of reference state
   */ 
  void setState(States state, bool value) {
    _states[state] = value;
  }

  /**
   *  Get the value of specfied state
   *  \param state The query state
   *  \return Value of state
   */ 
  bool getState(States state) const {
    return _states[state];
  }
  
private:
  bool _initialised;
  States _states[LAST_STATE];
};
	
} /* namespace Sear */

#endif /* SEAR_ENGINE_STATE_H */
