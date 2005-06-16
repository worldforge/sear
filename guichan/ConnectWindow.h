// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_OPTIONSWINDOW_H
#define SEAR_GUICHAN_OPTIONSWINDOW_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

namespace gcn {

class Button;
class TextField;

} // namespace gcn

namespace Sear {

class ActionListenerSigC;

class ConnectWindow : virtual public SigC::Object, public gcn::Window {
protected:
  gcn::TextField * m_serverField;

  gcn::Button * m_connectButton;
  gcn::Button * m_closeButton;

  ActionListenerSigC * m_buttonListener;
public:
  ConnectWindow();
  virtual ~ConnectWindow();

  void actionPressed(std::string);
};

} // namespace Sear

#endif // SEAR_GUICHAN_OPTIONSWINDOW_H
