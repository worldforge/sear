// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Button.h"

#include "gui/Label.h"

Button::Button(const std::string & text)
{
    Label * l = new Label(text);
    l->setParent(this);
    m_children.insert(l);
}
