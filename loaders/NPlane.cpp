// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: NPlane.cpp,v 1.27 2006-01-28 15:35:49 simon Exp $

#include "common/Utility.h"

#include "src/System.h"
#include "renderers/Render.h"

#include "NPlane.h"

#include <iostream>

#include "renderers/RenderSystem.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

NPlane::NPlane(Render *render) : Model(render),
  m_num_planes(0),
  m_vertex_data(NULL),
  m_normal_data(NULL),
  m_texture_data(NULL),
  m_initialised(false),
  m_disp(0),
  m_select_disp(0)
{}

NPlane::~NPlane() {
  assert(m_initialised == false);
}
  
int NPlane::init(const std::string &texture, unsigned int num_planes, float width, float height) {
  assert(m_initialised == false);
 
  // Store texture name and get ID numbers
  m_texture_name = texture;
  m_texture_id = RenderSystem::getInstance().requestTexture(texture);
  m_texture_mask_id = RenderSystem::getInstance().requestTexture(texture, true);

  m_num_planes = num_planes;

  // Allocate mem
  float rads_per_segment = WFMath::Pi / (float)num_planes;
  m_vertex_data = new Vertex_3[8 * num_planes];
  m_normal_data = new Normal[8 * num_planes];
  m_texture_data = new Texel[8 * num_planes];

  assert (m_vertex_data && m_normal_data && m_texture_data);

  float in[3][3];
  float out[3];
  // Compute normals
  for (unsigned int i = 0; i < num_planes; i++) {
    float x = width * cos ((float)i * rads_per_segment) / 2.0f;
    float y = width * sin ((float)i * rads_per_segment) / 2.0f;
    in[0][0] = m_vertex_data[8 * i].x = x;
    in[0][1] = m_vertex_data[8 * i].y = y;
    in[0][2] = m_vertex_data[8 * i].z = 0.0f;
    in[1][0] = m_vertex_data[8 * i + 1].x = -x;
    in[1][1] = m_vertex_data[8 * i + 1].y = -y;
    in[1][2] = m_vertex_data[8 * i + 1].z = 0.0f;
    in[2][0] = m_vertex_data[8 * i + 2].x = -x;
    in[2][1] = m_vertex_data[8 * i + 2].y = -y;
    in[2][2] = m_vertex_data[8 * i + 2].z = height;
    m_vertex_data[8 * i + 3].x = x;
    m_vertex_data[8 * i + 3].y = y;
    m_vertex_data[8 * i + 3].z = height;

    m_vertex_data[8 * i + 4].x = x;
    m_vertex_data[8 * i + 4].y = y;
    m_vertex_data[8 * i + 4].z = height;
    m_vertex_data[8 * i + 5].x = -x;
    m_vertex_data[8 * i + 5].y = -y;
    m_vertex_data[8 * i + 5].z = height;
    m_vertex_data[8 * i + 6].x = -x;
    m_vertex_data[8 * i + 6].y = -y;
    m_vertex_data[8 * i + 6].z = 0.0f;
    m_vertex_data[8 * i + 7].x = x;
    m_vertex_data[8 * i + 7].y = y;
    m_vertex_data[8 * i + 7].z = 0.0f;
    calcNormal(in, out);
    for (unsigned int j = 0; j < 4; ++j) {
      m_normal_data[8 * i + j].x = out[0];
      m_normal_data[8 * i + j].y = out[1];
      m_normal_data[8 * i + j].z = out[2];
      m_normal_data[8 * i + j + 4].x = -out[0];
      m_normal_data[8 * i + j + 4].y = -out[1];
      m_normal_data[8 * i + j + 4].z = -out[2];
    }
   
    m_texture_data[8 * i].s = 0.0f; m_texture_data[8 * i].t = 0.0f;
    m_texture_data[8 * i + 1].s = 1.0f; m_texture_data[8 * i + 1].t = 0.0f;
    m_texture_data[8 * i + 2].s = 1.0f; m_texture_data[8 * i + 2].t = 1.0f;
    m_texture_data[8 * i + 3].s = 0.0f; m_texture_data[8 * i + 3].t = 1.0f;

    m_texture_data[8 * i + 4].s =  0.0f; m_texture_data[8 * i + 4].t =  1.0f;
    m_texture_data[8 * i + 5].s = 1.0f; m_texture_data[8 * i + 5].t = 1.0f;
    m_texture_data[8 * i + 6].s = 1.0f; m_texture_data[8 * i + 6].t = 0.0f;
    m_texture_data[8 * i + 7].s = 0.0f; m_texture_data[8 * i + 7].t = 0.0f;
    
  }
  m_initialised = true;
  return 0;
}

int NPlane::shutdown() {
  assert(m_initialised == true);
  if (m_vertex_data) delete [] m_vertex_data;
  if (m_normal_data) delete [] m_normal_data;
  if (m_texture_data) delete [] m_texture_data;

  contextDestroyed(true);

  m_initialised = false;
  return 0;
}

void NPlane::contextCreated() {}

void NPlane::contextDestroyed(bool check) {
  assert(m_render);
  if (check) {
    m_render->freeList(m_disp);
    m_render->freeList(m_select_disp);
  }
  m_disp = 0;
  m_select_disp = 0;
}

void NPlane::render(bool select_mode) {
  assert(m_render);

  static float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

  if (select_mode) {
    if (m_select_disp > 0) {
      m_render->playList(m_select_disp);
    } else {
      m_select_disp = m_render->getNewList();
      m_render->beginRecordList(m_select_disp);
      m_render->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
      RenderSystem::getInstance().switchTexture(m_texture_mask_id);
      m_render->renderArrays(Graphics::RES_QUADS, 0, m_num_planes * 8, m_vertex_data, m_texture_data, m_normal_data,false);
      m_render->endRecordList();
    }
  } else {
    if (m_disp > 0) {
      m_render->playList(m_disp);
    } else {
      m_disp = m_render->getNewList();
      m_render->beginRecordList(m_disp);
      m_render->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
      RenderSystem::getInstance().switchTexture(m_texture_id);
      m_render->renderArrays(Graphics::RES_QUADS, 0, m_num_planes * 8, m_vertex_data, m_texture_data, m_normal_data,false);
      m_render->endRecordList();
    }
  }
}

} /* namespace Sear */
