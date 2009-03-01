// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch
// Copyright (C) 2006 - 2009 Simon Goodall

#ifndef SEAR_GUICHAN_INVENTORY_H
#define SEAR_GUICHAN_INVENTORY_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

#include "common/SPtr.h"

namespace gcn {

class ListBox;
namespace contrib {
class AdjustingContainer;
}
} // namespace gcn

namespace Sear {

class InventoryListAdaptor;
class ActionListenerSigC;
class ActionImageBox;

class Inventory : virtual public sigc::trackable, public gcn::Window {
public:
  Inventory();
  virtual ~Inventory();

  void actionPressed(std::string);
  void buttonPressed(std::string);
  virtual void logic();

protected:
  std::list<SPtr<gcn::Widget> > m_widgets;
 
  gcn::ListBox * m_items;
  gcn::contrib::AdjustingContainer *m_grid;

  std::vector<SPtr<ActionImageBox> > m_images;
  std::string m_selected;

  ActionListenerSigC * m_buttonListener;
  ActionListenerSigC * m_imageListener;
};

} // namespace Sear

#endif // SEAR_GUICHAN_INVENTORY_H
