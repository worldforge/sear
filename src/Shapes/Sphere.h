// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef _SPHERE_H_
#define _SPHERE_H_ 1

#include "Shape.h"

namespace Sear {

class Sphere : public Shape {
public:
  Sphere();
  ~Sphere();

  bool init(const unsigned int h_res, const unsigned int v_res);
  void shutdown();
  
  const unsigned int getNumLayers() const { return _num_layers; }
  const Model *getLayer(const unsigned int layer) const { return _layers[layer]; }
  
protected:
  unsigned int _num_layers;
  Models *_layers;
};

}


#endif /* _SPHERE_H_ */
