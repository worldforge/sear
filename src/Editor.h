// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 - 2006 Simon Goodall

// $Id: Editor.h,v 1.4 2006-04-30 18:13:41 alriddoch Exp $

#include "Console.h"
#include <wfmath/vector.h>

namespace Eris {
  class Avatar;
}

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

