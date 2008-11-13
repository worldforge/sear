// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

// $Id: Localserver.h,v 1.4 2008-05-08 20:22:56 simon Exp $

#ifndef SEAR_METASERVER_H
#define SEAR_METASERVER_H 1

#include <map>
#include <string>
#include <memory>

#include <sigc++/trackable.h>

#include <varconf/config.h>

#include <Eris/ServerInfo.h>

#include <interfaces/ConsoleObject.h>

namespace Eris {
  class Meta;
}

namespace Sear {

class Console;

class Avahi;
class Bonjour;

typedef struct {
  std::string hostname;
  std::string servername;
  std::string ruleset;
  std::string server;
  std::string version;
  std::string build_date;
  int num_clients;
  int ping;
  double uptime;
  unsigned short port;
} ServerObject;

typedef std::map<std::string, ServerObject> ServerList;

class Localserver : public ConsoleObject, public sigc::trackable {
public:
  Localserver();
  ~Localserver();

  int init();
  void shutdown();
  bool isInitialised() const { return m_initialised; }

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
 
  void poll();

  const ServerList &getServerList() const { return m_server_list; } 
  void addServerObject(const ServerObject &server);
 
private:
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
  void varconf_error_callback(const char *message);
 
  void onReceivedServerInfo(const Eris::ServerInfo &info);
 
  bool m_initialised;

  Eris::Meta *m_meta;
  ServerList m_server_list;
  std::auto_ptr<Avahi> m_avahi;
  std::auto_ptr<Bonjour> m_bonjour;
};
  
} /* namespace Sear */

#endif /* SEAR_METASERVER_H */
