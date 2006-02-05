// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 - 2006 Simon Goodall

// $Id: Editor.h,v 1.3 2006-02-05 21:09:48 simon Exp $

#include "Console.h"
#include <wfmath/vector.h>
#include <Eris/Avatar.h>

namespace Sear {

class Editor : public ConsoleObject
{
public:

    virtual void registerCommands(Console* con);
    virtual void runCommand(const std::string &command, const std::string &args);

private:
    
    void rotateZ(const std::string& target, double r);
    void translate(const std::string& target, WFMath::Vector<3> delta, bool local);
    
    void deleteEntity(const std::string& target);
    
    Eris::Avatar* getAvatar() const;
};


}

