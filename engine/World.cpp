// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: World.cpp,v 1.1.2.2 2003-01-05 18:18:27 simon Exp $

#include "World.h"

#include <Eris/Entity.h>
#include <Eris/World.h>

#include "script/ScriptEngine.h"

#include "Factory.h"

namespace Sear {

static const bool debug = true;
	
World::World() :
  _initialised(false),
  _world(NULL)
{ }

World::~World() {
  if (_initialised) shutdown();
}

void World::init(Eris::World *world) {
  if (_initialised) shutdown();
  _world = world;

  _world->Entered.connect(SigC::slot(*this, &World::NetEntered));
  _world->Appearance.connect(SigC::slot(*this, &World::NetAppearance));
  _world->Disappearance.connect(SigC::slot(*this, &World::NetDisappearance));
  _world->EntityCreate.connect(SigC::slot(*this, &World::NetEntityCreate));
  _world->EntityDelete.connect(SigC::slot(*this, &World::NetEntityDelete));
 
  _world->registerFactory(new Factory());
  
  _initialised = true;
}

void World::shutdown() {
  // Remove signals
  _world = NULL;
  _initialised = false;
}

void World::NetEntityCreate(Eris::Entity *e) {
  if (debug) std::cout << "Created: " << e->getName() << std::endl;
  ScriptEngine::instance()->entityEvent(e->getID(),"onCreate", "");
}

void World::NetEntityDelete(Eris::Entity *e) {
  if (debug) std::cout << "Deleted: " << e->getName() << std::endl;
  ScriptEngine::instance()->entityEvent(e->getID(),"onDelete", "");
}

void World::NetEntered(Eris::Entity *e) {
  if (debug) std::cout << "Entered: " << e->getName() << std::endl;
  ScriptEngine::instance()->entityEvent(e->getID(),"onEntered", "");
}

void World::NetAppearance(Eris::Entity *e) {
  if (debug) std::cout << "Appearance: " << e->getName() << std::endl;
  ScriptEngine::instance()->entityEvent(e->getID(),"onAppearance", "");
}

void World::NetDisappearance(Eris::Entity *e) {
  if (debug) std::cout << "Disappearance: " << e->getName() << std::endl;
  ScriptEngine::instance()->entityEvent(e->getID(),"onDisappearance", "");
}
          
} /* namespace Sear */
