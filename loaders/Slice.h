// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

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
  
  bool init(const std::string &, float width, float height, Model *trunk_model);
  void shutdown();
  
  void render(bool); 
  bool useTextures() { return  _use_textures; }

  Graphics::RotationStyle rotationStyle() { return Graphics::ROS_POSITION; }
  
private:
  bool _use_textures;
  std::string _type;
  Slicing **slicings;
  Model *_trunk_model;
};

} /* namespace Sear */ 

#endif /* SEAR_SLICE_H */
