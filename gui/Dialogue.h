// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_DIALOGUE_H
#define SEAR_GUI_DIALOGUE_H

#include "gui/Toplevel.h"

#include <string>

namespace Sear {

class Window;
class VBox;

/// Toplevel widget with decorations.
class Dialogue : public Toplevel {
private:
  // Private and unimplemented to prevent slicing
  Dialogue(const Dialogue &);
  const Dialogue & operator=(const Dialogue &);
protected:
  VBox * m_contentBox;
public:
  explicit Dialogue(const std::string & title);
  virtual ~Dialogue();

  void setPane(Widget *);
};

} // namespace Sear

#endif // SEAR_GUI_DIALOGUE_H
