// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_ 1

/*
 * This class represents the data for a billboard model
 */ 

#include <string>

#include "src/Model.h"
#include "src/Graphics.h"


namespace Sear {

class BillBoard : public Model {
public:
  /*
   * Constructor suppling the desired width and height for the billboard.
   * Should probably move this info to init
   */ 	
  BillBoard();

  /*
   * Default destructor
   */ 
  ~BillBoard();
  
  /*
   * Initilises billboard model. Creates data arrays.
   */ 
  bool init(const std::string &type, float width, float height);

  /*
   * Cleans up billboard
   */ 
  void shutdown();

  void render(bool);

  Graphics::RotationStyle rotationStyle() { return Graphics::ROS_BILLBOARD; }

  bool useTextures() { return _use_textures; }
  
private:
  bool _use_textures;
  static const int _num_points = 4; // Number of points in billboard
  std::string _type;
  float _vertex_data[_num_points][3]; // Vertex array data
  float _texture_data[_num_points][2]; // Texture co-ordintate array data
  float _normal_data[_num_points][3]; // Normals array data
};

} /* namespace Sear */
#endif /* _BILLBOARD_H_ */
