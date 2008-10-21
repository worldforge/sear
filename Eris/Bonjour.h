// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

#ifndef SEAR_BONJOUR_H
#define SEAR_BONJOUR_H 1

#include <memory>

class BonjourUserData;

namespace Sear {

class Localserver;

class Bonjour {
public:
  Bonjour();
  ~Bonjour();

  int init(Localserver *meta);
  void shutdown();
  bool isInitialised() const { return m_initialised; }

  void poll();

private:
  bool m_initialised;
  std::auto_ptr<BonjourUserData> m_ud;
  void *m_hnd;

};

} /* namespace Sear */

#endif /* SEAR_BONJOUR_H */
