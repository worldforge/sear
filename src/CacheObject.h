// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: CacheObject.h,v 1.1 2005-01-09 16:09:44 simon Exp $

#ifndef SEAR_CACHEOBJECT_H
#define SEAR_CACHEOBJECT_H 1

#include <string>

namespace Sear {

class CacheObject {
public:
  CacheObject() {}
  virtual ~CacheObject() {}

  /**
   * Returns a unique ID for each subclass of CacheObject. used to determine 
   * to load a cached object
   * @return Cache Object type id
   */
  virtual int getType() = 0;

  /**
  * Create a new instance of this object.
  * @return New instance of tis cache object type
  */
  virtual CacheObject *newInstance() = 0;

  /**
   * Load a cache object from the specified filename
   * @param filename The filename of the cache object
   * @return 0 on success, 1 on failure
   */
  virtual int load(const std::string &filename) = 0;

  /**
   * Save a cache objectto the specified filename
   * @param filename The filename of the cache object
   * @return 0 on success, 1 on failure
   */
  virtual int save(const std::string &filename) = 0;

  /**
   * Check file type and return cache object type id
   * @param filename Filename of cache object
   * @return Cache object type id, or -1 on error
   */
  static int getTypeID(const std::string &filename);
};

} /* namespace Sear */

#endif /* SEAR_CACHEOBJECT_H */
