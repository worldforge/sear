// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Workspace.h"

#include "gui/RootWindow.h"

#include "src/System.h"
#include "src/Graphics.h"
#include "src/Render.h"
#include "renderers/RenderSystem.h"

#include <sage/sage.h>
#include <sage/GL.h>

namespace Sear {

static const std::string WORKSPACE = "workspace";

Workspace::Workspace(System * system) : m_system(system),
                                        m_rootWindow(new RootWindow())
{
}

Workspace::~Workspace()
{
}

void Workspace::draw()
{
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState(WORKSPACE));
  Render *renderer = m_system->getGraphics()->getRender();
  renderer->setViewMode(ORTHOGRAPHIC);

  glEnableClientState(GL_VERTEX_ARRAY);

  const GLshort coords[] = {300, 300, 300, 400, 400, 400, 400, 300};
  glVertexPointer(2, GL_SHORT, 0, &coords[0]);

  renderer->setColour(1.0f, 1.0f, 1.0f, 0.5f);
  glDrawArrays(GL_QUADS, 0, 4);

  renderer->setColour(1.0f, 1.0f, 1.0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 4);

  glDisableClientState(GL_VERTEX_ARRAY);

  renderer->setViewMode(PERSPECTIVE);
}

} // namespace Sear
