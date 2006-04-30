// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_INVENTORY_H
#define SEAR_GUICHAN_INVENTORY_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

#include "common/SPtr.h"

namespace gcn {

class ListBox;

} // namespace gcn

namespace Sear {

class InventoryListAdaptor;
class ActionListenerSigC;

class Inventory : virtual public SigC::Object, public gcn::Window {
protected:
  std::list<SPtr<gcn::Widget> > m_widgets;
 
  gcn::ListBox * m_items;

  InventoryListAdaptor * m_inventory;

  ActionListenerSigC * m_buttonListener;
public:
  Inventory();
  virtual ~Inventory();

  void actionPressed(std::string);
};

} // namespace Sear

#endif // SEAR_GUICHAN_INVENTORY_H
