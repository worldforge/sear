// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Simon Goodall

#include "ImageBox.h"
#include "renderers/RenderSystem.h"
#include "renderers/Render.h"

namespace Sear {

static const float texcoords[] = { 0.0f, 0.0f,
                                   1.0f, 0.0f,
                                   1.0f, 1.0f,
                                   0.0f, 1.0f };

static const float vertices[] = { 0.0f, 0.0f, 0.f,
                            1.0f, 0.0f, 0.f,
                            1.0f, 1.0f, 0.f,
                            0.0f, 1.0f, 0.f };


ImageBox::ImageBox(const std::string &texture_name) : Widget(),
  m_texture_name(texture_name),
  m_angle(0.0f)
{
  m_texture_id = RenderSystem::getInstance().requestTexture(m_texture_name);
}

ImageBox::~ImageBox()
{
  RenderSystem::getInstance().releaseTexture(m_texture_id);
}

void ImageBox::logic()
{
  gcn::Widget::logic();
}

void ImageBox::draw(gcn::Graphics *graphics)
{
  Render *r = RenderSystem::getInstance().getRenderer();

  glEnable(GL_TEXTURE_2D);

  // Record current state
  r->store();
        
  const gcn::ClipRectangle &rect = graphics->getCurrentClipArea();

  r->translateObject(rect.xOffset, rect.yOffset, 0.0f);
  glScalef(rect.width, rect.height, 0.0f);

  glRotatef(m_angle, 0.0f, 0.0f, 1.0f);

  // Draw the texture
  RenderSystem::getInstance().switchTexture(m_texture_id);
  r->renderArrays(RES_QUADS, 0, 4, (Vertex_3*) vertices, (Texel*) texcoords, NULL, false);

  // Restore other state
  r->restore();

  glDisable(GL_TEXTURE_2D);
}

} // namespace Sear
