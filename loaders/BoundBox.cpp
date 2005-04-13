// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: BoundBox.cpp,v 1.26 2005-04-13 12:16:04 simon Exp $

#include "src/System.h"
#include "renderers/Graphics.h"
#include "renderers/Render.h"

#include "BoundBox.h"

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

BoundBox::BoundBox(Render *render) : Model(render), 
  m_initialised(false),
  m_use_textures(true),
  m_type("default"),
  m_list(0),
  m_list_select(0)
{}

BoundBox::~BoundBox() {
  assert(m_initialised == false);
  if (m_initialised) shutdown();
}
  
int BoundBox::init(WFMath::AxisBox<3> bbox, const std::string &type, bool _wrap) {
  assert(m_initialised == false);
  m_type = type;

  m_vertex_data[0].x = bbox.lowCorner().x(); m_vertex_data[0].y = bbox.highCorner().y(); m_vertex_data[0].z = bbox.lowCorner().z();
  m_vertex_data[1].x = bbox.lowCorner().x(); m_vertex_data[1].y = bbox.lowCorner().y(); m_vertex_data[1].z = bbox.lowCorner().z();
  m_vertex_data[2].x = bbox.highCorner().x(); m_vertex_data[2].y = bbox.lowCorner().y(); m_vertex_data[2].z = bbox.lowCorner().z();
  m_vertex_data[3].x = bbox.highCorner().x(); m_vertex_data[3].y = bbox.highCorner().y(); m_vertex_data[3].z = bbox.lowCorner().z();

  m_vertex_data[4].x = bbox.lowCorner().x(); m_vertex_data[4].y = bbox.lowCorner().y(); m_vertex_data[4].z = bbox.highCorner().z();
  m_vertex_data[5].x = bbox.lowCorner().x(); m_vertex_data[5].y = bbox.highCorner().y(); m_vertex_data[5].z = bbox.highCorner().z();
  m_vertex_data[6].x = bbox.highCorner().x(); m_vertex_data[6].y = bbox.highCorner().y(); m_vertex_data[6].z = bbox.highCorner().z();
  m_vertex_data[7].x = bbox.highCorner().x(); m_vertex_data[7].y = bbox.lowCorner().y(); m_vertex_data[7].z = bbox.highCorner().z();

  m_vertex_data[8].x = bbox.lowCorner().x(); m_vertex_data[8].y = bbox.highCorner().y(); m_vertex_data[8].z = bbox.lowCorner().z();
  m_vertex_data[9].x = bbox.lowCorner().x(); m_vertex_data[9].y = bbox.highCorner().y(); m_vertex_data[9].z = bbox.highCorner().z();
  m_vertex_data[10].x = bbox.lowCorner().x(); m_vertex_data[10].y = bbox.lowCorner().y(); m_vertex_data[10].z = bbox.highCorner().z();
  m_vertex_data[11].x = bbox.lowCorner().x(); m_vertex_data[11].y = bbox.lowCorner().y(); m_vertex_data[11].z = bbox.lowCorner().z();

  m_vertex_data[12].x = bbox.highCorner().x(); m_vertex_data[12].y = bbox.lowCorner().y(); m_vertex_data[12].z = bbox.lowCorner().z();
  m_vertex_data[13].x = bbox.highCorner().x(); m_vertex_data[13].y = bbox.lowCorner().y(); m_vertex_data[13].z = bbox.highCorner().z();
  m_vertex_data[14].x = bbox.highCorner().x(); m_vertex_data[14].y = bbox.highCorner().y(); m_vertex_data[14].z = bbox.highCorner().z();
  m_vertex_data[15].x = bbox.highCorner().x(); m_vertex_data[15].y = bbox.highCorner().y(); m_vertex_data[15].z = bbox.lowCorner().z();

  m_vertex_data[16].x = bbox.highCorner().x(); m_vertex_data[16].y = bbox.highCorner().y(); m_vertex_data[16].z = bbox.lowCorner().z();
  m_vertex_data[17].x = bbox.highCorner().x(); m_vertex_data[17].y = bbox.highCorner().y(); m_vertex_data[17].z = bbox.highCorner().z();
  m_vertex_data[18].x = bbox.lowCorner().x();  m_vertex_data[18].y = bbox.highCorner().y(); m_vertex_data[18].z = bbox.highCorner().z();
  m_vertex_data[19].x = bbox.lowCorner().x();  m_vertex_data[19].y = bbox.highCorner().y(); m_vertex_data[19].z = bbox.lowCorner().z();

  m_vertex_data[20].x = bbox.lowCorner().x();  m_vertex_data[20].y = bbox.lowCorner().y(); m_vertex_data[20].z = bbox.lowCorner().z();
  m_vertex_data[21].x = bbox.lowCorner().x(); m_vertex_data[21].y = bbox.lowCorner().y(); m_vertex_data[21].z = bbox.highCorner().z();
  m_vertex_data[22].x = bbox.highCorner().x(); m_vertex_data[22].y = bbox.lowCorner().y(); m_vertex_data[22].z = bbox.highCorner().z();
  m_vertex_data[23].x = bbox.highCorner().x(); m_vertex_data[23].y = bbox.lowCorner().y(); m_vertex_data[23].z = bbox.lowCorner().z();
  if (!_wrap) {
    m_texture_data[0].s = 0.0f; m_texture_data[0].t = 0.0f;
    m_texture_data[1].s = 0.0f; m_texture_data[1].t = 1.0f;
    m_texture_data[2].s = 1.0f; m_texture_data[2].t = 1.0f;
    m_texture_data[3].s = 1.0f; m_texture_data[3].t = 0.0f;
  
    m_texture_data[4].s = 0.0f; m_texture_data[4].t = 0.0f;
    m_texture_data[5].s = 0.0f; m_texture_data[5].t = 1.0f;
    m_texture_data[6].s = 1.0f; m_texture_data[6].t = 1.0f;
    m_texture_data[7].s = 1.0f; m_texture_data[7].t = 0.0f;

    m_texture_data[8].s = 0.0f; m_texture_data[8].t = 0.0f;
    m_texture_data[9].s = 0.0f; m_texture_data[9].t = 1.0f;
    m_texture_data[10].s = 1.0f; m_texture_data[10].t = 1.0f;
    m_texture_data[11].s = 1.0f; m_texture_data[11].t = 0.0f;

    m_texture_data[12].s = 0.0f; m_texture_data[12].t = 0.0f;
    m_texture_data[13].s = 0.0f; m_texture_data[13].t = 1.0f;
    m_texture_data[14].s = 1.0f; m_texture_data[14].t = 1.0f;
    m_texture_data[15].s = 1.0f; m_texture_data[15].t = 0.0f;

    m_texture_data[16].s = 0.0f; m_texture_data[16].t = 0.0f;
    m_texture_data[17].s = 0.0f; m_texture_data[17].t = 1.0f;
    m_texture_data[18].s = 1.0f; m_texture_data[18].t = 1.0f;
    m_texture_data[19].s = 1.0f; m_texture_data[19].t = 0.0f;

    m_texture_data[20].s = 0.0f; m_texture_data[20].t = 0.0f;
    m_texture_data[21].s = 0.0f; m_texture_data[21].t = 1.0f;
    m_texture_data[22].s = 1.0f; m_texture_data[22].t = 1.0f;
    m_texture_data[23].s = 1.0f; m_texture_data[23].t = 0.0f;
  } else {
    m_texture_data[0].s = bbox.lowCorner().x(); m_texture_data[0].t = bbox.highCorner().y();
    m_texture_data[1].s = bbox.lowCorner().x(); m_texture_data[1].t = bbox.lowCorner().y();
    m_texture_data[2].s = bbox.highCorner().x(); m_texture_data[2].t = bbox.lowCorner().y();
    m_texture_data[3].s = bbox.highCorner().x(); m_texture_data[3].t = bbox.highCorner().y();
      
    m_texture_data[4].s = bbox.lowCorner().x(); m_texture_data[4].t = bbox.lowCorner().y();
    m_texture_data[5].s = bbox.lowCorner().x(); m_texture_data[5].t = bbox.highCorner().y();
    m_texture_data[6].s = bbox.highCorner().x(); m_texture_data[6].t = bbox.highCorner().y();
    m_texture_data[7].s = bbox.highCorner().x(); m_texture_data[7].t = bbox.lowCorner().y();
	      
    m_texture_data[8].s = bbox.highCorner().y(); m_texture_data[8].t = bbox.lowCorner().z();
    m_texture_data[9].s = bbox.highCorner().y(); m_texture_data[9].t = bbox.highCorner().z();
    m_texture_data[10].s = bbox.lowCorner().y(); m_texture_data[10].t = bbox.highCorner().z();
    m_texture_data[11].s = bbox.lowCorner().y(); m_texture_data[11].t = bbox.lowCorner().z();

    m_texture_data[12].s = bbox.lowCorner().y(); m_texture_data[12].t = bbox.lowCorner().z();      
    m_texture_data[13].s = bbox.lowCorner().y(); m_texture_data[13].t = bbox.highCorner().z();
    m_texture_data[14].s = bbox.highCorner().y(); m_texture_data[14].t = bbox.highCorner().z();
    m_texture_data[15].s = bbox.highCorner().y(); m_texture_data[15].t = bbox.lowCorner().z();

    m_texture_data[16].s = bbox.highCorner().x(); m_texture_data[16].t = bbox.lowCorner().z();
    m_texture_data[17].s = bbox.highCorner().x(); m_texture_data[17].t = bbox.highCorner().z();
    m_texture_data[18].s = bbox.lowCorner().x(); m_texture_data[18].t = bbox.highCorner().z();
    m_texture_data[19].s = bbox.lowCorner().x(); m_texture_data[19].t = bbox.lowCorner().z();

    m_texture_data[20].s = bbox.lowCorner().x(); m_texture_data[20].t = bbox.lowCorner().z();
    m_texture_data[21].s = bbox.lowCorner().x(); m_texture_data[21].t = bbox.highCorner().z();
    m_texture_data[22].s = bbox.highCorner().x(); m_texture_data[22].t = bbox.highCorner().z();
    m_texture_data[23].s = bbox.highCorner().x(); m_texture_data[23].t = bbox.lowCorner().z();
  }
  m_normal_data[0].x =  0.0f; m_normal_data[0].y =  0.0f; m_normal_data[0].z = -1.0f;
  m_normal_data[1].x =  0.0f; m_normal_data[1].y =  0.0f; m_normal_data[1].z = -1.0f;
  m_normal_data[2].x =  0.0f; m_normal_data[2].y =  0.0f; m_normal_data[2].z = -1.0f;
  m_normal_data[3].x =  0.0f; m_normal_data[3].y =  0.0f; m_normal_data[3].z = -1.0f;
  m_normal_data[4].x =  0.0f; m_normal_data[4].y =  0.0f; m_normal_data[4].z =  1.0f;
  m_normal_data[5].x =  0.0f; m_normal_data[5].y =  0.0f; m_normal_data[5].z =  1.0f;
  m_normal_data[6].x =  0.0f; m_normal_data[6].y =  0.0f; m_normal_data[6].z =  1.0f;
  m_normal_data[7].x =  0.0f; m_normal_data[7].y =  0.0f; m_normal_data[7].z =  1.0f;
  m_normal_data[8].x = -1.0f; m_normal_data[8].y =  0.0f; m_normal_data[8].z =  0.0f;
  m_normal_data[9].x = -1.0f; m_normal_data[9].y =  0.0f; m_normal_data[9].z =  0.0f;
  m_normal_data[10].x = -1.0f; m_normal_data[10].y =  0.0f; m_normal_data[10].z =  0.0f;
  m_normal_data[11].x = -1.0f; m_normal_data[11].y =  0.0f; m_normal_data[11].z =  0.0f;
  m_normal_data[12].x =  1.0f; m_normal_data[12].y =  0.0f; m_normal_data[12].z =  0.0f;
  m_normal_data[13].x =  1.0f; m_normal_data[13].y =  0.0f; m_normal_data[13].z =  0.0f;
  m_normal_data[14].x =  1.0f; m_normal_data[14].y =  0.0f; m_normal_data[14].z =  0.0f;
  m_normal_data[15].x =  1.0f; m_normal_data[15].y =  0.0f; m_normal_data[15].z =  0.0f;
  m_normal_data[16].x =  0.0f; m_normal_data[16].y =  1.0f; m_normal_data[16].z =  0.0f;
  m_normal_data[17].x =  0.0f; m_normal_data[17].y =  1.0f; m_normal_data[17].z =  0.0f;
  m_normal_data[18].x =  0.0f; m_normal_data[18].y =  1.0f; m_normal_data[18].z =  0.0f;
  m_normal_data[19].x =  0.0f; m_normal_data[19].y =  1.0f; m_normal_data[19].z =  0.0f;
  m_normal_data[20].x =  0.0f; m_normal_data[20].y = -1.0f; m_normal_data[20].z =  0.0f;
  m_normal_data[21].x =  0.0f; m_normal_data[21].y = -1.0f; m_normal_data[21].z =  0.0f;
  m_normal_data[22].x =  0.0f; m_normal_data[22].y = -1.0f; m_normal_data[22].z =  0.0f;
  m_normal_data[23].x =  0.0f; m_normal_data[23].y = -1.0f; m_normal_data[23].z =  0.0f;

  m_initialised = true;

  return 0;
}

