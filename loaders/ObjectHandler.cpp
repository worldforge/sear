// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall

// $Id: ObjectHandler.cpp,v 1.11 2007-03-29 20:11:51 simon Exp $

#include <sigc++/object_slot.h>

#include <varconf/Config.h>

#include "common/Log.h"

#include "src/Console.h"
#include "src/FileHandler.h"
#include "src/System.h"
#include "ObjectHandler.h"
#include "ObjectRecord.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
  
namespace Sear {
	
static const std::string CMD_LOAD_OBJECT_RECORDS = "load_object_records";
static const std::string CMD_reload_config_objects = "reload_config_objects";

static const std::string KEY_DRAW_SELF = "draw_self";
static const std::string KEY_DRAW_MEMBERS = "draw_members";
static const std::string KEY_DRAW_ATTACHED = "draw_attached";
static const std::string KEY_LOW_QUALITY = "low_quality";
static const std::string KEY_MEDIUM_QUALITY = "medium_quality";
static const std::string KEY_HIGH_QUALITY = "high_quality";

ObjectHandler::ObjectHandler() :
  m_initialised(false)	
{}

ObjectHandler::~ObjectHandler() {
  assert(m_initialised == false);
}

int ObjectHandler::init() {
  assert(m_initialised == false);
  if (debug) std::cout << "Object Handler: Initialise" << std::endl;

  // Create default record
  SPtr<ObjectRecord> r(new ObjectRecord());
  r->name = "default";
  r->draw_self = true;
  r->draw_members = true;
  for (int i = 0; i < ObjectRecord::QUEUE_LAST; ++i) {
    r->quality_queue[i].push_back("default");
  }

  m_type_map["default"] = r;

  m_initialised = true;

  return 0;
}

void ObjectHandler::shutdown() {
  assert(m_initialised == true);

  if (debug) printf("Object Handler: Shutdown\n");

  // Clean up object records	
  m_id_map.clear();
  m_type_map.clear();

  m_initialised = false;
}

void ObjectHandler::loadObjectRecords(const std::string &filename) {
  varconf::Config config;
  config.sigsv.connect(SigC::slot(*this, &ObjectHandler::varconf_callback));
  config.sige.connect(SigC::slot(*this, &ObjectHandler::varconf_error_callback));
  config.readFromFile(filename);
}

SPtr<ObjectRecord> ObjectHandler::getObjectRecord(const std::string &id) {
  return (m_id_map.find(id) != m_id_map.end()) 
        ? (m_id_map[id])
        : SPtr<ObjectRecord>();
}

SPtr<ObjectRecord> ObjectHandler::instantiateRecord(const std::string &type, const std::string &id) {
  // See if the type exists
  ObjectRecordMap::const_iterator I = m_type_map.find(type);
  if (I == m_type_map.end()) return SPtr<ObjectRecord>();

  SPtr<ObjectRecord> type_record = I->second;

  // Create new record
  SPtr<ObjectRecord> record(new ObjectRecord());

  // Copy data
  record->draw_self = type_record->draw_self;
  record->draw_members = type_record->draw_members;
  record->draw_attached = type_record->draw_attached;
  
  // Copy the queues too
  for (int i = 0; i < ObjectRecord::QUEUE_LAST; ++i) {
    record->quality_queue[i] = type_record->quality_queue[i];
  }

  m_id_map[id] = record;

  return record;
}

void ObjectHandler::registerCommands(Console *console) {
  assert(console);
  console->registerCommand(CMD_LOAD_OBJECT_RECORDS, this);
  console->registerCommand(CMD_reload_config_objects, this);
}

void ObjectHandler::runCommand(const std::string &command, const std::string &args) {
  if (command == CMD_LOAD_OBJECT_RECORDS) {
    std::string args_cpy = args;
    m_object_configs.push_back(args);
    System::instance()->getFileHandler()->getFilePath(args_cpy);
    loadObjectRecords(args_cpy);
  }
  else if (command == CMD_reload_config_objects) {
    m_id_map.clear();
    m_type_map.clear();

    // Create default record
    SPtr<ObjectRecord> r(new ObjectRecord());
    r->name = "default";
    r->draw_self = true;
    r->draw_members = true;

    for (int i = 0; i < ObjectRecord::QUEUE_LAST; ++i) {
      r->quality_queue[i].push_back("default");
    }

    m_type_map["default"] = r;

    std::list<std::string>::const_iterator I = m_object_configs.begin();
    std::list<std::string>::const_iterator Iend = m_object_configs.end();
    while (I != Iend) {
      std::string args_cpy = *I++;
      System::instance()->getFileHandler()->getFilePath(args_cpy);
      loadObjectRecords(args_cpy);
    }
  }
}

void ObjectHandler::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  SPtr<ObjectRecord> record = m_type_map[section];
  // If record does not exist, create it.
  if (!record) {
    record = SPtr<ObjectRecord>(new ObjectRecord());
    record->name = section;
    record->draw_self = true;
    record->draw_members = true;
    m_type_map[section] = record;
    if (debug) {
      printf("Adding ObjectRecord: %s\n", section.c_str());
    }
  }

  if (key == KEY_DRAW_SELF) {
    record->draw_self = (bool)config.getItem(section, key);
  }
  else if (key == KEY_DRAW_MEMBERS) {
    record->draw_members = (bool)config.getItem(section, key);
  }
  else if (key == KEY_DRAW_ATTACHED) {
    record->draw_attached = (bool)config.getItem(section, key);
  }
  else if (key == KEY_LOW_QUALITY) {
    record->quality_queue[ObjectRecord::QUEUE_low].push_back((std::string)config.getItem(section, key));
  }
  else if (key == KEY_MEDIUM_QUALITY) { 
    record->quality_queue[ObjectRecord::QUEUE_medium].push_back((std::string)config.getItem(section, key));
  }
  else if (key == KEY_HIGH_QUALITY) {
    record->quality_queue[ObjectRecord::QUEUE_high].push_back((std::string)config.getItem(section, key));
  }
}

void ObjectHandler::varconf_error_callback(const char *message) {
  Log::writeLog(message, Log::LOG_ERROR);
}

void ObjectHandler::reset() {
  m_id_map.clear();
}

} /* namespace Sear */
