// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: BoundBox.cpp,v 1.21 2004-04-26 15:45:20 simon Exp $

#include "src/System.h"
#include "src/Graphics.h"
#include "src/Render.h"

#include "BoundBox.h"

#include "renderers/RenderSystem.h"

#ifdef HAVE_CONFIG
  #include "config.h"
#endif

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
  _type("default"),
  _use_textures(true),
  _initialised(false),
  _list(0),
  _list_select(0)
{}

BoundBox::~BoundBox() {
  if (_initialised) shutdown();
}
  
bool BoundBox::init(WFMath::AxisBox<3> _bbox, const std::string &type, bool _wrap) {
  _type = type;
  /*
  float min_x = _bbox.lowCorner().x();
  float max_x = _bbox.highCorner().x();
  float min_y = _bbox.lowCorner().y();
  float max_y = _bbox.highCorner().y();
  float min_z = _bbox.lowCorner().z();
  float max_z = _bbox.highCorner().z();

  if (detail < 0.0f) detail = 1.0f;

  float x_length = abs(max_x - min_x) / detail;
  float y_length = abs(max_y - min_y) / detail;
  float z_length = abs(max_z - min_z) / detail;
  
  unsigned int x, y, z;
  unsigned int vertex_counter = 0;
  unsigned int normal_counter = 0;
  unsigned int texture_counter = 0;
  
  // Top plane
  z = max_z;
  for (x = min_x; x < max_x; x += x_length) {
    for (y = min_y; y < max_y; y += y_length) {
      _vertex_data[vertex_counter++] = x;
      _vertex_data[vertex_counter++] = y;
      _vertex_data[vertex_counter++] = z;
      _vertex_data[vertex_counter++] = x + x_length;
      _vertex_data[vertex_counter++] = y;
      _vertex_data[vertex_counter++] = z;
      _vertex_data[vertex_counter++] = x + x_length;
      _vertex_data[vertex_counter++] = y + y_length;
      _vertex_data[vertex_counter++] = z;
      _vertex_data[vertex_counter++] = x;
      _vertex_data[vertex_counter++] = y + y_length;
      _vertex_data[vertex_counter++] = z;
      _texture_data[texture_counter++] = x / (max_x - min_x);
      _texture_data[texture_counter++] = y / (max_y - min_y);
      _texture_data[texture_counter++] = (x + x_length) / (max_x - min_x);
      _texture_data[texture_counter++] = y / (max_y - min_y);
      _texture_data[texture_counter++] = (x + x_length) / (max_x - min_x);
      _texture_data[texture_counter++] = (y + y_length) / (max_y - min_y);
      _texture_data[texture_counter++] = x / (max_x - min_x);
      _texture_data[texture_counter++] = (y + y_length) / (max_y - min_y);
    }
  }
  // Bottom plane
  z = min_z;
  for (x = min_x; x < max_x; x += x_length) {
    for (y = min_y; y < max_y; y += y_length) {
      _vertex_data[vertex_counter++] = x;
      _vertex_data[vertex_counter++] = y;
      _vertex_data[vertex_counter++] = z;
      _vertex_data[vertex_counter++] = x + x_length;
      _vertex_data[vertex_counter++] = y;
      _vertex_data[vertex_counter++] = z;
      _vertex_data[vertex_counter++] = x + x_length;
      _vertex_data[vertex_counter++] = y + y_length;
      _vertex_data[vertex_counter++] = z;
      _vertex_data[vertex_counter++] = x;
      _vertex_data[vertex_counter++] = y + y_length;
      _vertex_data[vertex_counter++] = z;
    }
  }
  */
  _vertex_data[0].x = _bbox.lowCorner().x(); _vertex_data[0].y = _bbox.highCorner().y(); _vertex_data[0].z = _bbox.lowCorner().z();
  _vertex_data[1].x = _bbox.lowCorner().x(); _vertex_data[1].y = _bbox.lowCorner().y(); _vertex_data[1].z = _bbox.lowCorner().z();
  _vertex_data[2].x = _bbox.highCorner().x(); _vertex_data[2].y = _bbox.lowCorner().y(); _vertex_data[2].z = _bbox.lowCorner().z();
  _vertex_data[3].x = _bbox.highCorner().x(); _vertex_data[3].y = _bbox.highCorner().y(); _vertex_data[3].z = _bbox.lowCorner().z();

  _vertex_data[4].x = _bbox.lowCorner().x(); _vertex_data[4].y = _bbox.lowCorner().y(); _vertex_data[4].z = _bbox.highCorner().z();
  _vertex_data[5].x = _bbox.lowCorner().x(); _vertex_data[5].y = _bbox.highCorner().y(); _vertex_data[5].z = _bbox.highCorner().z();
  _vertex_data[6].x = _bbox.highCorner().x(); _vertex_data[6].y = _bbox.highCorner().y(); _vertex_data[6].z = _bbox.highCorner().z();
  _vertex_data[7].x = _bbox.highCorner().x(); _vertex_data[7].y = _bbox.lowCorner().y(); _vertex_data[7].z = _bbox.highCorner().z();

  _vertex_data[8].x = _bbox.lowCorner().x(); _vertex_data[8].y = _bbox.highCorner().y(); _vertex_data[8].z = _bbox.lowCorner().z();
  _vertex_data[9].x = _bbox.lowCorner().x(); _vertex_data[9].y = _bbox.highCorner().y(); _vertex_data[9].z = _bbox.highCorner().z();
  _vertex_data[10].x = _bbox.lowCorner().x(); _vertex_data[10].y = _bbox.lowCorner().y(); _vertex_data[10].z = _bbox.highCorner().z();
  _vertex_data[11].x = _bbox.lowCorner().x(); _vertex_data[11].y = _bbox.lowCorner().y(); _vertex_data[11].z = _bbox.lowCorner().z();

  _vertex_data[12].x = _bbox.highCorner().x(); _vertex_data[12].y = _bbox.lowCorner().y(); _vertex_data[12].z = _bbox.lowCorner().z();
  _vertex_data[13].x = _bbox.highCorner().x(); _vertex_data[13].y = _bbox.lowCorner().y(); _vertex_data[13].z = _bbox.highCorner().z();
  _vertex_data[14].x = _bbox.highCorner().x(); _vertex_data[14].y = _bbox.highCorner().y(); _vertex_data[14].z = _bbox.highCorner().z();
  _vertex_data[15].x = _bbox.highCorner().x(); _vertex_data[15].y = _bbox.highCorner().y(); _vertex_data[15].z = _bbox.lowCorner().z();

  _vertex_data[16].x = _bbox.highCorner().x(); _vertex_data[16].y = _bbox.highCorner().y(); _vertex_data[16].z = _bbox.lowCorner().z();
  _vertex_data[17].x = _bbox.highCorner().x(); _vertex_data[17].y = _bbox.highCorner().y(); _vertex_data[17].z = _bbox.highCorner().z();
  _vertex_data[18].x = _bbox.lowCorner().x();  _vertex_data[18].y = _bbox.highCorner().y(); _vertex_data[18].z = _bbox.highCorner().z();
  _vertex_data[19].x = _bbox.lowCorner().x();  _vertex_data[19].y = _bbox.highCorner().y(); _vertex_data[19].z = _bbox.lowCorner().z();

  _vertex_data[20].x = _bbox.lowCorner().x();  _vertex_data[20].y = _bbox.lowCorner().y(); _vertex_data[20].z = _bbox.lowCorner().z();
  _vertex_data[21].x = _bbox.lowCorner().x(); _vertex_data[21].y = _bbox.lowCorner().y(); _vertex_data[21].z = _bbox.highCorner().z();
  _vertex_data[22].x = _bbox.highCorner().x(); _vertex_data[22].y = _bbox.lowCorner().y(); _vertex_data[22].z = _bbox.highCorner().z();
  _vertex_data[23].x = _bbox.highCorner().x(); _vertex_data[23].y = _bbox.lowCorner().y(); _vertex_data[23].z = _bbox.lowCorner().z();
  if (!_wrap) {
    _texture_data[0].s = 0.0f; _texture_data[0].t = 0.0f;
    _texture_data[1].s = 0.0f; _texture_data[1].t = 1.0f;
    _texture_data[2].s = 1.0f; _texture_data[2].t = 1.0f;
    _texture_data[3].s = 1.0f; _texture_data[3].t = 0.0f;
  
    _texture_data[4].s = 0.0f; _texture_data[4].t = 0.0f;
    _texture_data[5].s = 0.0f; _texture_data[5].t = 1.0f;
    _texture_data[6].s = 1.0f; _texture_data[6].t = 1.0f;
    _texture_data[7].s = 1.0f; _texture_data[7].t = 0.0f;

    _texture_data[8].s = 0.0f; _texture_data[8].t = 0.0f;
    _texture_data[9].s = 0.0f; _texture_data[9].t = 1.0f;
    _texture_data[10].s = 1.0f; _texture_data[10].t = 1.0f;
    _texture_data[11].s = 1.0f; _texture_data[11].t = 0.0f;

    _texture_data[12].s = 0.0f; _texture_data[12].t = 0.0f;
    _texture_data[13].s = 0.0f; _texture_data[13].t = 1.0f;
    _texture_data[14].s = 1.0f; _texture_data[14].t = 1.0f;
    _texture_data[15].s = 1.0f; _texture_data[15].t = 0.0f;

    _texture_data[16].s = 0.0f; _texture_data[16].t = 0.0f;
    _texture_data[17].s = 0.0f; _texture_data[17].t = 1.0f;
    _texture_data[18].s = 1.0f; _texture_data[18].t = 1.0f;
    _texture_data[19].s = 1.0f; _texture_data[19].t = 0.0f;

    _texture_data[20].s = 0.0f; _texture_data[20].t = 0.0f;
    _texture_data[21].s = 0.0f; _texture_data[21].t = 1.0f;
    _texture_data[22].s = 1.0f; _texture_data[22].t = 1.0f;
    _texture_data[23].s = 1.0f; _texture_data[23].t = 0.0f;
  } else {
    _texture_data[0].s = _bbox.lowCorner().x(); _texture_data[0].t = _bbox.highCorner().y();
    _texture_data[1].s = _bbox.lowCorner().x(); _texture_data[1].t = _bbox.lowCorner().y();
    _texture_data[2].s = _bbox.highCorner().x(); _texture_data[2].t = _bbox.lowCorner().y();
    _texture_data[3].s = _bbox.highCorner().x(); _texture_data[3].t = _bbox.highCorner().y();
      
    _texture_data[4].s = _bbox.lowCorner().x(); _texture_data[4].t = _bbox.lowCorner().y();
    _texture_data[5].s = _bbox.lowCorner().x(); _texture_data[5].t = _bbox.highCorner().y();
    _texture_data[6].s = _bbox.highCorner().x(); _texture_data[6].t = _bbox.highCorner().y();
    _texture_data[7].s = _bbox.highCorner().x(); _texture_data[7].t = _bbox.lowCorner().y();
	      
    _texture_data[8].s = _bbox.highCorner().y(); _texture_data[8].t = _bbox.lowCorner().z();
    _texture_data[9].s = _bbox.highCorner().y(); _texture_data[9].t = _bbox.highCorner().z();
    _texture_data[10].s = _bbox.lowCorner().y(); _texture_data[10].t = _bbox.highCorner().z();
    _texture_data[11].s = _bbox.lowCorner().y(); _texture_data[11].t = _bbox.lowCorner().z();

    _texture_data[12].s = _bbox.lowCorner().y(); _texture_data[12].t = _bbox.lowCorner().z();      
    _texture_data[13].s = _bbox.lowCorner().y(); _texture_data[13].t = _bbox.highCorner().z();
    _texture_data[14].s = _bbox.highCorner().y(); _texture_data[14].t = _bbox.highCorner().z();
    _texture_data[15].s = _bbox.highCorner().y(); _texture_data[15].t = _bbox.lowCorner().z();

    _texture_data[16].s = _bbox.highCorner().x(); _texture_data[16].t = _bbox.lowCorner().z();
    _texture_data[17].s = _bbox.highCorner().x(); _texture_data[17].t = _bbox.highCorner().z();
    _texture_data[18].s = _bbox.lowCorner().x(); _texture_data[18].t = _bbox.highCorner().z();
    _texture_data[19].s = _bbox.lowCorner().x(); _texture_data[19].t = _bbox.lowCorner().z();

    _texture_data[20].s = _bbox.lowCorner().x(); _texture_data[20].t = _bbox.lowCorner().z();
    _texture_data[21].s = _bbox.lowCorner().x(); _texture_data[21].t = _bbox.highCorner().z();
    _texture_data[22].s = _bbox.highCorner().x(); _texture_data[22].t = _bbox.highCorner().z();
    _texture_data[23].s = _bbox.highCorner().x(); _texture_data[23].t = _bbox.lowCorner().z();
  }
  _normal_data[0].x =  0.0f; _normal_data[0].y =  0.0f; _normal_data[0].z = -1.0f;
  _normal_data[1].x =  0.0f; _normal_data[1].y =  0.0f; _normal_data[1].z = -1.0f;
  _normal_data[2].x =  0.0f; _normal_data[2].y =  0.0f; _normal_data[2].z = -1.0f;
  _normal_data[3].x =  0.0f; _normal_data[3].y =  0.0f; _normal_data[3].z = -1.0f;
  _normal_data[4].x =  0.0f; _normal_data[4].y =  0.0f; _normal_data[4].z =  1.0f;
  _normal_data[5].x =  0.0f; _normal_data[5].y =  0.0f; _normal_data[5].z =  1.0f;
  _normal_data[6].x =  0.0f; _normal_data[6].y =  0.0f; _normal_data[6].z =  1.0f;
  _normal_data[7].x =  0.0f; _normal_data[7].y =  0.0f; _normal_data[7].z =  1.0f;
  _normal_data[8].x = -1.0f; _normal_data[8].y =  0.0f; _normal_data[8].z =  0.0f;
  _normal_data[9].x = -1.0f; _normal_data[9].y =  0.0f; _normal_data[9].z =  0.0f;
  _normal_data[10].x = -1.0f; _normal_data[10].y =  0.0f; _normal_data[10].z =  0.0f;
  _normal_data[11].x = -1.0f; _normal_data[11].y =  0.0f; _normal_data[11].z =  0.0f;
  _normal_data[12].x =  1.0f; _normal_data[12].y =  0.0f; _normal_data[12].z =  0.0f;
  _normal_data[13].x =  1.0f; _normal_data[13].y =  0.0f; _normal_data[13].z =  0.0f;
  _normal_data[14].x =  1.0f; _normal_data[14].y =  0.0f; _normal_data[14].z =  0.0f;
  _normal_data[15].x =  1.0f; _normal_data[15].y =  0.0f; _normal_data[15].z =  0.0f;
  _normal_data[16].x =  0.0f; _normal_data[16].y =  1.0f; _normal_data[16].z =  0.0f;
  _normal_data[17].x =  0.0f; _normal_data[17].y =  1.0f; _normal_data[17].z =  0.0f;
  _normal_data[18].x =  0.0f; _normal_data[18].y =  1.0f; _normal_data[18].z =  0.0f;
  _normal_data[19].x =  0.0f; _normal_data[19].y =  1.0f; _normal_data[19].z =  0.0f;
  _normal_data[20].x =  0.0f; _normal_data[20].y = -1.0f; _normal_data[20].z =  0.0f;
  _normal_data[21].x =  0.0f; _normal_data[21].y = -1.0f; _normal_data[21].z =  0.0f;
  _normal_data[22].x =  0.0f; _normal_data[22].y = -1.0f; _normal_data[22].z =  0.0f;
  _normal_data[23].x =  0.0f; _normal_data[23].y = -1.0f; _normal_data[23].z =  0.0f;
  _initialised = true;
  return true;
}

