// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: CacheManager.cpp,v 1.4 2005-03-15 17:55:05 simon Exp $

#include <stdlib.h>
#include <cassert>

#ifdef __WIN32__
  #include <direct.h>
#endif

#include "src/System.h"
#include "src/FileHandler.h"

#include "CacheManager.h"
#include "CacheObject.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

static const std::string KEY_CACHE = "cache";
static const std::string CACHE_PATH = "/cache/";
static const std::string CACHE_FILE = "cache.vconf";

CacheManager CacheManager::m_instance;

int CacheManager::init() {
  assert(m_initialised == false);

  FileHandler *fh = System::instance()->getFileHandler();

  assert(fh != NULL);

  // Cache path name
  std::string c_path = fh->getUserDataPath() + CACHE_PATH;

  // Check if path exists
  if (!fh->exists(c_path)) {
    printf("Creating Cache Directory.\n");
    fh->mkdir(c_path);
  }

  // Load cache data file
  if (fh->exists(c_path + CACHE_FILE)) {
    if (!m_cache.readFromFile(c_path + CACHE_FILE)) {
      fprintf(stderr, "Error reading cache config file.\n");
    }
  }

  m_initialised = true;

  return 0;
}

int CacheManager::shutdown() {
  assert(m_initialised == true);

  std::string c_path = System::instance()->getFileHandler()->getUserDataPath()
                       + CACHE_PATH;
  // Save cache data file
  m_cache.writeToFile(c_path + CACHE_FILE);

  // Clean up types list
  while (!m_types.empty()) {
    assert( m_types.begin()->second != NULL);
    delete m_types.begin()->second;
    m_types.erase(m_types.begin());
  }

  m_initialised = false;

  return 0;
}

/** 
 * Check to see if filename is in cache
 */ 
bool CacheManager::inCache(const std::string &filename) {
  assert(m_initialised == true);

  return m_cache.findItem(KEY_CACHE, filename);
}

/**
 * Load object from cache
 */
CacheObject* CacheManager::loadObject(const std::string &filename) {
  assert(m_initialised == true);
  // Check to see if object is in cache
  if (!inCache(filename)) {
    fprintf(stderr, "Object not in cache (%s).\n", filename.c_str());
    return NULL;
  }
  // Get filename of cached object
  std::string cache_name = m_cache.getItem(KEY_CACHE, filename);

  // Read header info
  int type = CacheObject::getTypeID(filename);

  // Check loader exists
  if (m_types.find(type) == m_types.end()) {
    fprintf(stderr, "Unknown cache object type: %d\n", type);
    return NULL;
  }

  // Create Cache Object of correct type
  CacheObject *obj = m_types[type]->newInstance();

  assert(obj != NULL);

  // Load object if possible
  if (obj->load(cache_name)) {
    fprintf(stderr, "Error loading cache object: %s\n", cache_name.c_str());
    delete obj;
    obj = NULL;
  }

  return obj;
}

/**
 * Add object to cache
 */ 
int CacheManager::add(CacheObject *obj, const std::string &filename) {
  assert(m_initialised == true);
  assert(System::instance()->getFileHandler() != NULL);

  std::string c_path = System::instance()->getFileHandler()->getUserDataPath()
                       + CACHE_PATH;

  // Check if object is already in cache
  if (inCache(filename)) {
    fprintf(stderr, "Object already cached.\n");
    return 1;
  }

  // Get new cache name
  std::string name = c_path + "objectXXXXXX";

  // Convert to character array for mktemp function
  char cache_name[name.size()];
  memcpy(cache_name, name.c_str(), name.size());

  // Generate unique unused filename
  if (!mktemp(cache_name)) {
    fprintf(stderr, "Error generating cache name.\n");
    return 1;
  }

  // Save object
  if (obj->save(cache_name)) {
    fprintf(stderr, "Error saving cache object.\n");
    return 1;
  }

  // Add record to cache
  m_cache.setItem(KEY_CACHE, filename, cache_name);

  return 0;
}

int CacheManager::addType(CacheObject *obj) {
  assert(m_initialised == true);
  assert(obj != NULL);
  // Add to types if it does not exist
  // Creates new instance
  // Error otherwise
  if (m_types.find(obj->getType()) == m_types.end()) {
    m_types[obj->getType()] = obj->newInstance();
  } else {
    fprintf(stderr, "CacheObject of type %d already exists.\n", obj->getType());
    return 1;
  }
  return 0;
}

} /* namespace Sear */

