// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: NPlane.cpp,v 1.22 2005-01-06 12:46:54 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

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
  _use_textures(true),
  _num_planes(0),
  _vertex_data(NULL),
  _normal_data(NULL),
  _texture_data(NULL),
  _initialised(false)
{}

NPlane::~NPlane() {
  if (_initialised) shutdown();
}
  
bool NPlane::init(const std::string &type, unsigned int num_planes, float width, float height) {
  if (_initialised) shutdown();
  _type = type;
  _num_planes = num_planes;
  float rads_per_segment = WFMath::Pi / (float)num_planes;
  _vertex_data = new Vertex_3[8 * num_planes];
  _normal_data = new Normal[8 * num_planes];
  _texture_data = new Texel[8 * num_planes];
  assert (_vertex_data && _normal_data && _texture_data);
  float in[3][3];
  float out[3];
  std::cout << num_planes << std::endl;
  for (unsigned int i = 0; i < num_planes; i++) {
    float x = width * cos ((float)i * rads_per_segment) / 2.0f;
    float y = width * sin ((float)i * rads_per_segment) / 2.0f;
    in[0][0] = _vertex_data[8 * i].x = x;
    in[0][1] = _vertex_data[8 * i].y = y;
    in[0][2] = _vertex_data[8 * i].z = 0.0f;
    in[1][0] = _vertex_data[8 * i + 1].x = -x;
    in[1][1] = _vertex_data[8 * i + 1].y = -y;
    in[1][2] = _vertex_data[8 * i + 1].z = 0.0f;
    in[2][0] = _vertex_data[8 * i + 2].x = -x;
    in[2][1] = _vertex_data[8 * i + 2].y = -y;
    in[2][2] = _vertex_data[8 * i + 2].z = height;
    _vertex_data[8 * i + 3].x = x;
    _vertex_data[8 * i + 3].y = y;
    _vertex_data[8 * i + 3].z = height;

    _vertex_data[8 * i + 4].x = x;
    _vertex_data[8 * i + 4].y = y;
    _vertex_data[8 * i + 4].z = height;
    _vertex_data[8 * i + 5].x = -x;
    _vertex_data[8 * i + 5].y = -y;
    _vertex_data[8 * i + 5].z = height;
    _vertex_data[8 * i + 6].x = -x;
    _vertex_data[8 * i + 6].y = -y;
    _vertex_data[8 * i + 6].z = 0.0f;
    _vertex_data[8 * i + 7].x = x;
    _vertex_data[8 * i + 7].y = y;
    _vertex_data[8 * i + 7].z = 0.0f;
    calcNormal(in, out);
    for (unsigned int j = 0; j < 4; ++j) {
      _normal_data[8 * i + j].x = out[0];
      _normal_data[8 * i + j].y = out[1];
      _normal_data[8 * i + j].z = out[2];
      _normal_data[8 * i + j + 4].x = -out[0];
      _normal_data[8 * i + j + 4].y = -out[1];
      _normal_data[8 * i + j + 4].z = -out[2];
    }
   
    _texture_data[8 * i].s = 0.0f; _texture_data[8 * i].t = 0.0f;
    _texture_data[8 * i + 1].s = 1.0f; _texture_data[8 * i + 1].t = 0.0f;
    _texture_data[8 * i + 2].s = 1.0f; _texture_data[8 * i + 2].t = 1.0f;
    _texture_data[8 * i + 3].s = 0.0f; _texture_data[8 * i + 3].t = 1.0f;

    _texture_data[8 * i + 4].s =  0.0f; _texture_data[8 * i + 4].t =  1.0f;
    _texture_data[8 * i + 5].s = 1.0f; _texture_data[8 * i + 5].t = 1.0f;
    _texture_data[8 * i + 6].s = 1.0f; _texture_data[8 * i + 6].t = 0.0f;
    _texture_data[8 * i + 7].s = 0.0f; _texture_data[8 * i + 7].t = 0.0f;
    
  }
  _initialised = true;
  return true;
}

void NPlane::shutdown() {
  if (_vertex_data) delete [] _vertex_data;
  if (_normal_data) delete [] _normal_data;
  if (_texture_data) delete []_texture_data;
  _initialised = false;
}

void NPlane::render(bool select_mode) {
  if (!_render) return;
  static float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  _render->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
  //TODO, should we use one texture for the whole model, or one per plane?
  if (select_mode) {
    RenderSystem::getInstance().switchTexture(RenderSystem::getInstance().requestTexture(_type, true));
  } else {
    RenderSystem::getInstance().switchTexture(RenderSystem::getInstance().requestTexture(_type));
  }
  _render->renderArrays(Graphics::RES_QUADS, 0, _num_planes * 8, _vertex_data, _texture_data, _normal_data,false);
}

} /* namespace Sear */
