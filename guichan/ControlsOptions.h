// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_CONTROLS_OPTIONS_H
#define SEAR_GUICHAN_CONTROLS_OPTIONS_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

#include "common/SPtr.h"

namespace gcn {
class TextField;
class TextBox;
}

namespace Sear {

class RootWidget;

class ControlsOptions : public gcn::Window, public gcn::ActionListener {
protected:
  std::list<SPtr<gcn::Widget> > m_widgets;
  RootWidget * m_top;

  gcn::TextField * m_key;
  gcn::TextField * m_action;
  gcn::TextBox * m_controlText;

  void readBindings();
public:
  explicit ControlsOptions(RootWidget * top);
  virtual ~ControlsOptions();

  void action(const std::string &);
};

} // namespace Sear

#endif // SEAR_GUICHAN_CONTROLS_OPTIONS_H
