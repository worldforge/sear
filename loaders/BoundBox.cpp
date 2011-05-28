// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall, University of Southampton

// $Id: BoundBox.cpp,v 1.35 2007-05-02 20:47:54 simon Exp $

#include "renderers/RenderSystem.h"

#include "BoundBox.h"
#include "StaticObject.h"

#include <wfmath/point.h>

#include <cassert>

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
  if (m_initialised) shutdown();
}

int BoundBox::init(WFMath::AxisBox<3> bbox, const std::string &texture, bool wrap) {
  assert(m_initialised == false);

  StaticObject* so = new StaticObject();
  so->init();
  // Set material properties
  so->setAmbient(1.0f, 1.0f, 1.0f, 1.0f);
  so->setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
  so->setSpecular(1.0f, 1.0f, 1.0f, 1.0f);
  so->setEmission(0.0f, 0.0f, 0.0f, 0.0f);
  so->setShininess(50.0f);

  so->setTexture(0,
    RenderSystem::getInstance().requestTexture(texture),
    RenderSystem::getInstance().requestTexture(texture, true));

  so->setNumPoints(36);
  // Allocate mem for data
  float *vertexptr = so->createVertexData(36 * 3);
  float *normalptr = so->createNormalData(36 * 3);
  float *texptr    = so->createTextureData(36 * 2);

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

  m_render_objects.push_back(so);

  contextCreated();

  m_initialised = true;

  return 0;
}

int BoundBox::shutdown() {
  assert(m_initialised == true);

  contextDestroyed(true);

  StaticObjectList::const_iterator I = m_render_objects.begin();
  StaticObjectList::const_iterator Iend = m_render_objects.end();
  for (; I != Iend; ++I) {
    StaticObject* so = *I;
    assert(so);
    int id, mask_id;
    so->getTexture(0, id, mask_id);
    RenderSystem::getInstance().releaseTexture(id);
    RenderSystem::getInstance().releaseTexture(mask_id);
    delete so;   
  }
  m_render_objects.clear();

  m_initialised = false;
  return 0;
}

void BoundBox::contextCreated() {
  StaticObjectList::const_iterator I = m_render_objects.begin();
  StaticObjectList::const_iterator Iend = m_render_objects.end();
  for (; I != Iend; ++I) {
    StaticObject* so = *I;
    assert(so);
    so->contextCreated();
  }
}

void BoundBox::contextDestroyed(bool check) {
  StaticObjectList::const_iterator I = m_render_objects.begin();
  StaticObjectList::const_iterator Iend = m_render_objects.end();
  for (; I != Iend; ++I) {
    StaticObject* so = *I;
    assert(so);
    so->contextDestroyed(check);
  }
}

void BoundBox::render(bool select_mode) {
  StaticObjectList::const_iterator I = m_render_objects.begin();
  StaticObjectList::const_iterator Iend = m_render_objects.end();
  for (; I != Iend; ++I) {
    StaticObject* so = *I;
    assert(so);
    so->render(select_mode);
  }
}

} /* namespace Sear */
