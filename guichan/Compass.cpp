// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch
// Copyright (C) 2009 Simon Goodall

#include "Compass.h"
#include "ImageBox.h"
#include "renderers/Camera.h"
#include "renderers/CameraSystem.h"
#include "renderers/RenderSystem.h"
#include "renderers/Render.h"
#include "src/System.h"
#include "src/client.h"
#include "src/Character.h"
#include "src/CharacterManager.h"

#include <Eris/Avatar.h>
#include <Eris/Entity.h>
#include <Eris/View.h>

#include <iostream>

namespace Sear
{

static int width = 64;
static int height = 64;

Compass::Compass() : Window(),
  m_mouseEntered(false),
  m_angle(45)
{

  setTitleBarHeight(1);
  setFrameSize(1);
  setPadding(0);

  int frame = 2 * getFrameSize();
  int padding = 2 * getPadding();
  setWidth(width + frame + padding);
  setHeight(height + frame + padding + getTitleBarHeight());

  setMovable(true);

  gcn::Color base = getBaseColor();
  base.a = 0;
  setBaseColor(base);

  m_compassCase = new ImageBox("compass_case");
  m_compassNeedle = new ImageBox("compass_needle");
  m_needleShadow = new ImageBox("compass_needle_shadow");

  m_compassCase->setWidth(width);
  m_compassNeedle->setWidth(width);
  m_needleShadow->setWidth(width);

  m_compassCase->setHeight(height);
  m_compassNeedle->setHeight(height);
  m_needleShadow->setHeight(height);

  add(m_compassCase);
  add(m_compassNeedle);
  add(m_needleShadow);
}

Compass::~Compass()
{
  delete m_compassCase;
  delete m_compassNeedle;
  delete m_needleShadow;
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

  WFMath::Vector<3> northAxis(0.0, 1.0, 0.0), facing(1.0, 0.0, 0.0);

  Eris::Avatar *avatar = System::instance()->getCharacterManager()->getActiveCharacter()->getAvatar();
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
    
    facing.rotate(q); // orient in direction avatar is facing
  }    
  double radAngle = WFMath::Angle<3>(northAxis, facing);
  if (facing.x() < 0) radAngle = (M_PI * 2) - radAngle;
  
  float cameraRotation = 0.0f;
  cameraRotation = RenderSystem::getInstance().getCameraSystem()->getCurrentCamera()->getRotation();  
  radAngle += cameraRotation;
  m_angle = -(radAngle * 180) / M_PI;

  m_compassNeedle->setRotation(m_angle);
  m_needleShadow->setRotation(m_angle);

  gcn::Window::logic();
}

void Compass::draw(gcn::Graphics *graphics)
{
  gcn::Window::draw(graphics);
}

void Compass::mousePressed(gcn::MouseEvent& mouseEvent) {

  if (getParent() != NULL)
  {
    getParent()->moveToTop(this);
  }

  mDragOffsetX = mouseEvent.getX();
  mDragOffsetY = mouseEvent.getY();

  mMoved = true;
}

void Compass::mouseDragged(gcn::MouseEvent& mouseEvent) {

  if (mouseEvent.isConsumed())
  {
    return;
  }

  if (isMovable() && mMoved)
  {
    setPosition(mouseEvent.getX() - mDragOffsetX + getX(),
                mouseEvent.getY() - mDragOffsetY + getY());
  }

  mouseEvent.consume();
}

} // namespace Sear
