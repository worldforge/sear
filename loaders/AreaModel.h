// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

#ifndef SEAR_AREAMODEL_H
#define SEAR_AREAMODEL_H

#include "Model.h"

namespace Mercator
{
    class Area;
}

namespace Sear {

class WorldEntity;
class AreaModelLoader;

class AreaModel : public Model {
public:
  AreaModel(WorldEntity *we, AreaModelLoader *loader);
  /**
   * initialise the model. Result indicates success (true) or failure
  */
  bool init();    
  virtual ~AreaModel();
  
  virtual int shutdown();

  virtual bool isInitialised() const { return m_initialised; }

  virtual void contextCreated();    
  virtual void contextDestroyed(bool check);
  
  int getLayer() const;
private:
  bool m_initialised;
  WorldEntity *m_entity;
  Mercator::Area* m_area;
  AreaModelLoader *m_loader;
};

}

#endif // of SEAR_AREAMODEL_H
