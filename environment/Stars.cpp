#include <sage/GL.h>
#include <wfmath/vector.h>
#include <wfmath/MersenneTwister.h>

#include "Stars.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

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

    
    glEnableClientState(GL_COLOR_ARRAY);
    
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, m_colors);
    glVertexPointer(3, GL_FLOAT, 0, m_locations);

    glDrawArrays(GL_POINTS, 0, 1000);

    glDisableClientState(GL_COLOR_ARRAY);
    
    glDepthMask(GL_TRUE);
 
}

} // of namespace Sear
