// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Simon Goodall

#ifndef SEAR_GUICHAN_ACTIONIMAGEBOX_H
#define SEAR_GUICHAN_ACTIONIMAGEBOX_H

#include <guichan.hpp>

#include "ImageBox.h"

namespace Sear {

class ActionImageBox : public ImageBox, public gcn::MouseListener {
public:
  ActionImageBox(const std::string &texture_name);
    
  virtual ~ActionImageBox();

  virtual void mouseReleased(gcn::MouseEvent& mouseEvent);    
};

} // of namespace Sear

#endif // SEAR_GUICHAN_ACTIONIMAGEBOX_H
