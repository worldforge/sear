// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: WireFrame.cpp,v 1.13 2004-04-27 15:07:01 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "src/System.h"
#include "src/Graphics.h"
#include "src/Render.h"

#include "WireFrame.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

WireFrame::WireFrame(Render *render) :
  Model(render),
  _initialised(false)
{}
 
WireFrame::~WireFrame() {
  if (_initialised) shutdown();
}
  
bool WireFrame::init(WFMath::AxisBox<3> _bbox) {
  if (_initialised) shutdown();
  _vertex_data[0].x = _bbox.lowCorner().x(); _vertex_data[0].y = _bbox.highCorner().y(); _vertex_data[0].z = _bbox.lowCorner().z();
  _vertex_data[1].x = _bbox.lowCorner().x();_vertex_data[1].y = _bbox.lowCorner().y(); _vertex_data[1].z = _bbox.lowCorner().z();

  _vertex_data[2].x = _bbox.lowCorner().x(); _vertex_data[2].y = _bbox.lowCorner().y(); _vertex_data[2].z = _bbox.lowCorner().z();
  _vertex_data[3].x = _bbox.highCorner().x(); _vertex_data[3].y = _bbox.lowCorner().y(); _vertex_data[3].z = _bbox.lowCorner().z();

  _vertex_data[4].x = _bbox.highCorner().x(); _vertex_data[4].y = _bbox.lowCorner().y(); _vertex_data[4].z = _bbox.lowCorner().z();
  _vertex_data[5].x = _bbox.highCorner().x(); _vertex_data[5].y = _bbox.highCorner().y(); _vertex_data[5].z = _bbox.lowCorner().z();

  _vertex_data[6].x = _bbox.highCorner().x(); _vertex_data[6].y = _bbox.highCorner().y(); _vertex_data[6].z = _bbox.lowCorner().z();
  _vertex_data[7].x = _bbox.lowCorner().x(); _vertex_data[7].y = _bbox.highCorner().y(); _vertex_data[7].z = _bbox.lowCorner().z();
      //Top
  _vertex_data[8].x = _bbox.lowCorner().x(); _vertex_data[8].y = _bbox.lowCorner().y(); _vertex_data[8].z = _bbox.highCorner().z();
  _vertex_data[9].x = _bbox.lowCorner().x(); _vertex_data[9].y = _bbox.highCorner().y(); _vertex_data[9].z = _bbox.highCorner().z();

  _vertex_data[10].x = _bbox.lowCorner().x(); _vertex_data[10].y = _bbox.highCorner().y(); _vertex_data[10].z = _bbox.highCorner().z();
  _vertex_data[11].x = _bbox.highCorner().x(); _vertex_data[11].y = _bbox.highCorner().y(); _vertex_data[11].z = _bbox.highCorner().z();

  _vertex_data[12].x = _bbox.highCorner().x(); _vertex_data[12].y = _bbox.highCorner().y(); _vertex_data[12].z = _bbox.highCorner().z();
  _vertex_data[13].x = _bbox.highCorner().x(); _vertex_data[13].y = _bbox.lowCorner().y(); _vertex_data[13].z = _bbox.highCorner().z();

  _vertex_data[14].x = _bbox.highCorner().x(); _vertex_data[14].y = _bbox.lowCorner().y(); _vertex_data[14].z = _bbox.highCorner().z();
  _vertex_data[15].x = _bbox.lowCorner().x(); _vertex_data[15].y = _bbox.lowCorner().y(); _vertex_data[15].z = _bbox.highCorner().z();

      //Verticals
  _vertex_data[16].x = _bbox.lowCorner().x(); _vertex_data[16].y = _bbox.highCorner().y(); _vertex_data[16].z = _bbox.lowCorner().z();
  _vertex_data[17].x = _bbox.lowCorner().x(); _vertex_data[17].y = _bbox.highCorner().y(); _vertex_data[17].z = _bbox.highCorner().z();


  _vertex_data[18].x = _bbox.lowCorner().x(); _vertex_data[18].y = _bbox.lowCorner().y(); _vertex_data[18].z = _bbox.highCorner().z();
  _vertex_data[19].x = _bbox.lowCorner().x(); _vertex_data[19].y = _bbox.lowCorner().y(); _vertex_data[19].z = _bbox.lowCorner().z();


      
  _vertex_data[20].x = _bbox.highCorner().x(); _vertex_data[20].y = _bbox.lowCorner().y(); _vertex_data[20].z = _bbox.lowCorner().z();
  _vertex_data[21].x = _bbox.highCorner().x(); _vertex_data[21].y = _bbox.lowCorner().y(); _vertex_data[21].z = _bbox.highCorner().z();
      
      
  _vertex_data[22].x = _bbox.highCorner().x(); _vertex_data[22].y = _bbox.highCorner().y(); _vertex_data[22].z = _bbox.highCorner().z();
  _vertex_data[23].x = _bbox.highCorner().x(); _vertex_data[23].y = _bbox.highCorner().y(); _vertex_data[23].z = _bbox.lowCorner().z();
      
     
  _vertex_data[24].x = _bbox.highCorner().x(); _vertex_data[24].y = _bbox.highCorner().y(); _vertex_data[24].z = _bbox.lowCorner().z();
  _vertex_data[25].x = _bbox.highCorner().x(); _vertex_data[25].y = _bbox.highCorner().y(); _vertex_data[25].z = _bbox.highCorner().z();
     
     
  _vertex_data[26].x = _bbox.lowCorner().x(); _vertex_data[26].y = _bbox.highCorner().y(); _vertex_data[26].z = _bbox.highCorner().z();
  _vertex_data[27].x = _bbox.lowCorner().x(); _vertex_data[27].y = _bbox.highCorner().y(); _vertex_data[27].z = _bbox.lowCorner().z();
     


  _vertex_data[28].x = _bbox.lowCorner().x(); _vertex_data[28].y = _bbox.lowCorner().y(); _vertex_data[28].z = _bbox.lowCorner().z();
  _vertex_data[29].x = _bbox.lowCorner().x(); _vertex_data[29].y = _bbox.lowCorner().y(); _vertex_data[29].z = _bbox.highCorner().z();


  _vertex_data[30].x = _bbox.highCorner().x(); _vertex_data[30].y = _bbox.lowCorner().y(); _vertex_data[30].z = _bbox.highCorner().z();
  _vertex_data[31].x = _bbox.highCorner().x(); _vertex_data[31].y = _bbox.lowCorner().y(); _vertex_data[31].z = _bbox.lowCorner().z();

  _initialised = true;    
  return true;
}

void WireFrame::shutdown() {
  _initialised = false;
}

void WireFrame::render(bool) {
  static float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  _render->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
  _render->renderArrays(Graphics::RES_LINES, 0, _num_points, &_vertex_data[0], NULL, NULL, false);
}

} /* namespace Sear */
