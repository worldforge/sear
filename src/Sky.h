// This file may be redistributed and modified only under the terms of
// // the GNU General Public License (See COPYING for details).
// // Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton
 
#ifndef _SKY_H_
#define _SKY_H_ 1

namespace Sear {
// Forward Decls

class Sky {
public:
  Sky() {}
  virtual ~Sky() {}
  
  virtual bool init() =0;
  virtual void shutdown() =0;
  
  virtual void draw() = 0;

  virtual void readConfig() {};
  virtual void writeConfig() {};

protected:
};

} /* namespace Sear */
#endif /* _SKY_H_ */
