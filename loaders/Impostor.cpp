// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "Impostor.h"
#include "../renderers/GL.h"

namespace Sear {

Impostor::Impostor() :
  _use_textures(true)

{}

Impostor::~Impostor() {}
  
bool Impostor::init(const std::string &type, float _width, float _height) {
  _type = type;
  if (!_width) _width = 2.0f;
  if (!_height) _height = 2.0f;
  float width_by_2 = _width / 2.0f;
  _vertex_data[0][0] = -width_by_2; _vertex_data[0][1] = 0.0f; _vertex_data[0][2] = 0.0f;
  _vertex_data[1][0] = -width_by_2; _vertex_data[1][1] = 0.0f; _vertex_data[1][2] = _height;
  _vertex_data[2][0] =  width_by_2; _vertex_data[2][1] = 0.0f; _vertex_data[2][2] = _height;
  _vertex_data[3][0] =  width_by_2; _vertex_data[3][1] = 0.0f; _vertex_data[3][2] = 0.0f;
  _vertex_data[4][0] = 0.0f; _vertex_data[4][1] = -width_by_2; _vertex_data[4][2] = 0.0f;
  _vertex_data[5][0] = 0.0f; _vertex_data[5][1] = -width_by_2; _vertex_data[5][2] = _height;
  _vertex_data[6][0] = 0.0f; _vertex_data[6][1] = width_by_2; _vertex_data[6][2] = _height;
  _vertex_data[7][0] = 0.0f; _vertex_data[7][1] = width_by_2; _vertex_data[7][2] = 0.0f;

  _texture_data[0][0] = 0.0f; _texture_data[0][1] = 0.0f;
  _texture_data[1][0] = 0.0f; _texture_data[1][1] = 1.0f;
  _texture_data[2][0] = 1.0f; _texture_data[2][1] = 1.0f;
  _texture_data[3][0] = 1.0f; _texture_data[3][1] = 0.0f;
  _texture_data[4][0] = 0.0f; _texture_data[4][1] = 0.0f;
  _texture_data[5][0] = 0.0f; _texture_data[5][1] = 1.0f;
  _texture_data[6][0] = 1.0f; _texture_data[6][1] = 1.0f;
  _texture_data[7][0] = 1.0f; _texture_data[7][1] = 0.0f;

  _normal_data[0][1] = 0.0f; _normal_data[0][2] = -1.0f; _normal_data[0][2] = 0.0f;
  _normal_data[1][1] = 0.0f; _normal_data[1][2] = -1.0f; _normal_data[1][2] = 0.0f;
  _normal_data[2][1] = 0.0f; _normal_data[2][2] = -1.0f; _normal_data[2][2] = 0.0f;
  _normal_data[3][1] = 0.0f; _normal_data[3][2] = -1.0f; _normal_data[3][2] = 0.0f;
  _normal_data[4][1] = 1.0f; _normal_data[4][2] = 0.0f; _normal_data[4][2] = 0.0f;
  _normal_data[5][1] = 1.0f; _normal_data[5][2] = 0.0f; _normal_data[5][2] = 0.0f;
  _normal_data[6][1] = 1.0f; _normal_data[6][2] = 0.0f; _normal_data[6][2] = 0.0f;
  _normal_data[7][1] = 1.0f; _normal_data[7][2] = 0.0f; _normal_data[7][2] = 0.0f;
  return true;
}

void Impostor::shutdown() {

}

void Impostor::render(bool select_mode) {
  if (select_mode) {
    GL::instance()->switchTexture(GL::instance()->requestTextureMask(_type));
  } else {
    GL::instance()->switchTexture(GL::instance()->requestTexture(_type));
  }
  GL::instance()->renderArrays(Models::QUADS, _num_points, &_vertex_data[0][0], &_texture_data[0][0], &_normal_data[0][0]);

}

} /* namespace Sear */
