// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/String.h"

#include "renderers/RenderSystem.h"

#include "src/Render.h"

#include <sage/sage.h>
#include <sage/GL.h>

#include <iostream>

namespace Sear {

String::String(const std::string & s)
{
    // Make sure the size is set correctly.
    setContent(s);
}

String::~String()
{
}

void String::setContent(const std::string & s)
{
  m_content = s;
  setSize((s.size() + 1) * 10, 16);
}

void String::render(Render * renderer)
{
  glColor3f(0.f, 0.f, 0.f);
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState("font"));
  glPushMatrix();
  glTranslatef(m_x, m_y, 0);
  renderer->print3D(m_content.c_str(), 0);
  glPopMatrix();
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState("workspace"));
}

} // namespace Sear
