// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _MULTIMODELS_H_
#define _MULTIMODELS_H_ 1

#include <stdlib.h>


namespace Sear {

class Models;

class MultiModels {
public:
  MultiModels() {}
  virtual ~MultiModels() {}
  
  virtual bool init() { return false; }
  virtual void shutdown() {}
  
  virtual const unsigned int getNumModels() { return 0; }
  virtual Models *getModel(unsigned int) { return NULL; }

};

} /* namespace Sear */

#endif /* _MULTIMODELS_H_ */
