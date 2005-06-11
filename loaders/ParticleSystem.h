// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

#ifndef SEAR_PARTICLE_SYSTEM_H
#define SEAR_PARTICLE_SYSTEM_H

#include "loaders/Model.h"
#include "renderers/RenderSystem.h"

namespace Sear
{

class Particle;

typedef WFMath::Point<3> Point3;
typedef WFMath::Vector<3> Vector3;
class Color_4d;

class DRange
{
public:
    DRange() : min(0.0), max(1.0)
    { }
    
    DRange(double lower, double upper) : 
        min(lower), max(upper)
    { }
    
    double random() const;
    
    double min, max;
};

class ParticleSystem : public Model
{
public:
    ParticleSystem(Render *render);
    virtual ~ParticleSystem();
    
    virtual void init();
    virtual int shutdown();

    virtual void update(float dt);

    virtual void invalidate();

    virtual void render(bool select_mode);

    void setTextureName(const std::string& nm);
private:
    friend class Particle;
    friend class ParticleSystemLoader;
    
    void submit(const Point3& pos, double size, const Color_4d&);

    void activate(Particle*);
    
    Vector3 initialVelocity() const;
    Point3 initialPos() const;
    
    std::vector<Particle*> m_particles;
    TextureID m_texture;
    
    Vertex_3* m_vertexBuffer;
    Texel* m_texCoordBuffer;
    Color_4* m_colorBuffer;
    
    Vector3 m_billboardX, m_billboardY;
    unsigned int m_activeCount;
    
// config data
    DRange m_createPerSec;
    DRange m_ttl;
    Vector3 m_basicVel, m_velocityDeviation;
    DRange m_initialVelMag;
    
    Point3 m_origin;
    Vector3 m_posDeviation;
    Vector3 m_accelVector;
    DRange m_accelMag;
    DRange m_initialSize;
    DRange m_finalSize;
        
    DRange m_initialAlpha, m_finalAlpha;
};

} // of namespace Sear

#endif // of SEAR_PARTICLE_SYSTEM_H
