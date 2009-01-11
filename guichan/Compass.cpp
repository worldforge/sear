// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch
// Copyright (C) 2009 Simon Goodall

#include "Compass.h"
#include "renderers/Camera.h"
#include "renderers/CameraSystem.h"
#include "renderers/RenderSystem.h"
#include "renderers/Render.h"
#include "src/System.h"
#include "src/client.h"

#include <Eris/Avatar.h>
#include <Eris/Entity.h>
#include <Eris/View.h>

#include <iostream>

namespace Sear
{
static float texcoords[] = { 0, 0,
                         1.0, 0,
                         1.0,1.0,
                          0,1.0 };

static int width = 64;
static int height = 64;
static float vertices[] = { -width/2.f, -height/2.f, 0.f,
                          width/2.f, -height/2.f, 0.f,
                          width/2.f, height/2.f, 0.f,
                         -width/2.f, height/2.f, 0.f };


Compass::Compass() : Window(),
  m_mouseEntered(false),
  m_angle(45)
{
  m_compassCase = RenderSystem::getInstance().requestTexture("compass_case");
  m_compassNeedle = RenderSystem::getInstance().requestTexture("compass_needle");
  m_needleShadow = RenderSystem::getInstance().requestTexture("compass_needle_shadow");

  setWidth(64);
  setHeight(64);

  gcn::Color base = getBaseColor();
  base.a = 0;
  setBaseColor(base);
}

Compass::~Compass()
{
  RenderSystem::getInstance().releaseTexture(m_compassCase);
  RenderSystem::getInstance().releaseTexture(m_compassNeedle);
  RenderSystem::getInstance().releaseTexture(m_needleShadow);
}

void Compass::logic()
{
  // Only show window background when mouse is over this window 
  gcn::Color base = getBaseColor();
  if (m_mouseEntered) {
    base.a = 50;
  } else {
    base.a = 0;
  }
  setBaseColor(base);
  // This logic should work out where north is, based on avatar rotation
  // and camera rotation.
  // Currently I do not believe it points in the correct direction

  Eris::Avatar *avatar = System::instance()->getClient()->getAvatar();
  if (avatar != 0) {
    Eris::Entity *focus = avatar->getEntity();
    if (!focus) {
      gcn::Window::logic();
      return;
    }
    
    WFMath::Quaternion q = focus->getOrientation();
    
    Eris::Entity* ent = focus->getLocation();
    while (ent && (ent != avatar->getView()->getTopLevel())) {
      q *= ent->getOrientation();
      ent = ent->getLocation();
    }
    
    WFMath::Vector<3> northAxis(0.0, 1.0, 0.0),
        facing(1.0, 0.0, 0.0);
    facing.rotate(q); // orient in direction avatar is facing
    
    double radAngle = WFMath::Angle<3>(northAxis, facing);
    if (facing.x() < 0) radAngle = (M_PI * 2) - radAngle;
  
    float cameraRotation = 0.0f;
    cameraRotation = RenderSystem::getInstance().getCameraSystem()->getCurrentCamera()->getRotation();  
    radAngle += cameraRotation;
    m_angle = -(radAngle * 180) / M_PI;
  }

  gcn::Window::logic();
}

void Compass::draw(gcn::Graphics *graphics)
{
  gcn::Window::draw(graphics);
  Render *r = RenderSystem::getInstance().getRenderer();

  // Note this code will break the current states during the sprite
  // rendering. We should re-factor that code into this method.

  glEnable(GL_TEXTURE_2D);

  // Record current state
  r->store();
        
  const gcn::ClipRectangle &rect = graphics->getCurrentClipArea();
  // Draw the compass case
  r->translateObject(rect.xOffset, rect.yOffset, 0.0f);
  r->translateObject(32, 32, 0.0f);
  RenderSystem::getInstance().switchTexture(m_compassCase);
  r->renderArrays(RES_QUADS, 0, 4, (Vertex_3*) vertices, (Texel*) texcoords, NULL, false);

   // draw the shadow, offset a little bit    
  r->store();
  r->translateObject(-2.0f, -2.0f, 0.01f);
  r->rotate(m_angle, 0.0f, 0.0f, 1.0f);
  RenderSystem::getInstance().switchTexture(m_needleShadow);
  r->renderArrays(RES_QUADS, 0, 4, (Vertex_3*) vertices, (Texel*) texcoords, NULL, false);
  r->restore();
    
  // finally draw the needle
  r->rotate(m_angle, 0.0f, 0.0f, 1.0f);
  RenderSystem::getInstance().switchTexture(m_compassNeedle);
  r->renderArrays(RES_QUADS, 0, 4, (Vertex_3*) vertices, (Texel*) texcoords, NULL, false);

  // Restore other state
  r->restore();

  glDisable(GL_TEXTURE_2D);
}

} // namespace Sear
