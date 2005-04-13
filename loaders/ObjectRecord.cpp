// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

#include "ObjectRecord.h"

#include "ModelSystem.h"
#include "Model.h"
#include "ModelHandler.h"
#include "ModelRecord.h"

namespace Sear
{

ObjectRecord::ObjectRecord() :
  name("object"),
  icon(0),
  draw_self(false),
  draw_members(false),
  bbox(WFMath::AxisBox<3>(WFMath::Point<3>(0.0f, 0.0f, 0.0f), WFMath::Point<3>(1.0f, 1.0f, 1.0f))),
  position(WFMath::Point<3>(0.0f, 0.0f, 0.0f)),
  orient(WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f))
{}

ObjectRecord::~ObjectRecord() {}

void ObjectRecord::action(const std::string &action)
{
for (ModelList::const_iterator I = low_quality.begin(); I != low_quality.end(); ++I) {
  ModelRecord *rec = ModelSystem::getInstance().getModel(NULL, this, *I);
  if (rec && rec->model) rec->model->action(action);
}
for (ModelList::const_iterator I = medium_quality.begin(); I != medium_quality.end(); ++I) {
  ModelRecord *rec = ModelSystem::getInstance().getModel(NULL, this, *I);
  if (rec && rec->model) rec->model->action(action);
}
for (ModelList::const_iterator I = high_quality.begin(); I != high_quality.end(); ++I) {
  ModelRecord *rec = ModelSystem::getInstance().getModel(NULL, this, *I);
  if (rec && rec->model) rec->model->action(action);
}
}

void ObjectRecord::setHeight(float height)
{
std::cout << "Setting Appearance" << std::endl << std::flush;
for (ModelList::const_iterator I = low_quality.begin(); I != low_quality.end(); ++I) {
  ModelRecord *rec = ModelSystem::getInstance().getModel(NULL, this, *I);
  if (rec && rec->model && rec->scaleByHeight) rec->model->setHeight(height);
}
for (ModelList::const_iterator I = medium_quality.begin(); I != medium_quality.end(); ++I) {
  ModelRecord *rec = ModelSystem::getInstance().getModel(NULL, this, *I);
  if (rec && rec->model && rec->scaleByHeight) rec->model->setHeight(height);
}
for (ModelList::const_iterator I = high_quality.begin(); I != high_quality.end(); ++I) {
  ModelRecord *rec = ModelSystem::getInstance().getModel(NULL, this, *I);
  if (rec && rec->model && rec->scaleByHeight) rec->model->setHeight(height);
}
}

void ObjectRecord::setAppearance(Atlas::Message::MapType &map)
{
std::cout << "Setting Appearance" << std::endl << std::flush;
for (ModelList::const_iterator I = low_quality.begin(); I != low_quality.end(); ++I) {
  ModelRecord *rec = ModelSystem::getInstance().getModel(NULL, this, *I);
  if (rec && rec->model) rec->model->setAppearance(map);
}
for (ModelList::const_iterator I = medium_quality.begin(); I != medium_quality.end(); ++I) {
  ModelRecord *rec = ModelSystem::getInstance().getModel(NULL, this, *I);
  if (rec && rec->model) rec->model->setAppearance(map);
}
for (ModelList::const_iterator I = high_quality.begin(); I != high_quality.end(); ++I) {
  ModelRecord *rec = ModelSystem::getInstance().getModel(NULL, this, *I);
  if (rec && rec->model) rec->model->setAppearance(map);
}

}
  
} // of namespace Seae
