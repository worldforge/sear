// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_LABEL_H
#define SEAR_GUI_LABEL_H

#include "Widget.h"

#include <string>

namespace Sear {

/// Higher level class defining any part in the gui.
/// They key difference is that some widgets may not have a visible component.
class Label : public Widget {
private:
  // Private and unimplemented to prevent slicing
  Label(const Label &);
  const Label & operator=(const Label &);
public:
  explicit Label(const std::string & text);
  virtual ~Label();

  virtual void show();
};

} // namespace Sear

#endif // SEAR_GUI_LABEL_H
