// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _OBJECTLOADER_H_
#define _OBJECTLOADER_H_ 1

#include <stdio.h>
#include <string>
#include <map>


namespace Sear {
  static const unsigned int OBJECT_LOADER_MAX_SIZE = 128;
	
typedef struct {
  float ambient[4];
  float diffuse[4];
  // ambient and diffuse
  float specular[4];
  float shininess;
  float emission[4];
  //colour indecies
} MaterialProperties;

typedef struct {
  float ambient[4];
  float diffuse[4];
  float specular[4];
  float shininess;
  float position[4];
  float spot_direction[3];
  float spot_exponent;
  float spot_cutoff;
  float constant_attenuation;
  float linear_attenuation;
  float quadratic_attenuation;
} LightingProperties;

typedef struct {
  char object_type[OBJECT_LOADER_MAX_SIZE];
  char model_type[OBJECT_LOADER_MAX_SIZE];
  bool use_lighting;
  bool wrap_texture;
  bool draw_self;
  bool draw_members;
  bool outline;
  bool model_by_type;
  char state[OBJECT_LOADER_MAX_SIZE];
  float width;
  float height;
  float texture_scale;
  MaterialProperties *material_properties;
  LightingProperties *lighting_properties;
} ObjectProperties;

class ObjectLoader {
public:
  void init();
  void shutdown();
  void readFiles(const std::string &);
  int readRecord(FILE *, ObjectProperties*);
  ObjectProperties *getObjectProperties(const std::string &object_type) { return _object_properties[object_type]; }

protected:
  std::map<std::string, ObjectProperties*> _object_properties;
};

} /* namespace Sear */
#endif /* _OBJECTLOADER_H_ */
