// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Slice.h,v 1.5 2002-09-26 17:17:46 simon Exp $

#ifndef SEAR_SLICE_H
#define SEAR_SLICE_H 1

#include <string>
#include <list>

#include "src/Model.h"
#include "src/Graphics.h"

namespace Sear {

class Render;
	
class Slice : public Model {
public:
  typedef struct {
    float vertex_data[4][3];
    float normal_data[4][3];
    float texture_data[4][2];
  } ASlice;

  typedef std::list<ASlice*> Slicing;
  
  Slice(Render*);
  ~Slice();
  
  bool init(const std::string &, float width, float height, unsigned int num_slicings, unsigned int slices_per_slicing);
  void shutdown();
  
  void render(bool); 
  bool useTextures() { return  _use_textures; }

  Graphics::RotationStyle rotationStyle() { return Graphics::ROS_POSITION; }
  
protected:
  bool _use_textures;
  std::string _type;
  Slicing **slicings;
  unsigned int _num_slicings;
  unsigned int _slices_per_slicing;
  bool _initialised;
};

} /* namespace Sear */ 

#endif /* SEAR_SLICE_H */
