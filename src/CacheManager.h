// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: CacheManager.h,v 1.2 2006-04-26 14:39:00 simon Exp $

#ifndef SEAR_CACHEMANAGER_H
#define SEAR_CACHEMANAGER_H 1

#include <string>
#include <map>

#include <sigc++/object_slot.h>

#include <varconf/Config.h>

namespace Sear {

class CacheObject;

class CacheManager {
public:
  CacheManager() :
    m_initialised(false)
  {}
  virtual ~CacheManager() {
    if (m_initialised) shutdown();
  }

  int init();
  int shutdown();

  /** 
   * Check to see if filename is in cache
   * @param filename Filename o f cache object
   * @return True if object is in cache
   */ 
  bool inCache(const std::string &filename);

  /**
   * Load object from cache
   * @param filenameFilename of Cache Object
   * @return Pointer to Cache Object, NULL on error
   */
  CacheObject* loadObject(const std::string &filename);

  /**
   * Add object to cache
   * @param obj Pointer to Cache Object
   * @param filename Filename to index cache object by
   * @return o on success, 1 on failure
   */ 
  int add(CacheObject *obj, const std::string &filename);

  /**
   * Add a new Cache Object to CacheManger. The Type number is matched against 
   * the cacheobject header to determine how to load it. obj is copied using 
   * obj->newInstance, so it can safely be deleted after adding
   * @param obj Pointer to cache object
   * @return 0 on success, 1 on failure
   */
  int addType(CacheObject *obj);

  /**
   * Get the singleton instance of this class
   * @return  Ref to singleton
   */
  static CacheManager &getInstance() { return m_instance; }

private:
  bool m_initialised; // Flag indication initialisation status
  static CacheManager m_instance; // Global instance of class

  std::map<int, CacheObject *> m_types; // Storage for different cache object
                                        // types
  varconf::Config m_cache; // Varconf object storing cache data
};

} /* namespace Sear */

#endif /* SEAR_CACHEMANAGER_H */
