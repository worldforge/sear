// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _MODEL_H_
#define _MODEL_H_ 1

#include <map>
#include <string>

#include "StateLoader.h"

#include "Graphics.h"

namespace Sear {

class Model {
public:
  Model() :
    _in_use(false)
  {}
  virtual ~Model() {}
  virtual bool init() { return false; }
  virtual void shutdown() {}
  virtual void update (float time_elapsed) {}
  virtual float getScale() { return 1.0f; }
  virtual void render(bool select_mode) {}
  virtual void setDetailLevel(float f) {}

  virtual bool useTextures() { return false; }
  virtual void action(const std::string &action) {}

  virtual void setFlag(const std::string &flag, bool state) { _flag_map[flag] = state; }
  virtual bool getFlag(const std::string &flag) { return _flag_map[flag]; }

  bool getInUse() { return _in_use;}
  void setInUse(bool in_use) { _in_use = in_use; }
  
  virtual Graphics::RotationStyle rotationStyle() { return Graphics::ROS_NONE; }

  void setSelectState(StateProperties *sp) { _select_state = sp; }
 
  StateProperties *getSelectState() { return _select_state; }
  
private:
  bool _in_use;
  std::map<std::string, bool> _flag_map;
  StateProperties *_select_state;
  
};

} /* namespace Sear */

#endif /* _MODEL_H_ */