// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: World.h,v 1.1.2.1 2003-01-05 14:22:01 simon Exp $

#ifndef SEAR_WORLD_H
#define SEAR_WORLD_H 1

#include <sigc++/object_slot.h>

namespace Eris {
class Entity;
class World;
}

namespace Sear {

class World : public SigC::Object {
public:
  World();
  ~World();

  void init(Eris::World*);
  void shutdown();

//  void update(float time_elapsed);


  
private:
  bool _initialised;
  Eris::World *_world;

  void NetEntityCreate(Eris::Entity*);
  void NetEntityDelete(Eris::Entity*);
  void NetEntered(Eris::Entity*);
  void NetAppearance(Eris::Entity*);
  void NetDisappearance(Eris::Entity*);
	    

  
};
	
} /* namespace Sear */

#endif /* SEAR_WORLD_H */
