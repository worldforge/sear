// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_OPTIONSWINDOW_H
#define SEAR_GUICHAN_OPTIONSWINDOW_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

#include "common/SPtr.h"

namespace gcn {

class ListBox;
class TextField;
class Button;

} // namespace gcn

namespace Sear {

class ActionListenerSigC;
class ServerListModel;

class ConnectWindow : virtual public SigC::Object, public gcn::Window {
protected:
  std::list<SPtr<gcn::Widget> > m_widgets;

  gcn::ListBox * m_servers;

  gcn::TextField * m_serverField;

  gcn::Button * m_connectButton;
  gcn::Button * m_closeButton;
  gcn::Button * m_refreshButton;

  ServerListModel * m_serverListModel;
  ActionListenerSigC * m_buttonListener;

  int m_selected;

  virtual void logic();
public:
  ConnectWindow();
  virtual ~ConnectWindow();

  void actionPressed(std::string);
};

} // namespace Sear

#endif // SEAR_GUICHAN_OPTIONSWINDOW_H
