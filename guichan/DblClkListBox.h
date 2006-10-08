// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 Simon Goodall

#ifndef SEAR_GUICHAN_DBLCLKLISTBOX_H
#define SEAR_GUICHAN_DBLCLKLISTBOX_H 1

#include <guichan.hpp>
#include <string>
#include <SDL/SDL.h>

class DblClkListBox : public gcn::ListBox
{
public:
  DblClkListBox(gcn::ListModel *m, gcn::ActionListener *a, const std::string &event) :
     ListBox(m),
     m_action(a),
     m_event(event),
     m_last_time(-1),
     m_last_button(-1)
  {}


  virtual void mousePress(int x, int y, int button) {
    ListBox::mousePress(x, y, button);
    long int time = SDL_GetTicks();
    if (button == m_last_button) {
      // TODO Make 500 configurable
      if (time - m_last_time < 500) {
        if (m_action) m_action->action(m_event, this);
        // Reset state
        time = -1;
        button = -1;
      }
    }
    m_last_time = time;
    m_last_button = button;
  }

protected:
  gcn::ActionListener *m_action;
  std::string m_event;
  long int m_last_time;
  int m_last_button;
};

#endif /* SEAR_GUICHAN_DBLCLKLISTBOX_H */
