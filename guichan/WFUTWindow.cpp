// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

#include <libwfut/WFUT.h>

#include <guichan.hpp>

#include "guichan/WFUTWindow.h"
#include "guichan/ActionListenerSigC.h"
#include "guichan/box.hpp"
#include "guichan/bar.hpp"

#include "src/System.h"
#include "src/MediaManager.h"

namespace Sear {

WFUTWindow::WFUTWindow()
{
  m_media_manager = System::instance()->getMediaManager();
  m_media_manager->DownloadComplete.connect(sigc::mem_fun(this, &WFUTWindow::onDownloadComplete));
  m_media_manager->DownloadFailed.connect(sigc::mem_fun(this, &WFUTWindow::onDownloadFailed));

  // Setup base colour
  gcn::Color base = getBaseColor();
  setBaseColor(base);

  // Let this window be moved
  setMovable(true);

  // Set window title
  setCaption("Media Updater"); 

  m_button_listener = new ActionListenerSigC();
  m_button_listener->Action.connect(sigc::mem_fun(this, &WFUTWindow::actionPressed));

  gcn::VBox *vbox = new gcn::VBox();
  m_widgets.push_back(SPtr<gcn::Widget>(vbox));

  // Add in a progress bar
  m_progress_bar = new gcn::Bar();
  m_widgets.push_back(SPtr<gcn::Widget>(m_progress_bar));
  m_progress_bar->setValue(0.0);
  m_progress_bar->setWidth(256);
  m_progress_bar->setHeight(10);
  vbox->pack(m_progress_bar);

  // Add some buttons
  gcn::HBox *hbox = new gcn::HBox();
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  // Add in the current status.
  gcn::Label *l = new gcn::Label("Status: ");
  m_widgets.push_back(SPtr<gcn::Widget>(l));
  hbox->pack(l);

  // Add in the current status.
  m_status = new gcn::Label("");
  m_status->setWidth(256);
  m_widgets.push_back(SPtr<gcn::Widget>(m_status));
  hbox->pack(m_status);

  vbox->pack(hbox);

  hbox = new gcn::HBox();
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));
  // Add an Update button
  gcn::Button *update = new gcn::Button("Update");
  m_widgets.push_back(SPtr<gcn::Widget>(update));
  update->setActionEventId("update");
  update->addActionListener(m_button_listener);
  hbox->pack(update);

/*
  // Add a Cancel button
  gcn::Button *cancel = new gcn::Button("Cancel");
  m_widgets.push_back(SPtr<gcn::Widget>(cancel));
  cancel->setActionEventId("cancel");
  cancel->addActionListener(m_button_listener);
  // Not enabled until the update starts.
  cancel->setFocusable(false);
  hbox->pack(cancel);
*/
  vbox->pack(hbox);

  add(vbox);
  resizeToContent();
}

WFUTWindow::~WFUTWindow() {
  delete m_button_listener;
}

void WFUTWindow::logic() {
  gcn::Window::logic();
}

void WFUTWindow::actionPressed(std::string event) {

  if (event == "update") {
    System::instance()->runCommand("/enable_updates");
    m_status->setCaption("Checking for updates");
    System::instance()->runCommand("/check_for_updates");
    m_updates_total = m_media_manager->getNumUpdates();
    m_updates_completed = 0;
    if (m_updates_total > 0) {
      System::instance()->runCommand("/download_updates");
    }
  } else if (event == "cancel") {

  } else {
    assert(false);
  }
}

void WFUTWindow::onDownloadComplete(const std::string &url, const std::string &filename) {
  ++m_updates_completed;
  m_status->setCaption("Downloaded: " + filename);
  m_progress_bar->setValue((double)m_updates_completed / (double)m_updates_total);
}

void WFUTWindow::onDownloadFailed(const std::string &url, const std::string &filename, const std::string &reason) {
  ++m_updates_completed;
  m_progress_bar->setValue((double)m_updates_completed / (double)m_updates_total);
}


} // namespace Sear
