// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_ACTIONLISTENERSIGC_H
#define SEAR_GUICHAN_ACTIONLISTENERSIGC_H

#include <guichan.hpp>

class ActionListenerSigC : public gcn::ActionListener
{
  protected:
    void action(const std::string & eventId) {
      Action.emit(eventId);
    }

  public:
    std::string command;
};

#endif // SEAR_GUICHAN_ACTIONLISTENERSIGC_H
