// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: ObjectRecord.h,v 1.6 2004-06-21 15:56:03 simon Exp $

#ifndef SEAR_OBJECTRECORD_H
#define SEAR_OBJECTRECORD_H 1

#include <string>
#include <list>

#include <wfmath/axisbox.h>
#include <wfmath/point.h>
#include <wfmath/quaternion.h>

#include "src/System.h"
#include "src/Model.h"
#include "src/ModelHandler.h"
#include "src/ModelRecord.h"

namespace Sear {

class ObjectRecord {
public:
  typedef std::list<std::string> ModelList;
  
  ObjectRecord() :
    name("object"),
    low_quality(ModelList()),
    medium_quality(ModelList()),
    high_quality(ModelList()),
    icon(0),
    draw_self(false),
    draw_members(false),
    bbox(WFMath::AxisBox<3>(WFMath::Point<3>(0.0f, 0.0f, 0.0f), WFMath::Point<3>(1.0f, 1.0f, 1.0f))),
    position(WFMath::Point<3>(0.0f, 0.0f, 0.0f)),
    orient(WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f))
  {}
    
  ~ObjectRecord() {}

  void action(const std::string &action) {
    for (ModelList::const_iterator I = low_quality.begin(); I != low_quality.end(); ++I) {
      ModelRecord *rec = System::instance()->getModelHandler()->getModel(NULL, this, *I);
      if (rec && rec->model) rec->model->action(action);
    }
    for (ModelList::const_iterator I = medium_quality.begin(); I != medium_quality.end(); ++I) {
      ModelRecord *rec = System::instance()->getModelHandler()->getModel(NULL, this, *I);
      if (rec && rec->model) rec->model->action(action);
    }
    for (ModelList::const_iterator I = high_quality.begin(); I != high_quality.end(); ++I) {
      ModelRecord *rec = System::instance()->getModelHandler()->getModel(NULL, this, *I);
      if (rec && rec->model) rec->model->action(action);
    }
  }
 
  void setHeight(float height) {
    std::cout << "Setting Appearance" << std::endl << std::flush;
    for (ModelList::const_iterator I = low_quality.begin(); I != low_quality.end(); ++I) {
      ModelRecord *rec = System::instance()->getModelHandler()->getModel(NULL, this, *I);
      if (rec && rec->model && rec->scaleByHeight) rec->model->setHeight(height);
    }
    for (ModelList::const_iterator I = medium_quality.begin(); I != medium_quality.end(); ++I) {
      ModelRecord *rec = System::instance()->getModelHandler()->getModel(NULL, this, *I);
      if (rec && rec->model && rec->scaleByHeight) rec->model->setHeight(height);
    }
    for (ModelList::const_iterator I = high_quality.begin(); I != high_quality.end(); ++I) {
      ModelRecord *rec = System::instance()->getModelHandler()->getModel(NULL, this, *I);
      if (rec && rec->model && rec->scaleByHeight) rec->model->setHeight(height);
    }
  }
  void setAppearance(Atlas::Message::Element::MapType &map) {
    std::cout << "Setting Appearance" << std::endl << std::flush;
    for (ModelList::const_iterator I = low_quality.begin(); I != low_quality.end(); ++I) {
      ModelRecord *rec = System::instance()->getModelHandler()->getModel(NULL, this, *I);
      if (rec && rec->model) rec->model->setAppearance(map);
    }
    for (ModelList::const_iterator I = medium_quality.begin(); I != medium_quality.end(); ++I) {
      ModelRecord *rec = System::instance()->getModelHandler()->getModel(NULL, this, *I);
      if (rec && rec->model) rec->model->setAppearance(map);
    }
    for (ModelList::const_iterator I = high_quality.begin(); I != high_quality.end(); ++I) {
      ModelRecord *rec = System::instance()->getModelHandler()->getModel(NULL, this, *I);
      if (rec && rec->model) rec->model->setAppearance(map);
    }
 

  }
 
  std::string name;
  std::string id;
  std::string type;
  ModelList low_quality;
  ModelList medium_quality;
  ModelList high_quality;
  int icon;
  bool draw_self;
  bool draw_members;
  WFMath::AxisBox<3> bbox;
  WFMath::Point<3> position;
  WFMath::Quaternion orient;
  WorldEntity *entity;
};

} /* namespace Sear */

#endif /* SEAR_OBJECTRECORD_H */
