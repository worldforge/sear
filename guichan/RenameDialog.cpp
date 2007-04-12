// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch
// Copyright (C) 2007 Simon Goodall

#include "guichan/RenameDialog.h"

#include "guichan/Alert.h"
#include "guichan/ActionListenerSigC.h"
#include "guichan/box.hpp"
#include "guichan/actiontextfield.h"
#include "guichan/Workarea.h"

#include "src/System.h"

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

RenameDialog::RenameDialog(const std::string &id, const std::string &old_name) : 
  gcn::Window("Rename Entity"),
  m_entity_id(id),
  m_old_name(old_name)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(sigc::mem_fun(this, &RenameDialog::actionPressed));

  gcn::Box * vbox = new gcn::VBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(vbox));

  gcn::Box * hbox = new gcn::HBox(6);

  m_widgets.push_back(SPtr<gcn::Widget>(hbox));
  gcn::Label * l1 = new gcn::Label("Old Name");
  m_widgets.push_back(SPtr<gcn::Widget>(l1));
  hbox->pack(l1);
  l1 = new gcn::Label(m_old_name);
  m_widgets.push_back(SPtr<gcn::Widget>(l1));
  hbox->pack(l1);

  vbox->pack(hbox);

  hbox = new gcn::HBox(6);
 
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));
  l1 = new gcn::Label("New Name");
  m_widgets.push_back(SPtr<gcn::Widget>(l1));
  m_nameField = new ActionTextField("                ", m_buttonListener, gcn::ActionEvent(this, "rename"));
  m_widgets.push_back(SPtr<gcn::Widget>(m_nameField));
  m_nameField->setText("");
  hbox->pack(l1);
  hbox->pack(m_nameField);

  vbox->pack(hbox);

  hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));
  m_renameButton = new gcn::Button("Rename");
  m_widgets.push_back(SPtr<gcn::Widget>(m_renameButton));
  m_renameButton->setFocusable(false);
  m_renameButton->setActionEventId("rename");
  m_renameButton->addActionListener(m_buttonListener);
  hbox->pack(m_renameButton);

  m_cancelButton = new gcn::Button("Close");
  m_widgets.push_back(SPtr<gcn::Widget>(m_cancelButton));
  m_cancelButton->setFocusable(false);
  m_cancelButton->setActionEventId("close");
  m_cancelButton->addActionListener(m_buttonListener);
  hbox->pack(m_cancelButton);

  vbox->pack(hbox);

  hbox = new gcn::HBox();
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));
  hbox->pack(vbox, 6);

  vbox = new gcn::VBox();
  m_widgets.push_back(SPtr<gcn::Widget>(vbox));
  vbox->pack(hbox, 6);

  add(vbox);

  resizeToContent();
}

RenameDialog::~RenameDialog()
{
  delete m_buttonListener;
}

void RenameDialog::logic()
{
  m_nameField->setEnabled(true);
  m_nameField->setVisible(true);
  gcn::Window::logic();
}

void RenameDialog::actionPressed(std::string event)
{
  bool close = false;
//  bool password_error = false;

  gcn::Widget * parent_widget = getParent();
  if (parent_widget == 0) {
    std::cout << "NO PARENT" << std::endl << std::flush;
    return;
  }
  gcn::Container * parent = dynamic_cast<gcn::Container *>(parent_widget);
  if (parent == 0) {
    std::cout << "WEIRD PARENT" << std::endl << std::flush;
    return;
  }

  std::string cmd;
  if (event == "rename") {
    cmd = "/rename_entity ";
    cmd += m_entity_id;
    cmd += " ";
    cmd += m_nameField->getText();
    System::instance()->runCommand(cmd);
    close = true;
  } else if (event == "close") {
    close = true;
  } else {
  }

  if (!close) { return; }

  //parent->remove(this);
  System::instance()->getWorkarea()->removeLater(this);
}

} // namespace Sear
