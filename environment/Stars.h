
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