// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Simon Goodall

#include "ActionImageBox.h"

namespace Sear {


ActionImageBox::ActionImageBox(const std::string &texture_name) : ImageBox(texture_name)
{
  addMouseListener(this);
}

ActionImageBox::~ActionImageBox()
{
}

void ActionImageBox::mouseReleased(gcn::MouseEvent& mouseEvent) {
  if (mouseEvent.getButton() == gcn::MouseEvent::LEFT) {
    distributeActionEvent();
    mouseEvent.consume();
  } else if (mouseEvent.getButton() == gcn::MouseEvent::LEFT) {
    mouseEvent.consume();
  }
}

} // namespace Sear
