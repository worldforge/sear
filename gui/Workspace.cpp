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
                                        m_rootWindow(new RootWindow()),
                                        m_focusPolicy(FOCUS_CLICK)
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

void Workspace::mouseMotion(Window & w, short x, short y)
{
  const std::set<Window *> & children = w.getChildren();
  std::set<Window *>::const_iterator I = children.begin();
  for(; I != children.end(); ++I) {
    Window & c = **I;
    short rx = x - c.x(),
          ry = y - c.y();
    if ((rx < 0) || (rx >= c.w()) ||
        (ry < 0) || (ry >= c.h())) {
      continue;
    }
    mouseMotion(c, rx, ry);
    c.mouseMotion(rx, ry);
  }
}

void Workspace::mouseDown(Window & w, short x, short y)
{
  const std::set<Window *> & children = w.getChildren();
  std::set<Window *>::const_iterator I = children.begin();
  for(; I != children.end(); ++I) {
    Window & c = **I;
    short rx = x - c.x(),
          ry = y - c.y();
    if ((rx < 0) || (rx >= c.w()) ||
        (ry < 0) || (ry >= c.h())) {
      continue;
    }
    c.mouseDown(rx, ry);
  }
}

void Workspace::mouseUp(Window & w, short x, short y)
{
  const std::set<Window *> & children = w.getChildren();
  std::set<Window *>::const_iterator I = children.begin();
  for(; I != children.end(); ++I) {
    Window & c = **I;
    short rx = x - c.x(),
          ry = y - c.y();
    if ((rx < 0) || (rx >= c.w()) ||
        (ry < 0) || (ry >= c.h())) {
      continue;
    }
    c.mouseUp(rx, ry);
  }
}

void Workspace::handleEvent(const SDL_Event & event)
{
  assert(m_rootWindow != 0);
  Render *renderer = m_system->getGraphics()->getRender();

  switch (event.type) {
    case SDL_MOUSEMOTION: {
        short x = event.motion.x;
        short y = renderer->getWindowHeight() - event.motion.y;
        mouseMotion(*m_rootWindow, x, y);
      }
      break;
    case SDL_MOUSEBUTTONDOWN: {
        short x = event.motion.x;
        short y = renderer->getWindowHeight() - event.motion.y;
        mouseDown(*m_rootWindow, x, y);
      }
      break;
    case SDL_MOUSEBUTTONUP: {
        short x = event.motion.x;
        short y = renderer->getWindowHeight() - event.motion.y;
        mouseUp(*m_rootWindow, x, y);
      }
      break;
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

void Workspace::map(Window *, int, int, int &, int &)
{
}

} // namespace Sear
