// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall

// $Id: ObjectHandler.cpp,v 1.1 2005-01-06 12:46:54 simon Exp $
#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

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
	
static const std::string LOAD_OBJECT_RECORDS = "load_object_records";
	
ObjectHandler::ObjectHandler() :
  _initialised(false)	
{
  
}

ObjectHandler::~ObjectHandler() {
  if (_initialised) shutdown();
}

void ObjectHandler::init() {
  if (debug) Log::writeLog("Initialising Object Handler", Log::LOG_DEFAULT);
  if (_initialised) shutdown();
  _object_records = ObjectRecordMap();
  // Create default record
  ObjectRecord *r = new ObjectRecord();
  r->name = "default";
  r->draw_self = true;
  r->draw_members = true;
  r->low_quality.push_back("default");
  _object_records["default"] = r;
  _initialised = true;
}

void ObjectHandler::shutdown() {
  if (debug) Log::writeLog("Shutting down Object Handler", Log::LOG_DEFAULT);
  // Clean up object records	
  while (!_object_records.empty()) {
    ObjectRecord *record = _object_records.begin()->second;
    if (record) delete record;
    _object_records.erase(_object_records.begin());
  }
  _initialised = false;
}

void ObjectHandler::loadObjectRecords(const std::string &file_name) {
  varconf::Config config;
  config.sigsv.connect(SigC::slot(*this, &ObjectHandler::varconf_callback));
  config.sige.connect(SigC::slot(*this, &ObjectHandler::varconf_error_callback));
  config.readFromFile(file_name);
}

ObjectRecord *ObjectHandler::getObjectRecord(const std::string &id) {
  return _object_records[id];	
}

void ObjectHandler::copyObjectRecord(const std::string &id, ObjectRecord *object_record) {
  if (!object_record) return;
  ObjectRecord *record = new ObjectRecord();
  record->draw_self = object_record->draw_self;
  record->draw_members = object_record->draw_members;
  // hopefully this will copy the lists. Need to check tho
  record->low_quality = object_record->low_quality;
  record->medium_quality = object_record->medium_quality;
  record->high_quality = object_record->high_quality;
  if (_object_records[id]) { // Clean up existing record
    delete _object_records[id];
  }
  _object_records[id] = record;
}

void ObjectHandler::registerCommands(Console *console) {
  console->registerCommand(LOAD_OBJECT_RECORDS, this);
}

void ObjectHandler::runCommand(const std::string &command, const std::string &args) {
  if (command == LOAD_OBJECT_RECORDS) {
    loadObjectRecords(args);
  }
}

void ObjectHandler::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  ObjectRecord *record = _object_records[section];
  // If record does not exist, create it.
  if (!record) {
    record = new ObjectRecord();
    record->name = section;
    record->draw_self = true;
    record->draw_members = true;
    _object_records[section] = record;
    if (debug) {
      std::cout << "Adding ObjectRecord: " << section << std::endl;
    }
  }
  if (key == "draw_self") record->draw_self = (bool)config.getItem(section, key);
  else if (key == "draw_members") record->draw_members = (bool)config.getItem(section, key);
  else if (key == "low_quality") {
    record->low_quality.push_back((std::string)config.getItem(section, key));
  }
  else if (key == "medium_quality") record->medium_quality.push_back((std::string)config.getItem(section, key));
  else if (key == "high_quality") record->high_quality.push_back((std::string)config.getItem(section, key));
}

void ObjectHandler::varconf_error_callback(const char *message) {
  Log::writeLog(message, Log::LOG_ERROR);
}

} /* namespace Sear */
