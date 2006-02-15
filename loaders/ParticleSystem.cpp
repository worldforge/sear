// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall, University of Southampton

/* TODO

 -- we can probably remove the twist to camera code in hhere in favour o f the general twist to camera Rotation
 * We should be able to make good use of the GL_ARB_point_sprite or 
 * GL_ARB_point_paramaters extension here
I think this requires setting glPointParameterfARB functions and rendering a
GL_POINTS array.
For rendering the textuere use gl PointSprite
glEnable(GL_POINT_SPRITE_ARB);
glTexEnv <--to tell GL how to map the texture

 */


#include "loaders/ParticleSystem.h"
#include "common/types.h"
#include "src/WorldEntity.h"

#include <wfmath/MersenneTwister.h>
#include <iostream>
#include <sage/sage.h>
#include <sage/GL.h>

#include "DynamicObject.h"

namespace Sear {

WFMath::MTRand twister;

Vector3 randomVector() {
  return Vector3(twister.rand(2.0) - 1.0,
    twister.rand(2.0) - 1.0,
    twister.rand(2.0) - 1.0);
}

double DRange::random() const {
  return min + twister.rand(max - min); 
}

Vector3 memberMult(const Vector3& a, const Vector3& b) {
  return Vector3(a.x() * b.x(), a.y() * b.y(), a.z() * b.z());
}

//////////////////////////////////////////////////////////////////////////


const Color_4d operator*(const Color_4d& c, const double scalar) {
  return Color_4d(c.r * scalar, c.g * scalar, c.b * scalar, c.a * scalar);
}

const Color_4d operator-(const Color_4d& c, const Color_4d& d) {
  return Color_4d(c.r - d.r, c.g -d.g, c.b - d.b, c.a - d.a);
}

Color_4d interpolate(double frac, const Color_4d& c, const Color_4d& d) {
  assert(frac >= 0.0);
  assert(frac <= 1.0);
  double mf = 1.0 - frac;
    
  return Color_4d(frac * c.r + mf * d.r,
    frac * c.g + mf * d.g,
    frac * c.b + mf * d.b,
    frac * c.a + mf * d.a);
}

////////////////////////////////////////////////////////////////////////

class Particle {
public:
  Particle(ParticleSystem* ps) :
    m_active(false),
    m_system(ps)
  {   /* everything else can wait until init() is called */  }
    
  void init(double ttl, 
        const Point3& pos,
        const Vector3& vel,
        const Vector3& acc,
        double sz, double dsz,
        const Color_4d& col,
        const Color_4d& dc,
        double spinSpeed, double spinAcc)
  {
    assert(!isActive()); // don't try to re-init active particle!
    m_active = true;
        
    m_timeToLive = ttl;
    m_pos = pos;
    m_velocity = vel;
    m_accel = acc;
        
    m_size = sz;
    m_sizeDelta = dsz;
        
    m_color = col;
    m_colorDelta = dc;
        
    m_spinAngle = 0.0;
    m_spinSpeed = spinSpeed;
    m_spinAcc = spinAcc;

  }
    
