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
