// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Component.h,v 1.1 2002-09-27 15:46:42 simon Exp $

#ifndef SEAR_GUI_COMPONENT_H
#define SEAR_GUI_COMPONENT_H 1

#include <string>
#include <map>

#include <SDL/SDL.h>

#include <varconf/Config.h>

namespace Sear {

namespace Gui {

class Component {
public:
  Component() :
    _initialised(false),
    _width(0),
    _height(0),
    _left(0),
    _top(0)
  {}

  Component(unsigned int width, unsigned int height, unsigned int left, unsigned int top) :
    _initialised(false),
    _width(width),
    _height(height),
    _left(left),
    _top(top)
  {}
  
  virtual ~Component() { if (_initialised) shutdown(); }

  virtual void init() {}
  virtual void shutdown() {}

  virtual bool contains(unsigned int x, unsigned int y) {
    return (
      (x >= _left && x <= _left + _width) &&
      (y <= _top && y >= _top - _height)
    );
  }

  virtual void render() = 0;

  virtual void event(SDL_Event &event) {}

  virtual void setExtra(const std::string &name, varconf::Variable &value) {}

  
  virtual unsigned int getLeft() const { return _left; }
  virtual void setLeft(unsigned int left) { _left = left; }

  virtual unsigned int getTop() const { return _top; }
  virtual void setTop(unsigned int top) { _top = top; }
  
  virtual void move(unsigned int x_diff, unsigned int y_diff) { _left += x_diff; _top += y_diff; }
  virtual void moveTo(unsigned int left, unsigned int top) { _left = left; _top = top; }
  
  virtual void resize(unsigned int width, unsigned int height) { _width = width; _height = height; }

  virtual unsigned int getWidth() const { return _width; }
  virtual void setWidth(unsigned int width) { _width = width; }

  virtual unsigned int getHeight() const { return _height; }
  virtual void setHeight(unsigned int height) { _height = height; }
  
protected:
  bool _initialised;
  unsigned int _width;
  unsigned int _height;
  unsigned int _left;
  unsigned int _top;

};


} /* namespace Gui */
	
} /* namespace Sear */

#endif /* SEAR_GUI_GUIHANDLER_H */
