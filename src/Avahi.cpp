// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <cassert>
#include <stdlib.h>
#include <string>

#ifdef HAVE_AVAHI
#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#endif

#include "Avahi.h"
#include "Metaserver.h"

#ifdef HAVE_AVAHI
static void resolve_callback(
  AvahiServiceResolver *r,
  AvahiIfIndex interface,
  AvahiProtocol protocol,
  AvahiResolverEvent event,
  const char *name,
  const char *type,
  const char *domain,
  const char *host_name,
  const AvahiAddress *address,
  uint16_t port,
  AvahiStringList *txt,
  AvahiLookupResultFlags flags,
  void* userdata) {

  assert(r);
  assert(userdata);
  UserData *ud = reinterpret_cast<UserData*>(userdata);

  /* Called whenever a service has been resolved successfully or timed out */

  switch (event) {
    case AVAHI_RESOLVER_FAILURE:
      fprintf(stderr, "[Avahi] Failed to resolve service '%s' of type '%s' in domain '%s': %s\n", name, type, domain, avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(r))));
      break;

    case AVAHI_RESOLVER_FOUND: {
      char addr[AVAHI_ADDRESS_STR_MAX];
      avahi_address_snprint(addr, sizeof(addr), address);

      printf("[Avahi] Found avahi host %s : %hu\n", host_name, port);
      Sear::ServerObject so;
      so.servername = name;
      so.hostname = addr;
      so.port = port;
      so.ping = -1; // How to get this info? -- Hook into eris code maybe?
      // Process the TXT field to get the other data
      AvahiStringList *ptxt = txt;
      char  *key, *value;
      while (ptxt != 0) {
        if (avahi_string_list_get_pair(ptxt, &key, &value, NULL) == 0) {
          printf("[Avahi] Key: %s value %s\n", key, value);
          std::string skey = key;
          if (skey == "ruleset") so.ruleset = value;
          else if (skey == "server") so.server = value;
          else if (skey == "version") so.version = value;
          else if (skey == "builddate") so.build_date = value;
          else if (skey == "clients") {
            sscanf(value, "%d", &so.num_clients);
          }
          else if (skey == "uptime") {
            sscanf(value, "%lf", &so.uptime);
          }
          avahi_free(key);
          avahi_free(value);
        }
        ptxt = ptxt->next;
      }
      ud->meta->addServerObject(so);
    }
  }

  avahi_service_resolver_free(r);
}

static void browse_callback(
  AvahiServiceBrowser *b,
  AvahiIfIndex interface,
  AvahiProtocol protocol,
  AvahiBrowserEvent event,
  const char *name,
  const char *type,
  const char *domain,
  AvahiLookupResultFlags flags,
  void* userdata) {

  assert(b);
  assert(userdata); 
  UserData *ud = reinterpret_cast<UserData*>(userdata);

  AvahiClient *c = ud->c;

  /* Called whenever a new services becomes available on the LAN or is removed from the LAN */
  AvahiLookupFlags f = (AvahiLookupFlags)0;
  switch (event) {
    case AVAHI_BROWSER_FAILURE:
      
      fprintf(stderr, "[Avahi] %s\n", avahi_strerror(avahi_client_errno(avahi_service_browser_get_client(b))));
      avahi_simple_poll_quit(ud->simple_poll);
      return;

    case AVAHI_BROWSER_NEW:
      if (!(avahi_service_resolver_new(c, interface, protocol, name, type, domain, AVAHI_PROTO_UNSPEC, f, resolve_callback, userdata)))
        fprintf(stderr, "[Avahi] Failed to resolve service '%s': %s\n", name, avahi_strerror(avahi_client_errno(c)));
      
      break;

    case AVAHI_BROWSER_REMOVE: // TODO: Something here?
    case AVAHI_BROWSER_ALL_FOR_NOW:
    case AVAHI_BROWSER_CACHE_EXHAUSTED:
      break;
  }
}

static void client_callback(AvahiClient *c, AvahiClientState state, void * userdata) {
  assert(c);
  assert(userdata);
  UserData *ud = reinterpret_cast<UserData*>(userdata);

  /* Called whenever the client or server state changes */
  if (state == AVAHI_CLIENT_FAILURE) {
    fprintf(stderr, "[Avahi] Server connection failure: %s\n", avahi_strerror(avahi_client_errno(c)));
    avahi_simple_poll_quit(ud->simple_poll);
  }
}

#endif

namespace Sear {

Avahi::Avahi() :
  m_initialised(false)
{}

Avahi::~Avahi() {
  if (m_initialised) shutdown();
}

void Avahi::poll() {
  assert(m_initialised == true);
#ifdef HAVE_AVAHI
  if (m_ud->simple_poll) {
    avahi_simple_poll_iterate(m_ud->simple_poll, 10);
  }
#endif
}

int Avahi::init(Metaserver *meta) {
  assert(m_initialised == false);
#ifdef HAVE_AVAHI
  m_ud = std::auto_ptr<UserData>(new UserData());
  int error;
  m_ud->meta = meta;

  AvahiClientFlags cf = (AvahiClientFlags)0;
  AvahiLookupFlags lf = (AvahiLookupFlags)0;

  /* Allocate main loop object */
  if (!(m_ud->simple_poll = avahi_simple_poll_new())) {
    fprintf(stderr, "[Avahi] Failed to create simple poll object.\n");
    goto fail;
  }

  /* Allocate a new client */
  m_ud->c = avahi_client_new(avahi_simple_poll_get(m_ud->simple_poll), cf, client_callback, m_ud.get(), &error);

  /* Check whether creating the client object succeeded */
  if (!m_ud->c) {
    fprintf(stderr, "[Avahi] Failed to create client: %s\n", avahi_strerror(error));
    goto fail;
  }

  /* Create the service browser */
  if (!(m_ud->sb = avahi_service_browser_new(m_ud->c, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, "_worldforge._tcp", NULL, lf, browse_callback, m_ud.get()))) {
    fprintf(stderr, "[Avahi] Failed to create service browser: %s\n", avahi_strerror(avahi_client_errno(m_ud->c)));
    goto fail;
  }

#endif
 m_initialised = true;

  return 0;

#ifdef HAVE_AVAHI
fail:
  if (m_ud->sb)
    avahi_service_browser_free(m_ud->sb);
  
  if (m_ud->c)
    avahi_client_free(m_ud->c);

  if (m_ud->simple_poll)
    avahi_simple_poll_free(m_ud->simple_poll);

  m_ud.release();
  return 1;
#endif 
}

void Avahi::shutdown() {
  assert(m_initialised == true);
#ifdef HAVE_AVAHI
  if (m_ud->sb)
    avahi_service_browser_free(m_ud->sb);
  
  if (m_ud->c)
    avahi_client_free(m_ud->c);

  if (m_ud->simple_poll)
    avahi_simple_poll_free(m_ud->simple_poll);

  m_ud.release();

#endif
  m_initialised = false;
}

} // namespace Sear
