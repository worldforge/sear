// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#include "MetaServer.h"

#include <sigc++/object_slot.h>
#include <sigc++/object.h>

#include <iostream.h>

namespace Sear {

MetaServer::MetaServer() :
  _meta(NULL)
{

}

MetaServer::~MetaServer() {

}

bool MetaServer::init(const std::string &meta_server) {
  // Update variables to constsants!!
  _meta = new Eris::Meta("Sear", meta_server, 10);
  _meta->ReceivedServerInfo.connect(SigC::slot(*this, &MetaServer::ReceivedServerInfo));
  _meta->GotServerCount.connect(SigC::slot(*this, &MetaServer::GotServerCount));
  _meta->CompletedServerList.connect(SigC::slot(*this, &MetaServer::CompletedServerList));
  _meta->Failure.connect(SigC::slot(*this, &MetaServer::Failure));

  return true;
}
void MetaServer::shutdown() {

}
/*
void MetaServer::connect(const std::string meta_server, const int port) {

}
*/
void MetaServer::query() {
  _meta->getGameServerList();
  _meta->refresh();
}

void MetaServer::ReceivedServerInfo(Eris::ServerInfo si) {
  std::cout << " Hostname: " << si.getHostname();
  std::cout << " ServerName: " << si.getServername();
  std::cout << " Ruleset: " << si.getRuleset();
  std::cout << " Server Type: " << si.getServer();
  std::cout << " Clients: " << si.getNumClients();
  std::cout << " Ping: " << si.getPing();
  std::cout << " Uptime: " << si.getUptime();
  std::cout << std::endl;
}

void MetaServer::GotServerCount(int count) {
  std::cout << "Got " << count << " servers." << std::endl;
}

void MetaServer::CompletedServerList() {
  std::cout << "Got all servers." << std::endl;
}

void MetaServer::Failure(const std::string &msg) {
  std::cerr << "Got Failure Message: " << msg << std::endl;
}

}



int main(int argc, char**argv) {
  std::cout << "Metaserver Test" << std::endl;
  Sear::MetaServer *meta = new Sear::MetaServer();
  std::cout << "Init" << std::endl;
  meta->init("monty");
  std::cout << "End of Init" << std::endl;
  meta->query();
  std::cout << "End of Query" << std::endl;
  delete meta;
  return 0;
}
