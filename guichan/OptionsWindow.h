// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_OPTIONSWINDOW_H
#define SEAR_GUICHAN_OPTIONSWINDOW_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

#include <map>

namespace gcn {

class Button;

} // namespace gcn

namespace Sear {

class ActionListenerSigC;
class RootWidget;

class OptionsWindow : virtual public SigC::Object, public gcn::Window {
public:
  typedef std::map<std::string, std::pair<int, int> > CoordDict;
protected:
  ActionListenerSigC * m_buttonListener;

  std::map<std::string, gcn::Window *> m_windows;

  RootWidget * m_top;
public:
  explicit OptionsWindow(RootWidget * top);
  virtual ~OptionsWindow();

  void actionPressed(std::string);
};

} // namespace Sear

#endif // SEAR_GUICHAN_OPTIONSWINDOW_H
