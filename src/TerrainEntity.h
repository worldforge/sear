// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Simon Goodall, University of Southampton

// $Id: TerrainEntity.h,v 1.1 2005-02-18 17:06:16 simon Exp $

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
