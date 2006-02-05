// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 - 2006 Simon Goodall

// $Id: Stars.h,v 1.3 2006-02-05 21:09:50 simon Exp $

#ifndef SEAR_STARS_H
#define SEAR_STARS_H

#include "common/types.h"

namespace Sear {

class Stars
{
public:
    Stars();
    ~Stars();
    
    void render();

private:
    Vertex_3* m_locations;
    Color_4* m_colors;
};

} // of namespace Sear

#endif // of SEAR_STARS_H
