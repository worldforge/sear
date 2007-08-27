// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: Light.h,v 1.3 2007-08-27 16:04:20 simon Exp $

#ifndef SEAR_LIGHT_H
#define SEAR_LIGHT_H 1

#include <wfmath/point.h>

namespace Sear {

class Light {
public:
  // Defines types of light source
  typedef enum {
    POINT = 0,
    SPOT,
    LAST_LIGHT_TYPE
  } LightType;

  typedef enum {
    RED = 0,
    GREEN,
    BLUE,
    ALPHA
  } Channels;
  
  Light() :
    light_type(POINT),
//    ambient({0.0f, 0.0f, 0.0f, 0.0f}),
    attenuation_constant(1.0f),
    attenuation_linear(0.0f),
    attenuation_quadratic(0.0f),
    enabled(false)
  {
    // Init light components to black
    memset(ambient, 0, 4 * sizeof(float));
    memset(diffuse, 0, 4 * sizeof(float));
    memset(specular, 0, 4 * sizeof(float));
  }

  ~Light() {}


  // TODO these need to take into account enabled state
  // Required for priority queue sorting
  bool operator<(const Light &l) const {
    return weight < l.weight;
  }
  
  bool operator>(const Light &l) const {
    return weight > l.weight;
  }
  
  // Position of light source
  WFMath::Point<3> position;
  // Type of light source
  LightType light_type;

  // ambient light component
  float ambient[4];
  // diffuse light component
  float diffuse[4];
  // specular light component
  float specular[4];
  // attenuation values
  float attenuation_constant;
  float attenuation_linear;
  float attenuation_quadratic;

  // TODO is this required - or do we just set the relevant
  // parts to 0?
  // Whether to use various parts of the light source.
  bool use_ambient_component;
  bool use_diffuse_component;
  bool use_specular_component;
  
  // Whether light is in use
  bool enabled;

  // How important it is to use this light source
  float weight;
};
	
} /* namespace Sear */

#endif /* SEAR_LIGHT_H */
