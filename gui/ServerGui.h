// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: ServerGui.h,v 1.1 2002-10-21 20:00:05 simon Exp $

#ifndef SEAR_GUI_SERVERGUI_H
#define SEAR_GUI_SERVERGUI_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vector>
#include <string>
#include <list>

#ifdef HAVE_GLGOOEY
#include "glgooey/WindowManager.h"
#include "glgooey/Rectangle.h"
#include "glgooey/FrameWindow.h"
#include "glgooey/CheckBox.h"
#include "glgooey/CheckBoxGroup.h"
#include "glgooey/Button.h"
#include "glgooey/Panel.h"
#include "glgooey/EditField.h"
#include "glgooey/MultiTextButton.h"
#include "glgooey/Font.h"
#include "glgooey/ScrollBar.h"
#include "glgooey/ListBox.h"
#include "glgooey/ListControl.h"
#include "glgooey/StaticText.h"
#include "glgooey/ProgressBar.h"
#include "glgooey/ComplexGridLayouter.h"
#include "glgooey/StaticBitmap.h"
#include "glgooey/TimeManager.h"
#include "glgooey/SelectionMessage.h"

#endif

#include <Eris/ServerInfo.h>

#include "src/System.h"

namespace Sear {
#ifdef HAVE_GLGOOEY
class ServerButtonListener : public Gooey::ActionListener {
public:
  ServerButtonListener(Gooey::EditField &hostname, Gooey::EditField &port): _hostname(hostname), _port(port) { }
protected:
  virtual ActionListener& onAction(const std::string& aName)  {
    if(aName == "Connect") {
      std::string connect_string = "/connect " + _hostname.text() + " " + _port.text();
      System::instance()->runCommand(connect_string);  
    }
    else if(aName == "Reconnect") {
      System::instance()->runCommand("/reconnect");  
    }
    else if(aName == "Disconnect") {
      System::instance()->runCommand("/disconnect");  
    }
    return *this;
  }
  

private:
  Gooey::EditField &_hostname;
  Gooey::EditField &_port;
};

class MetaButtonListener : public Gooey::ActionListener {
public:
  MetaButtonListener(Gooey::ListControl &listcontrol) : _listcontrol(listcontrol) {}
protected:
  virtual Gooey::ActionListener& onAction(const std::string &aName) {
    if (aName == "Refresh") {
      while (_listcontrol.numberOfRows() > 1) {
        _listcontrol.removeRowAt(2);
      }
      System::instance()->runCommand("/get_servers");
    }
    else if (aName == "Cancel") {
      System::instance()->runCommand("/stop_servers");
    }
    return *this;
  }

private:
  Gooey::ListControl &_listcontrol;
};

class ListControlListener: public Gooey::SelectionListener {
public:
  ListControlListener(Gooey::EditField &hostname, Gooey::ListControl &list): _hostname(hostname), _list(list){ }
protected:
  virtual SelectionListener& onDeselected(unsigned int index) {
    return *this;
  }

  virtual SelectionListener& onSelected(unsigned int index) {
    _hostname.setText(_list.getRowAt(_list.selectedIndex()).stringAt(0));
    return *this;
  }

private:
  Gooey::EditField &_hostname;
  Gooey::ListControl &_list;
};
#endif

class ServerGui {
public:
  ServerGui();
  ~ServerGui();

  void addServer(Eris::ServerInfo);
	
protected:
#ifdef HAVE_GLGOOEY
  ServerButtonListener *sbl;
  MetaButtonListener *mbl;
  ListControlListener *lcl;
  Gooey::FrameWindow *server_window;
  Gooey::FrameWindow *server_list_window;
  
  Gooey::Panel *panel;
  Gooey::Panel *server_list_panel;
  
  static const unsigned int num_server_list_columns = 7;
  Gooey::ListControl *server_list;

  Gooey::Button *button_connect;
  Gooey::Button *button_disconnect;
  Gooey::Button *button_reconnect;
  Gooey::Button *button_refresh;
  Gooey::Button *button_cancel;

  Gooey::StaticText *label_hostname;
  Gooey::StaticText *label_port;
  
  Gooey::EditField *textbox_hostname;
  Gooey::EditField *textbox_port;
#endif
};

} /* namespace Sear */

#endif /* SEAR_GUI_SERVERGUI_H */
