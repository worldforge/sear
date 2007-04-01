// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall

#include "ObjectRecord.h"

#include "ModelSystem.h"
#include "Model.h"
#include "ModelHandler.h"
#include "ModelRecord.h"
#include "src/WorldEntity.h"

namespace Sear {

ObjectRecord::ObjectRecord() :
  name("object"),
  draw_self(false),
  draw_members(false),
  draw_attached(false),
  bbox(WFMath::AxisBox<3>(WFMath::Point<3>(0.0f, 0.0f, 0.0f), WFMath::Point<3>(1.0f, 1.0f, 1.0f))),
  position(WFMath::Point<3>(0.0f, 0.0f, 0.0f)),
  orient(WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f))
{}

ObjectRecord::~ObjectRecord() {}

void ObjectRecord::action(const std::string &action) {
  ModelSystem &ms = ModelSystem::getInstance();
  for (int i = 0; i < QUEUE_LAST; ++i) {
    ModelList::const_iterator I = quality_queue[i].begin();
    ModelList::const_iterator Iend = quality_queue[i].end();
    for (; I != Iend; ++I) {
      SPtr<ModelRecord> rec = ms.getModel(*I, dynamic_cast<WorldEntity*>(entity.get()));
      if (rec && rec->model) rec->model->action(action);
    }
  }
}

void ObjectRecord::animate(const std::string &action) {
  ModelSystem &ms = ModelSystem::getInstance();
  for (int i = 0; i < QUEUE_LAST; ++i) {
    ModelList::const_iterator I = quality_queue[i].begin();
    ModelList::const_iterator Iend = quality_queue[i].end();
    for (; I != Iend; ++I) {
      SPtr<ModelRecord> rec = ms.getModel(*I, dynamic_cast<WorldEntity*>(entity.get()));
      if (rec && rec->model) rec->model->animate(action);
    }
  }
}

void ObjectRecord::clearOutfit() {
  ModelSystem &ms = ModelSystem::getInstance();
  for (int i = 0; i < QUEUE_LAST; ++i) {
    ModelList::const_iterator I = quality_queue[i].begin();
    ModelList::const_iterator Iend = quality_queue[i].end();
    for (; I != Iend; ++I) {
      SPtr<ModelRecord> rec = ms.getModel(*I, dynamic_cast<WorldEntity*>(entity.get()));
      if (rec && rec->model) rec->model->clearOutfit();
    }
  }
}

void ObjectRecord::entityWorn(const std::string &where, WorldEntity *we) {
  ModelSystem &ms = ModelSystem::getInstance();
  for (int i = 0; i < QUEUE_LAST; ++i) {
    ModelList::const_iterator I = quality_queue[i].begin();
    ModelList::const_iterator Iend = quality_queue[i].end();
    for (; I != Iend; ++I) {
      SPtr<ModelRecord> rec = ms.getModel(*I, dynamic_cast<WorldEntity*>(entity.get()));
      if (rec && rec->model) rec->model->entityWorn(where, we);
    }
  }
}

void ObjectRecord::entityWorn(WorldEntity *we) {
    ModelSystem &ms = ModelSystem::getInstance();
  for (int i = 0; i < QUEUE_LAST; ++i) {
    ModelList::const_iterator I = quality_queue[i].begin();
    ModelList::const_iterator Iend = quality_queue[i].end();
    for (; I != Iend; ++I) {
      SPtr<ModelRecord> rec = ms.getModel(*I, dynamic_cast<WorldEntity*>(entity.get()));
      if (rec && rec->model) rec->model->entityWorn(we);
    }
  }
}

void ObjectRecord::entityRemoved(WorldEntity *we) {
  ModelSystem &ms = ModelSystem::getInstance();
  for (int i = 0; i < QUEUE_LAST; ++i) {
    ModelList::const_iterator I = quality_queue[i].begin();
    ModelList::const_iterator Iend = quality_queue[i].end();
    for (; I != Iend; ++I) {
      SPtr<ModelRecord> rec = ms.getModel(*I, dynamic_cast<WorldEntity*>(entity.get()));
      if (rec && rec->model) rec->model->entityRemoved(we);
    }
  }
}
    
} // of namespace Seae
