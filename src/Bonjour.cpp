// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "SDL_loadso.h"

#include "src/Bonjour.h"
#include "src/Metaserver.h"

#include <string>
#include <stdio.h>

#include <errno.h>  
#include <unistd.h>   
#include <vector>
#include <cassert>

#ifdef HAVE_BONJOUR
// dns_sd.h defines inttypes itself, unless we define this;
#define _MSL_STDINT_H
#include <inttypes.h>
#include "dns_sd.h"
#include <winsock2.h>

//  Default search strings
static const std::string STR_service = "_worldforge._tcp";  
static const std::string STR_domain  = "local";

// Declare funtion pointers
typedef int (DNSSD_API *pfnDNSServiceRefSockFD)(DNSServiceRef sdRef);
typedef DNSServiceErrorType (DNSSD_API *pfnDNSServiceProcessResult) (DNSServiceRef sdRef);
typedef void (DNSSD_API *pfnDNSServiceRefDeallocate) (DNSServiceRef sdRef);
typedef DNSServiceErrorType (DNSSD_API *pfnDNSServiceBrowse) (DNSServiceRef *, DNSServiceFlags, uint32_t, const char *, const char*, DNSServiceBrowseReply, void *);
typedef DNSServiceErrorType (DNSSD_API *pfnDNSServiceResolve) (DNSServiceRef*, DNSServiceFlags, uint32_t, const char *, const char*,const char*, DNSServiceResolveReply, void *);
typedef DNSServiceErrorType (DNSSD_API *pfnDNSServiceQueryRecord) (DNSServiceRef *, DNSServiceFlags, uint32_t, const char*, uint16_t, uint16_t, DNSServiceQueryRecordReply, void *);

typedef const void * (DNSSD_API *pfnTXTRecordGetValuePtr) (uint16_t , const void *, const char *, uint8_t *);
typedef int (DNSSD_API *pfnTXTRecordContainsKey) (uint16_t, const void *, const char *);

pfnDNSServiceRefSockFD SEAR_DNSServiceRefSockFD = 0;
pfnDNSServiceQueryRecord SEAR_DNSServiceQueryRecord = 0;
pfnDNSServiceResolve SEAR_DNSServiceResolve = 0;
pfnDNSServiceBrowse SEAR_DNSServiceBrowse = 0;
pfnDNSServiceProcessResult SEAR_DNSServiceProcessResult = 0;
pfnDNSServiceRefDeallocate SEAR_DNSServiceRefDeallocate = 0;

pfnTXTRecordGetValuePtr SEAR_TXTRecordGetValuePtr = 0;
pfnTXTRecordContainsKey SEAR_TXTRecordContainsKey = 0;

typedef std::vector<std::pair<DNSServiceRef,bool> > ResolverList;
#endif

// Data strcture passed about
class BonjourUserData  {
public:
#ifdef HAVE_BONJOUR
  BonjourUserData():
    meta(0)
  {}
  ~BonjourUserData() {
    // Clean up if required
    if (client) SEAR_DNSServiceRefDeallocate(client);
    while (resolvers.empty() == false) {
      DNSServiceRef c = resolvers.begin()->first;
      if (c) SEAR_DNSServiceRefDeallocate(c);
      resolvers.erase(resolvers.begin());
    }
  }

  ResolverList resolvers;
  Sear::Metaserver *meta;
  DNSServiceRef client;
#endif
};

#ifdef HAVE_BONJOUR

// Small structure passed into the resolver to keep track of the server name
typedef struct {
  BonjourUserData *ud;
  std::string name;
} ResolveData;

