// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common/Log.h"

#include "ObjectLoader.h"

namespace Sear {

void ObjectLoader::init() {
  Log::writeLog("Object Loader: Initialising.", Log::LOG_DEFAULT);
  _object_properties = std::map<std::string, ObjectProperties*>();
  ObjectProperties *op = (ObjectProperties*)malloc(sizeof(ObjectProperties));
  MaterialProperties *mp = (MaterialProperties *)malloc(sizeof(MaterialProperties));
  memset(op, 0, sizeof(ObjectProperties));
  memset(mp, 0, sizeof(MaterialProperties));
  strcat(op->object_type, "default\0");
  strcat(op->model_type, "wire_frame\0");
  op->use_lighting = false;
  op->wrap_texture = false;
  op->draw_self = true;
  op->draw_members = true;
  strcat(op->state, "default\0");
  strcat(op->select_state, "select\0");
  op->detail = 1.0f;
  op->width = 1.0f;
  op->height = 1.0f;
  op->num_planes = 1;
  op->texture_scale = 1.0f;
  op->scale = 1.0f;
  op->material_properties = mp;
  op->lighting_properties = NULL;
  mp->ambient[0] = 1.0f; 
  mp->ambient[1] = 1.0f; 
  mp->ambient[2] = 1.0f; 
  mp->ambient[3] = 1.0f; 
  mp->diffuse[0] = 1.0f;
  mp->diffuse[1] = 1.0f;
  mp->diffuse[2] = 1.0f;
  mp->diffuse[3] = 1.0f;
  mp->specular[0] = 0.0f;
  mp->specular[1] = 0.0f;
  mp->specular[2] = 0.0f;
  mp->specular[3] = 0.0f;
  mp->shininess = (float)50.0f;
  mp->emission[0] = 0.0f;
  mp->emission[1] = 0.0f;
  mp->emission[2] = 0.0f;
  mp->emission[3] = 0.0f;
  _object_properties["default"] = op;
}

void ObjectLoader::shutdown() {
  Log::writeLog("Object Loader: Shutting Down", Log::LOG_DEFAULT);
  while (!_object_properties.empty()) {
    if (_object_properties.begin()->second) {
      if (_object_properties.begin()->second->lighting_properties) free(_object_properties.begin()->second->lighting_properties);
      free(_object_properties.begin()->second->material_properties);
      free(_object_properties.begin()->second);
    }
    _object_properties.erase(_object_properties.begin());
  }
}

void ObjectLoader::readFiles(const std::string &file_name) {
  FILE *object_file = NULL;
  Log::writeLog(std::string("Object Loader: Loading file - ") + file_name, Log::LOG_DEFAULT);
  object_file = fopen(file_name.c_str(),"r");
  if (object_file == NULL) {
    Log::writeLog(std::string("Object Loader: Error opening object file - ") + file_name, Log::LOG_ERROR);
    return;
  }
  
  while(!feof(object_file)) {
    ObjectProperties *op = (ObjectProperties*)malloc(sizeof(ObjectProperties));
    if (readRecord(object_file, op)) {
      printf("Added Object: %s\n", op->object_type);
      if (_object_properties[op->object_type] != NULL) {
      free(_object_properties[op->object_type]->material_properties);
      if (_object_properties[op->object_type]->lighting_properties != NULL) free(_object_properties[op->object_type]->lighting_properties);
      free(_object_properties[op->object_type]);
      }
      _object_properties[op->object_type] = op;
    } else {
      free(op);
    }
  }
  fclose(object_file);
}

int ObjectLoader::readRecord(FILE *object_file, ObjectProperties *op) {
  char *str = NULL, *str_start;
  char string_data[OBJECT_LOADER_MAX_SIZE];
  str = (char*)malloc(OBJECT_LOADER_MAX_SIZE);
  memset(string_data, '\0', OBJECT_LOADER_MAX_SIZE);
  memset(str, '\0', OBJECT_LOADER_MAX_SIZE);
//  op = (ObjectProperties*)malloc(sizeof(ObjectProperties));
  MaterialProperties *mp = (MaterialProperties *)malloc(sizeof(MaterialProperties));
  LightingProperties *lp = (LightingProperties *)malloc(sizeof(LightingProperties));
  memset(op, 0, sizeof(ObjectProperties));
  memset(mp, 0, sizeof(MaterialProperties));
  memset(lp, 0, sizeof(LightingProperties));

  str_start = str;  
  // Get object type name
  while (!feof(object_file)) {
    str = str_start;
    memset(string_data, '\0', OBJECT_LOADER_MAX_SIZE);
    memset(str, '\0', OBJECT_LOADER_MAX_SIZE);
    fgets(str, OBJECT_LOADER_MAX_SIZE, object_file);
    sscanf(str, "%s\n", &string_data[0]);
    std::string tag = std::string(string_data);
    if (tag.empty()) continue;
    if (tag.c_str()[0] == '#') continue;
    if (tag == "end_of_record") {
      op->material_properties = mp;
      if (op->use_lighting) {
        op->lighting_properties = lp;
      }
      else {
        op->lighting_properties = NULL;
	free(lp);
      }
      return 1;
    }
    sscanf(str, "%s = %*s\n", &string_data[0]);
    tag = std::string(string_data);
    if (tag == "object_type") {
      sscanf(str, "%*s = %s\n", &op->object_type[0]);  
    }
    else if (tag == "state") {
      sscanf(str, "%*s = %s\n", &op->state[0]);
    }
    else if (tag == "select_state") {
      sscanf(str, "%*s = %s\n", &op->select_state[0]);
    }
    else if (tag == "model_type") {
      sscanf(str, "%*s = %s\n", &op->model_type[0]);
    }
    else if (tag == "use_lighting") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      op->use_lighting = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "wrap_texture") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      op->wrap_texture = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "draw_self") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      op->draw_self = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "draw_members") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      op->draw_members = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "model_by_type") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      op->model_by_type = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "multi_textures") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      op->multi_textures = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "outline") {
      sscanf(str, "%*s = %s\n", &string_data[0]);  
      op->outline = (std::string(string_data) == "true") ? (true) : (false);
    }
    else if (tag == "width") {
      sscanf(str, "%*s = %f", &op->width);
    }
    else if (tag == "height") {
      sscanf(str, "%*s = %f", &op->height);
    }
    else if (tag == "num_planes") {
      sscanf(str, "%*s = %d", &op->num_planes);
    }
    else if (tag == "num_slicings") {
      sscanf(str, "%*s = %d", &op->num_slicings);
    }
    else if (tag == "slices_per_slicing") {
      sscanf(str, "%*s = %d", &op->slices_per_slicing);
    }
    else if (tag == "texture_scale") {
      sscanf(str, "%*s = %f", &op->texture_scale);
    }
    else if (tag == "scale") {
      sscanf(str, "%*s = %f", &op->scale);
    }
    else if (tag == "detail") {
      sscanf(str, "%*s = %f", &op->detail);
    }
    else if (tag == "offset_x") {
      sscanf(str, "%*s = %f %f %f", &op->offset[0], &op->offset[1], &op->offset[2]);
    }
    else if (tag == "mat_ambient") {
      sscanf(str, "%*s = %f %f %f %f", &mp->ambient[0], &mp->ambient[1], &mp->ambient[2], &mp->ambient[3]);
    }
    else if (tag == "mat_diffuse") {
      sscanf(str, "%*s = %f %f %f %f", &mp->diffuse[0], &mp->diffuse[1], &mp->diffuse[2], &mp->diffuse[3]);
    }
    else if (tag == "mat_specular") {
      sscanf(str, "%*s = %f %f %f %f", &mp->specular[0], &mp->specular[1], &mp->specular[2], &mp->specular[3]);
    }
    else if (tag == "mat_shininess") {
      sscanf(str, "%*s = %f", &mp->shininess);
    }
    else if (tag == "mat_emission") {
      sscanf(str, "%*s = %f %f %f %f", &mp->emission[0], &mp->emission[1], &mp->emission[2], &mp->emission[3]);
    }
    else if (tag == "light_ambient") {
      sscanf(str, "%*s = %f %f %f %f", &lp->ambient[0], &lp->ambient[1], &lp->ambient[2], &lp->ambient[3]);
    }
    else if (tag == "light_diffuse") {
      sscanf(str, "%*s = %f %f %f %f", &lp->diffuse[0], &lp->diffuse[1], &lp->diffuse[2], &lp->diffuse[3]);
    }
    else if (tag == "light_specular") {
      sscanf(str, "%*s = %f %f %f %f", &lp->specular[0], &lp->specular[1], &lp->specular[2], &lp->specular[3]);
    }
    else if (tag == "light_shininess") {
      sscanf(str, "%*s = %f", &lp->shininess);
    }
    else if (tag == "light_position") {
      sscanf(str, "%*s = %f %f %f %f", &lp->position[0], &lp->position[1], &lp->position[2], &lp->position[3]);
    }
    else if (tag == "light_spot_direction") {
      sscanf(str, "%*s = %f %f %f", &lp->spot_direction[0], &lp->spot_direction[1], &lp->spot_direction[2]);
    }
    else if (tag == "light_spot_exponent") {
      sscanf(str, "%*s = %f", &lp->spot_exponent);
    }
    else if (tag == "light_constant_attenuation") {
      sscanf(str, "%*s = %f", &lp->constant_attenuation);
    }
    else if (tag == "light_linear_attenuation") {
      sscanf(str, "%*s = %f", &lp->linear_attenuation);
    }
    else if (tag == "light_quadratic_attenuation") {
      sscanf(str, "%*s = %f", &lp->quadratic_attenuation);
    }
    else {
      Log::writeLog(std::string("Object Loader: Unknown Tag - ") + tag, Log::LOG_ERROR);
    }
  }
  free(lp);
  free(mp);
  return 0;
}

} /* namespace Sear */
