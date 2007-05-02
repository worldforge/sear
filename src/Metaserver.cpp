// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

#include "src/Metaserver.h"

#include <cassert>

#include <Eris/Metaserver.h>

#include "src/Console.h"

static const std::string CMD_refresh_servers = "refresh_server_list";

namespace Sear {

Metaserver::Metaserver() :
  m_initialised(false),
  m_meta(0)
{}

Metaserver::~Metaserver() {
  if (m_initialised) shutdown();
}

int Metaserver::init() {
  assert(m_initialised == false);

  // TODO Get params from configs
  m_meta = new Eris::Meta("metaserver.worldforge.org", 16);

  // Hook up signals
  m_meta->ReceivedServerInfo.connect(sigc::mem_fun(this, &Metaserver::onReceivedServerInfo));
 
  m_initialised = true;

  return 0;
}
void Metaserver::shutdown() {
  assert(m_initialised == true);

  delete m_meta;

  m_initialised = false;
}

void Metaserver::registerCommands(Console *console) {
  assert(console);
  console->registerCommand(CMD_refresh_servers, this);
}
void Metaserver::runCommand(const std::string &command, const std::string &args) {
  if (command == CMD_refresh_servers) {
    m_meta->refresh();
  }
}
 

void Metaserver::poll() {
  // metaserver polling is done during eris poll
}

 
void Metaserver::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  
}

void Metaserver::varconf_error_callback(const char *message) {
  printf("[Metaserver] %s\n", message);
}


void Metaserver::onReceivedServerInfo(const Eris::ServerInfo &info) {
//  ServerList::iterator I = m_server_list.find(info.getServername());
  printf("Got a server: %s\n", info.getServername().c_str());
  ServerObject so;
  so.hostname    = info.getHostname();
  so.servername  = info.getServername();
  so.ruleset     = info.getRuleset();
  so.server      = info.getServer();
  so.version     = info.getVersion();
  so.build_date  = info.getBuildDate();
  so.num_clients = info.getNumClients();
  so.ping        = info.getPing();
  so.uptime      = info.getUptime();
  so.port        = 6767;

  m_server_list[so.servername] = so;
}
  
} /* namespace Sear */

