// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: NPlane.cpp,v 1.11 2002-10-09 17:13:39 alriddoch Exp $

#include "common/Utility.h"

#include "src/System.h"
#include "src/Render.h"

#include "NPlane.h"

#include <iostream>

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
  _vertex_data = (float*)malloc(8 * num_planes * 3 * sizeof(float));
  _normal_data = (float*)malloc(8 * num_planes * 3 * sizeof(float));
  _texture_data = (float*)malloc(8 * num_planes * 2 * sizeof(float));
  float in[3][3];
  float out[3];
  for (unsigned int i = 0; i < num_planes; i++) {
    float x = width * cos ((float)i * rads_per_segment) / 2.0f;
    float y = width * sin ((float)i * rads_per_segment) / 2.0f;
    std::cout << "Plane: " << i << " X: " << x << " Y: " << y <<std::endl;
    in[0][0] = _vertex_data[24 * i + 0] = x;
    in[0][1] = _vertex_data[24 * i + 1] = y;
    in[0][2] = _vertex_data[24 * i + 2] = 0.0f;
    in[1][0] = _vertex_data[24 * i + 3] = -x;
    in[1][1] = _vertex_data[24 * i + 4] = -y;
    in[1][2] = _vertex_data[24 * i + 5] = 0.0f;
    in[2][0] = _vertex_data[24 * i + 6] = -x;
    in[2][1] = _vertex_data[24 * i + 7] = -y;
    in[2][2] = _vertex_data[24 * i + 8] = height;
    _vertex_data[24 * i + 9] = x;
    _vertex_data[24 * i + 10] = y;
    _vertex_data[24 * i + 11] = height;

    _vertex_data[24 * i + 12] = x;
    _vertex_data[24 * i + 13] = y;
    _vertex_data[24 * i + 14] = height;
    _vertex_data[24 * i + 15] = -x;
    _vertex_data[24 * i + 16] = -y;
    _vertex_data[24 * i + 17] = height;
    _vertex_data[24 * i + 18] = -x;
    _vertex_data[24 * i + 19] = -y;
    _vertex_data[24 * i + 20] = 0.0f;
    _vertex_data[24 * i + 21] = x;
    _vertex_data[24 * i + 22] = y;
    _vertex_data[24 * i + 23] = 0.0f;
    calcNormal(in, out);
    for (unsigned int j = 0; j < 4; ++j) {
      _normal_data[24 * i + 6 * j + 0] = out[0];
      _normal_data[24 * i + 6 * j + 1] = out[1];
      _normal_data[24 * i + 6 * j + 2] = out[2];
      _normal_data[24 * i + 6 * j + 3] = _normal_data[24 * i + 6 * j + 0];
      _normal_data[24 * i + 6 * j + 4] = _normal_data[24 * i + 6 * j + 1];
      _normal_data[24 * i + 6 * j + 5] = _normal_data[24 * i + 6 * j + 2]; 
    }
   
    _texture_data[16 * i + 0] = 0.0f; _texture_data[16 * i + 1] = 0.0f;
    _texture_data[16 * i + 2] = 1.0f; _texture_data[16 * i + 3] = 0.0f;
    _texture_data[16 * i + 4] = 1.0f; _texture_data[16 * i + 5] = 1.0f;
    _texture_data[16 * i + 6] = 0.0f; _texture_data[16 * i + 7] = 1.0f;

    _texture_data[16 * i + 8] =  0.0f; _texture_data[16 * i + 9] =  1.0f;
    _texture_data[16 * i + 10] = 1.0f; _texture_data[16 * i + 11] = 1.0f;
    _texture_data[16 * i + 12] = 1.0f; _texture_data[16 * i + 13] = 0.0f;
    _texture_data[16 * i + 14] = 0.0f; _texture_data[16 * i + 15] = 0.0f;
    
  }
  _initialised = true;
  return true;
}

void NPlane::shutdown() {
  if (_vertex_data) free(_vertex_data);
  if (_normal_data) free(_normal_data);
  if (_texture_data) free(_texture_data);
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
    _render->switchTexture(_render->requestMipMapMask("nplane", _type, true));
  } else {
    _render->switchTexture(_render->requestMipMap("nplane", _type, true));
  }
  _render->renderArrays(Graphics::RES_QUADS, 0, _num_planes * 8, _vertex_data, _texture_data, _normal_data);
}

} /* namespace Sear */
