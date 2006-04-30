// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_ALERT_H
#define SEAR_GUICHAN_ALERT_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

#include "common/SPtr.h"

namespace gcn {

class Button;
class Container;

} // namespace gcn

namespace Sear {

class ActionListenerSigC;

class Alert : virtual public SigC::Object, public gcn::Window {
protected:
  gcn::Button * m_okButton;

  ActionListenerSigC * m_buttonListener;

  std::list<SPtr<gcn::Widget> > m_widgets;
public:
  explicit Alert(gcn::Container *, const std::string &);
  virtual ~Alert();

  void actionPressed(std::string);

  virtual void logic();
};

} // namespace Sear

#endif // SEAR_GUICHAN_ALERT_H
