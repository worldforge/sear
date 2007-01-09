// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: ObjectRecord.h,v 1.13 2007-01-09 17:11:25 simon Exp $

#ifndef SEAR_LOADERS_OBJECTRECORD_H
#define SEAR_LOADERS_OBJECTRECORD_H 1

#include <string>
#include <list>
#include <map>
#include <Eris/EntityRef.h>
#include <wfmath/axisbox.h>
#include <wfmath/point.h>
#include <wfmath/quaternion.h>

namespace Atlas
{
namespace Message
{
class Element;
typedef std::map<std::string, Element> MapType;
}
}

namespace Sear {

class WorldEntity;

class ObjectRecord {
public:
  typedef std::list<std::string> ModelList;
  
  ObjectRecord();    
  ~ObjectRecord();

  void action(const std::string &action); 
  void animate(const std::string &action); 
  void setAppearance(const Atlas::Message::MapType &map);
 
  void clearOutfit();
  void entityWorn(const std::string &where, WorldEntity *we);
  void entityWorn(WorldEntity *we);
  void entityRemoved(WorldEntity *we);
 
  std::string name;
  std::string id;
  std::string type;

  ModelList low_quality;
  ModelList medium_quality;
  ModelList high_quality;

  bool draw_self;
  bool draw_members;
  bool draw_attached;
  WFMath::AxisBox<3> bbox;
  WFMath::Point<3> position;
  WFMath::Quaternion orient;

  Eris::EntityRef entity;
};

} /* namespace Sear */

#endif /* SEAR_OBJECTRECORD_H */
