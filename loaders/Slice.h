// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Slice.h,v 1.6 2003-03-06 23:50:38 simon Exp $

#ifndef SEAR_SLICE_H
#define SEAR_SLICE_H 1

#include <string>
#include <list>

#include "common/types.h"

#include "src/Model.h"
#include "src/Graphics.h"

namespace Sear {

class Render;
	
class Slice : public Model {
public:
  typedef struct {
    Vertex_3 vertex_data[4];
    Normal normal_data[4];
    Texel texture_data[4];
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
