// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

#ifndef SEAR_AVAHI_H
#define SEAR_AVAHI_H 1


#include <memory>

class AvahiClient;
class AvahiSimplePoll;
class AvahiServiceBrowser;

namespace Sear {
class Metaserver;
}

typedef struct {
  AvahiClient *c;
  AvahiServiceBrowser *sb;
  AvahiSimplePoll *simple_poll;
  Sear::Metaserver *meta;
} UserData;

namespace Sear {

class Avahi {
public:
  Avahi();
  ~Avahi();

  int init(Metaserver *meta);
  void shutdown();
  bool isInitialised() const { return m_initialised; }

  void poll();

private:
  bool m_initialised;
  std::auto_ptr<UserData> m_ud;

};

} /* namespace Sear */

#endif /* SEAR_AVAHI_H */
