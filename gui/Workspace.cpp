// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Workspace.h"

#include "gui/RootWindow.h"
#include "gui/Frame.h"

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
    Frame * f = new Frame();

    m_rootWindow->addChild(f);
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
  glLineWidth(1.0f);

#if 0
  static const GLshort coords[] = {300, 300, 300, 400, 400, 400, 400, 300};
  static const GLushort line_i[] = {0, 1, 2, 3, 0};
  glVertexPointer(2, GL_SHORT, 0, coords);

  renderer->setColour(1.0f, 1.0f, 1.0f, 0.5f);
  glDrawArrays(GL_QUADS, 0, 4);

  renderer->setColour(1.0f, 1.0f, 1.0f, 1.0f);
  glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_SHORT, line_i);
#endif

  // Render the gui recursively
  m_rootWindow->render(renderer);

  glDisableClientState(GL_VERTEX_ARRAY);

  renderer->setViewMode(PERSPECTIVE);
}

} // namespace Sear
