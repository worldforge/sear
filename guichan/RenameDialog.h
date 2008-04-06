// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch
// Copyright (C) 2007 Simon Goodall

#ifndef SEAR_GUICHAN_RENAME_DIALOG_H
#define SEAR_GUICHAN_RENAME_DIALOG_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

#include "common/SPtr.h"

namespace gcn {

class Label;
class Button;
class TextField;
class CheckBox;

} // namespace gcn

namespace Sear {

class PasswordField;

class ActionListenerSigC;

class RenameDialog : virtual public sigc::trackable, public gcn::Window {
protected:
  std::list<SPtr<gcn::Widget> > m_widgets;
  gcn::Button * m_renameButton;
  gcn::Button * m_cancelButton;
  gcn::TextField * m_nameField;
  gcn::Label * m_nameLabel;

  std::string m_entity_id, m_old_name;

  ActionListenerSigC * m_buttonListener;

  virtual void logic();
public:
  RenameDialog(const std::string &id, const std::string &old_name);
  virtual ~RenameDialog();

  void actionPressed(std::string);
};

} // namespace Sear

#endif // SEAR_GUICHAN_RENAME_DIALOG_H
