// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 - 2008 Simon Goodall

#include <libwfut/WFUT.h>

#include <guichan.hpp>

#include "common/Utility.h"

#include "guichan/WFUTWindow.h"
#include "guichan/Workarea.h"
#include "guichan/ActionListenerSigC.h"
#include "guichan/box.hpp"
#include "guichan/bar.hpp"

#include "src/System.h"
#include "src/MediaManager.h"

namespace Sear {


/* TODO
 * Much larger status/message window.
 * -- Perhaps hook into a larger system messages gui component?
 * -- Also logged.
 * Better reporting of failed downloads.
 * Hook into re-loading of config files rather than suggest a restart
 * Might be useful to trigger update check in a thread, rather than causing UI to block.
 * Should re-work into showing table of updates + installed status/version etc
 * Tick box user updates?
 *
 * Need much better one-off update support.
 * -- Need complete callback. 
 *  -- Better error report callback.
 *  Automatic re-initialisation (complete shutdown / restart of system)?
 */

//** Need Table widget
//** Need Image widget from Sprite
//----- Convert compass to such a creation.

static void enableButton(gcn::Button *btn) {
    btn->setEnabled(true);
    btn->setFocusable(true);
    // TODO: Remove hard-coded colours
    btn->setBaseColor(gcn::Color(0x808090));
}

static void disableButton(gcn::Button *btn) {
    btn->setEnabled(false);
    btn->setFocusable(false);
    // TODO: Remove hard-coded colours
    btn->setBaseColor(gcn::Color(0x505070));
}

WFUTWindow::WFUTWindow()
{
  m_media_manager = System::instance()->getMediaManager();
  m_media_manager->DownloadComplete.connect(sigc::mem_fun(this, &WFUTWindow::onDownloadComplete));
  m_media_manager->DownloadFailed.connect(sigc::mem_fun(this, &WFUTWindow::onDownloadFailed));
  m_media_manager->UpdateReason.connect(sigc::mem_fun(this, &WFUTWindow::onUpdateReason));

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
  m_progress_bar->setWidth(400);
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
  m_status->setWidth(400 - l->getWidth());
  m_widgets.push_back(SPtr<gcn::Widget>(m_status));
  hbox->pack(m_status);

  vbox->pack(hbox);

  // Add a log area;
  // TODO: Make collapsable
  m_log = new gcn::TextBox("");
  m_widgets.push_back(SPtr<gcn::Widget>(m_log));
  m_log->setEditable(false);
  m_log->setOpaque(false);
  m_log->setFocusable(false);
  m_log->setEnabled(false);
  m_log->setBackgroundColor(gcn::Color(0,0,0,0));

  gcn::ScrollArea *scrollArea = new gcn::ScrollArea(m_log,
                                       gcn::ScrollArea::SHOW_AUTO,
                                       gcn::ScrollArea::SHOW_ALWAYS);
  m_widgets.push_back(SPtr<gcn::Widget>(scrollArea));
  scrollArea->setWidth(400);
  scrollArea->setHeight(100);
  scrollArea->setFrameSize(1);

  vbox->pack(scrollArea);

  hbox = new gcn::HBox();
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));
  // Add an Update button
  m_update = new gcn::Button("Update");
  m_widgets.push_back(SPtr<gcn::Widget>(m_update));
  m_update->setActionEventId("update");
  m_update->addActionListener(m_button_listener);
  hbox->pack(m_update);

  // Add a Cancel button
  m_cancel = new gcn::Button("Cancel");
  m_widgets.push_back(SPtr<gcn::Widget>(m_cancel));
  m_cancel->setActionEventId("cancel");
  m_cancel->addActionListener(m_button_listener);
  // Not enabled until the update starts.
  m_cancel->setFocusable(false);
  m_cancel->setEnabled(false);
  hbox->pack(m_cancel);

  // Add a Close button
  m_close = new gcn::Button("Close");
  m_widgets.push_back(SPtr<gcn::Widget>(m_close));
  m_close->setActionEventId("close");
  m_close->addActionListener(m_button_listener);
  hbox->pack(m_close);

  vbox->pack(hbox);

  add(vbox);
  resizeToContent();
}

WFUTWindow::~WFUTWindow() {
  delete m_button_listener;
}

void WFUTWindow::logic() {

  // Update button enabled state.
  if (m_updates_completed == m_updates_total) {
    enableButton(m_update);
    disableButton(m_cancel);
  } else {
    disableButton(m_update);
    enableButton(m_cancel);
  }

  gcn::Window::logic();
}

void WFUTWindow::actionPressed(std::string event) {

  if (event == "update") {
    System::instance()->runCommand("/enable_updates");
    m_status->setCaption("Checking for updates");
    System::instance()->runCommand("/check_for_updates");
    m_updates_total = m_media_manager->getNumUpdates();
    m_updates_completed = 0;
    writeLogEntry("Found " + string_fmt(m_updates_total) + " updates");
    if (m_updates_total > 0) {
      m_status->setCaption("Downloading Updates");
      System::instance()->runCommand("/download_updates");
    } else {
      m_status->setCaption("No updates available");
    }
  } else if (event == "cancel") {
    m_status->setCaption("Cancelled Updater");
    m_media_manager->cancelAll();
  } else if (event == "close") {
    System::instance()->getWorkarea()->removeLater(this);
  } else {
    assert(false);
  }
}

void WFUTWindow::onDownloadComplete(const std::string &url, const std::string &filename) {
  ++m_updates_completed;
  m_status->setCaption("Downloaded: " + filename);
  writeLogEntry("Downloaded: " + filename);
  m_progress_bar->setValue((double)m_updates_completed / (double)m_updates_total);
  if (m_updates_completed == m_updates_total && m_updates_total > 0) {
    m_status->setCaption("Finished. Please restart Sear");
  }
}

void WFUTWindow::onDownloadFailed(const std::string &url, const std::string &filename, const std::string &reason) {
  ++m_updates_completed;
  m_progress_bar->setValue((double)m_updates_completed / (double)m_updates_total);
  writeLogEntry("Download Failed: " + filename);
  if (m_updates_completed == m_updates_total && m_updates_total > 0) {
    m_status->setCaption("Finished. Please restart Sear");
  }
}

void WFUTWindow::onUpdateReason(const std::string &filename, const WFUT::WFUTUpdateReason &reason) {
  if (reason != WFUT::WFUT_UPDATE_NONE && reason != WFUT::WFUT_UPDATE_DELETED && reason != WFUT::WFUT_UPDATE_MODIFIED) {
    writeLogEntry("Updating: " + filename);
  }
}

void WFUTWindow::writeLogEntry(const std::string &entry) {
  m_log->addRow(entry);
  m_log->setCaretRow(m_log->getNumberOfRows());
  m_log->scrollToCaret();
}

} // namespace Sear
