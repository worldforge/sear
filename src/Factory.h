// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: Factory.h,v 1.9 2006-05-17 23:15:35 alriddoch Exp $

#ifndef SEAR_FACTORY_H 
#define SEAR_FACTORY_H 1

#include <Eris/Factory.h>
#include <Eris/TypeInfo.h>
#include <Atlas/Objects/ObjectsFwd.h>

namespace Sear {

class Factory : public Eris::Factory {

public:
  Factory(Eris::TypeService &ts) :
    terrainType(ts.getTypeByName("world"))
  {}

  virtual bool accept(const Atlas::Objects::Entity::RootEntity &, Eris::TypeInfo  * type) { return true; }

  virtual Eris::Entity * instantiate(const Atlas::Objects::Entity::RootEntity & ge, Eris::TypeInfo * type, Eris::View *view);

private:
  Eris::TypeInfo *terrainType;
};

} /* namespace Sear */

#endif /* SEAR_FACTORY_H */
