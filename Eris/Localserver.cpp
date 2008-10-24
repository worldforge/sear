// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 - 2008 Simon Goodall

#include "Localserver.h"
#include "Avahi.h"
#include "Bonjour.h"

#include <cassert>

#include <Eris/Metaserver.h>

#include "src/Console.h"

static const std::string CMD_refresh_servers = "refresh_server_list";

namespace Sear {

Localserver::Localserver() :
  m_initialised(false),
  m_meta(0)
{}

Localserver::~Localserver() {
  if (m_initialised) shutdown();
}

int Localserver::init() {
  assert(m_initialised == false);

  // TODO Get params from configs
  m_meta = new Eris::Meta("metaserver.worldforge.org", 16);

  // Hook up signals
  m_meta->ReceivedServerInfo.connect(sigc::mem_fun(this, &Localserver::onReceivedServerInfo));

  m_avahi = std::auto_ptr<Avahi>(new Avahi());
  if (m_avahi->init(this)) {
    m_avahi.reset(0);
  }

  m_bonjour = std::auto_ptr<Bonjour>(new Bonjour());
  if (m_bonjour->init(this)) {
    m_bonjour.reset(0);
  }
 
  m_initialised = true;

  return 0;
}
void Localserver::shutdown() {
  assert(m_initialised == true);

  m_avahi.reset(0);
  m_bonjour.reset(0);

  delete m_meta;

  m_initialised = false;
}

void Localserver::registerCommands(Console *console) {
  assert(console);
  console->registerCommand(CMD_refresh_servers, this);
}
void Localserver::runCommand(const std::string &command, const std::string &args) {
  if (command == CMD_refresh_servers) {
    m_meta->refresh();
  }
}
 

void Localserver::poll() {
  assert(m_initialised == true);
  // metaserver polling is done during eris poll
  if (m_avahi.get()) m_avahi->poll();
  if (m_bonjour.get()) m_bonjour->poll();
}

void Localserver::addServerObject(const ServerObject &so) {
  m_server_list[so.hostname] = so;
}

 
void Localserver::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  
}

void Localserver::varconf_error_callback(const char *message) {
  printf("[Localserver] %s\n", message);
}


void Localserver::onReceivedServerInfo(const Eris::ServerInfo &info) {
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
  so.port        = 6767; // The Metaserver can't handle different ports

  m_server_list[so.hostname] = so;
}
  
} /* namespace Sear */