int BoundBox::shutdown() {
  assert(m_initialised == true);

  invalidate();

  m_initialised = false;
  return 0;
}

void BoundBox::invalidate() {
  assert(m_render != NULL);
  // Clear up display lists
  m_render->freeList(m_list);
  m_render->freeList(m_list_select);

  m_list = 0;
  m_list_select = 0;
}

void BoundBox::render(bool select_mode) {
  assert(m_render && "BoundBox m_render is null");

  static float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  
  if (select_mode) {
     if (m_list_select) {
      m_render->playList(m_list_select);
    } else {
      m_list_select = m_render->getNewList();
      m_render->beginRecordList(m_list_select);
//      m_render->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
      m_render->renderArrays(Graphics::RES_QUADS, 0, m_num_points, &m_vertex_data[0], NULL, NULL, false);
      m_render->endRecordList();
    } 
  } else {
    RenderSystem::getInstance().switchTexture(RenderSystem::getInstance().requestTexture(m_type));
    if (m_list) {
      m_render->playList(m_list);
    } else {
      printf("BoundBox: New Display list\n");
      m_list = m_render->getNewList();
      m_render->beginRecordList(m_list);
      m_render->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
      m_render->renderArrays(Graphics::RES_QUADS, 0, m_num_points, &m_vertex_data[0], &m_texture_data[0], &m_normal_data[0], false);
      m_render->endRecordList();
    } 
  }
}

} /* namespace Sear */
