// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_STRING_H
#define SEAR_GUI_STRING_H

#include "gui/Window.h"

#include <string>

namespace Sear {

/// A rectangular window
class String : public Window {
private:
  // Private and unimplemented to prevent slicing
  String(const String &);
  const String & operator=(const String &);
protected:
  std::string m_content;
public:
  explicit String(const std::string &);
  virtual ~String();

  const std::string & content() const {
    return m_content;
  }

  std::string & content() {
    return m_content;
  }

  void setContent(const std::string &);

  virtual void render(Render *);
};

} // namespace Sear

#endif // SEAR_GUI_STRING_H
