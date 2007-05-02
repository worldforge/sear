// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

// $Id: Metaserver.h,v 1.1 2007-05-02 20:47:55 simon Exp $

#ifndef SEAR_METASERVER_H
#define SEAR_METASERVER_H 1

#include <map>
#include <string>

#include <sigc++/trackable.h>

#include <varconf/Config.h>

#include <Eris/ServerInfo.h>

#include <interfaces/ConsoleObject.h>

namespace Eris {
  class Meta;
}

namespace Sear {

class Console;

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

class Metaserver : public ConsoleObject, public sigc::trackable {
public:
  Metaserver();
  ~Metaserver();

  int init();
  void shutdown();
  bool isInitialised() const { return m_initialised; }

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
 
  void poll();

  const ServerList &getServerList() const { return m_server_list; } 
 
private:
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
  void varconf_error_callback(const char *message);
 
  void onReceivedServerInfo(const Eris::ServerInfo &info);
 
  bool m_initialised;

  Eris::Meta *m_meta;
  ServerList m_server_list;
};
  
} /* namespace Sear */

#endif /* SEAR_METASERVER_H */
