// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/HelpOptions.h"

#include "guichan/RootWidget.h"
#include "guichan/box.hpp"

#include "src/System.h"
#include "src/FileHandler.h"

#include <guichan.hpp>

#include <iostream>

#include <sys/types.h>

#include <dirent.h>

namespace Sear {

class HelpFileList : public gcn::ListModel
{
protected:
  std::vector<std::string> files;
public:
  HelpFileList()
  {
    std::string help_dir = "${SEAR_INSTALL}/data/help";
    System::instance()->getFileHandler()->getFilePath(help_dir);
    DIR * help_list = opendir(help_dir.c_str());
    if (help_list == NULL) {
      std::cout << "Unable to open help file directory" << std::endl << std::flush;
    } else {
      struct dirent * entry;
      while ((entry = readdir(help_list)) != NULL) {
        if (entry->d_name[0] != '.') {
          files.push_back(entry->d_name);
          std::cout << "Adding " << entry->d_name << std::endl << std::flush;
        }
      }
      closedir(help_list);
    } 
  }

  virtual ~HelpFileList() {
  }

  virtual int getNumberOfElements()
  {
    return files.size();
  }

  virtual std::string getElementAt(int n)
  {
    if (n >= files.size()) {
      return "NONE";
    }
    return files[n];
  }
};

HelpOptions::HelpOptions(RootWidget * top) : gcn::Window("Help"),
                                             m_top(top), m_lastSelection(-1)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  gcn::Box * vbox = new gcn::VBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(vbox));

  gcn::Box * hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  m_fileList = new HelpFileList;

  m_helpFiles = new gcn::ListBox(m_fileList);
  m_widgets.push_back(SPtr<gcn::Widget>(m_helpFiles));
  m_helpFiles->setFocusable(false);
  m_filesScroll = new gcn::ScrollArea(m_helpFiles, gcn::ScrollArea::SHOW_AUTO,
                                                   gcn::ScrollArea::SHOW_ALWAYS);
  m_widgets.push_back(SPtr<gcn::Widget>(m_filesScroll));
  m_filesScroll->setWidth(100);
  m_filesScroll->setHeight(300);
  m_filesScroll->setBorderSize(1);
  hbox->pack(m_filesScroll);

  m_helpText = new gcn::TextBox;
  m_widgets.push_back(SPtr<gcn::Widget>(m_helpText));
  m_helpText->setEditable(false);
  m_helpText->setOpaque(false);
  m_helpText->setFocusable(false);
  m_helpText->setEnabled(false);
  m_textScroll = new gcn::ScrollArea(m_helpText, gcn::ScrollArea::SHOW_AUTO,
                                                 gcn::ScrollArea::SHOW_ALWAYS);
  m_widgets.push_back(SPtr<gcn::Widget>(m_textScroll));
  m_textScroll->setWidth(500);
  m_textScroll->setHeight(300);
  m_textScroll->setBorderSize(1);
  hbox->pack(m_textScroll);

  vbox->pack(hbox);

  hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  gcn::Button * b = new gcn::Button("Apply");
  m_widgets.push_back(SPtr<gcn::Widget>(b));
  b->setEventId("apply");
  b->setFocusable(false);
  b->addActionListener(this);
  hbox->pack(b);

  b = new gcn::Button("Close");
  m_widgets.push_back(SPtr<gcn::Widget>(b));
  b->setEventId("close");
  b->setFocusable(false);
  b->addActionListener(this);
  hbox->pack(b);

  vbox->pack(hbox);

  add(vbox);

  resizeToContent();
}

HelpOptions::~HelpOptions()
{
  delete m_fileList;
}

void HelpOptions::action(const std::string & event, gcn::Widget *widget)
{
  if (event == "apply") {
    std::cout << "Apply changes" << std::endl << std::flush;
  } else if (event == "close") {
    m_top->closeWindow(this);
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }
}

void HelpOptions::logic()
{
  int new_file = m_helpFiles->getSelected();
  if (m_lastSelection != new_file) {
    m_lastSelection = new_file;
    m_helpText->setText("");

    std::string help_dir = "${SEAR_INSTALL}/data/help";
    System::instance()->getFileHandler()->expandString(help_dir);

    std::string filename = help_dir + "/" + m_fileList->getElementAt(new_file);
    FILE * help_file = fopen(filename.c_str(), "r");
    if (help_file != NULL) {
      char buf[256];
      while (fgets(buf, 256, help_file) != NULL) {
        size_t n = strlen(buf);
        if (n > 0) {
          // Wipe out the trailing \n
          if (buf[n - 1] == '\n') {
            buf[n - 1] = 0;
          }
        }
        m_helpText->addRow(buf);
      }
    } else {
      std::cerr << "Unable to open help file";
    }
  }
  gcn::Window::logic();
}

} // namespace Sear
