// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "src/System.h"
#include "src/Graphics.h"
#include "src/Render.h"

#include "BoundBox.h"

namespace Sear {

BoundBox::BoundBox(Render *render) : Model(render), 
  _type("default"),
  _use_textures(true)
{}

BoundBox::~BoundBox() {}
  
bool BoundBox::init(WFMath::AxisBox<3> _bbox, const std::string &type, bool _wrap) {
  _type = type;
  _vertex_data[0][0] = _bbox.lowCorner().x(); _vertex_data[0][1] = _bbox.highCorner().y(); _vertex_data[0][2] = _bbox.lowCorner().z();
  _vertex_data[1][0] = _bbox.lowCorner().x(); _vertex_data[1][1] = _bbox.lowCorner().y(); _vertex_data[1][2] = _bbox.lowCorner().z();
  _vertex_data[2][0] = _bbox.highCorner().x(); _vertex_data[2][1] = _bbox.lowCorner().y(); _vertex_data[2][2] = _bbox.lowCorner().z();
  _vertex_data[3][0] = _bbox.highCorner().x(); _vertex_data[3][1] = _bbox.highCorner().y(); _vertex_data[3][2] = _bbox.lowCorner().z();

  _vertex_data[4][0] = _bbox.lowCorner().x(); _vertex_data[4][1] = _bbox.lowCorner().y(); _vertex_data[4][2] = _bbox.highCorner().z();
  _vertex_data[5][0] = _bbox.lowCorner().x(); _vertex_data[5][1] = _bbox.highCorner().y(); _vertex_data[5][2] = _bbox.highCorner().z();
  _vertex_data[6][0] = _bbox.highCorner().x(); _vertex_data[6][1] = _bbox.highCorner().y(); _vertex_data[6][2] = _bbox.highCorner().z();
  _vertex_data[7][0] = _bbox.highCorner().x(); _vertex_data[7][1] = _bbox.lowCorner().y(); _vertex_data[7][2] = _bbox.highCorner().z();

  _vertex_data[8][0] = _bbox.lowCorner().x(); _vertex_data[8][1] = _bbox.highCorner().y(); _vertex_data[8][2] = _bbox.lowCorner().z();
  _vertex_data[9][0] = _bbox.lowCorner().x(); _vertex_data[9][1] = _bbox.highCorner().y(); _vertex_data[9][2] = _bbox.highCorner().z();
  _vertex_data[10][0] = _bbox.lowCorner().x(); _vertex_data[10][1] = _bbox.lowCorner().y(); _vertex_data[10][2] = _bbox.highCorner().z();
  _vertex_data[11][0] = _bbox.lowCorner().x(); _vertex_data[11][1] = _bbox.lowCorner().y(); _vertex_data[11][2] = _bbox.lowCorner().z();

  _vertex_data[12][0] = _bbox.highCorner().x(); _vertex_data[12][1] = _bbox.lowCorner().y(); _vertex_data[12][2] = _bbox.lowCorner().z();
  _vertex_data[13][0] = _bbox.highCorner().x(); _vertex_data[13][1] = _bbox.lowCorner().y(); _vertex_data[13][2] = _bbox.highCorner().z();
  _vertex_data[14][0] = _bbox.highCorner().x(); _vertex_data[14][1] = _bbox.highCorner().y(); _vertex_data[14][2] = _bbox.highCorner().z();
  _vertex_data[15][0] = _bbox.highCorner().x(); _vertex_data[15][1] = _bbox.highCorner().y(); _vertex_data[15][2] = _bbox.lowCorner().z();

  _vertex_data[16][0] = _bbox.highCorner().x(); _vertex_data[16][1] = _bbox.highCorner().y(); _vertex_data[16][2] = _bbox.lowCorner().z();
  _vertex_data[17][0] = _bbox.highCorner().x(); _vertex_data[17][1] = _bbox.highCorner().y(); _vertex_data[17][2] = _bbox.highCorner().z();
  _vertex_data[18][0] = _bbox.lowCorner().x();  _vertex_data[18][1] = _bbox.highCorner().y(); _vertex_data[18][2] = _bbox.highCorner().z();
  _vertex_data[19][0] = _bbox.lowCorner().x();  _vertex_data[19][1] = _bbox.highCorner().y(); _vertex_data[19][2] = _bbox.lowCorner().z();

  _vertex_data[20][0] = _bbox.lowCorner().x();  _vertex_data[20][1] = _bbox.lowCorner().y(); _vertex_data[20][2] = _bbox.lowCorner().z();
  _vertex_data[21][0] = _bbox.lowCorner().x(); _vertex_data[21][1] = _bbox.lowCorner().y(); _vertex_data[21][2] = _bbox.highCorner().z();
  _vertex_data[22][0] = _bbox.highCorner().x(); _vertex_data[22][1] = _bbox.lowCorner().y(); _vertex_data[22][2] = _bbox.highCorner().z();
  _vertex_data[23][0] = _bbox.highCorner().x(); _vertex_data[23][1] = _bbox.lowCorner().y(); _vertex_data[23][2] = _bbox.lowCorner().z();
  if (!_wrap) {
    _texture_data[0][0] = 0.0f; _texture_data[0][1] = 0.0f;
    _texture_data[1][0] = 0.0f; _texture_data[1][1] = 1.0f;
    _texture_data[2][0] = 1.0f; _texture_data[2][1] = 1.0f;
    _texture_data[3][0] = 1.0f; _texture_data[3][1] = 0.0f;
  
    _texture_data[4][0] = 0.0f; _texture_data[4][1] = 0.0f;
    _texture_data[5][0] = 0.0f; _texture_data[5][1] = 1.0f;
    _texture_data[6][0] = 1.0f; _texture_data[6][1] = 1.0f;
    _texture_data[7][0] = 1.0f; _texture_data[7][1] = 0.0f;

    _texture_data[8][0] = 0.0f; _texture_data[8][1] = 0.0f;
    _texture_data[9][0] = 0.0f; _texture_data[9][1] = 1.0f;
    _texture_data[10][0] = 1.0f; _texture_data[10][1] = 1.0f;
    _texture_data[11][0] = 1.0f; _texture_data[11][1] = 0.0f;

    _texture_data[12][0] = 0.0f; _texture_data[12][1] = 0.0f;
    _texture_data[13][0] = 0.0f; _texture_data[13][1] = 1.0f;
    _texture_data[14][0] = 1.0f; _texture_data[14][1] = 1.0f;
    _texture_data[15][0] = 1.0f; _texture_data[15][1] = 0.0f;

    _texture_data[16][0] = 0.0f; _texture_data[16][1] = 0.0f;
    _texture_data[17][0] = 0.0f; _texture_data[17][1] = 1.0f;
    _texture_data[18][0] = 1.0f; _texture_data[18][1] = 1.0f;
    _texture_data[19][0] = 1.0f; _texture_data[19][1] = 0.0f;

    _texture_data[20][0] = 0.0f; _texture_data[20][1] = 0.0f;
    _texture_data[21][0] = 0.0f; _texture_data[21][1] = 1.0f;
    _texture_data[22][0] = 1.0f; _texture_data[22][1] = 1.0f;
    _texture_data[23][0] = 1.0f; _texture_data[23][1] = 0.0f;
  } else {
    _texture_data[0][0] = _bbox.lowCorner().x(); _texture_data[0][1] = _bbox.highCorner().y();
    _texture_data[1][0] = _bbox.lowCorner().x(); _texture_data[1][1] = _bbox.lowCorner().y();
    _texture_data[2][0] = _bbox.highCorner().x(); _texture_data[2][1] = _bbox.lowCorner().y();
    _texture_data[3][0] = _bbox.highCorner().x(); _texture_data[3][1] = _bbox.highCorner().y();
      
    _texture_data[4][0] = _bbox.lowCorner().x(); _texture_data[4][1] = _bbox.lowCorner().y();
    _texture_data[5][0] = _bbox.lowCorner().x(); _texture_data[5][1] = _bbox.highCorner().y();
    _texture_data[6][0] = _bbox.highCorner().x(); _texture_data[6][1] = _bbox.highCorner().y();
    _texture_data[7][0] = _bbox.highCorner().x(); _texture_data[7][1] = _bbox.lowCorner().y();
	      
    _texture_data[8][0] = _bbox.highCorner().y(); _texture_data[8][1] = _bbox.lowCorner().z();
    _texture_data[9][0] = _bbox.highCorner().y(); _texture_data[9][1] = _bbox.highCorner().z();
    _texture_data[10][0] = _bbox.lowCorner().y(); _texture_data[10][1] = _bbox.highCorner().z();
    _texture_data[11][0] = _bbox.lowCorner().y(); _texture_data[11][1] = _bbox.lowCorner().z();

    _texture_data[12][0] = _bbox.lowCorner().y(); _texture_data[12][1] = _bbox.lowCorner().z();      
    _texture_data[13][0] = _bbox.lowCorner().y(); _texture_data[13][1] = _bbox.highCorner().z();
    _texture_data[14][0] = _bbox.highCorner().y(); _texture_data[14][1] = _bbox.highCorner().z();
    _texture_data[15][0] = _bbox.highCorner().y(); _texture_data[15][1] = _bbox.lowCorner().z();

    _texture_data[16][0] = _bbox.highCorner().x(); _texture_data[16][1] = _bbox.lowCorner().z();
    _texture_data[17][0] = _bbox.highCorner().x(); _texture_data[17][1] = _bbox.highCorner().z();
    _texture_data[18][0] = _bbox.lowCorner().x(); _texture_data[18][1] = _bbox.highCorner().z();
    _texture_data[19][0] = _bbox.lowCorner().x(); _texture_data[19][1] = _bbox.lowCorner().z();

    _texture_data[20][0] = _bbox.lowCorner().x(); _texture_data[20][1] = _bbox.lowCorner().z();
    _texture_data[21][0] = _bbox.lowCorner().x(); _texture_data[21][1] = _bbox.highCorner().z();
    _texture_data[22][0] = _bbox.highCorner().x(); _texture_data[22][1] = _bbox.highCorner().z();
    _texture_data[23][0] = _bbox.highCorner().x(); _texture_data[23][1] = _bbox.lowCorner().z();
  }
  _normal_data[0][0] =  0.0f; _normal_data[0][1] =  0.0f; _normal_data[0][2] = -1.0f;
  _normal_data[1][0] =  0.0f; _normal_data[1][1] =  0.0f; _normal_data[1][2] = -1.0f;
  _normal_data[2][0] =  0.0f; _normal_data[2][1] =  0.0f; _normal_data[2][2] = -1.0f;
  _normal_data[3][0] =  0.0f; _normal_data[3][1] =  0.0f; _normal_data[3][2] = -1.0f;
  _normal_data[4][0] =  0.0f; _normal_data[4][1] =  0.0f; _normal_data[4][2] =  1.0f;
  _normal_data[5][0] =  0.0f; _normal_data[5][1] =  0.0f; _normal_data[5][2] =  1.0f;
  _normal_data[6][0] =  0.0f; _normal_data[6][1] =  0.0f; _normal_data[6][2] =  1.0f;
  _normal_data[7][0] =  0.0f; _normal_data[7][1] =  0.0f; _normal_data[7][2] =  1.0f;
  _normal_data[8][0] = -1.0f; _normal_data[8][1] =  0.0f; _normal_data[8][2] =  0.0f;
  _normal_data[9][0] = -1.0f; _normal_data[9][1] =  0.0f; _normal_data[9][2] =  0.0f;
  _normal_data[10][0] = -1.0f; _normal_data[10][1] =  0.0f; _normal_data[10][2] =  0.0f;
  _normal_data[11][0] = -1.0f; _normal_data[11][1] =  0.0f; _normal_data[11][2] =  0.0f;
  _normal_data[12][0] =  1.0f; _normal_data[12][1] =  0.0f; _normal_data[12][2] =  0.0f;
  _normal_data[13][0] =  1.0f; _normal_data[13][1] =  0.0f; _normal_data[13][2] =  0.0f;
  _normal_data[14][0] =  1.0f; _normal_data[14][1] =  0.0f; _normal_data[14][2] =  0.0f;
  _normal_data[15][0] =  1.0f; _normal_data[15][1] =  0.0f; _normal_data[15][2] =  0.0f;
  _normal_data[16][0] =  0.0f; _normal_data[16][1] =  1.0f; _normal_data[16][2] =  0.0f;
  _normal_data[17][0] =  0.0f; _normal_data[17][1] =  1.0f; _normal_data[17][2] =  0.0f;
  _normal_data[18][0] =  0.0f; _normal_data[18][1] =  1.0f; _normal_data[18][2] =  0.0f;
  _normal_data[19][0] =  0.0f; _normal_data[19][1] =  1.0f; _normal_data[19][2] =  0.0f;
  _normal_data[20][0] =  0.0f; _normal_data[20][1] = -1.0f; _normal_data[20][2] =  0.0f;
  _normal_data[21][0] =  0.0f; _normal_data[21][1] = -1.0f; _normal_data[21][2] =  0.0f;
  _normal_data[22][0] =  0.0f; _normal_data[22][1] = -1.0f; _normal_data[22][2] =  0.0f;
  _normal_data[23][0] =  0.0f; _normal_data[23][1] = -1.0f; _normal_data[23][2] =  0.0f;
  return true;
}

void BoundBox::shutdown() {

}

void BoundBox::render(bool select_mode) {
  if (!_render) return;
  static float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  _render->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
  if (select_mode) {
    _render->renderArrays(Graphics::RES_QUADS, 0, _num_points, &_vertex_data[0][0], NULL, NULL);
  } else {
    _render->switchTexture(_render->requestTexture("boundbox", _type));
    _render->renderArrays(Graphics::RES_QUADS, 0, _num_points, &_vertex_data[0][0], &_texture_data[0][0], &_normal_data[0][0]);
  }
}

} /* namespace Sear */
