// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _MODELS_H_
#define _MODELS_H_ 1

#include <string>

namespace Sear {

class Models {
public:
  Models() :
    _in_use(false)
  {}
  virtual ~Models() {}
  virtual bool init() { return false; }
  virtual void shutdown() {}
  virtual void update (float time_elapsed) {}
  virtual float getScale() { return 1.0f; }
  virtual void render(bool select_mode) {}

  virtual bool useTextures() { return false; }
  virtual void action(const std::string &action) {}

  virtual void setFlag(const std::string &flag, bool state) {}
  
  virtual bool getFlag(const std::string &flag) { return false; }

  bool getInUse() { return _in_use;}
  void setInUse(bool in_use) { _in_use = in_use; }
  
  typedef enum {
    NONE = 0,
    NORMAL,
    POSITION,
    BILLBOARD,
    HALO
  } RotationStyle;
  virtual RotationStyle rotationStyle() { return NONE; }
 
  
  typedef enum {
    INVALID = 0,
    POINT,
    LINES,
    TRIANGLES,
    QUADS,
    TRIANGLE_FAN,
    TRIANGLE_STRIP,
    QUAD_STRIP
  } Type;

private:
  bool _in_use;
  
};

} /* namespace Sear */

#endif /* _MODELS_H_ */
