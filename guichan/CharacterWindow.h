// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_CHARACTERWINDOW_H
#define SEAR_GUICHAN_CHARACTERWINDOW_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

#include "common/SPtr.h"

namespace gcn {

class ListBox;
class DropDown;
class TextField;
class Button;

} // namespace gcn

namespace Sear {

class ActionListenerSigC;
class CharacterListModel;

class CharacterWindow : virtual public sigc::trackable, public gcn::Window {
protected:
  std::list<SPtr<gcn::Widget> > m_widgets;
  gcn::ListBox * m_characters;
  gcn::DropDown * m_types;

  gcn::TextField * m_nameField;
  std::string m_typeField;

  gcn::Button * m_refreshButton;
  gcn::Button * m_charButton;
  gcn::Button * m_closeButton;

  CharacterListModel * m_characterListModel;
  ActionListenerSigC * m_buttonListener;

  int m_charSelected;
  int m_typeSelected;

  virtual void logic();
public:
  CharacterWindow();
  virtual ~CharacterWindow();

  void actionPressed(std::string);
};

} // namespace Sear

#endif // SEAR_GUICHAN_CHARACTERWINDOW_H
