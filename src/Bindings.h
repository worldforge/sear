// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _BINDINGS_H_
#define _BINDINGS_H_ 1

#include <string>
#include <map>

// Forward Declarations
class Config;

class Bindings {
public:
  static void init();
  static void shutdown();
  static void loadBindings(const std::string &);
  static void saveBindings();
  static void saveBindings(const std::string &);
  static void bind(std::string, std::string);
  static std::string idToString(int);
  static std::string getBinding(std::string);

protected:
  static void initKeyMap();
  static std::map<int, std::string> keymap;
  static Config* _bindings;
};

#endif /* _BINDINGS_H_ */
