// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall
 
#ifndef _SKY_H_
#define _SKY_H_ 1
 
namespace Sear {

class Sky {
	
public:
  Sky() {}
  virtual ~Sky() {}
  
  virtual bool init() { return false; }
  virtual void shutdown() {}

  virtual void update(float time_elapsed) {}
  
  virtual float *getVertexData() { return 0; }
  virtual float *getTextureData() { return 0; }
  virtual float *getNormalData() { return 0; }
  
  virtual bool hasVertexData() { return false; }
  virtual bool hasTextureData() { return false; }
  virtual bool hasNormalData() { return false; }
};

}

#endif /* _SKY_H_ */
