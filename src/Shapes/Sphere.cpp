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
  return true;
}
void Shpere::shutdown() {
  if (layers) {
    for (int i = 0; i < num_layers; i++) {
      if (layers[i]) {
        layers[i]->shutdown();
	delete layers[i];
      }
    }
    delete [] layers;
  }
}

}
