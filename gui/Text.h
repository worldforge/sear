// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_TEXT_H
#define SEAR_GUI_TEXT_H

#include "gui/Window.h"

#include <string>

namespace Sear {

/// A rectangular window
class Text : public Window {
private:
  // Private and unimplemented to prevent slicing
  Text(const Text &);
  const Text & operator=(const Text &);
protected:
  std::string m_content;
public:
  explicit Text(const std::string &);
  virtual ~Text();

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

#endif // SEAR_GUI_TEXT_H
