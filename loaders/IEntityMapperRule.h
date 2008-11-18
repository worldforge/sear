// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2008 Simon Goodall

#ifndef SEAR_LOADERS_IENTITYMAPPERRULE_H
#define SEAR_LOADERS_IENTITYMAPPERRULE_H 1

#include <string>
#include <vector>

namespace Sear {

class WorldEntity;

class IEntityMapperRule {
public:
  typedef std::vector<std::string> StringList;

  virtual ~IEntityMapperRule() {}

  /** Apply rule to choose an entity mapping for the given entity.
   */
  virtual std::string getEntityMapping(const WorldEntity *we, const StringList &options) const = 0;
};

} /* namespace Sear */

#endif /* SEAR_LOADERS_IENTITYMAPPERRULE_H */
