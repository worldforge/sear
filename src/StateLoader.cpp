// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton


#include <stdlib.h>
#include <string.h>

#include "common/Log.h"

#include "StateLoader.h"

namespace Sear {

void StateLoader::init() {
  Log::writeLog("State Loader: Initialising.", Log::LOG_DEFAULT);
  _state_properties = std::map<std::string, StateProperties*>();
  StateProperties *sp = (StateProperties*)malloc(sizeof(StateProperties));
  memset(sp, 0, sizeof(StateProperties));
  strcat(sp->state, "default\0");
  _state_properties["default"] = sp;
}

void StateLoader::shutdown() {
  Log::writeLog("State Loader: Shutting Down", Log::LOG_DEFAULT);
  while (!_state_properties.empty()) {
    if (_state_properties.begin()->second) free(_state_properties.begin()->second);
    _state_properties.erase(_state_properties.begin());
  }
}

void StateLoader::readFiles(const std::string &file_name) {
  FILE *state_file = NULL;
  Log::writeLog(std::string("State Loader: Loading file - ") + file_name, Log::LOG_DEFAULT);
  state_file = fopen(file_name.c_str(),"r");
  if (state_file == NULL) {
    Log::writeLog(std::string("State Loader: Error opening state file - ") + file_name, Log::LOG_ERROR);
    return;
  }
  
  while(!feof(state_file)) {
    StateProperties *sp = readRecord(state_file);
    if (sp) {
      printf("Added State: %s\n", sp->state);
      // If record already existis, remove it
      if (_state_properties[sp->state] != NULL) free(_state_properties[sp->state]);
      _state_properties[sp->state] = sp;
    } else {
      free(sp);
    }
  }
  fclose(state_file);
  state_file = NULL;
}

StateProperties *StateLoader::readRecord(FILE *state_file) {
  char *str = NULL, *str_start;
  char string_data[MAX_STRING_SIZE];
  str = (char*)malloc(MAX_STRING_SIZE);
  memset(string_data, '\0', MAX_STRING_SIZE);
  memset(str, '\0', MAX_STRING_SIZE);
  StateProperties *sp = (StateProperties*)malloc(sizeof(StateProperties));
  memset(sp, 0, sizeof(StateProperties));

  str_start = str;  
  // Get object type name
  while (!feof(state_file)) {
    str = str_start;
    memset(string_data, '\0', MAX_STRING_SIZE);
    memset(str, '\0', MAX_STRING_SIZE);
    fgets(str, MAX_STRING_SIZE, state_file);
    sscanf(str, "%s\n", &string_data[0]);
    std::string tag = std::string(string_data);
    if (tag == "end_of_state") {
      // If no name was found, return NULL;	    
      if (!sp->state) {
        free(sp);
	sp = NULL;
      }
      if (str) free(str);
      return sp;
    }
    sscanf(str, "%s = %*s\n", &string_data[0]);
    tag = std::string(string_data);
    if (tag == "state") {
      sscanf(str, "%*s = %s\n", &sp->state[0]);  
    }
    else if (tag == "alpha_test") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      sp->alpha_test = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "blend") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      sp->blend = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "lighting") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      sp->lighting = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "textures") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      sp->textures = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "colour_material") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      sp->colour_material = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "depth_test") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      sp->depth_test = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "cull_face") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      sp->cull_face = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "cull_face_cw") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      sp->cull_face_cw = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "stencil") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      sp->stencil = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "fog") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      sp->fog = (std::string(string_data) == "true") ? (true) : (false);
    }
    else {
      Log::writeLog(std::string("State Loader: Unknown Tag - ") + tag, Log::LOG_ERROR);
    }
  }
  free (sp);
  return NULL;
}

} /* namespace Sear */
