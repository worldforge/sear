// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: BoundBox.cpp,v 1.32 2006-05-06 13:50:22 simon Exp $

#include "renderers/RenderSystem.h"

#include "BoundBox.h"
#include "StaticObject.h"


#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

BoundBox::BoundBox() :
  Model(), 
  m_initialised(false)
{}

BoundBox::~BoundBox() {
  assert(m_initialised == false);
}
  
int BoundBox::init(WFMath::AxisBox<3> bbox, const std::string &texture, bool wrap) {
  assert(m_initialised == false);

  m_so = SPtrShutdown<StaticObject>(new StaticObject());
  m_so->init();
  // Set material properties
  m_so->setAmbient(1.0f, 1.0f, 1.0f, 1.0f);
  m_so->setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
  m_so->setSpecular(1.0f, 1.0f, 1.0f, 1.0f);
  m_so->setEmission(0.0f, 0.0f, 0.0f, 0.0f);
  m_so->setShininess(50.0f);

  m_so->setTexture(0,
    RenderSystem::getInstance().requestTexture(texture),
    RenderSystem::getInstance().requestTexture(texture, true));

  m_so->setNumPoints(36);
  // Allocate mem for data
  float *vertexptr = m_so->createVertexData(36 * 3);
  float *normalptr = m_so->createNormalData(36 * 3);
  float *texptr    = m_so->createTextureData(36 * 2);

  // TODO: Convert Quads into triangles.
  int vertex_counter = -1;
  int normal_counter = -1;
  int texture_counter = -1;

  vertexptr[++vertex_counter] = bbox.lowCorner().x(); vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();
  vertexptr[++vertex_counter] = bbox.lowCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();
  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();

  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();
  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();
  vertexptr[++vertex_counter] = bbox.lowCorner().x(); vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();

  vertexptr[++vertex_counter] = bbox.lowCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();
  vertexptr[++vertex_counter] = bbox.lowCorner().x(); vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();
  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();

  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();
  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();
  vertexptr[++vertex_counter] = bbox.lowCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();

  vertexptr[++vertex_counter] = bbox.lowCorner().x(); vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();
  vertexptr[++vertex_counter] = bbox.lowCorner().x(); vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();
  vertexptr[++vertex_counter] = bbox.lowCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();

  vertexptr[++vertex_counter] = bbox.lowCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();
  vertexptr[++vertex_counter] = bbox.lowCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();
  vertexptr[++vertex_counter] = bbox.lowCorner().x(); vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();

  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();
  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();
  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();

  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();
  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();
  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();

  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();
  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();
  vertexptr[++vertex_counter] = bbox.lowCorner().x();  vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();

  vertexptr[++vertex_counter] = bbox.lowCorner().x();  vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();
  vertexptr[++vertex_counter] = bbox.lowCorner().x();  vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();
  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.highCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();

  vertexptr[++vertex_counter] = bbox.lowCorner().x();  vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();
  vertexptr[++vertex_counter] = bbox.lowCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();
  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();

  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.highCorner().z();
  vertexptr[++vertex_counter] = bbox.highCorner().x(); vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();
  vertexptr[++vertex_counter] = bbox.lowCorner().x();  vertexptr[++vertex_counter] = bbox.lowCorner().y(); vertexptr[++vertex_counter] = bbox.lowCorner().z();

  if (!wrap) {
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 1.0f;

    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 0.0f;
  
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 1.0f;

    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 0.0f;

    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 1.0f;

    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 0.0f;

    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 1.0f;

    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 0.0f;

    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 1.0f;

    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 0.0f;

    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 1.0f;

    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 0.0f;
  } else {
    texptr[++texture_counter] = bbox.lowCorner().x(); texptr[++texture_counter] = bbox.highCorner().y();
    texptr[++texture_counter] = bbox.lowCorner().x(); texptr[++texture_counter] = bbox.lowCorner().y();
    texptr[++texture_counter] = bbox.highCorner().x(); texptr[++texture_counter] = bbox.lowCorner().y();

    texptr[++texture_counter] = bbox.highCorner().x(); texptr[++texture_counter] = bbox.lowCorner().y();
    texptr[++texture_counter] = bbox.highCorner().x(); texptr[++texture_counter] = bbox.highCorner().y();
    texptr[++texture_counter] = bbox.lowCorner().x(); texptr[++texture_counter] = bbox.highCorner().y();
      
    texptr[++texture_counter] = bbox.lowCorner().x(); texptr[++texture_counter] = bbox.lowCorner().y();
    texptr[++texture_counter] = bbox.lowCorner().x(); texptr[++texture_counter] = bbox.highCorner().y();
    texptr[++texture_counter] = bbox.highCorner().x(); texptr[++texture_counter] = bbox.highCorner().y();

    texptr[++texture_counter] = bbox.highCorner().x(); texptr[++texture_counter] = bbox.highCorner().y();
    texptr[++texture_counter] = bbox.highCorner().x(); texptr[++texture_counter] = bbox.lowCorner().y();
    texptr[++texture_counter] = bbox.lowCorner().x(); texptr[++texture_counter] = bbox.lowCorner().y();
	     
    texptr[++texture_counter] = bbox.highCorner().y(); texptr[++texture_counter] = bbox.lowCorner().z();
    texptr[++texture_counter] = bbox.highCorner().y(); texptr[++texture_counter] = bbox.highCorner().z();
    texptr[++texture_counter] = bbox.lowCorner().y(); texptr[++texture_counter] = bbox.highCorner().z();

    texptr[++texture_counter] = bbox.lowCorner().y(); texptr[++texture_counter] = bbox.highCorner().z();
    texptr[++texture_counter] = bbox.lowCorner().y(); texptr[++texture_counter] = bbox.lowCorner().z();
    texptr[++texture_counter] = bbox.highCorner().y(); texptr[++texture_counter] = bbox.lowCorner().z();

    texptr[++texture_counter] = bbox.lowCorner().y(); texptr[++texture_counter] = bbox.lowCorner().z();      
    texptr[++texture_counter] = bbox.lowCorner().y(); texptr[++texture_counter] = bbox.highCorner().z();
    texptr[++texture_counter] = bbox.highCorner().y(); texptr[++texture_counter] = bbox.highCorner().z();

    texptr[++texture_counter] = bbox.highCorner().y(); texptr[++texture_counter] = bbox.highCorner().z();
    texptr[++texture_counter] = bbox.highCorner().y(); texptr[++texture_counter] = bbox.lowCorner().z();
    texptr[++texture_counter] = bbox.lowCorner().y(); texptr[++texture_counter] = bbox.lowCorner().z();      

    texptr[++texture_counter] = bbox.highCorner().x(); texptr[++texture_counter] = bbox.lowCorner().z();
    texptr[++texture_counter] = bbox.highCorner().x(); texptr[++texture_counter] = bbox.highCorner().z();
    texptr[++texture_counter] = bbox.lowCorner().x(); texptr[++texture_counter] = bbox.highCorner().z();

    texptr[++texture_counter] = bbox.lowCorner().x(); texptr[++texture_counter] = bbox.highCorner().z();
    texptr[++texture_counter] = bbox.lowCorner().x(); texptr[++texture_counter] = bbox.lowCorner().z();
    texptr[++texture_counter] = bbox.highCorner().x(); texptr[++texture_counter] = bbox.lowCorner().z();

    texptr[++texture_counter] = bbox.lowCorner().x(); texptr[++texture_counter] = bbox.lowCorner().z();
    texptr[++texture_counter] = bbox.lowCorner().x(); texptr[++texture_counter] = bbox.highCorner().z();
    texptr[++texture_counter] = bbox.highCorner().x(); texptr[++texture_counter] = bbox.highCorner().z();

    texptr[++texture_counter] = bbox.highCorner().x(); texptr[++texture_counter] = bbox.highCorner().z();
    texptr[++texture_counter] = bbox.highCorner().x(); texptr[++texture_counter] = bbox.lowCorner().z();
    texptr[++texture_counter] = bbox.lowCorner().x(); texptr[++texture_counter] = bbox.lowCorner().z();
  }
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] = -1.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] = -1.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] = -1.0f;

  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] = -1.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] = -1.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] = -1.0f;

  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  1.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  1.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  1.0f;

  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  1.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  1.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  1.0f;

  normalptr[++normal_counter] = -1.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] = -1.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] = -1.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f;

  normalptr[++normal_counter] = -1.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] = -1.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] = -1.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f;

  normalptr[++normal_counter] =  1.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] =  1.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] =  1.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f;

  normalptr[++normal_counter] =  1.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] =  1.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] =  1.0f; normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  0.0f;

  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  1.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  1.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  1.0f; normalptr[++normal_counter] =  0.0f;

  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  1.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  1.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] =  1.0f; normalptr[++normal_counter] =  0.0f;

  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] = -1.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] = -1.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] = -1.0f; normalptr[++normal_counter] =  0.0f;

  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] = -1.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] = -1.0f; normalptr[++normal_counter] =  0.0f;
  normalptr[++normal_counter] =  0.0f; normalptr[++normal_counter] = -1.0f; normalptr[++normal_counter] =  0.0f;

  contextCreated();

  m_initialised = true;

  return 0;
}

int BoundBox::shutdown() {
  assert(m_initialised == true);

  contextDestroyed(true);

  m_initialised = false;
  return 0;
}

void BoundBox::contextCreated() {
  if (m_so) m_so->contextCreated();
}

void BoundBox::contextDestroyed(bool check) {
  if (m_so) m_so->contextDestroyed(check);
}

void BoundBox::render(bool select_mode) {
  assert(m_so);
  m_so->render(select_mode);
}

} /* namespace Sear */
