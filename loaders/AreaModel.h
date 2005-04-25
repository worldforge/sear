// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

#ifndef SEAR_AREAMODEL_H
#define SEAR_AREAMODEL_H

#include "Model.h"

namespace Sear
{

class ObjectRecord;

class AreaModel : public Model
{
public:
    AreaModel(Render*, ObjectRecord* orec);
    void init();    
    virtual ~AreaModel();
    
    virtual int shutdown();
    virtual void invalidate();    
private:
    ObjectRecord* m_object;
};

}

#endif // of SEAR_AREAMODEL_H
