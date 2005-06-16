// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_OPTIONSWINDOW_H
#define SEAR_GUICHAN_OPTIONSWINDOW_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

namespace gcn {

class Button;

} // namespace gcn

namespace Sear {

class ActionListenerSigC;

class OptionsWindow : virtual public SigC::Object, public gcn::Window {
protected:
  gcn::Button * m_applyButton;
  gcn::Button * m_closeButton;

  ActionListenerSigC * m_buttonListener;
public:
  OptionsWindow();
  virtual ~OptionsWindow();

  void actionPressed(std::string);
};

} // namespace Sear

#endif // SEAR_GUICHAN_OPTIONSWINDOW_H
