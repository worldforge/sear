// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

//$Id: Model.h,v 1.3 2005-03-15 17:33:58 simon Exp $

#ifndef SEAR_MODEL_H
#define SEAR_MODEL_H 1

#include <map>
#include <string>

#include <Atlas/Message/Element.h>

#include "renderers/Render.h"
#include "renderers/Graphics.h"

namespace Sear {

class Model {
public:
  Model(Render *render) :
    m_render(render),
    m_in_use(true)
  {}
  virtual ~Model() {}
//  virtual int init() = 0;
  virtual int shutdown() = 0;

  virtual void invalidate() = 0;

  virtual void update (float time_elapsed) {}
  virtual void render(bool select_mode) {}
  virtual void setDetailLevel(float f) {}

  virtual void action(const std::string &action) {}
  virtual void setHeight(float height) {} 
  virtual void setAppearance(Atlas::Message::MapType &mt) {}

  virtual void setFlag(const std::string &flag, bool state) { m_flag_map[flag] = state; }
  virtual bool getFlag(const std::string &flag) const {
    std::map<std::string, bool>::const_iterator I = m_flag_map.find(flag);
    return ((I != m_flag_map.end()) ? (I->second) : (false));
  }

  bool getInUse() const { return m_in_use;}
  void setInUse(bool in_use) { m_in_use = in_use; }
  
  virtual Graphics::RotationStyle rotationStyle() { return Graphics::ROS_NONE; }

  float getLastTime() const { return m_last_time; }
  void setLastTime(float t) { m_last_time = t; }
  
protected: 
  Render *m_render;
  float m_last_time;
private:
  bool m_in_use;
  std::map<std::string, bool> m_flag_map;
  
};

} /* namespace Sear */

#endif /* SEAR_MODEL_H */
