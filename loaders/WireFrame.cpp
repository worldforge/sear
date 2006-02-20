// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: WireFrame.cpp,v 1.20 2006-02-20 20:16:21 simon Exp $

#include <sage/sage.h>
#include <sage/GL.h>

#include "src/System.h"
#include "renderers/RenderTypes.h"
#include "renderers/Render.h"
#include "renderers/RenderSystem.h"

#include "WireFrame.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

WireFrame::WireFrame() :
  Model(),
  m_initialised(false),
  m_disp(0)
{}
 
WireFrame::~WireFrame() {
  assert(m_initialised == false);

}
  
int WireFrame::init(WFMath::AxisBox<3> bbox) {
  assert(m_initialised == false);

  m_vertex_data[0].x = bbox.lowCorner().x(); m_vertex_data[0].y = bbox.highCorner().y(); m_vertex_data[0].z = bbox.lowCorner().z();
  m_vertex_data[1].x = bbox.lowCorner().x();m_vertex_data[1].y = bbox.lowCorner().y(); m_vertex_data[1].z = bbox.lowCorner().z();

  m_vertex_data[2].x = bbox.lowCorner().x(); m_vertex_data[2].y = bbox.lowCorner().y(); m_vertex_data[2].z = bbox.lowCorner().z();
  m_vertex_data[3].x = bbox.highCorner().x(); m_vertex_data[3].y = bbox.lowCorner().y(); m_vertex_data[3].z = bbox.lowCorner().z();

  m_vertex_data[4].x = bbox.highCorner().x(); m_vertex_data[4].y = bbox.lowCorner().y(); m_vertex_data[4].z = bbox.lowCorner().z();
  m_vertex_data[5].x = bbox.highCorner().x(); m_vertex_data[5].y = bbox.highCorner().y(); m_vertex_data[5].z = bbox.lowCorner().z();

  m_vertex_data[6].x = bbox.highCorner().x(); m_vertex_data[6].y = bbox.highCorner().y(); m_vertex_data[6].z = bbox.lowCorner().z();
  m_vertex_data[7].x = bbox.lowCorner().x(); m_vertex_data[7].y = bbox.highCorner().y(); m_vertex_data[7].z = bbox.lowCorner().z();
      //Top
  m_vertex_data[8].x = bbox.lowCorner().x(); m_vertex_data[8].y = bbox.lowCorner().y(); m_vertex_data[8].z = bbox.highCorner().z();
  m_vertex_data[9].x = bbox.lowCorner().x(); m_vertex_data[9].y = bbox.highCorner().y(); m_vertex_data[9].z = bbox.highCorner().z();

  m_vertex_data[10].x = bbox.lowCorner().x(); m_vertex_data[10].y = bbox.highCorner().y(); m_vertex_data[10].z = bbox.highCorner().z();
  m_vertex_data[11].x = bbox.highCorner().x(); m_vertex_data[11].y = bbox.highCorner().y(); m_vertex_data[11].z = bbox.highCorner().z();

  m_vertex_data[12].x = bbox.highCorner().x(); m_vertex_data[12].y = bbox.highCorner().y(); m_vertex_data[12].z = bbox.highCorner().z();
  m_vertex_data[13].x = bbox.highCorner().x(); m_vertex_data[13].y = bbox.lowCorner().y(); m_vertex_data[13].z = bbox.highCorner().z();

  m_vertex_data[14].x = bbox.highCorner().x(); m_vertex_data[14].y = bbox.lowCorner().y(); m_vertex_data[14].z = bbox.highCorner().z();
  m_vertex_data[15].x = bbox.lowCorner().x(); m_vertex_data[15].y = bbox.lowCorner().y(); m_vertex_data[15].z = bbox.highCorner().z();

      //Verticals
  m_vertex_data[16].x = bbox.lowCorner().x(); m_vertex_data[16].y = bbox.highCorner().y(); m_vertex_data[16].z = bbox.lowCorner().z();
  m_vertex_data[17].x = bbox.lowCorner().x(); m_vertex_data[17].y = bbox.highCorner().y(); m_vertex_data[17].z = bbox.highCorner().z();


  m_vertex_data[18].x = bbox.lowCorner().x(); m_vertex_data[18].y = bbox.lowCorner().y(); m_vertex_data[18].z = bbox.highCorner().z();
  m_vertex_data[19].x = bbox.lowCorner().x(); m_vertex_data[19].y = bbox.lowCorner().y(); m_vertex_data[19].z = bbox.lowCorner().z();


      
  m_vertex_data[20].x = bbox.highCorner().x(); m_vertex_data[20].y = bbox.lowCorner().y(); m_vertex_data[20].z = bbox.lowCorner().z();
  m_vertex_data[21].x = bbox.highCorner().x(); m_vertex_data[21].y = bbox.lowCorner().y(); m_vertex_data[21].z = bbox.highCorner().z();
      
      
  m_vertex_data[22].x = bbox.highCorner().x(); m_vertex_data[22].y = bbox.highCorner().y(); m_vertex_data[22].z = bbox.highCorner().z();
  m_vertex_data[23].x = bbox.highCorner().x(); m_vertex_data[23].y = bbox.highCorner().y(); m_vertex_data[23].z = bbox.lowCorner().z();
      
     
  m_vertex_data[24].x = bbox.highCorner().x(); m_vertex_data[24].y = bbox.highCorner().y(); m_vertex_data[24].z = bbox.lowCorner().z();
  m_vertex_data[25].x = bbox.highCorner().x(); m_vertex_data[25].y = bbox.highCorner().y(); m_vertex_data[25].z = bbox.highCorner().z();
     
     
  m_vertex_data[26].x = bbox.lowCorner().x(); m_vertex_data[26].y = bbox.highCorner().y(); m_vertex_data[26].z = bbox.highCorner().z();
  m_vertex_data[27].x = bbox.lowCorner().x(); m_vertex_data[27].y = bbox.highCorner().y(); m_vertex_data[27].z = bbox.lowCorner().z();
     


  m_vertex_data[28].x = bbox.lowCorner().x(); m_vertex_data[28].y = bbox.lowCorner().y(); m_vertex_data[28].z = bbox.lowCorner().z();
  m_vertex_data[29].x = bbox.lowCorner().x(); m_vertex_data[29].y = bbox.lowCorner().y(); m_vertex_data[29].z = bbox.highCorner().z();


  m_vertex_data[30].x = bbox.highCorner().x(); m_vertex_data[30].y = bbox.lowCorner().y(); m_vertex_data[30].z = bbox.highCorner().z();
  m_vertex_data[31].x = bbox.highCorner().x(); m_vertex_data[31].y = bbox.lowCorner().y(); m_vertex_data[31].z = bbox.lowCorner().z();

  m_initialised = true;    
  return 0;
}

int WireFrame::shutdown() {
  assert(m_initialised == true);

  contextDestroyed(true);
 
  m_initialised = false;
  return 0;
}

void WireFrame::contextCreated() {}

void WireFrame::contextDestroyed(bool check) {
  Render *render = RenderSystem::getInstance().getRenderer();
  assert(render);
  if (check) {
    if (glIsList(m_disp)) glDeleteLists(m_disp, 1);
  }
  m_disp = 0;
}

void WireFrame::render(bool) {
  Render *render = RenderSystem::getInstance().getRenderer();
  assert(render);
  static float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

  if (glIsList(m_disp)) {
    glCallList(m_disp);
  } else {
    glNewList(m_disp, GL_COMPILE_AND_EXECUTE);
    render->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
    render->renderArrays(RES_LINES, 0, m_num_points, &m_vertex_data[0], NULL, NULL, false);
    glEndList();
  }
}

} /* namespace Sear */