static void DNSSD_API resolve_callback(DNSServiceRef client, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
        const char *fullname, const char *hosttarget, uint16_t opaqueport, uint16_t txtLen, const unsigned char *txtRecord, void *context)
        {
  assert(context);
  ResolveData *rd = reinterpret_cast<ResolveData*>(context);
  BonjourUserData *ud = rd->ud;

  Sear::ServerObject so;
  so.hostname = hosttarget;
  so.servername = rd->name;
  so.port = ntohs(opaqueport);
  so.ping = -1;
  so.num_clients = -1;
  so.uptime = -1.0;

  // Process the TXT record
  uint8_t len;
  char *ptr;
  if (SEAR_TXTRecordContainsKey(txtLen, txtRecord, "server")) {
    ptr = (char*)SEAR_TXTRecordGetValuePtr(txtLen, txtRecord, "server", &len);
    so.server = std::string(ptr, len);
  }
  if (SEAR_TXTRecordContainsKey(txtLen, txtRecord, "ruleset")) {
    ptr = (char*)SEAR_TXTRecordGetValuePtr(txtLen, txtRecord, "ruleset", &len);
    so.ruleset = std::string(ptr, len);
  }
  if (SEAR_TXTRecordContainsKey(txtLen, txtRecord, "version")) {
    ptr = (char*)SEAR_TXTRecordGetValuePtr(txtLen, txtRecord, "version", &len);
    so.version = std::string(ptr, len);
  }

  if (SEAR_TXTRecordContainsKey(txtLen, txtRecord, "clients")) {
    ptr = (char*)SEAR_TXTRecordGetValuePtr(txtLen, txtRecord, "clients", &len);
    sscanf(ptr, "%d", &so.num_clients);
  }
  if (SEAR_TXTRecordContainsKey(txtLen, txtRecord, "uptime")) {
    ptr = (char*)SEAR_TXTRecordGetValuePtr(txtLen, txtRecord, "uptime", &len);
    sscanf(ptr, "%lf", &so.uptime);
  }

/*
  // Here we try to get the IP of the target.
  // gethostbyname is a blocking call however!
  hostent *he = gethostbyname(hosttarget);
  if (he != 0) {
    // If gethostbyname succeeded, we now have an address
    sockaddr_in out_peer;
    ::memcpy(&((sockaddr_in&)out_peer).sin_addr, he->h_addr_list[0],
                                                 he->h_length);

    so.histname = inet_ntoa(out_peer.sin_addr);
  }
*/

  // We need to tell the system that this client object is no longer required.
  ResolverList::iterator I = ud->resolvers.begin();
  ResolverList::const_iterator Iend = ud->resolvers.end();
  while (I != Iend) {
   if (I->first == client) {
     I->second = true;
     break;
   }
    ++I;
  }

  // Add the server record to the metaserver list
  ud->meta->addServerObject(so);

  // Delete the struct now we are finished with it
  delete rd;
}


static void DNSSD_API browse_callback(DNSServiceRef client, DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorcode, const char *replyName, const char *replyType, const char *replyDomain, void *context) {
  assert(context);
  BonjourUserData *ud = reinterpret_cast<BonjourUserData*>(context);

  uint32_t opinterface = kDNSServiceInterfaceIndexAny;

  // Create struct to pass replyName into the resolve callback
  ResolveData *rd = new ResolveData();
  rd->ud = ud;
  rd->name = replyName;

  // Create a new entry by expanding list
  int idx = ud->resolvers.size();
  ud->resolvers.resize(ud->resolvers.size() + 1);

  // Hook up callback to reolve this entry
  SEAR_DNSServiceResolve(&(ud->resolvers[idx].first), 0, opinterface, replyName, replyType, replyDomain, (DNSServiceResolveReply)resolve_callback, rd);
}

#endif

