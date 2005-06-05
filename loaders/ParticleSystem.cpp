// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

#include "loaders/ParticleSystem.h"
#include "common/types.h"
#include "renderers/Render.h"
#include <wfmath/MersenneTwister.h>
#include <iostream>

namespace Sear
{

WFMath::MTRand twister;

Vector3 randomVector()
{
    return Vector3(twister.rand(2.0) - 1.0,
        twister.rand(2.0) - 1.0,
        twister.rand(2.0) - 1.0);
}

double randomInRange(double min, double max)
{
    return min + twister.rand(max - min);
}

Vector3 memberMult(const Vector3& a, const Vector3& b)
{
    return Vector3(a.x() * b.x(), a.y() * b.y(), a.z() * b.z());
}

//////////////////////////////////////////////////////////////////////////

class Color_4d
{
public:
  Color_4d() :
    r(1.0), g(1.0), b(1.0), a(1.0)
  {}


  Color_4d(double rx, double gx, double bx, double ax) :
    r(rx), g(gx), b(bx), a(ax)
  {}
  
  double r;
  double g;
  double b;
  double a;
  
  Color_4d& operator+=(const Color_4d& x)
  {
    r += x.r;
    g += x.g;
    b += x.b;
    a += x.a;
    return *this;
  }
};

const Color_4d operator*(const Color_4d& c, const double scalar)
{
    return Color_4d(c.r * scalar, c.g * scalar, c.b * scalar, c.a * scalar);
}

class Particle
{
public:
    Particle(ParticleSystem* ps) :
        m_active(false),
        m_system(ps)
    {   /* everything else can wait until init() is called */  }
    
    void init(double ttl, 
            const Point3& pos,
            const Vector3& vel,
            const Vector3& acc)
    {
        assert(!isActive()); // don't try to re-init active particle!
        m_active = true;
        
        m_timeToLive = ttl;
        m_pos = pos;
        m_velocity = vel;
        m_accel = acc;
    }
    
    void render(Vertex_3* vertBuffer, Texel* texBuffer) const
    {
        
    }
    
    void update(double dt)
    {
        m_timeToLive -= dt;
        if (m_timeToLive < 0) {
            m_active = false; // deactivate!
            return;
        }
        
        m_pos = m_pos + (m_velocity * dt) + (0.5 * m_accel * dt * dt);
        m_velocity = m_velocity + (m_accel * dt);
        
        m_color = m_colorDelta * dt;
    }
    
    bool isActive() const
    { return m_active; }
private:
    bool m_active;
    ParticleSystem* m_system;
    
    double m_timeToLive;
    Point3 m_pos;
    Vector3 m_velocity;
    Vector3 m_accel;
    Color_4d m_color;
    Color_4d m_colorDelta;
};

//////////////////////////////////////

ParticleSystem::ParticleSystem(Render *render) : 
    Model(render)
{

}

ParticleSystem::~ParticleSystem()
{
    // should be empty if we got shutdown ok.
    assert(m_particles.empty());
}

void ParticleSystem::init()
{
    // arbitrary low starting number; we re-allocate storage if required
    // during update()
    int numParticles = 100;
    
    m_particles.resize(numParticles);
    for (unsigned int p=0; p < m_particles.size(); ++p) {
        m_particles[p] = new Particle(this);
    }
    
    m_vertexBuffer = new Vertex_3[numParticles * 4];
    m_texCoordBuffer = new Texel[numParticles * 4];
}

int ParticleSystem::shutdown()
{
    // get rid of everything
    for (unsigned int p=0; p < m_particles.size(); ++p) delete m_particles[p];

    delete[] m_vertexBuffer;
    delete[] m_texCoordBuffer;

    return 0; // what does this indicate?
}

void ParticleSystem::update(float elapsed)
{
    int numToCreate = 
        lrintf(randomInRange(m_minCreatePerSec, m_maxCreatePerSec) * elapsed);

    for (unsigned int p=0; p < m_particles.size(); ++p) {
        if (m_particles[p]->isActive()) {
            m_particles[p]->update(elapsed);
        } else if (numToCreate > 0) {
            activate(m_particles[p]);
            // randomise the position / color slightly, so it's less obvious
            // when many particles are created at once.
            m_particles[p]->update(twister.rand(elapsed));
        }
    }  
    
    if (numToCreate > 0) {
        // need to re-allocate things a bit ... bigger
        unsigned int firstNew = m_particles.size(),
            newSize = m_particles.size() + numToCreate;
        m_particles.resize(newSize);
        
        std::cout << "re-allocating particle storage with size=" << 
            newSize << std::endl;
    // re-alloc storage
        delete[] m_vertexBuffer;
        delete[] m_texCoordBuffer;
        m_vertexBuffer = new Vertex_3[newSize * 4];
        m_texCoordBuffer = new Texel[newSize * 4];
        
        for (unsigned int p=firstNew; p < m_particles.size(); ++p) {
            m_particles[p] = new Particle(this);
            activate(m_particles[p]);
            m_particles[p]->update(twister.rand(elapsed));
        }
    } // of spilled particles case
}

void ParticleSystem::render(bool select_mode)
{
    if (select_mode) return; // particles can't be selected, for now
    
    // figure out the up and left vectors for the billboard, based on the
    // modelview matrix. The following code was 'borrowed' from a snippet
    // on the web; apologies for it's obscurity.
    
    float modelview[4][4];
    m_render->getModelviewMatrix(modelview);
    
    Vector3 billboardX(modelview[0][0], modelview[1][0], modelview[2][0]),
        billboardY(modelview[0][1], modelview[1][1], modelview[2][1]);

    Vertex_3* vptr = m_vertexBuffer;
    Texel* texptr = m_texCoordBuffer;
    int activeCount = 0;
    
    for (unsigned int p=0; p < m_particles.size(); ++p) {
        if (m_particles[p]->isActive()) {
            m_particles[p]->render(vptr, texptr);
            vptr += 4;
            texptr += 4;
            ++activeCount;
        }
    }
    
    m_render->renderArrays( Graphics::RES_QUADS, 0, activeCount, 
        m_vertexBuffer, m_texCoordBuffer, 
        NULL /* no normal data */,
        false /* no multi-texture */);
}

void ParticleSystem::invalidate()
{
    // invalidate the texture
}

void ParticleSystem::activate(Particle* p)
{            
    double ttl = randomInRange(m_minTTL, m_maxTTL);
    Vector3 acc = m_accelVector * randomInRange(m_minAccelMag, m_maxAccelMag);
    
    p->init(ttl, initialPos(), initialVelocity(), acc);
}

Vector3 ParticleSystem::initialVelocity() const
{
    return m_basicVel + memberMult(randomVector(), m_velocityDeviation);
}

Point3 ParticleSystem::initialPos() const
{
    return m_origin + memberMult(randomVector(), m_posDeviation);
}

} // of namespace Sear