  void render() const
  {
      m_system->submit(m_pos, m_size, m_color, m_spinAngle);
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
      
    m_spinAngle = m_spinAngle + (m_spinSpeed * dt) + (0.5 * m_spinAcc * dt * dt);
    m_spinSpeed = m_spinSpeed + (m_spinAcc * dt);
        
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
  
  double m_spinSpeed, m_spinAngle, m_spinAcc;
};

//////////////////////////////////////

ParticleSystem::ParticleSystem(WorldEntity *we) : 
  Model(),
  m_initialised(false),
  m_entity(we)
{   
  m_origin = Point3(0, 0, 0);
  m_posDeviation = Vector3(0.5, 0.5, 0.0);
  m_createPerSec = DRange(100, 100);
}

ParticleSystem::~ParticleSystem() {
  assert(m_initialised == false);
  // should be empty if we got shutdown ok.
  assert(m_particles.empty());
}

void ParticleSystem::init()
{
  assert(m_initialised == false);
//sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT] = false;

  m_do = SPtrShutdown<DynamicObject>(new DynamicObject());
  m_do->init();
  m_do->setAmbient(1.0f, 1.0f, 1.0f, 1.0f);
  m_do->setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
  m_do->setSpecular(1.0f, 1.0f, 1.0f, 1.0f);
  m_do->setEmission(1.0f, 1.0f, 1.0f, 1.0f);
  m_do->setShininess(50.0f);
  // arbitrary low starting number; we re-allocate storage if required
  // during update()
  int numParticles = 100;
    
  m_particles.resize(numParticles);
  for (unsigned int p=0; p < m_particles.size(); ++p) {
    m_particles[p] = new Particle(this);
  }
    
  m_vertexBuffer = new Vertex_3[numParticles * 6];
  m_texCoordBuffer = new Texel[numParticles * 6];
  m_colorBuffer = new Color_4[numParticles * 6];
    
// default data for fire - this will gradually all become dynamic data
  m_ttl = DRange(0.5, 1.6);
  
  m_basicVel = Vector3(0.0, 0.0, 1.0);
  m_velocityDeviation = Vector3(0.8, 0.8, 0.8);
  m_initialVelMag = DRange(0.3, 0.2); // max < min : is this intentional?
   
  m_accelVector = Vector3(0.0, 0.0, 1.0);
  m_accelMag = DRange(0.3, 0.4);
  m_initialSize = DRange(0.08, 0.15);
  m_finalSize = DRange(0.04, 0.08);
        
  m_initialAlpha = DRange(1.0, 1.0);
  m_finalAlpha = DRange(0.0, 0.0);
    
  m_emitSpinSpeed = DRange(-0.82 * M_PI, 0.82 * M_PI);
    
  m_initialColors[0] = Color_4d(1.0, 0.0, 0.0, 1.0);
  m_initialColors[1] = Color_4d(1.0, 0.5, 0.0, 1.0);
    
  m_finalColors[0] = Color_4d(1.0, 1.0, 1.0, 1.0);
  m_finalColors[1] = Color_4d(1.0, 0.5, 0.0, 1.0);
  m_initialised = true;
}

int ParticleSystem::shutdown()
{
  assert(m_initialised == true);
  // get rid of everything
  for (unsigned int p=0; p < m_particles.size(); ++p) delete m_particles[p];
  m_particles.clear();
  
  delete[] m_vertexBuffer;
  delete[] m_texCoordBuffer;
  delete[] m_colorBuffer;
  m_initialised = false; 
  return 0; // what does this indicate?
}

void ParticleSystem::update(float elapsed)
{
  double status = m_entity->getStatus();
  if ((status < 0.0) || (status >= 1.0)) {
    std::cout << "invalid status " << status << " for particle system" << std::endl;
    return;
  }
    
  int numToCreate = lrintf(m_createPerSec.random() * elapsed * status * 2.f);

  for (unsigned int p=0; p < m_particles.size(); ++p) {
    if (m_particles[p]->isActive()) {
      m_particles[p]->update(elapsed);
    } else if (numToCreate > 0) {
      --numToCreate;
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
        
    // re-alloc storage
    delete[] m_vertexBuffer;
    delete[] m_texCoordBuffer;
    delete[] m_colorBuffer;
        
    m_vertexBuffer = new Vertex_3[newSize * 6];
    m_texCoordBuffer = new Texel[newSize * 6];
    m_colorBuffer = new Color_4[newSize * 6];
        
    for (unsigned int p=firstNew; p < m_particles.size(); ++p) {
      m_particles[p] = new Particle(this);
      activate(m_particles[p]);
      m_particles[p]->update(twister.rand(elapsed));
    }
  } // of spilled particles case

//  m_do->copyVertexData((float*)m_vertexBuffer, m_particles.size() * 6 * 3);
//  m_do->copyColourData((unsigned char*)m_colorBuffer, m_particles.size() * 6 * 3);
//  m_do->copyTextureData((float*)m_texCoordBuffer, m_particles.size() * 6 * 2);

//  m_do->setNumPoints(m_particles.size() * 6);
}

void ParticleSystem::render(bool select_mode) {
  // figure out the up and left vectors for the billboard, based on the
  // modelview matrix. The following code was 'borrowed' from a snippet
  // on the web; apologies for it's obscurity.
    
  float modelview[4][4];
  Render *render = RenderSystem::getInstance().getRenderer();
  render->getModelviewMatrix(modelview);
    
  // setup submit data
  m_billboardX = Vector3(modelview[0][0], modelview[1][0], modelview[2][0]);
  m_billboardY = Vector3(modelview[0][1], modelview[1][1], modelview[2][1]);
  m_activeCount = 0;
    
  for (unsigned int p=0; p < m_particles.size(); ++p) {
    if (m_particles[p]->isActive()) m_particles[p]->render();
  }

  // Note: This should ideally be done during the update function
  // However, the camera angle is unknown at this time.
  m_do->copyVertexData((float*)m_vertexBuffer, m_activeCount * 6 * 3);
  m_do->copyColourData((unsigned char*)m_colorBuffer, m_activeCount * 6 * 3);
  m_do->copyTextureData((float*)m_texCoordBuffer, m_activeCount * 6 * 2);

//  m_do->setNumPoints(m_particles.size() * 6);
  m_do->setNumPoints(m_activeCount * 6);

  m_do->render(select_mode);
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

void ParticleSystem::submit(const Point3& pos, double sz, const Color_4d& c, double spin)
{
  Vertex_3* vptr = &(m_vertexBuffer[m_activeCount * 6]);
  Texel* texptr = &(m_texCoordBuffer[m_activeCount * 6]);
  Color_4* colorptr = &(m_colorBuffer[m_activeCount * 6]);
    
  Vector3 bx = m_billboardX * cos(spin) + m_billboardY * sin(spin);
  Vector3 by = m_billboardY * cos(spin) - m_billboardX * sin(spin);
    
  *vptr++ = vertexFromPoint(pos - (bx * 0.5 * sz) - (by * 0.5 * sz));
  *vptr++ = vertexFromPoint(pos - (bx * 0.5 * sz) + (by * 0.5 * sz));
  *vptr++ = vertexFromPoint(pos + (bx * 0.5 * sz) + (by * 0.5 * sz));

  *vptr++ = vertexFromPoint(pos + (bx * 0.5 * sz) + (by * 0.5 * sz));
  *vptr++ = vertexFromPoint(pos + (bx * 0.5 * sz) - (by * 0.5 * sz));
  *vptr++ = vertexFromPoint(pos - (bx * 0.5 * sz) - (by * 0.5 * sz));
    
  *texptr++ = makeTexel(0.0, 0.0);
  *texptr++ = makeTexel(0.0, 1.0);
  *texptr++ = makeTexel(1.0, 1.0);

  *texptr++ = makeTexel(1.0, 1.0);
  *texptr++ = makeTexel(1.0, 0.0);
  *texptr++ = makeTexel(0.0, 0.0);
    
  Color_4 uc(c.asColor_4());
  *colorptr++ = uc;
  *colorptr++ = uc;
  *colorptr++ = uc;

  *colorptr++ = uc;
  *colorptr++ = uc;
  *colorptr++ = uc;
    
  ++m_activeCount;
}

void ParticleSystem::contextCreated() {
  m_do->contextCreated();
}
void ParticleSystem::contextDestroyed(bool check) {
  m_do->contextDestroyed(check);
}

void ParticleSystem::activate(Particle* p)
{            
  double ttl = m_ttl.random();
  Vector3 acc = m_accelVector * m_accelMag.random();
        
  Color_4d initialColor = interpolate(twister.rand(), m_initialColors[0], m_initialColors[1]);
  Color_4d finalColor = interpolate(twister.rand(), m_finalColors[0], m_finalColors[1]);
    
  initialColor.a = m_initialAlpha.random();
  finalColor.a = m_finalAlpha.random();
    
  double initialSize = m_initialSize.random();
    
  p->init(ttl, initialPos(), initialVelocity(), acc, 
    initialSize,
    (m_finalSize.random() - initialSize) / ttl,
    initialColor,
    (finalColor - initialColor) * (1.0 / ttl),
    m_emitSpinSpeed.random(), 0.0
  );
}

Vector3 ParticleSystem::initialVelocity() const
{
  Vector3 vd = memberMult(randomVector(), m_velocityDeviation);
  return (m_basicVel + vd) * m_initialVelMag.random();
}

Point3 ParticleSystem::initialPos() const
{
  return m_origin + memberMult(randomVector(), m_posDeviation);
}

void ParticleSystem::setTextureName(const std::string& nm)
{
  m_do->setTexture(0,
                   RenderSystem::getInstance().requestTexture(nm),
                   RenderSystem::getInstance().requestTexture(nm, true));
}

void ParticleSystem::setBBox(const WFMath::AxisBox<3>& bb)
{
  m_origin = Point3(0, 0, 0);
  m_posDeviation = Vector3(bb.highCorner().x(), bb.highCorner().y(), 0.0);
    
  double diameter = sqrt((bb.highCorner().x() - bb.lowCorner().x()) * 
    (bb.highCorner().y() - bb.lowCorner().y()));
  m_createPerSec = DRange(1200 * diameter, 1200 * diameter);
}

} // of namespace Sear
