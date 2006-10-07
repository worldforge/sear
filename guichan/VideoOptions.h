// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_VIDEO_OPTIONS_H
#define SEAR_GUICHAN_VIDEO_OPTIONS_H

#include <guichan/widgets/window.hpp>
#include <guichan/actionlistener.hpp>

#include <sigc++/object.h>

#include "common/SPtr.h"

namespace gcn {
class CheckBox;
class ListBox;
}

namespace Sear {

class ActionListenerSigC;
class RootWidget;
class ResolutionListModel;

class VideoOptions : virtual public SigC::Object, public gcn::Window, virtual public gcn::ActionListener {
protected:
  std::list<SPtr<gcn::Widget> > m_widgets;
  RootWidget * m_top;

  gcn::CheckBox * m_fullCheck;
  gcn::ListBox * m_resolutions;

  ResolutionListModel * m_resolutionList;
  ActionListenerSigC * m_buttonListener;

  bool m_checkFullChanged;
public:
  explicit VideoOptions(RootWidget * top);
  virtual ~VideoOptions();

  void actionPressed(std::string);

  virtual void logic();
  virtual void action(const std::string &, gcn::Widget *widget);
};

} // namespace Sear

#endif // SEAR_GUICHAN_VIDEO_OPTIONS_H
