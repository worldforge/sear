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
 *  $Id: actiontextfield.cpp,v 1.1 2007-01-27 11:38:50 simon Exp $
 */

#include "actiontextfield.h"

#include <guichan/font.hpp>
#include <guichan/graphics.hpp>
#include <guichan/key.hpp>
#include <guichan/actionlistener.hpp>

#include <iostream>

namespace Sear {

ActionTextField::ActionTextField(const std::string& text, gcn::ActionListener *a, const std::string &e):
    TextField(text),
    m_action(a),
    m_event(e)
{
}

void ActionTextField::keyPress(const gcn::Key& key) {
  if (key.getValue() == gcn::Key::ENTER && m_action != 0) {
    m_action->action(m_event, this);
  } else {
    TextField::keyPress(key);
  }
}


}
