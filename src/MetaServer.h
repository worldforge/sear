// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef _METASERVER_H_
#define _METASERVER_H_ 1

#include <Eris/Metaserver.h>
#include <Eris/ServerInfo.h>

namespace Sear {

class MetaServer: public SigC::Object {
public:
  MetaServer();
  ~MetaServer();

  bool init(const std::string &);
  void shutdown();

  void connect();
  void query();

  void ReceivedServerInfo(Eris::ServerInfo);
  void GotServerCount(int);
  void CompletedServerList();
  void Failure(const std::string &);

protected:
  Eris::Meta *_meta;

};

}

#endif /* _METASERVER_H_ */
