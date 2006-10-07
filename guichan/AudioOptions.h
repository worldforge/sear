// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_AUDIO_OPTIONS_H
#define SEAR_GUICHAN_AUDIO_OPTIONS_H

#include <guichan/widgets/window.hpp>
#include <guichan/actionlistener.hpp>

#include <sigc++/object.h>

#include "common/SPtr.h"

namespace Sear {

class RootWidget;

class AudioOptions : public gcn::Window, public gcn::ActionListener {
protected:
  RootWidget * m_top;
  std::list<SPtr<gcn::Widget> > m_widgets;
public:
  explicit AudioOptions(RootWidget * top);
  virtual ~AudioOptions();

  void action(const std::string &, gcn::Widget *widget);
};

} // namespace Sear

#endif // SEAR_GUICHAN_AUDIO_OPTIONS_H
