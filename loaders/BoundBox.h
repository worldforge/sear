// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: BoundBox.h,v 1.18 2006-12-03 13:38:47 simon Exp $

#ifndef SEAR_BOUNDBOX_H
#define SEAR_BOUNDBOX_H 1

/*
 * This reprents a model based upon its bounding box.
 *
 */ 

#include <wfmath/axisbox.h>

#include "Model.h"

#include "common/types.h"

namespace Sear {

class StaticObject;

class BoundBox : public Model {
public:
  /*
   * Constructor stores the bounding box for the basis of this model.
   * wrap indicates how to produce the texture coords. IE 0->1 or 0->size of face
   */ 	
  BoundBox();

  /*
   * Default destructor
   */ 
  ~BoundBox();
  
  /*
   * Creates the data arrays
   */ 
  int init(WFMath::AxisBox<3>, const std::string &type, bool wrap_texture);

  /*
   * Cleans up object
   */ 
  virtual int shutdown();

  virtual bool isInitialised() const { return m_initialised; }

  virtual void render(bool); 

  virtual void contextCreated();
  virtual void contextDestroyed(bool check);

  virtual bool hasStaticObjects() const { return true; }
  virtual StaticObjectList &getStaticObjects() { return m_render_objects; }

private:
  bool m_initialised;
  StaticObjectList m_render_objects;
};

} /* namespace Sear */
#endif /* SEAR_BOUNDBOX_H */
