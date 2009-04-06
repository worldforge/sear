// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_ROOTWIDGET_H
#define SEAR_GUICHAN_ROOTWIDGET_H

#include <guichan.hpp>

namespace Sear {

class RootWidget : public gcn::Container
{
  public:
    typedef std::map<std::string, std::pair<int, int> > CoordDict;
  protected:
    CoordDict m_coords;
  public:
    RootWidget();

    void setWindowCoords(gcn::Window * win, std::pair<int, int> c) {
      m_coords[win->getCaption()] = c;
    }

    void resize(int width, int height, int old_width, int old_height);

    void openWindow(gcn::Window *);
    void closeWindow(gcn::Window *);

    virtual void logic();
    void contextCreated();
    void contextDestroyed(bool check);
};

} // namespace Sear

#endif // SEAR_GUICHAN_ROOTWIDGET_H
