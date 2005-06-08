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

////////////////////////////////////////////////////////////////////////

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
            const Vector3& acc,
            double sz, double dsz)
    {
        assert(!isActive()); // don't try to re-init active particle!
        m_active = true;
        
        m_timeToLive = ttl;
        m_pos = pos;
        m_velocity = vel;
        m_accel = acc;
        
        m_size = sz;
        m_sizeDelta = dsz;
    }
    
    void render() const
    {
        m_system->submit(m_pos, m_size);
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
        
        m_color += m_colorDelta * dt;
        
        m_size += m_sizeDelta * dt;
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
    double m_size, m_sizeDelta;
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
    
// default data for fire - this will gradually all become dynamic data
    /*
    	g_ParticleSystem1.Initialize(300);

	g_ParticleSystem1.m_bRecreateWhenDied = false;
	g_ParticleSystem1.m_fMinDieAge = 0.5f;

	g_ParticleSystem1.SetCreationColor(1.0f,0.0f,0.0f,
									1.0f,0.5f,0.0f);
	g_ParticleSystem1.SetDieColor(1.0f,1.0f,1.0f,
							      1.0f,0.5f,0.0f);

	g_ParticleSystem1.SetAlphaValues(1.0f,1.0f,0.0f,0.0f);

	g_ParticleSystem1.m_bParticlesLeaveSystem = true;
	g_ParticleSystem1.SetSpinSpeed(-0.82*PI,0.82*PI);
	g_ParticleSystem1.LoadTextureFromFile("particle1.tga");
    */
    
    m_origin = Point3(0, 0, 0);
    m_posDeviation = Vector3(0.5, 0, 0.5);
    
    m_minCreatePerSec = m_maxCreatePerSec = 300;
    m_minTTL = 0.5;
    m_maxTTL = 1.5;
    m_basicVel = Vector3(0.0, 1.0, 0.0);
    m_velocityDeviation = Vector3(0.8, 0.8, 0.8);
    m_minInitialVelMag = 0.3;
    m_maxInitialVelMag = 0.2; // max < min : is this intentional?
    
    m_accelVector = Vector3(0.0, 1.0, 0.0);
    m_minAccelMag = 0.3;
    m_maxAccelMag = 0.4;
    
    m_minInitialSize = 0.04;
    m_maxInitialSize = 0.08;
    m_minFinalSize = 0.06;
    m_maxFinalSize = 0.12;
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
    
    // setup submit data
    m_billboardX = Vector3(modelview[0][0], modelview[1][0], modelview[2][0]);
    m_billboardY = Vector3(modelview[0][1], modelview[1][1], modelview[2][1]);
    m_activeCount = 0;
    
    for (unsigned int p=0; p < m_particles.size(); ++p) {
        if (m_particles[p]->isActive()) m_particles[p]->render();
    }
    
    m_render->renderArrays( Graphics::RES_QUADS, 0, m_activeCount, 
        m_vertexBuffer, m_texCoordBuffer, 
        NULL /* no normal data */,
        false /* no multi-texture */);
}

static Vertex_3 vertexFromPoint(const Point3& p)
{
    Vertex_3 v = { p.x(), p.y(), p.z() };
    return v;
}

static Texel makeTexel(double u, double v)
{
    Texel t = { u, v };
    return t;
}

void ParticleSystem::submit(const Point3& pos, double sz)
{
    Vertex_3* vptr = &(m_vertexBuffer[m_activeCount * 4]);
    Texel* texptr = &(m_texCoordBuffer[m_activeCount * 4]);
    
    *vptr++ = vertexFromPoint(pos - (m_billboardX * 0.5 * sz) - (m_billboardY * 0.5 * sz));
    *vptr++ = vertexFromPoint(pos - (m_billboardX * 0.5 * sz) + (m_billboardY * 0.5 * sz));
    *vptr++ = vertexFromPoint(pos + (m_billboardX * 0.5 * sz) + (m_billboardY * 0.5 * sz));
    *vptr++ = vertexFromPoint(pos + (m_billboardX * 0.5 * sz) - (m_billboardY * 0.5 * sz));
    
    *texptr++ = makeTexel(0.0, 0.0);
    *texptr++ = makeTexel(0.0, 1.0);
    *texptr++ = makeTexel(1.0, 1.0);
    *texptr++ = makeTexel(1.0, 0.0);
    
    ++m_activeCount;
}

void ParticleSystem::invalidate()
{
    // invalidate the texture
}

void ParticleSystem::activate(Particle* p)
{            
    double ttl = randomInRange(m_minTTL, m_maxTTL);
    Vector3 acc = m_accelVector * randomInRange(m_minAccelMag, m_maxAccelMag);
        
    p->init(ttl, initialPos(), initialVelocity(), acc, 
        randomInRange(m_minInitialSize, m_maxInitialSize),
        randomInRange(m_minFinalSize, m_maxFinalSize) / ttl
    );
}

Vector3 ParticleSystem::initialVelocity() const
{
    Vector3 vd = memberMult(randomVector(), m_velocityDeviation);
    return (m_basicVel + vd) * randomInRange(m_minInitialVelMag, m_maxInitialVelMag);
}

Point3 ParticleSystem::initialPos() const
{
    return m_origin + memberMult(randomVector(), m_posDeviation);
}

void ParticleSystem::setTextureName(const std::string& nm)
{
    m_texture = RenderSystem::getInstance().requestTexture(nm);
}

} // of namespace Sear
