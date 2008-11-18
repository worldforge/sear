// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2008 Simon Goodall

#include <string.h>
#include <inttypes.h>
 
#include "EntityMapperRule_Random.h"

#include "src/WorldEntity.h"

namespace Sear {

  /** Apply rule to choose an entity mapping for the given entity.
   */
std::string EntityMapperRule_Random::getEntityMapping(const WorldEntity *we, const StringList &options) const {
  // Setup seed based on entity ID.
  const std::string &id = we->getId();

  // To allow aliasing
  union { char c[4]; uint32_t i; } u;

  // Blank array as ID might not fill it up
  memset(u.c, '\0', sizeof(char) * 4);

  // Take last 4 chars of ID if possible as these change more than the
  // first few I.e. entities created sequentially could have the same
  // beginning for their ID.
  unsigned int idx = (id.size() > 4) ? (id.size() - 3) : (0);

  // Copy up to 4 chars of the id field
  strncpy(u.c, &id.c_str()[idx], 4);

  // Seed RND
  srand(u.i);

  // Gen random index
  float r = (float)rand() / (float)RAND_MAX * (float)options.size();

  // Cast to integer
  idx = (unsigned int)(r);

  // Check bounds
  if (idx == options.size()) --idx;

  assert(idx >= 0);
  assert(idx < options.size());

  return options[idx];

}

} /* namespace Sear */
