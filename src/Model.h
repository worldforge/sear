// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

//$Id: Model.h,v 1.4 2002-09-26 20:23:03 simon Exp $

#ifndef SEAR_MODEL_H
#define SEAR_MODEL_H 1

#include <map>
#include <string>

#include "StateLoader.h"

#include "Render.h"
#include "Graphics.h"

namespace Sear {

class Model {
public:
  Model(Render *render) :
    _render(render),
    _in_use(true)
  {}
  virtual ~Model() {}
  virtual bool init() { return false; }
  virtual void shutdown() {}
  virtual void update (float time_elapsed) {}
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
  float getLastTime() { return _last_time; }
  void setLastTime(float t) { _last_time = t; }
  
protected: 
  Render *_render;
  float _last_time;
private:
  bool _in_use;
  std::map<std::string, bool> _flag_map;
  StateProperties *_select_state;
  
  
};

} /* namespace Sear */

#endif /* SEAR_MODEL_H */
