// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "Config.h"
#include <stdio.h>
#include <fstream.h>

#include <unistd.h>
#include "Log.h"


namespace Sear {

void Config::shutdown() {
  // Clear map data
}

void Config::loadConfig(const std::string &file_name, bool prefix_cwd) {
  FILE *configfile = NULL;
  char *str = NULL;
  Log::writeLog(std::string("Config: Loading file - ") + file_name, Log::LOG_DEFAULT);
  configfile = fopen(file_name.c_str(), READ_ACCESS);
  if (configfile == NULL) {
    Log::writeLog(std::string("Config: Error opening config file - ") + file_name, Log::LOG_ERROR);
    return;
  }
  _last_file_name = file_name;
  while(!feof(configfile)) {
    str = (char*)malloc(MAX_SIZE);
    memset(str, '\0', MAX_SIZE);
    fgets(str, MAX_SIZE, configfile);
    char key[MAX_SIZE], value[MAX_SIZE];
    memset(key, '\0', MAX_SIZE);
    memset(value, '\0', MAX_SIZE);
    sscanf(str, FORMAT,  &key[0], &value[0]);
    if (prefix_cwd) {
      char cwd[256];
      memset(cwd, '\0', 256);
      getcwd(cwd, 255);
      std::string val = std::string(cwd) + "/" + std::string(value);
      _attributes[key] = val;
    }
    else _attributes[key] = value;
    free (str);
  }
  fclose(configfile);
}

void Config::saveConfig() {
  saveConfig(_last_file_name);
}

void Config::saveConfig(const std::string &file_name) {
  FILE *configfile = NULL;
  Log::writeLog(std::string("Config: Saving file - ") + file_name, Log::LOG_DEFAULT);
  configfile = fopen(file_name.c_str(), WRITE_ACCESS);
  if (configfile == NULL) {
    Log::writeLog(std::string("Config: Error opening config file - ") + file_name, Log::LOG_ERROR);
    return;
  }
  for (std::map<std::string, std::string>::const_iterator I = _attributes.begin(); I != _attributes.end(); I++) {
    std::string key = I->first;
    std::string value = I->second;
    if (!key.empty() && !value.empty()) {
      std::string str = key + " = " + value + "\n\0";
      fwrite(str.c_str(), str.size(), 1, configfile);
    }
  }
  fclose(configfile);
}

std::string Config::getAttribute(const std::string &attrib) {
//  if (!_attributes[attrib]) _attributes[attrib] = "";
  return _attributes[attrib];
}

void Config::setAttribute(const std::string &attribname, const std::string &attribvalue) {
  _attributes[attribname] = attribvalue;
}

}
