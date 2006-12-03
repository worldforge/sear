// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include <sage/sage.h>
#include <sage/GL.h>
#include "gui/Workspace.h"

#include "gui/RootWindow.h"
#include "gui/Toplevel.h"
#include "gui/focus.h"

#include "src/System.h"
#include "renderers/Graphics.h"
#include "renderers/Render.h"

#include "renderers/RenderSystem.h"


namespace Sear {

static const std::string WORKSPACE = "workspace";

Workspace::Workspace(System * system) : m_system(system),
                                        m_rootWindow(new RootWindow()),
                                        m_focusPolicy(FOCUS_CLICK),
                                        m_oldx(-1), m_oldy(-1)
{
}

Workspace::~Workspace()
{
  delete m_rootWindow;
}

void Workspace::draw()
{
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState(WORKSPACE));
  Render *renderer = RenderSystem::getInstance().getRenderer();
  renderer->setViewMode(ORTHOGRAPHIC);

  glPushMatrix();
  glTranslatef(0.f, renderer->getWindowHeight(), 0.f);
  glScalef(1.f, -1.f, 1.f);

  glLineWidth(1.0f);

  // Render the gui recursively
  m_rootWindow->render(renderer);


  glPopMatrix();

  renderer->setViewMode(PERSPECTIVE);
}

void Workspace::mouseMotion(Window & w, short x, short y, short ox, short oy)
{
  const std::set<Window *> & children = w.getChildren();
  std::set<Window *>::const_iterator I = children.begin();
  for(; I != children.end(); ++I) {
    Window & c = **I;
    short rx = x - c.x(),
          ry = y - c.y(),
          rox = ox - c.x(),
          roy = oy - c.y();
    if ((rx < 0) || (rx >= c.w()) ||
        (ry < 0) || (ry >= c.h())) {
      continue;
    }
    // mouseMotion(c, rx, ry);
    c.mouseMotion(rx, ry, rox, roy);
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

// FIXME Why pass in the window? Its always rootWindow.
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

#if 0
void Workspace::keyPress(Window & w, short x, short y, SDLKey ks, Uint16 ch)
{
  std::cout << "Key down" << std::endl << std::flush;
  const std::set<Window *> & children = w.getChildren();
  std::set<Window *>::const_iterator I = children.begin();
  for(; I != children.end(); ++I) {
    std::cout << "Key down 1" << std::endl << std::flush;
    Window & c = **I;
    short rx = x - c.x(),
          ry = y - c.y();
    if ((rx < 0) || (rx >= c.w()) ||
        (ry < 0) || (ry >= c.h())) {
      continue;
    }
    std::cout << "Key down 2" << std::endl << std::flush;
    c.keyPress(rx, ry, ks, ch);
  }
}
#endif

void Workspace::handleEvent(const SDL_Event & event)
{
  assert(m_rootWindow != 0);

  switch (event.type) {
    case SDL_MOUSEMOTION: {
        short x = event.motion.x;
        short y = event.motion.y;

        mouseMotion(*m_rootWindow, x, y, m_oldx, m_oldy);
        m_oldx = x;
        m_oldy = y;
      }
      break;
    case SDL_MOUSEBUTTONDOWN: {
        short x = event.button.x;
        short y = event.button.y;

        mouseDown(*m_rootWindow, x, y);
      }
      break;
    case SDL_MOUSEBUTTONUP: {
        short x = event.button.x;
        short y = event.button.y;

        mouseUp(*m_rootWindow, x, y);
      }
      break;
    case SDL_KEYDOWN: {
        // FIXME This should be focus based.
        Window * focus = queryFocus();
        if (focus != 0) {
          focus->keyPress(event.key.keysym.sym, event.key.keysym.unicode);
        }
      }
      break;
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

void Workspace::show()
{
  int width, height;
  std::set<Widget *>::const_iterator I = m_children.begin();
  for(; I != m_children.end(); ++I) {
    (*I)->map(m_rootWindow, 0, 0, width, height);
  }
}

void Workspace::map(Window *, int, int, int &, int &)
{
}

} // namespace Sear
