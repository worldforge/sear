// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _MODELS_H_
#define _MODELS_H_ 1

#include <stdlib.h>

class Models {
public:
  Models() {}
  virtual ~Models() {}
  
  virtual bool init() { return false; }
  virtual void shutdown() {}
  
  virtual const int getNumPoints() const { return 0; }  
  virtual const float *getVertexData() const { return NULL; }
  virtual const float *getTextureData() const { return NULL; }
  virtual const float *getNormalData() const { return NULL; }

  virtual const bool hasVertexData() const { return false; }
  virtual const bool hasTextureData() const { return false; }
  virtual const bool hasNormalData() const { return false; }

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

  virtual const Type getType() const { return INVALID; }
  
private:
};

#endif /* _MODELS_H_ */
