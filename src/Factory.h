// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Factory.h,v 1.5 2004-03-30 11:36:47 simon Exp $

#ifndef SEAR_FACTORY_H 
#define SEAR_FACTORY_H 1

#include <Eris/Factory.h>
#include <Eris/World.h>
#include <Eris/TypeInfo.h>

#include <Atlas/Objects/Entity/GameEntity.h>
                                                                                


#include "WorldEntity.h"

namespace Sear {

class Factory : public Eris::Factory {

public:
                                                                                

  Factory(Eris::TypeService &ts) :
    terrainType(ts.getTypeByName("world"))
  {}

  virtual bool accept(const Atlas::Objects::Entity::GameEntity &, Eris::World *) { return true; }

  virtual Eris::EntityPtr instantiate(const Atlas::Objects::Entity::GameEntity & ge, Eris::World *world);
private:
  Eris::TypeInfo *terrainType;
};

} /* namespace Sear */

#endif /* SEAR_FACTORY_H */
