// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 - 2006 Simon Goodall

// $Id: Stars.cpp,v 1.8 2007-05-03 11:02:21 simon Exp $


#include <sage/GL.h>
#include <wfmath/vector.h>
#include <wfmath/MersenneTwister.h>

#include "Stars.h"

namespace Sear {

Stars::Stars()
{
  m_locations = new Vertex_3[1000];
  m_colors = new Color_4[1000];
    
  WFMath::MTRand twister;
    
  for (unsigned int S=0; S < 1000; ++S) {
    WFMath::Vector<3> dir(twister.rand(), twister.rand(), twister.rand());
    dir -= WFMath::Vector<3>(0.5, 0.5, 0.5);
    dir = dir.normalize() * 100;
        
    m_locations[S].x = dir.x();
    m_locations[S].y = dir.y();
    m_locations[S].z = dir.z();
        
    m_colors[S].r = twister.randInt(128) + 128;
    m_colors[S].g = m_colors[S].b = m_colors[S].r; // always white for now
    m_colors[S].a = 0xff;
  }

}

Stars::~Stars()
{
    delete[] m_locations;
    delete[] m_colors;

}

void Stars::render()
{
    glDepthMask(GL_FALSE);
    glPointSize(1);

    
    glEnableClientState(GL_COLOR_ARRAY);
    
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, m_colors);
    glVertexPointer(3, GL_FLOAT, 0, m_locations);

    glDrawArrays(GL_POINTS, 0, 1000);

    glDisableClientState(GL_COLOR_ARRAY);
    
    glDepthMask(GL_TRUE);
 
}

} // of namespace Sear