void BoundBox::shutdown() {
  _initialised = false;
  if (_render) {
    _render->freeList(_list);
    _render->freeList(_list_select);
  }
  _list = 0;
  _list_select =0;
}

void BoundBox::invalidate() {
  // Clear up display lists
  if (_render) {
    _render->freeList(_list);
    _render->freeList(_list_select);
  }
  _list = 0;
  _list_select =0;
}

void BoundBox::render(bool select_mode) {
  assert(_render && "BoundBox _render is null");
//  if (!_render) return;
  static float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  
  if (select_mode) {
     if (_list_select) {
      _render->playList(_list_select);
    } else {
      _list_select = _render->getNewList();
      _render->beginRecordList(_list_select);
//      _render->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
      _render->renderArrays(Graphics::RES_QUADS, 0, _num_points, &_vertex_data[0], NULL, NULL, false);
      _render->endRecordList();
    } 
  } else {
    if (_list) {
      _render->playList(_list);
    } else {
      _list = _render->getNewList();
      _render->beginRecordList(_list);
      _render->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
      RenderSystem::getInstance().switchTexture(RenderSystem::getInstance().requestTexture(_type ));
      _render->renderArrays(Graphics::RES_QUADS, 0, _num_points, &_vertex_data[0], &_texture_data[0], &_normal_data[0], false);
      _render->endRecordList();
    } 
  }
}

} /* namespace Sear */
