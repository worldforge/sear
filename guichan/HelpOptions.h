// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_HELP_OPTIONS_H
#define SEAR_GUICHAN_HELP_OPTIONS_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

#include "common/SPtr.h"

namespace gcn {
class ListBox;
class TextBox;
class ScrollArea;
}

namespace Sear {

class RootWidget;
class HelpFileList;

class HelpOptions : public gcn::Window, public gcn::ActionListener {
protected:
  std::list<SPtr<gcn::Widget> > m_widgets;
  RootWidget * m_top;

  HelpFileList * m_fileList;

  gcn::ListBox * m_helpFiles;
  gcn::TextBox * m_helpText;
  gcn::ScrollArea * m_filesScroll;
  gcn::ScrollArea * m_textScroll;

  int m_lastSelection;
public:
  explicit HelpOptions(RootWidget * top);
  virtual ~HelpOptions();

  void action(const std::string &);
  virtual void logic();
};

} // namespace Sear

#endif // SEAR_GUICHAN_HELP_OPTIONS_H
