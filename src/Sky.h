// This file may be redistributed and modified only under the terms of
// // the GNU General Public License (See COPYING for details).
// // Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton
 
// $Id: Sky.h,v 1.3 2002-09-08 00:24:53 simon Exp $

#ifndef SEAR_SKY_H
#define SEAR_SKY_H 1

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
#endif /* SEAR_SKY_H */
