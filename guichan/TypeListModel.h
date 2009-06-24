// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch
// Copyright (C) 2006 - 2009 Simon Goodall

#ifndef SEAR_GUICHAN_TYPELISTMODEL_H
#define SEAR_GUICHAN_TYPELISTMODEL_H 1

#include <guichan.hpp>

namespace Sear {

class TypeListModel : public gcn::ListModel
{
public:
  TypeListModel();
  virtual int getNumberOfElements();
  virtual std::string getElementAt(int i);
};

} // namespace Sear

#endif // SEAR_GUICHAN_TYPELISTMODEL_H
