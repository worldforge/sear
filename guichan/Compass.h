// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch
// Copyright (C) 2009 Simon Goodall

#ifndef SEAR_COMPASS_H
#define SEAR_COMPASS_H

#include "renderers/RenderTypes.h"

#include <guichan.hpp>

namespace Sear
{

class ImageBox;

class Compass : public gcn::Window
{
  public:
    Compass();
    
    virtual ~Compass();

    virtual void logic();
    
    virtual void draw(gcn::Graphics *);
    
    virtual void mouseEntered(gcn::MouseEvent &event) {
      m_mouseEntered = true;
    }

    virtual void mouseExited(gcn::MouseEvent &event) {
      m_mouseEntered = false;
    }

    virtual void mouseDragged(gcn::MouseEvent &event);
    virtual void mousePressed(gcn::MouseEvent &event);

protected:
  bool m_mouseEntered;

  float m_angle;
    
  ImageBox *m_compassCase, *m_compassNeedle, *m_needleShadow;
};

} // of namespace Sear

#endif // SEAR_COMPASS_H
