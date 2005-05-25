#ifndef SEAR_NULL_MODEL
#define SEAR_NULL_MODEL

#include "loaders/Model.h"

namespace Sear
{

class NullModel : public Model
{
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
    
    virtual void invalidate()
    {
    }
};

}

#endif // of SEAR_NULL_MODEL
