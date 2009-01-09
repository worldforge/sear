// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef SEAR_COMPASS_H
#define SEAR_COMPASS_H

#include "renderers/Sprite.h"


#include <guichan.hpp>

namespace Sear
{

class Render;

class Compass : public gcn::Window
{
  public:
    Compass();
    
    virtual ~Compass() {}

    virtual void logic();
    
    /** update the compass angle using Eris data, and factor in the
    camera's rotation too */
//    void update(double cameraRotation);
    virtual void draw(gcn::Graphics *);
    
//    virtual void click();
 //   virtual void release();
   // virtual bool key(int,int);

    //void setAngle(float angle) { m_angle = angle; }
    int width();
    
protected:
    float m_x, m_y;
    float m_angle;
    
    Sprite m_compassCase, m_compassNeedle, m_needleShadow;
};

} // of namespace Sear

#endif // SEAR_COMPASS_H
