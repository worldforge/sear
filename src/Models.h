// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _MODELS_H_
#define _MODELS_H_ 1

#include <stdlib.h>

namespace Sear {

class Models {
public:
  Models() {}
  virtual ~Models() {}
  
  virtual bool init() { return false; }
  virtual void shutdown() {}
  
  virtual const int getNumPoints() { return 0; }  
  virtual const float *getVertexData() { return NULL; }
  virtual const float *getTextureData() { return NULL; }
  virtual const float *getNormalData() { return NULL; }

  virtual const bool hasVertexData() { return false; }
  virtual const bool hasTextureData() { return false; }
  virtual const bool hasNormalData() { return false; }

  typedef enum {
    INVALID = 0,
    POINT,
    LINES,
    TRIANGLES,
    QUADS,
    TRIANGLE_FAN,
    TRIANGLE_STRIP,
    QUAD_STRIP
  } Type;

  virtual const Type getType() { return INVALID; }
  
};

} /* namespace Sear */

#endif /* _MODELS_H_ */
