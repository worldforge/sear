// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "Compass.h"
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

Compass::Compass(float pos_x, float pos_y) :  
    m_x(pos_x), 
    m_y(pos_y),
    m_angle(45)
{
}

Compass::~Compass()
{
}

void Compass::setup()
{    
    m_compassCase = Sprite("compass_case");
    m_compassNeedle = Sprite("compass_needle");
    m_needleShadow = Sprite("compass_needle_shadow");
}

void Compass::update(double cameraRotation)
{
  Eris::Avatar *avatar = System::instance()->getClient()->getAvatar();
  Eris::Entity *focus = avatar->getEntity();
//    Eris::Entity* focus = Eris::World::Instance()->getFocusedEntity();
    if (!focus) return;
    
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
    
    radAngle += cameraRotation;
    m_angle = (radAngle * 180) / M_PI;
}

void Compass::draw(Render*r, bool select)
{
    if (select) return;
        
    r->setViewMode(ORTHOGRAPHIC);
    
    r->translateObject(m_x, m_y, 0.0f);
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
        
    r->setViewMode(PERSPECTIVE);
}

void Compass::click()
{
}

} // namespace Sear
