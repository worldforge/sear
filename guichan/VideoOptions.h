// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_VIDEO_OPTIONS_H
#define SEAR_GUICHAN_VIDEO_OPTIONS_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

namespace Sear {

class ActionListenerSigC;
class RootWidget;

class VideoOptions : virtual public SigC::Object, public gcn::Window {
protected:
  RootWidget * m_top;
public:
  explicit VideoOptions(RootWidget * top);
  virtual ~VideoOptions();

  void actionPressed(std::string);
};

} // namespace Sear

#endif // SEAR_GUICHAN_VIDEO_OPTIONS_H
