// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Workspace.h"

#include "gui/RootWindow.h"

#include "src/System.h"
#include "src/Graphics.h"
#include "src/Render.h"

#include <sage/sage.h>
#include <sage/GL.h>

namespace Sear {

Workspace::Workspace(System * system) : m_system(system),
                                        m_rootWindow(new RootWindow())
{
}

Workspace::~Workspace()
{
}

void Workspace::draw()
{
  Render *renderer = m_system->getGraphics()->getRender();
  renderer->setViewMode(ORTHOGRAPHIC);
  glColor3f(1,1,1);
  glBegin(GL_TRIANGLES);
  glVertex3f(0,20,0);
  glVertex3f(0,0,0);
  glVertex3f(20,0,0);
  glEnd();
  renderer->setViewMode(PERSPECTIVE);
}

} // namespace Sear
