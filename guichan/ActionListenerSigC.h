// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_ACTIONLISTENERSIGC_H
#define SEAR_GUICHAN_ACTIONLISTENERSIGC_H

#include <guichan.hpp>

#include <sigc++/signal.h>

namespace Sear {

class ActionListenerSigC : public gcn::ActionListener
{
  protected:
    virtual void action(const gcn::ActionEvent& actionEvent) {
      Action.emit(actionEvent.getId());
    }

  public:
    SigC::Signal1<void, std::string> Action;
};

} // namespace Sear

#endif // SEAR_GUICHAN_ACTIONLISTENERSIGC_H
