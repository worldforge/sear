// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Workspace.h"

#include "gui/RootWindow.h"
#include "gui/Toplevel.h"

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
  glLineWidth(1.0f);

  // Render the gui recursively
  m_rootWindow->render(renderer);

  glDisableClientState(GL_VERTEX_ARRAY);

  renderer->setViewMode(PERSPECTIVE);
}

void Workspace::show()
{
}

void Workspace::handleEvent(const SDL_Event & event)
{
  Render *renderer = m_system->getGraphics()->getRender();

  switch (event.type) {
    case SDL_MOUSEMOTION: {
        short x = event.motion.x;
        short y = renderer->getWindowHeight() - event.motion.y;
        std::cout << "Mouse motion " << x << " " << y
                  << std::endl << std::flush;
        m_rootWindow->mouseMotion(x, y);
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      // This is the type of event we are interested in
    default:
      break;
  }
}

void Workspace::addToplevel(Toplevel * t)
{
  addChild(t);
}

} // namespace Sear
