// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "Use.h"
#include "Wield.h"

namespace Atlas { namespace Objects { namespace Operation {

Use::Use() : Action("", "use")
{
}

Use::Use(const char * id, const char * parent) : Action(id, parent)
{
}

Use::~Use()
{
}

Use Use::Class()
{
    Use value("use", "action");
    value.setObjtype(std::string("op_definition"));
    return value;
}

Wield::Wield() : Set("", "wield")
{
}

Wield::Wield(const char * id, const char * parent) : Set(id, parent)
{
}

Wield::~Wield()
{
}

Wield Wield::Class()
{
    Wield value("wield", "set");
    value.setObjtype(std::string("op_definition"));
    return value;
}

} } }
