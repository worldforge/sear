// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#include "Sphere.h"
#include "Models.h"

namespace Sear {

Sphere::Sphere() :
  num_layers(0),
  layers(NULL)
{

}

Sphere::~Sphere() {

}

bool Sphere::init(const unsigned int h_res, const unsigned int v_res) {
  const float radius = 0.5f;


  // Calculate Total num of points
  unsigned int num_points = 0;
  // Allocate memory for models
  _layers = (Models*)malloc(SIZE * size_of(Models));
  
  // Calculate Vertices, Normals and Texture Coords.
  
  
  
  // Create Top fan
  float top_fan_vertex_data[SIZE];
  float top_fan_normal_data[SIZE];
  float top_fan_texture_data[SIZE];

  
  // Create bottom fan
  float top_fan_vertex_data[SIZE];
  float top_fan_normal_data[SIZE];
  float top_fan_texture_data[SIZE];
  

  // Create rings
  for (int i = 0; i < NUM_OF_LAYERS; i++) {
    //Create new layer model
   
  }

  
  return true;
}
void Shpere::shutdown() {
  if (_layers) {
    for (int i = 0; i < _num_layers; i++) {
      if (_layers[i]) {
        _layers[i]->shutdown();
	delete _layers[i];
      }
    }
    delete [] _layers;
  }
}

} /* namespace Sear */
