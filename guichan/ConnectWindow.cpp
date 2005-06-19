// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/ConnectWindow.h"

#include "guichan/ActionListenerSigC.h"
#include "guichan/box.hpp"

#include "src/System.h"

#include <Eris/Metaserver.h>

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

class ServerListModel : public gcn::ListModel
{
public:
  Eris::Meta * m_metaQuery;

  ServerListModel()
  {
    m_metaQuery = new Eris::Meta("metaserver.worldforge.org", 16);
    m_metaQuery->refresh();
  }

  virtual int getNumberOfElements()
  {
    return m_metaQuery->getGameServerCount();
  }

  virtual std::string getElementAt(int i)
  {
    const Eris::ServerInfo & si = m_metaQuery->getInfoForServer(i);
    switch (si.getStatus()) {
      case Eris::ServerInfo::QUERYING:
        return si.getHostname();
        break;
      case Eris::ServerInfo::VALID:
        return si.getServername();
        break;
      case Eris::ServerInfo::TIMEOUT:
        return "<timeout>";
        break;
      case Eris::ServerInfo::INVALID:
      default:
        return "...";
        break;
    }
  }
};


ConnectWindow::ConnectWindow() : gcn::Window(), m_selected(-1)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  gcn::Box * vbox = new gcn::VBox(6);

  m_serverListModel = new ServerListModel;

  m_servers = new gcn::ListBox(m_serverListModel);
  m_servers->setWidth(200);
  m_servers->setFocusable(false);
  gcn::ScrollArea * scroll_area = new gcn::ScrollArea(m_servers,
                                      gcn::ScrollArea::SHOW_NEVER,
                                      gcn::ScrollArea::SHOW_ALWAYS);
  scroll_area->setWidth(200);
  scroll_area->setHeight(200);
  scroll_area->setBorderSize(1);
  vbox->pack(scroll_area);

  m_serverField = new gcn::TextField("                ");
  m_serverField->setText("");
  vbox->pack(m_serverField);

  gcn::Box * hbox = new gcn::HBox(6);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(SigC::slot(*this, &ConnectWindow::actionPressed));

  m_connectButton = new gcn::Button("Connect");
  m_connectButton->setFocusable(false);
  m_connectButton->setEventId("connect");
  m_connectButton->addActionListener(m_buttonListener);
  hbox->pack(m_connectButton);

  m_closeButton = new gcn::Button("Close");
  m_closeButton->setFocusable(false);
  m_closeButton->setEventId("close");
  m_closeButton->addActionListener(m_buttonListener);
  hbox->pack(m_closeButton);

  vbox->pack(hbox);

  setContent(vbox);

  resizeToContent();
}

ConnectWindow::~ConnectWindow()
{
}

void ConnectWindow::logic()
{
  int new_selected = m_servers->getSelected();
  if (new_selected != m_selected) {
    m_selected = new_selected;
    if (m_selected >= 0 &&
        m_selected < m_serverListModel->m_metaQuery->getGameServerCount()) {
      const Eris::ServerInfo & si = m_serverListModel->m_metaQuery->getInfoForServer(m_selected);
      if ((si.getStatus() == Eris::ServerInfo::QUERYING) ||
          (si.getStatus() == Eris::ServerInfo::VALID)) {
        m_serverField->setText(si.getHostname());
      } else {
        m_serverField->setText("");
      }
    }
  }
  gcn::Window::logic();
}

void ConnectWindow::actionPressed(std::string event)
{
  if (event == "connect") {
    std::cout << "Connect " << m_serverField->getText() << std::endl << std::flush;
    std::string cmd("/connect ");
    cmd += m_serverField->getText();
    System::instance()->runCommand(cmd);
  } else if (event == "close") {
    std::cout << "Close window" << std::endl << std::flush;
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }
  gcn::BasicContainer * parent_widget = getParent();
  if (parent_widget == 0) {
    std::cout << "NO PARENT" << std::endl << std::flush;
    return;
  }
  gcn::Container * parent = dynamic_cast<gcn::Container *>(parent_widget);
  if (parent == 0) {
    std::cout << "WEIRD PARENT" << std::endl << std::flush;
    return;
  }
  parent->remove(this);

}

} // namespace Sear
