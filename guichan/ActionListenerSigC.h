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
    void action(const std::string & eventId, gcn::Widget *widget) {
      Action.emit(eventId);
    }

  public:
    SigC::Signal1<void, std::string> Action;
};

} // namespace Sear

#endif // SEAR_GUICHAN_ACTIONLISTENERSIGC_H
