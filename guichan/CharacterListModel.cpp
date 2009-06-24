// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch
// Copyright (C) 2006 - 2009 Simon Goodall

#include "CharacterListModel.h"

#include <Atlas/Objects/Entity.h>

#include <Eris/Account.h>

#include "src/client.h"
#include "src/System.h"

namespace Sear {

CharacterListModel::CharacterListModel()
  {
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account == 0) { return; }
    account->refreshCharacterInfo();
  }

  int CharacterListModel::getNumberOfElements()
  {
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account == 0) { return 0; }
    return account->getCharacters().size();
  }

  std::string CharacterListModel::getElementAt(int i)
  {
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account == 0) { return ""; }
    const Eris::CharacterMap & ci = account->getCharacters();
    Eris::CharacterMap::const_iterator I = ci.begin();
    Eris::CharacterMap::const_iterator Iend = ci.end();
    for (int j = 0; I != Iend; ++I, ++j) {
      if (i == j) {
        return I->second->getName();
      }
    }
    return "UNKNOWN";
  }

} // namespace Sear
