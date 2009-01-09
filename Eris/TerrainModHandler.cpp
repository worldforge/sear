// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2008 Simon Goodall

#include <sigc++/bind.h>
#include "TerrainModHandler.h"

#include <Eris/Entity.h>
#include <Eris/TerrainMod.h>
#include <Eris/View.h>

#include <Mercator/TerrainMod.h>

// Loop through the entity tree triggering the created method.
static void checkEntities(Eris::Entity *e, Eris::TerrainModHandler *tmh) {
  if (e != 0) {
    tmh->onEntityCreated(e);
    unsigned int numContained = e->numContained();
    for (unsigned int n = 0; n < numContained; ++n) {
      Eris::Entity *ee = e->getContained(n);
      checkEntities(ee, tmh);
    }
  }
}

namespace Eris {

bool TerrainModHandler::init() {

  assert(m_initialised == false);

  m_initialised = true;

  return true;
}

void TerrainModHandler::shutdown() {

  assert(m_initialised == true);

  // Disconnect observers
  while (!m_slots.empty()) {
    std::map<Entity*, Entity::AttrChangedSlot>::iterator I = m_slots.begin();
    I->second.disconnect();
    m_slots.erase(I);
  }

  while (!m_modMap.empty()) {
    TerrainModMap::iterator I = m_modMap.begin();
    delete I->second;
    m_modMap.erase(I);
  }

  m_initialised = false;
}

void TerrainModHandler::setView(View *view) {
  // TODO: Entity Created does not appear to be fired
  // Perhaps setView is called too late in the start up process
  // Use of appearance and disappearance should also work, but lead to an
  // increased number of add/remove calls

  // By the time this method is called, several entities have already been seen.
  // We need to run through the list and hook up these entities.
  Entity *e = view->getTopLevel();
  checkEntities(e, this);

  // view->EntityCreated.connect(sigc::mem_fun(this, &TerrainModHandler::onEntityCreated));
  //view->EntityDeleted.connect(sigc::mem_fun(this, &TerrainModHandler::onEntityDeleted));

  view->Appearance.connect(sigc::mem_fun(this, &TerrainModHandler::onEntityCreated));
  view->Disappearance.connect(sigc::mem_fun(this, &TerrainModHandler::onEntityDeleted));
}

void TerrainModHandler::onEntityCreated(Entity *e) {
 
  TerrainMod *tm = createTerrainMod(e);
  if (tm != 0) {
    onEventModChanged(tm);
  }
/*
  if (m_slots.find(e) == m_slots.end()) {
    // TODO: Bind entity to the method
    Entity::AttrChangedSlot slot = sigc::bind(sigc::mem_fun(*this, &TerrainModHandler::onAttrChanged), e);
    e->observe("terrainmod", slot);
    m_slots[e] = slot;
    if (e->hasAttr("terrainmod")) {
      onAttrChanged(e->valueOfAttr("terrainmod"), e);
    }
  }
  */
}

void TerrainModHandler::onEntityDeleted(Entity *e) {
  // Clean up observers
  std::map<Entity*, Entity::AttrChangedSlot>::iterator J = m_slots.find(e);
  if (J != m_slots.end()) {
    J->second.disconnect();
    m_slots.erase(J);

    // clean up any terrain mod
    TerrainModMap::iterator I = m_modMap.find(e->getId());
    if (I != m_modMap.end()) {
      delete I->second;
      m_modMap.erase(I);
    }
  }
}

void TerrainModHandler::onAttrChanged(const Atlas::Message::Element& attributeValue, Entity *e) {
  // Delete existing TM if required
  // TODO

  // Create TM if required
  TerrainMod *tm = createTerrainMod(e);

  // Fire of changed signal
  // --- TM should do this?
 // if (tm != 0) {
  //  onEventModChanged(tm);
 // }
}

TerrainMod *TerrainModHandler::createTerrainMod(Entity *e) {
  TerrainModMap::iterator I = m_modMap.find(e->getId());
  if (I != m_modMap.end()) {
    // Already added...
    return I->second;
  }

  // Else create a new mod
  TerrainMod *tm = new TerrainMod(e);

  if (tm->init(true)) {
    // Hook up changed signals
    tm->ModChanged.connect(sigc::bind(sigc::mem_fun(*this, &TerrainModHandler::onEventModChanged), tm));
    tm->ModDeleted.connect(sigc::bind(sigc::mem_fun(*this, &TerrainModHandler::onEventModChanged), tm));

    // Store a mapping.
    m_modMap[e->getId()] = tm;

    return tm;
  } else {
    return 0;
  }
}

void TerrainModHandler::onEventModChanged(TerrainMod *tm) {

  // Perhaps we need an extra EventModAdded signal?1
//  TerrainModAdded.emit(tm->getEntity(), tm->getMod());
  TerrainModChanged.emit(tm->getEntity(), tm->getMod());
}

void TerrainModHandler::onEventModDeleted(TerrainMod *tm) {

  TerrainModDeleted.emit(tm->getEntity(), tm->getMod());
} 

} // namespace Eris
