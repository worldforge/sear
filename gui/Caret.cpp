// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Caret.h"

#include "renderers/RenderSystem.h"

#include "renderers/Render.h"

#include <sage/sage.h>
#include <sage/GL.h>

#include <iostream>

namespace Sear {

Caret::Caret(int h)
{
    setSize(1, h);
}

Caret::~Caret()
{
}

void Caret::render(Render * renderer)
{
  const float vertices[] = { 0, 0, 0, m_h };
  glPushMatrix();
  glTranslatef(m_x, m_y, 0);
  glVertexPointer(2, GL_FLOAT, 0, vertices);
  glDrawArrays(GL_LINES, 0, 2);
  glPopMatrix();
}

} // namespace Sear
