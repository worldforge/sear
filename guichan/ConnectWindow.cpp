// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch
// Copyright (C) 2007 Simon Goodall

#include "guichan/ConnectWindow.h"
#include "guichan/DblClkListBox.h"

#include "guichan/ActionListenerSigC.h"
#include "guichan/Alert.h"
#include "guichan/Workarea.h"
#include "guichan/box.hpp"

#include "src/System.h"
#include "src/Metaserver.h"

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>
#include <sstream>

namespace Sear {

class ServerListModel : public gcn::ListModel
{
public:
  Metaserver *m_meta;

  ServerListModel()
  {
    m_meta = System::instance()->getMetaserver();
    m_meta->runCommand("refresh_server_list", "");
  }

  virtual ~ServerListModel() {
  }

  virtual int getNumberOfElements()
  {
    const ServerList &server_list = m_meta->getServerList();
    return server_list.size();
  }

  virtual std::string getElementAt(int i)
  {
    const ServerList &server_list = m_meta->getServerList();
    ServerList::const_iterator I = server_list.begin();
    for (size_t ii = 0; ii < (size_t) i; ++ii, ++I);
    return (I->second).servername;
  }
};

ConnectWindow::ConnectWindow() : gcn::Window("Connect to Server"),
                                 m_selected(-1)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  gcn::Box * vbox = new gcn::VBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(vbox));

  m_serverListModel = new ServerListModel;

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(sigc::mem_fun(*this, &ConnectWindow::actionPressed));

  m_servers = new DblClkListBox(m_serverListModel, m_buttonListener, gcn::ActionEvent(this, "connect"));
  m_widgets.push_back(SPtr<gcn::Widget>(m_servers));
  m_servers->setWidth(200);
  m_servers->setFocusable(false);
  gcn::ScrollArea * scroll_area = new gcn::ScrollArea(m_servers,
                                      gcn::ScrollArea::SHOW_NEVER,
                                      gcn::ScrollArea::SHOW_ALWAYS);
  m_widgets.push_back(SPtr<gcn::Widget>(scroll_area));
  scroll_area->setWidth(200);
  scroll_area->setHeight(200);
  scroll_area->setFrameSize(1);
  vbox->pack(scroll_area);

  m_serverField = new gcn::TextField("                ");
  m_widgets.push_back(SPtr<gcn::Widget>(m_serverField));
  m_serverField->setText("");
  vbox->pack(m_serverField);

  gcn::Box * hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  m_connectButton = new gcn::Button("Connect");
  m_widgets.push_back(SPtr<gcn::Widget>(m_connectButton));
  m_connectButton->setFocusable(false);
  m_connectButton->setActionEventId("connect");
  m_connectButton->addActionListener(m_buttonListener);
  hbox->pack(m_connectButton);

  m_refreshButton = new gcn::Button("Refresh");
  m_widgets.push_back(SPtr<gcn::Widget>(m_refreshButton));
  m_refreshButton->setFocusable(false);
  m_refreshButton->setActionEventId("refresh");
  m_refreshButton->addActionListener(m_buttonListener);
  hbox->pack(m_refreshButton);

  m_closeButton = new gcn::Button("Close");
  m_widgets.push_back(SPtr<gcn::Widget>(m_closeButton));
  m_closeButton->setFocusable(false);
  m_closeButton->setActionEventId("close");
  m_closeButton->addActionListener(m_buttonListener);
  hbox->pack(m_closeButton);

  vbox->pack(hbox);

  add(vbox);

  resizeToContent();
}

ConnectWindow::~ConnectWindow()
{
  delete m_serverListModel;
  delete m_buttonListener;
}

void ConnectWindow::logic()
{
  int new_selected = m_servers->getSelected();
  if (new_selected != m_selected) {
    m_selected = new_selected;

    const ServerList &server_list = m_serverListModel->m_meta->getServerList();

    if (m_selected >= 0 && (size_t) m_selected < server_list.size()) {
      ServerList::const_iterator I = server_list.begin();
      for (size_t ii = 0; ii < (size_t) m_selected; ++ii, ++I);
      std::stringstream ss;
      ss << (I->second).hostname << " " << (I->second).port;
      m_serverField->setText(ss.str());
    }
  }
  gcn::Window::logic();
}

void ConnectWindow::actionPressed(std::string event)
{
  bool close = false;

  gcn::Widget * parent_widget = getParent();
  if (parent_widget == 0) {
    std::cerr << "NO PARENT" << std::endl << std::flush;
    return;
  }
  gcn::Container * parent = dynamic_cast<gcn::Container *>(parent_widget);
  if (parent == 0) {
    std::cerr << "WEIRD PARENT" << std::endl << std::flush;
    return;
  }

  if (event == "connect") {
    std::cout << "Connect " << m_serverField->getText() << std::endl << std::flush;
    std::string cmd("/connect ");
    const std::string & server = m_serverField->getText();
    if (server.empty()) {
      new Alert(parent, "No server specified");
    } else {
      cmd += m_serverField->getText();
      System::instance()->runCommand(cmd);
      close = true;
    }
  } else if (event == "refresh") {
    m_serverListModel->m_meta->runCommand("refresh_server_list", "");
  } else if (event == "close") {
    std::cout << "Close window" << std::endl << std::flush;
    close = true;
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }

  if (!close) { return; }

  System::instance()->getWorkarea()->removeLater(this);
}

} // namespace Sear
