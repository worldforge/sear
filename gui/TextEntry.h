// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_TEXT_ENTRY_H
#define SEAR_GUI_TEXT_ENTRY_H

#include "gui/Container.h"

#include <sigc++/signal.h>

namespace Sear {

class String;
class Frame;

/// Higher level class defining any part in the gui.
/// They key difference is that some widgets may not have a visible component.
class TextEntry : public Widget {
private:
  // Private and unimplemented to prevent slicing
  TextEntry(const TextEntry &);
  const TextEntry & operator=(const TextEntry &);
protected:
  Frame * m_frame;
  String * m_text;
  int m_size;
  unsigned int m_border;

public:
  explicit TextEntry(int size, const std::string & text);
  virtual ~TextEntry();

  virtual void show();
  virtual void map(Window * win, int x, int y, int & w, int & h);
};

} // namespace Sear

#endif // SEAR_GUI_TEXT_ENTRY_H
