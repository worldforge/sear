// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "Config.h"
#include <stdio.h>
#include "debug.h"
//#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>

#include <unistd.h>

namespace Sear {

void Config::shutdown() {
  // Clear map data
}

void Config::loadConfig(const std::string &file_name, bool prefix_cwd) {
  FILE *configfile = NULL;
  char *str = NULL;
  std::cout << "Config: Loading file - " << file_name << std::endl;
  configfile = fopen(file_name.c_str(), READ_ACCESS);
  if (configfile == NULL) {
    std::cerr << "Config: Error opening config file - " <<  file_name << std::endl;
    return;
  }
  _last_file_name = file_name;
  while(!feof(configfile)) {
    str = (char*)malloc(MAX_SIZE);
    fgets(str, MAX_SIZE, configfile);
    char key[MAX_SIZE], value[MAX_SIZE];
    sscanf(str, FORMAT,  &key[0], &value[0]);
    if (prefix_cwd) {
      char cwd[257];
      memset(cwd, '\0', 257);
      getcwd(cwd, 256);
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
  std::cout << "Config: Saving file - " << file_name << std::endl;
  configfile = fopen(file_name.c_str(), WRITE_ACCESS);
  if (configfile == NULL) {
    std::cerr << "Config: Error opening config file - " <<  file_name << std::endl;
    return;
  }
  for (std::map<std::string, std::string>::const_iterator I = _attributes.begin(); I != _attributes.end(); I++) {
    if (!(I->first).empty() && !(I->second).empty()) {
      std::string str = (std::string)(I->first) + " = " + (std::string)(I->second) + "\n";
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
