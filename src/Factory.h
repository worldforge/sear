// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _FACTORY_H_ 
#define _FACTORY_H_ 1

#include <Eris/Factory.h>
#include <Eris/World.h>

#include "WorldEntity.h"

namespace Sear {

class Factory : public Eris::Factory {
public:
  virtual bool accept(const Atlas::Objects::Entity::GameEntity &, Eris::World *) { return true; }
  virtual Eris::EntityPtr instantiate(const Atlas::Objects::Entity::GameEntity & ge, Eris::World *world) { return new WorldEntity(ge, world); }

};
} /* namespace Sear */
#endif /* _FACTORY_H_ */
