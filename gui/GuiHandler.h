// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: GuiHandler.h,v 1.2 2002-10-20 13:22:26 simon Exp $

#ifndef SEAR_GUI_GUIHANDLER_H
#define SEAR_GUI_GUIHANDLER_H 1

#include <string>
#include <map>
#include <list>

//#include "Component.h"

namespace Sear {

namespace Gui {
class Component;
	
class GuiHandler {
public:
  GuiHandler();
  ~GuiHandler();

  void init();
  void shutdown();

  void readGuiConfig(const std::string &file_name);
  
  void selectGui(const std::string &gui);

  void render();
  void render(const std::string &gui);
  
  void addComponent(const std::string &gui, Component *component);
  void removeComponent(const std::string &gui, Component *component);

  Component *findFocusedComponent(unsigned int x, unsigned int y);

  
protected:
  typedef std::list<Component*> ComponentList;
  typedef std::map<std::string, ComponentList> GuiMap;
  
  GuiMap _gui_map;
  ComponentList _current_gui;
  bool _initialised;

};


} /* namespace Gui */
	
} /* namespace Sear */

#endif /* SEAR_GUI_GUIHANDLER_H */
