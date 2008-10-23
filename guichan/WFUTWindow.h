// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 - 2008 Simon Goodall

#ifndef SEAR_GUICHAN_WFUTWINDOW_H
#define SEAR_GUICHAN_WFUTWINDOW_H 1

#include <vector>

#include <guichan/widgets/window.hpp>
#include <sigc++/trackable.h>

#include <libwfut/WFUT.h>

#include "common/SPtr.h"

namespace gcn {

  class Bar;
  class Button;
  class Label;
  class TextBox;
} // namespace gcn

namespace Sear {

class ActionListenerSigC;
class MediaManager;

class WFUTWindow : virtual public sigc::trackable, public gcn::Window  {
public:
  WFUTWindow();
  virtual ~WFUTWindow();

  virtual void actionPressed(std::string);
  virtual void logic();

protected:

  void onUpdateReason(const std::string&, const WFUT::WFUTUpdateReason&);
  void onDownloadComplete(const std::string&, const std::string&);
  void onDownloadFailed(const std::string&, const std::string&, const std::string&);

  void writeLogEntry(const std::string &);

  ActionListenerSigC * m_button_listener;

  std::vector<SPtr<gcn::Widget> > m_widgets;

  gcn::Bar *m_progress_bar;
  gcn::Label *m_status;
  gcn::Button *m_cancel, *m_check, *m_update, *m_close;
  gcn::TextBox *m_log;

  size_t m_updates_total, m_updates_completed;
  MediaManager *m_media_manager;

};

} // namespace Sear

#endif // SEAR_GUICHAN_WFUTWINDOW_H
