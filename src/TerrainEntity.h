// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

// $Id: TerrainEntity.h,v 1.2 2006-04-26 14:39:00 simon Exp $

#ifndef SEAR_TERRAINENTITY_H
#define SEAR_TERRAINENTITY_H 1

//#include <string>
#include <Eris/Entity.h>
//#include <Eris/Types.h>
//#include "common/types.h"
#include "WorldEntity.h"

namespace Sear {

class TerrainEntity : public WorldEntity {
public:
  TerrainEntity(const std::string &id, Eris::TypeInfo *ty, Eris::View *view);
  ~TerrainEntity() {}
  
  void changed(const Eris::StringSet &s);
  void updateTerrain();

};

} /* namespace Sear */

#endif /* SEAR_TERRAINENTITY_H */
