// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2008 Simon Goodall

#ifndef ERIS_TERRAINMODHANDLER_H
#define ERIS_TERRAINMODHANDLER_H 1

#include <string>
#include <map>
#include <sigc++/signal.h>
#include <Atlas/Message/Element.h>
#include <Eris/Entity.h>


namespace Mercator {
  class TerrainMod;
}

namespace Eris {
  class Entity;
  class View;
  class TerrainMod;

class TerrainModHandler {
public:

  TerrainModHandler() :
    m_initialised(false)
  {}

  virtual ~TerrainModHandler() {}

  virtual bool init();
  virtual void shutdown();

  bool isInitialised() const { return m_initialised; }

  // Register View to listen for entity updates
  void setView(View *view);

  // Output signals
  sigc::signal<void, Entity*, Mercator::TerrainMod*> TerrainModAdded;
  sigc::signal<void, Entity*, Mercator::TerrainMod*> TerrainModDeleted;
  sigc::signal<void, Entity*, Mercator::TerrainMod*> TerrainModChanged;

  // Internal signals to catch entity creation/deletion
  void onEntityCreated(Entity*);
  void onEntityDeleted(Entity*);

private:
  void onAttrChanged(const Atlas::Message::Element& attributeValue, Entity*);

  void onEventModChanged(TerrainMod *);
  void onEventModDeleted(TerrainMod *);

  bool m_initialised;

  TerrainMod *createTerrainMod(Entity *e);

  std::map<Entity*, Entity::AttrChangedSlot> m_slots;
  typedef std::map<std::string, TerrainMod*> TerrainModMap;
  TerrainModMap m_modMap;
};

} // namespace Eris

#endif // ERIS_TERRAINMODHANDLER_H