namespace Sear {

Bonjour::Bonjour() :
  m_initialised(false),
  m_hnd(0)
{}

Bonjour::~Bonjour() {
  if (m_initialised) shutdown();
} 

void Bonjour::poll() {
  assert(m_initialised == true);
#ifdef HAVE_BONJOUR

  DNSServiceErrorType err;
  // Poll for events
  fd_set  readfds;
  struct timeval tv;

  // Get socket for main bonjour stuff
  int fd = SEAR_DNSServiceRefSockFD(m_ud->client);

  // Init the fd set
  FD_ZERO(&readfds);
  // Add in the main fd
  FD_SET(fd, &readfds);

  // Hook in the resolver socket fd's
  int nfds = fd + 1; // This is the highest numbers fd in the set +1
  ResolverList::const_iterator I = m_ud->resolvers.begin();
  ResolverList::const_iterator Iend = m_ud->resolvers.end();
  while (I != Iend) {
    int fd_2 =  (I->first) ? SEAR_DNSServiceRefSockFD(I->first) : -1;
    if (fd_2 >= nfds) nfds = fd_2 + 1;
    FD_SET(fd_2, &readfds);
    ++I;
  }

  // Setting to 0 for immediate return since we are polling
  tv.tv_sec = 0;
  tv.tv_usec = 0;

  // See if there is any data waiting
  int result = select(nfds, &readfds, (fd_set*)NULL, (fd_set*)NULL, &tv);
  if (result > 0) { // There is some data!
    // Check main socket first
    if (FD_ISSET(fd, &readfds)) {
      err = SEAR_DNSServiceProcessResult(m_ud->client);
      if (err) {
        printf("Err  %d\n", err);
        return;
      }
    }
    // Loop through all resolvers and process data
    ResolverList::const_iterator I = m_ud->resolvers.begin();
    ResolverList::const_iterator Iend = m_ud->resolvers.end();
    while (I != Iend) {
      int fd_2 =  (I->first) ? SEAR_DNSServiceRefSockFD(I->first) : -1;
      if ( FD_ISSET(fd_2, &readfds)) {
        err = SEAR_DNSServiceProcessResult(I->first);
        if (err) {
          printf("Err  %d\n", err);
          return;
        }
      }
      ++I;
    }
  } else if (result == 0) {
     // Nothing to process
  } else {
    printf("select() returned %d errno %d %s\n", result, errno, strerror(errno));
    if (errno != EINTR) {
      printf("EINTR\n");
      return;
    } 
  }

  // Clean up finished resolvers
  ResolverList::iterator J = m_ud->resolvers.begin();
  ResolverList::const_iterator Jend = m_ud->resolvers.end();
  while (J != Jend) {
    if (J->second) {
      SEAR_DNSServiceRefDeallocate(J->first);
      m_ud->resolvers.erase(J);
      // Reset iterators
      J = m_ud->resolvers.begin();
      Jend = m_ud->resolvers.end();
    } else {
      ++J;
    }
  }
#endif
}

int Bonjour::init(Metaserver *meta) {
  assert(m_initialised == false); 
#ifdef HAVE_BONJOUR
  // Try and load the bonjour dll
  m_hnd = SDL_LoadObject("dnssd.dll");
  if (m_hnd == 0) return 1;

  // Hook up function pointers
  SEAR_DNSServiceRefSockFD = (pfnDNSServiceRefSockFD)SDL_LoadFunction(m_hnd, "DNSServiceRefSockFD");
  SEAR_DNSServiceQueryRecord = (pfnDNSServiceQueryRecord)SDL_LoadFunction(m_hnd, "DNSServiceQueryRecord");
  SEAR_DNSServiceResolve = (pfnDNSServiceResolve)SDL_LoadFunction(m_hnd, "DNSServiceResolve");
  SEAR_DNSServiceBrowse = (pfnDNSServiceBrowse)SDL_LoadFunction(m_hnd, "DNSServiceBrowse");
  SEAR_DNSServiceProcessResult = (pfnDNSServiceProcessResult)SDL_LoadFunction(m_hnd, "DNSServiceProcessResult");
  SEAR_DNSServiceRefDeallocate = (pfnDNSServiceRefDeallocate)SDL_LoadFunction(m_hnd, "DNSServiceRefDeallocate");
  
  SEAR_TXTRecordGetValuePtr = (pfnTXTRecordGetValuePtr)SDL_LoadFunction(m_hnd, "TXTRecordGetValuePtr");
  SEAR_TXTRecordContainsKey = (pfnTXTRecordContainsKey)SDL_LoadFunction(m_hnd, "TXTRecordContainsKey");

  // Create the shared data object
  m_ud = std::auto_ptr<BonjourUserData>(new BonjourUserData());
  m_ud->meta = meta;

  // Fire off the browser query
  DNSServiceFlags f = 0;
  DNSServiceErrorType err = SEAR_DNSServiceBrowse(&m_ud->client, f, kDNSServiceInterfaceIndexAny, STR_service.c_str(), STR_domain.c_str(), browse_callback, (void*)m_ud.get());

  if (!m_ud->client || err != 0) {
    m_ud.release();
    return 1;
  }
#endif

  m_initialised = true;

  return 0;
}

void Bonjour::shutdown() {
  assert(m_initialised == true);
#ifdef HAVE_BONJOUR
  m_ud.release();

  SDL_UnloadObject(m_hnd);
  m_hnd = 0;

#endif
  m_initialised = false;
}


} // namespace Sear
