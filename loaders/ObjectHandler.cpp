// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: ObjectHandler.cpp,v 1.3 2005-04-13 12:16:04 simon Exp $

#include <varconf/Config.h>

#include "common/Log.h"

#include "src/Console.h"
#include "ObjectHandler.h"
#include "ObjectRecord.h"


#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif


#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
  
namespace Sear {
	
static const std::string CMD_LOAD_OBJECT_RECORDS = "load_object_records";

static const std::string KEY_DRAW_SELF = "draw_self";
static const std::string KEY_DRAW_MEMBERS = "draw_members";
static const std::string KEY_LOW_QUALITY = "low_quality";
static const std::string KEY_MEDIUM_QUALITY = "medium_quality";
static const std::string KEY_HIGH_QUALITY = "high_quality";

ObjectHandler::ObjectHandler() :
  m_initialised(false)	
{}

ObjectHandler::~ObjectHandler() {
  assert(m_initialised == false);
}

void ObjectHandler::init() {
  assert(m_initialised == false);
  if (debug) std::cout << "Object Handler: Initialise" << std::endl;

  m_object_records_map = ObjectRecordMap();
  // Create default record
  ObjectRecord *r = new ObjectRecord();
  r->name = "default";
  r->draw_self = true;
  r->draw_members = true;
  r->low_quality.push_back("default");
  m_object_records_map["default"] = r;
  m_initialised = true;
}

void ObjectHandler::shutdown() {
  assert(m_initialised == true);

  if (debug) std::cout << "Object Handler: Shutdown" << std::endl;

  // Clean up object records	
  while (!m_object_records_map.empty()) {
    ObjectRecord *record = m_object_records_map.begin()->second;
    assert(record != NULL);
    delete record;
    m_object_records_map.erase(m_object_records_map.begin());
  }
  m_initialised = false;
}

void ObjectHandler::loadObjectRecords(const std::string &filename) {
  varconf::Config config;
  config.sigsv.connect(SigC::slot(*this, &ObjectHandler::varconf_callback));
  config.sige.connect(SigC::slot(*this, &ObjectHandler::varconf_error_callback));
  config.readFromFile(filename);
}

ObjectRecord *ObjectHandler::getObjectRecord(const std::string &id) {
  return (m_object_records_map.find(id) != m_object_records_map.end()) 
        ? (m_object_records_map[id]) : (NULL);
}

void ObjectHandler::copyObjectRecord(const std::string &id, ObjectRecord *object_record) {
  assert(object_record != NULL);

  ObjectRecord *record = new ObjectRecord();
  record->draw_self = object_record->draw_self;
  record->draw_members = object_record->draw_members;
  // hopefully this will copy the lists. Need to check tho
  record->low_quality = object_record->low_quality;
  record->medium_quality = object_record->medium_quality;
  record->high_quality = object_record->high_quality;

  if (m_object_records_map[id]) { // Clean up existing record
    delete m_object_records_map[id];
  }

  m_object_records_map[id] = record;
}

void ObjectHandler::registerCommands(Console *console) {
  console->registerCommand(CMD_LOAD_OBJECT_RECORDS, this);
}

void ObjectHandler::runCommand(const std::string &command, const std::string &args) {
  if (command == CMD_LOAD_OBJECT_RECORDS) {
    loadObjectRecords(args);
  }
}

void ObjectHandler::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  ObjectRecord *record = m_object_records_map[section];
  // If record does not exist, create it.
  if (!record) {
    record = new ObjectRecord();
    record->name = section;
    record->draw_self = true;
    record->draw_members = true;
    m_object_records_map[section] = record;
    if (debug) {
      std::cout << "Adding ObjectRecord: " << section << std::endl;
    }
  }

  if (key == KEY_DRAW_SELF) {
    record->draw_self = (bool)config.getItem(section, key);
  }
  else if (key == KEY_DRAW_MEMBERS) {
    record->draw_members = (bool)config.getItem(section, key);
  }
  else if (key == KEY_LOW_QUALITY) {
    record->low_quality.push_back((std::string)config.getItem(section, key));
  }
  else if (key == KEY_MEDIUM_QUALITY) { 
    record->medium_quality.push_back((std::string)config.getItem(section, key));
  }
  else if (key == KEY_HIGH_QUALITY) {
    record->high_quality.push_back((std::string)config.getItem(section, key));
  }
}

void ObjectHandler::varconf_error_callback(const char *message) {
  Log::writeLog(message, Log::LOG_ERROR);
}

} /* namespace Sear */
