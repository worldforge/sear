// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_BOX_H
#define SEAR_GUI_BOX_H

#include "gui/Container.h"

namespace Sear {

/// Abstract class for widget containing a series of other widgets
/// If this starts to seem a little gtk+, then that's because I stole
/// the idea from gtk+
class Box : public Container {
protected:
  Box();

private:
  // Private and unimplemented to prevent slicing
  Box(const Box &);
  const Box & operator=(const Box &);
public:
  virtual ~Box();

  void render();
};

class VBox : public Box {
private:
  // Private and unimplemented to prevent slicing
  VBox(const VBox &);
  const VBox & operator=(const VBox &);
public:
  VBox();
  virtual ~VBox();
};

class HBox : public Box {
private:
  // Private and unimplemented to prevent slicing
  HBox(const HBox &);
  const HBox & operator=(const HBox &);
public:
  HBox();
  virtual ~HBox();
};

} // namespace Sear

#endif // SEAR_GUI_BOX_H
