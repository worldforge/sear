// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "common/Utility.h"

#include "src/System.h"
#include "src/Render.h"

#include "NPlane.h"

namespace Sear {

NPlane::NPlane(Render *render) : Model(render),
  _use_textures(true),
  _num_planes(0),
  _vertex_data(NULL),
  _normal_data(NULL),
  _texture_data(NULL)

{}

NPlane::~NPlane() {}
  
bool NPlane::init(const std::string &type, unsigned int num_planes, float width, float height) {
  _type = type;
  _num_planes = num_planes;
  float rads_per_segment = 2 * WFMath::Pi / (float)num_planes;
  _vertex_data = (float*)malloc(4 * num_planes * 3 * sizeof(float));
  _normal_data = (float*)malloc(4 * num_planes * 3 * sizeof(float));
  _texture_data = (float*)malloc(4 * num_planes * 2 * sizeof(float));
  float in[3][3];
  float out[3];
  for (unsigned int i = 0; i < num_planes; i++) {
    float x = width * cos ((float)i * rads_per_segment) / 2.0f;
    float y = width * sin ((float)i * rads_per_segment) / 2.0f;
    in[0][0] = _vertex_data[12 * i + 0] = x;
    in[0][1] = _vertex_data[12 * i + 1] = y;
    in[0][2] = _vertex_data[12 * i + 2] = 0.0f;
    in[1][0] = _vertex_data[12 * i + 3] = -x;
    in[1][1] = _vertex_data[12 * i + 4] = -y;
    in[1][2] = _vertex_data[12 * i + 5] = 0.0f;
    in[2][0] = _vertex_data[12 * i + 6] = -x;
    in[2][1] = _vertex_data[12 * i + 7] = -y;
    in[2][2] = _vertex_data[12 * i + 8] = height;
    _vertex_data[12 * i + 9] = x;
    _vertex_data[12 * i + 10] = y;
    _vertex_data[12 * i + 11] = height;
    calcNormal(in, out);
    for (unsigned int j = 0; j < 4; j++) {
      _normal_data[12 * i + 3 * j + 0] = out[0];
      _normal_data[12 * i + 3 * j + 1] = out[1];
      _normal_data[12 * i + 3 * j + 2] = out[2];
    }
   
    _texture_data[8 * i + 0] = 0.0f; _texture_data[8 * i + 1] = 0.0f;
    _texture_data[8 * i + 2] = 1.0f; _texture_data[8 * i + 3] = 0.0f;
    _texture_data[8 * i + 4] = 1.0f; _texture_data[8 * i + 5] = 1.0f;
    _texture_data[8 * i + 6] = 0.0f; _texture_data[8 * i + 7] = 1.0f;
    
  }
  return true;
}

void NPlane::shutdown() {
  if (_vertex_data) free(_vertex_data);
  if (_normal_data) free(_normal_data);
  if (_texture_data) free(_texture_data);
}

void NPlane::render(bool select_mode) {
  if (!_render) return;
  static float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  _render->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
  //TODO, should we use one texture for the whole model, or one per plane?
  if (select_mode) {
    _render->switchTexture(_render->requestTextureMask("nplane", _type, true));
  } else {
    _render->switchTexture(_render->requestTexture("nplane", _type, true));
  }
  _render->renderArrays(Graphics::RES_QUADS, 0, _num_planes * 4, _vertex_data, _texture_data, _normal_data);
}

} /* namespace Sear */
