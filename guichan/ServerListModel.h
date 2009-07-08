// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch
// Copyright (C) 2006 - 2009 Simon Goodall

#ifndef SEAR_GUICHAN_SERVERLISTMODEL_H
#define SEAR_GUICHAN_SERVERLISTMODEL_H 1

#include <guichan.hpp>

#include "Eris/Localserver.h"

namespace Sear {

class ServerListModel : public gcn::ListModel
{
public:
  Localserver *m_meta;

  ServerListModel();
  virtual ~ServerListModel();

  virtual int getNumberOfElements();
  virtual std::string getElementAt(int i);
  virtual ServerObject getElementDataAt(int i);
};

} // namespace Sear

#endif // SEAR_GUICHAN_SERVERLISTMODEL_H
