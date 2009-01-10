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

Compass::Compass() : Window(),
  m_mouseEntered(false),
  m_angle(45)
{
  m_compassCase = Sprite("compass_case");
  m_compassNeedle = Sprite("compass_needle");
  m_needleShadow = Sprite("compass_needle_shadow");

  setWidth(64);
  setHeight(64);

  gcn::Color base = getBaseColor();
  base.a = 0;
  setBaseColor(base);
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

  // Record current state
  r->store();
  glPushAttrib(GL_ALL_ATTRIB_BITS); 
  glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS); 
        
  const gcn::ClipRectangle &rect = graphics->getCurrentClipArea();

  // Draw the compass case
  r->translateObject(rect.xOffset, rect.yOffset, 0.0f);
  r->translateObject(32, 32, 0.0f);
  m_compassCase.draw(r);

 // draw the shadow, offset a little bit    
  r->store();
  r->translateObject(-3.0f, -3.0f, 0.01f);
  r->rotate(m_angle, 0.0f, 0.0f, 1.0f);
  m_needleShadow.draw(r);
  r->restore();
    
  // finally draw the needle
  r->rotate(m_angle, 0.0f, 0.0f, 1.0f);
  m_compassNeedle.draw(r);

  // Restore other state
  r->restore();
  glPopClientAttrib();
  glPopAttrib();

  RenderSystem::getInstance().switchTexture(0); 
}

} // namespace Sear
