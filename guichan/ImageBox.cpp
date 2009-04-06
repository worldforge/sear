// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Simon Goodall

#include "ImageBox.h"
#include "renderers/RenderSystem.h"
#include "renderers/Render.h"

namespace Sear {

static const float texcoords[] = { 0.0f, 1.0f,
                                   1.0f, 1.0f,
                                   1.0f, 0.0f,
                                   0.0f, 0.0f };

static const float vertices[] = { -1.0f, -1.0f, 0.f,
                                  1.0f, -1.0f, 0.f,
                                  1.0f, 1.0f, 0.f,
                                  -1.0f, 1.0f, 0.f };


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

void ImageBox::setTextureName(const std::string &texture_name) {
  RenderSystem::getInstance().releaseTexture(m_texture_id);
  m_texture_name = texture_name;
  m_texture_id = RenderSystem::getInstance().requestTexture(m_texture_name);
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
  
  // Translate to correct position      
  int xpos, ypos;
  getAbsolutePosition(xpos, ypos);
  r->translateObject(xpos, ypos, 0.0f);

  /// Get widget dimensions
  const int width = getWidth();
  const int height = getHeight();

  // Translate a by half to position widget in correct position
  glTranslatef(width/2,height/2,0.0f);

  // Rotate widget if required
  if (m_angle != 0) {
    glRotatef(m_angle, 0.0f, 0.0f, 1.0f);
  }

  // Scale image to size of widget
  glScalef(width/2, height/2, 0.0f);

  // Draw the texture
  RenderSystem::getInstance().switchTexture(m_texture_id);
  r->renderArrays(RES_QUADS, 0, 4, (Vertex_3*) vertices, (Texel*) texcoords, NULL, false);

  // Restore other state
  r->restore();

  if (m_text.size() > 0) {
    graphics->drawText(m_text, 0, 0);
    // Rendering text switches textures outside of our manager, need to force
    // a texture change again
    RenderSystem::getInstance().switchTexture(0);
  }


  glDisable(GL_TEXTURE_2D);
}

} // namespace Sear
