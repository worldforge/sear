// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Frame.h"

#include "src/Render.h"

#include <sage/sage.h>
#include <sage/GL.h>

namespace Sear {

Frame::Frame()
{
}

Frame::~Frame()
{
}

void Frame::render(Render * renderer)
{
  static const GLushort line_i[] = {0, 1, 2, 3, 0};

  const GLshort coords[] = { m_x, m_y,
                             m_x, m_y + m_h,
                             m_x + m_w, m_y + m_h,
                             m_x + m_w, m_y };

  glVertexPointer(2, GL_SHORT, 0, coords);

  renderer->setColour(1.0f, 1.0f, 1.0f, 0.5f);
  glDrawArrays(GL_QUADS, 0, 4);

  renderer->setColour(1.0f, 1.0f, 1.0f, 1.0f);
  glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_SHORT, line_i);
}

} // namespace Sear
