// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: ObjectHandler.cpp,v 1.6 2006-02-07 11:31:03 simon Exp $

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

void ObjectHandler::init() {
  assert(m_initialised == false);
  if (debug) std::cout << "Object Handler: Initialise" << std::endl;

//  m_object_records_map = ObjectRecordMap();
  // Create default record
  SPtr<ObjectRecord> r(new ObjectRecord());
  r->name = "default";
  r->draw_self = true;
  r->draw_members = true;
  r->low_quality.push_back("default");
  m_type_map["default"] = r;
  m_initialised = true;
}

void ObjectHandler::shutdown() {
  assert(m_initialised == true);

  if (debug) std::cout << "Object Handler: Shutdown" << std::endl;

  // Clean up object records	
//  while (!m_id_map.empty()) {
//    SPtr<ObjectRecord> record = m_id_map.begin()->second;
//    assert(record != NULL);
//    delete record;
//    m_type_map.erase(m_id_map.begin());
//  }
  m_id_map.clear();
  // Clean up object records	
//  while (!m_type_map.empty()) {
//    ObjectRecord *record = m_type_map.begin()->second;
//    assert(record != NULL);
//    delete record;
//    m_type_map.erase(m_type_map.begin());
//  }

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
        ? (m_id_map[id]) : SPtr<ObjectRecord>();
}

SPtr<ObjectRecord> ObjectHandler::instantiateRecord(const std::string &type, const std::string &id) {

  if (m_type_map.find(type) == m_type_map.end()) return SPtr<ObjectRecord>();

  SPtr<ObjectRecord> type_record = m_type_map[type];

  SPtr<ObjectRecord> record(new ObjectRecord());
  record->draw_self = type_record->draw_self;
  record->draw_members = type_record->draw_members;
  record->draw_attached = type_record->draw_attached;
  
  // hopefully this will copy the lists. Need to check tho
  record->low_quality = type_record->low_quality;
  record->medium_quality = type_record->medium_quality;
  record->high_quality = type_record->high_quality;

//  if (m_object_records_map[id]) { // Clean up existing record
//    delete m_object_records_map[id];
//  }

  m_id_map[id] = record;
  return record;
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
  SPtr<ObjectRecord> record = m_type_map[section];
  // If record does not exist, create it.
  if (!record) {
    record = SPtr<ObjectRecord>(new ObjectRecord());
    record->name = section;
    record->draw_self = true;
    record->draw_members = true;
    m_type_map[section] = record;
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
  else if (key == KEY_DRAW_ATTACHED) {
    record->draw_attached = (bool)config.getItem(section, key);
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

void ObjectHandler::reset() {
//  ObjectRecordMap::iterator I = m_id_map.begin();
//  while(I != m_id_map.end()) {
//    delete I->second;
//    ++I;
//  }
  m_id_map.clear();
}

} /* namespace Sear */
