// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Impostor.cpp,v 1.12 2002-09-07 23:27:06 simon Exp $

#include "src/System.h"
#include "src/Render.h"

#include "Impostor.h"

namespace Sear {

Impostor::Impostor(Render *render) : Model(render),
  _use_textures(true),
  _multi_textures(false),
  _initialised(false)

{}

Impostor::~Impostor() {
  if (_initialised) shutdown();
}
  
bool Impostor::init(const std::string &type, float _width, float _height, bool multi_textures) {
  if (_initialised) shutdown();
  _type = type;
  _multi_textures = multi_textures;
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
  _initialised = true;
  return true;
}

void Impostor::shutdown() {
  _type = "";
  _initialised = false;
}

void Impostor::render(bool select_mode) {
  static float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  _render->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
  if (!_multi_textures) {
    if (select_mode) {
      _render->switchTexture(_render->requestTextureMask("impostor", _type));
    } else {
      _render->switchTexture(_render->requestTexture("impostor", _type));
    }
    _render->renderArrays(Graphics::RES_QUADS, 0, _num_points, &_vertex_data[0][0], &_texture_data[0][0], &_normal_data[0][0]);
  } else {
    if (select_mode) {
      _render->switchTexture(_render->requestTextureMask("impostor_front", _type, true));
    } else {
      _render->switchTexture(_render->requestTexture("impostor_front", _type, true));
    }
    _render->renderArrays(Graphics::RES_QUADS, 0, 4, &_vertex_data[0][0], &_texture_data[0][0], &_normal_data[0][0]);

    if (select_mode) {
      _render->switchTexture(_render->requestTextureMask("impostor_side", _type, true));
    } else {
      _render->switchTexture(_render->requestTexture("impostor_side", _type, true));
    }
    _render->renderArrays(Graphics::RES_QUADS, 4, 4, &_vertex_data[0][0], &_texture_data[0][0], &_normal_data[0][0]);
  }
}

} /* namespace Sear */
