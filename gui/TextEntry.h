// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_TEXT_ENTRY_H
#define SEAR_GUI_TEXT_ENTRY_H

#include "gui/Container.h"

#include <sigc++/signal.h>

#include <SDL/SDL.h>

#include <string>

namespace Sear {

class String;
class Frame;
class Caret;

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
  Caret * m_caret;
  std::string m_input;
  unsigned int m_textOffset;
  unsigned int m_size;
  unsigned int m_border;
  unsigned int m_caretPos;

  void setText();
public:
  explicit TextEntry(int size, const std::string & text);
  virtual ~TextEntry();

  virtual void map(Window * win, int x, int y, int & w, int & h);
  virtual void show() { }

  void onKeyPress(SDLKey, Uint16);
  void onPressed();
  void onFocus(Window *);
};

} // namespace Sear

#endif // SEAR_GUI_TEXT_ENTRY_H