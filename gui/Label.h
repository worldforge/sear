// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Label.h,v 1.1 2002-09-27 15:46:42 simon Exp $

#ifndef SEAR_GUI_LABEL_H
#define SEAR_GUI_LABEL_H 1

#include <string>
#include <map>

#include "Component.h"

namespace Sear {

namespace Gui {
	
class Label: public Component{
public:
  Label() : Component() {}

  Label(unsigned int width, unsigned int height, unsigned int left, unsigned int top) : Component(width, height, left, top) {}

  virtual ~Label() {}

  virtual void render();

  virtual std::string getText() const { return _text; }
  virtual void setText(const std::string &text) { _text = text; }
  
  virtual void setExtra(const std::string &name, varconf::Variable &value);
  
protected:
  bool _initialised;
  std::string _text;

};


} /* namespace Gui */
	
} /* namespace Sear */

#endif /* SEAR_GUI_LABEL_H */
