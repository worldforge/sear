// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: GuiHandler.cpp,v 1.3 2003-03-06 23:50:37 simon Exp $



#include <string>
#include <map>

#include "GuiHandler.h"
#include "Component.h"

#ifdef HAVE_CONFIG
  #include "config.h"
#endif

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif


namespace Sear {

namespace Gui {

	
GuiHandler::GuiHandler() :
  _initialised(false)
{}

GuiHandler::~GuiHandler() {
  if (_initialised) shutdown();
}

void GuiHandler::init() {
  if (_initialised) shutdown();
  _initialised = true;
}

void GuiHandler::shutdown() {
  _initialised = false;
}

void GuiHandler::readGuiConfig(const std::string &file_name) {


}

void GuiHandler::selectGui(const std::string &gui) {
  _current_gui = _gui_map[gui];
}

void GuiHandler::render() {
  cout << "Rendering current GUI" << endl;
}

void GuiHandler::render(const std::string &gui) {
  cout << "Rendering GUI - " << gui << endl;
}

void GuiHandler::addComponent(const std::string &gui, Component *component) {
  _gui_map[gui].push_back(component);
}

void GuiHandler::removeComponent(const std::string &gui, Component *component) {
//  _gui_map[gui].push_back(component);
}

Component *GuiHandler::findFocusedComponent(unsigned int x, unsigned int y) {
  for (ComponentList::const_iterator I = _current_gui.begin(); I != _current_gui.end(); ++I) {
    Component *component = *I;
    if (component->contains(x, y)) return component;
  }
  return NULL;
}
  
//void GuiHandler::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  
//}


} /* namespace Gui */
	
} /* namespace Sear */

