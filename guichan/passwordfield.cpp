/*
 *  The Mana World
 *  Copyright 2004 The Mana World Development Team
 *
 *  This file is part of The Mana World.
 *
 *  The Mana World is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  The Mana World is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Mana World; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  $Id: passwordfield.cpp,v 1.4 2007-04-12 21:51:45 simon Exp $
 */

#include "passwordfield.h"

#include <guichan/font.hpp>
#include <guichan/graphics.hpp>
#include <guichan/key.hpp>
#include <guichan/keyevent.hpp>
#include <guichan/actionlistener.hpp>

#include <iostream>

namespace Sear {

PasswordField::PasswordField(const std::string& text, gcn::ActionListener *a, const gcn::ActionEvent &e):
    TextField(text),
    m_action(a),
    m_event(e)
{
}

void PasswordField::draw(gcn::Graphics *graphics)
{
      std::string stars;
      stars.assign(getText().size(), '*');

     gcn::Color faceColor = getBaseColor();
     gcn::Color highlightColor, shadowColor;
     int alpha = getBaseColor().a;
     highlightColor = faceColor + 0x303030;
     highlightColor.a = alpha;
     shadowColor = faceColor - 0x303030;
     shadowColor.a = alpha;
 
     // Draw a border.
     graphics->setColor(shadowColor);
     graphics->drawLine(0, 0, getWidth() - 1, 0);
     graphics->drawLine(0, 1, 0, getHeight() - 2);
     graphics->setColor(highlightColor);
     graphics->drawLine(getWidth() - 1, 1, getWidth() - 1, getHeight() - 1);
     graphics->drawLine(0, getHeight() - 1, getWidth() - 1, getHeight() - 1);
 
     // Push a clip area so the other drawings don't need to worry
     // about the border.
     graphics->pushClipArea(gcn::Rectangle(1, 1, getWidth() - 2, getHeight() - 2));
 
     graphics->setColor(getBackgroundColor());
     graphics->fillRectangle(gcn::Rectangle(0, 0, getWidth(), getHeight()));

     if (isFocused())
     {
       drawCaret(graphics,
       getFont()->getWidth(stars.substr(0, getCaretPosition())) -
       mXScroll);
       //drawCaret(graphics, mText->getCaretX(getFont()) - mXScroll);
     }
 
     graphics->setColor(getForegroundColor());
     graphics->setFont(getFont());
 
     graphics->drawText(stars, 1 - mXScroll, 1);

     graphics->popClipArea();
}

void PasswordField::keyPressed(gcn::KeyEvent& key) {
  if (key.getKey().getValue() == gcn::Key::ENTER && m_action != 0) {
    m_action->action(m_event);
  } else {
    TextField::keyPressed(key);
  }
}


}
