// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "WireFrame.h"

#include "../src/System.h"
#include "../src/Render.h"

namespace Sear {

WireFrame::WireFrame() {}
 
WireFrame::~WireFrame() {}
  
bool WireFrame::init(WFMath::AxisBox<3> _bbox) {
  _vertex_data[0][0] = _bbox.lowCorner().x(); _vertex_data[0][1] = _bbox.highCorner().y(); _vertex_data[0][2] = _bbox.lowCorner().z();
  _vertex_data[1][0] = _bbox.lowCorner().x();_vertex_data[1][1] = _bbox.lowCorner().y(); _vertex_data[1][2] = _bbox.lowCorner().z();

  _vertex_data[2][0] = _bbox.lowCorner().x(); _vertex_data[2][1] = _bbox.lowCorner().y(); _vertex_data[2][2] = _bbox.lowCorner().z();
  _vertex_data[3][0] = _bbox.highCorner().x(); _vertex_data[3][1] = _bbox.lowCorner().y(); _vertex_data[3][2] = _bbox.lowCorner().z();

  _vertex_data[4][0] = _bbox.highCorner().x(); _vertex_data[4][1] = _bbox.lowCorner().y(); _vertex_data[4][2] = _bbox.lowCorner().z();
  _vertex_data[5][0] = _bbox.highCorner().x(); _vertex_data[5][1] = _bbox.highCorner().y(); _vertex_data[5][2] = _bbox.lowCorner().z();

  _vertex_data[6][0] = _bbox.highCorner().x(); _vertex_data[6][1] = _bbox.highCorner().y(); _vertex_data[6][2] = _bbox.lowCorner().z();
  _vertex_data[7][0] = _bbox.lowCorner().x(); _vertex_data[7][1] = _bbox.highCorner().y(); _vertex_data[7][2] = _bbox.lowCorner().z();
      //Top
  _vertex_data[8][0] = _bbox.lowCorner().x(); _vertex_data[8][1] = _bbox.lowCorner().y(); _vertex_data[8][2] = _bbox.highCorner().z();
  _vertex_data[9][0] = _bbox.lowCorner().x(); _vertex_data[9][1] = _bbox.highCorner().y(); _vertex_data[9][2] = _bbox.highCorner().z();

  _vertex_data[10][0] = _bbox.lowCorner().x(); _vertex_data[10][1] = _bbox.highCorner().y(); _vertex_data[10][2] = _bbox.highCorner().z();
  _vertex_data[11][0] = _bbox.highCorner().x(); _vertex_data[11][1] = _bbox.highCorner().y(); _vertex_data[11][2] = _bbox.highCorner().z();

  _vertex_data[12][0] = _bbox.highCorner().x(); _vertex_data[12][1] = _bbox.highCorner().y(); _vertex_data[12][2] = _bbox.highCorner().z();
  _vertex_data[13][0] = _bbox.highCorner().x(); _vertex_data[13][1] = _bbox.lowCorner().y(); _vertex_data[13][2] = _bbox.highCorner().z();

  _vertex_data[14][0] = _bbox.highCorner().x(); _vertex_data[14][1] = _bbox.lowCorner().y(); _vertex_data[14][2] = _bbox.highCorner().z();
  _vertex_data[15][0] = _bbox.lowCorner().x(); _vertex_data[15][1] = _bbox.lowCorner().y(); _vertex_data[15][2] = _bbox.highCorner().z();

      //Verticals
  _vertex_data[16][0] = _bbox.lowCorner().x(); _vertex_data[16][1] = _bbox.highCorner().y(); _vertex_data[16][2] = _bbox.lowCorner().z();
  _vertex_data[17][0] = _bbox.lowCorner().x(); _vertex_data[17][1] = _bbox.highCorner().y(); _vertex_data[17][2] = _bbox.highCorner().z();


  _vertex_data[18][0] = _bbox.lowCorner().x(); _vertex_data[18][1] = _bbox.lowCorner().y(); _vertex_data[18][2] = _bbox.highCorner().z();
  _vertex_data[19][0] = _bbox.lowCorner().x(); _vertex_data[19][1] = _bbox.lowCorner().y(); _vertex_data[19][2] = _bbox.lowCorner().z();


      
  _vertex_data[20][0] = _bbox.highCorner().x(); _vertex_data[20][1] = _bbox.lowCorner().y(); _vertex_data[20][2] = _bbox.lowCorner().z();
  _vertex_data[21][0] = _bbox.highCorner().x(); _vertex_data[21][1] = _bbox.lowCorner().y(); _vertex_data[21][2] = _bbox.highCorner().z();
      
      
  _vertex_data[22][0] = _bbox.highCorner().x(); _vertex_data[22][1] = _bbox.highCorner().y(); _vertex_data[22][2] = _bbox.highCorner().z();
  _vertex_data[23][0] = _bbox.highCorner().x(); _vertex_data[23][1] = _bbox.highCorner().y(); _vertex_data[23][2] = _bbox.lowCorner().z();
      
     
  _vertex_data[24][0] = _bbox.highCorner().x(); _vertex_data[24][1] = _bbox.highCorner().y(); _vertex_data[24][2] = _bbox.lowCorner().z();
  _vertex_data[25][0] = _bbox.highCorner().x(); _vertex_data[25][1] = _bbox.highCorner().y(); _vertex_data[25][2] = _bbox.highCorner().z();
     
     
  _vertex_data[26][0] = _bbox.lowCorner().x(); _vertex_data[26][1] = _bbox.highCorner().y(); _vertex_data[26][2] = _bbox.highCorner().z();
  _vertex_data[27][0] = _bbox.lowCorner().x(); _vertex_data[27][1] = _bbox.highCorner().y(); _vertex_data[27][2] = _bbox.lowCorner().z();
     


  _vertex_data[28][0] = _bbox.lowCorner().x(); _vertex_data[28][1] = _bbox.lowCorner().y(); _vertex_data[28][2] = _bbox.lowCorner().z();
  _vertex_data[29][0] = _bbox.lowCorner().x(); _vertex_data[29][1] = _bbox.lowCorner().y(); _vertex_data[29][2] = _bbox.highCorner().z();


  _vertex_data[30][0] = _bbox.highCorner().x(); _vertex_data[30][1] = _bbox.lowCorner().y(); _vertex_data[30][2] = _bbox.highCorner().z();
  _vertex_data[31][0] = _bbox.highCorner().x(); _vertex_data[31][1] = _bbox.lowCorner().y(); _vertex_data[31][2] = _bbox.lowCorner().z();

      
  return true;
}

void WireFrame::shutdown() {
}

void WireFrame::render(bool) {
  static Render *render = System::instance()->getRenderer();
  render->renderArrays(Models::LINES, _num_points, &_vertex_data[0][0], NULL, NULL);
}

} /* namespace Sear */
