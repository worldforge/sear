// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_OVERLAY_H
#define SEAR_GUICHAN_OVERLAY_H

#include <guichan.hpp>

namespace gcn {
}

namespace Sear {

class Overlay {
protected:
  Overlay();
  ~Overlay();

  static Overlay * m_instance;
public:
  static Overlay * instance() {
    if (m_instance == 0) {
      m_instance = new Overlay();
    }
    return m_instance;
  }
};

} // namespace Sear

#endif // SEAR_GUICHAN_OVERLAY_H
