// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

// $Id: NullModel.h,v 1.3 2006-02-05 21:09:50 simon Exp $

#ifndef SEAR_NULL_MODEL
#define SEAR_NULL_MODEL

#include "loaders/Model.h"

namespace Sear {

class NullModel : public Model {
public:
    NullModel(Render* r) : Model(r)
    {
    }
    
    virtual ~NullModel()
    {
    }
    
    virtual int shutdown()
    {
        return 0;
    }
    
    virtual void contextCreated() {}
    virtual void contextDestroyed(bool check) {}
};

}

#endif // of SEAR_NULL_MODEL
