// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Text.h"

#include "renderers/RenderSystem.h"

#include "renderers/Render.h"

#include <sage/sage.h>
#include <sage/GL.h>

#include <iostream>

namespace Sear {

Text::Text(const std::string & s, float scale) : m_scale(scale)
{
    // Make sure the size is set correctly.
    setContent(s);
}

Text::~Text()
{
}

void Text::setContent(const std::string & s)
{
  m_content = s;
  setSize((s.size() + 1) * 10 * m_scale, 16 * m_scale);
}

void Text::render(Render * renderer)
{
  glColor3f(0.f, 0.f, 0.f);
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState("font"));
  glPushMatrix();
  glTranslatef(m_x, m_y + 16 * m_scale, 0);
  glScalef(m_scale, -m_scale, m_scale);
  renderer->print3D(m_content.c_str(), 0);
  glPopMatrix();
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState("workspace"));
}

} // namespace Sear
