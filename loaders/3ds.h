// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef _3DS_H_
#define _3DS_H_ 1

#include "../src/Models.h"
/*
 * This class represents a 3D Studio model. It makes use of the lib3ds library
 * to retrieve the model data. It uses the Models and MultiModels interfaces to
 * provide a generic interface for use with the rest of Sear.
 */ 


#include <stdlib.h>
#include <string>


namespace Sear {

// Forward declarations	

class ThreeDS : public Models {
public:
  /*
   * Defualt constructor
   */ 
  ThreeDS();

  /*
   * Destructor
   */ 
  ~ThreeDS();
  
  /*
   * Initialise 3ds model. Should probably be combined with loadModel
   */ 
  bool init(const std::string &file_name);

  /*
   * Called when model is to be removed from memory. It cleans up its children.
   */
  void shutdown();
  void render(bool);

protected:
//  unsigned int _num_models; // Number of models currently held
//  ThreeDSMesh **_models;  // The models
};

} /* namespace Sear */

#endif /* _3DS_H_ */
