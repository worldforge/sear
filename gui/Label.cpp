// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Label.cpp,v 1.2 2003-03-06 23:50:37 simon Exp $


#include "Label.h"

#ifdef HAVE_CONFIG
  #include "config.h"
#endif

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif


namespace Sear {

namespace Gui {


void Label::render() {}
 
void Label::setExtra(const std::string &name, varconf::Variable &value) {}
  

} /* namespace Gui */
	
} /* namespace Sear */

