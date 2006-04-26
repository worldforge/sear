// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: CacheObject.cpp,v 1.2 2006-04-26 14:39:00 simon Exp $

#include "CacheObject.h"

namespace Sear {

int CacheObject::getTypeID(const std::string &filename) {
  // Open binary file for reading
  FILE *fp = fopen(filename.c_str(), "rb");

  if (!fp) {
    fprintf(stderr, "Error opening %s for reading.\n", filename.c_str());
    return -1;
  }

  // Read header bytes
  char buf[8];
  size_t num = fread(&buf[0], sizeof(char), 8, fp);
  
  if (num != 8) {
    fprintf(stderr, "Error reading header (%s).\n", filename.c_str());
    fclose(fp);
    return -1;
  }

  // Check header is as expected 
  if (!strncmp(buf, "CACHEOBJ", 8)) {
    fprintf(stderr, "File is not a cache object (%s).\n", filename.c_str());
    fclose(fp);
    return -1;
  }

  // Read ID for Cache Object type
  int id;
  num = fread(&id, sizeof(int), 1, fp);
  if (num != 1) {
    fprintf(stderr, "Error reading ID.\n");
    fclose(fp);
    return -1;
  }
  
  fclose(fp);
  return id;
}
 
} /* namespace Sear */
