// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef _3DS_H_
#define _3DS_H_ 1

#include <stdlib.h>
#include <string>

#include "MultiModels.h"

namespace Sear {

class Models;
class Model_3dsMesh;

class Model_3ds : public MultiModels {
public:
  Model_3ds();
  ~Model_3ds();
  
  bool init();
  void shutdown();
  bool loadModel(const std::string &file_name);

void render();  
  const unsigned int getNumModels() { return _num_models; }
  Models *getModel(unsigned int);

protected:
  unsigned int _num_models;
  Model_3dsMesh **_models;
};

} /* namespace Sear */

#endif /* _3DS_H_ */
