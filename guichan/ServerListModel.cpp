// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch
// Copyright (C) 2006 - 2009 Simon Goodall

#include "ServerListModel.h"

#include "Eris/Localserver.h"
#include "src/System.h"

namespace Sear {

  ServerListModel::ServerListModel()
  {
    m_meta = System::instance()->getLocalserver();
    m_meta->runCommand("refresh_server_list", "");
  }

  ServerListModel::~ServerListModel() {
  }

  int ServerListModel::getNumberOfElements()
  {
    const ServerList &server_list = m_meta->getServerList();
    return server_list.size();
  }

  std::string ServerListModel::getElementAt(int i)
  {
    return getElementDataAt(i).servername;
  }

  ServerObject ServerListModel::getElementDataAt(int i)
  {
    const ServerList &server_list = m_meta->getServerList();
    ServerList::const_iterator I = server_list.begin();
    for (size_t ii = 0; ii < (size_t) i; ++ii, ++I);
    return (I->second);
  }

} // namespace Sear
