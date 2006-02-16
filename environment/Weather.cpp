// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 Simon Goodall

#include <sigc++/bind.h>

#include "src/WorldEntity.h"

#include "Weather.h"

static const bool debug = false;

void changed(const Eris::StringSet &s, Sear::WorldEntity *we) {
if (debug) printf(">>>>Weather Desc: %s\n", we->valueOfAttr("desc").asString().c_str());
if (debug) printf(">>>>Weather Snow: %f\n", we->valueOfAttr("snow").asNum());
if (debug) printf(">>>>Weather Rain: %f\n", we->valueOfAttr("rain").asNum());
  Eris::Entity::AttrMap am = we->getAttributes();
  Eris::Entity::AttrMap::const_iterator I = am.begin();
  while (I != am.end()) {
if (debug)     printf("Weather has attr: %s\n", I->first.c_str());
    ++I;
  }
}
   


namespace Sear {

Weather::Weather() :
  m_initialised(false)
{}

Weather::~Weather() {
  assert(m_initialised == false);
}

int Weather::init() {
  assert(m_initialised == false);
  m_initialised = true;
  return 0;
}

void Weather::shutdown() {
  assert(m_initialised == true);
  m_initialised = false;
}

void Weather::setWeatherEntity(WorldEntity *we) {
  assert(m_initialised == true);
  Eris::Entity::AttrMap am = we->getAttributes();
  Eris::Entity::AttrMap::const_iterator I = am.begin();
  while (I != am.end()) {
if (debug)     printf("Weather has attr: %s\n", I->first.c_str());
    ++I;
  }
we->Changed.connect(sigc::bind(sigc::ptr_fun(changed), we));
}



} // namespace Sear
