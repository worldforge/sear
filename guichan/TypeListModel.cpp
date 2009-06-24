// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch
// Copyright (C) 2006 - 2009 Simon Goodall

#include "TypeListModel.h"

#include <Eris/Account.h>

#include "src/System.h"
#include "src/client.h"

namespace Sear {

TypeListModel::TypeListModel()
  {
  }

 int TypeListModel::getNumberOfElements()
  {
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account == 0) { return 0; }
    return account->getCharacterTypes().size();
  }

  std::string TypeListModel::getElementAt(int i)
  {
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account == 0) { return ""; }
    const std::vector<std::string> & types = account->getCharacterTypes();
    if ((size_t) i < types.size()) {
      return types[i];
    } else {
      return "UNKNOWN";
    }
  }

} // namespace Sear
