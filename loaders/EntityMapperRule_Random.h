// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2008 Simon Goodall

#ifndef SEAR_LOADERS_ENTITYMAPPERRULE_RANDOM_H
#define SEAR_LOADERS_ENTITYMAPPERRULE_RANDOM_H 1

#include <string>
#include "IEntityMapperRule.h"

namespace Sear {

class WorldEntity;

class EntityMapperRule_Random : public IEntityMapperRule {
public:

  EntityMapperRule_Random() {}
  virtual ~EntityMapperRule_Random() {}

  /** Apply rule to choose an entity mapping for the given entity.
   */
  std::string getEntityMapping(const WorldEntity *we, const StringList &options) const;
};

} /* namespace Sear */

#endif /* SEAR_LOADERS_IENTITYMAPPERRULE_RANOM_H */
