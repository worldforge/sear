// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: StateLoader.h,v 1.5 2002-09-08 00:24:53 simon Exp $

#ifndef SEAR_STATELOADER_H
#define SEAR_STATELOADER_H 1

#include <string>
#include <stdio.h>
#include <map>

namespace Sear {
  static const unsigned int MAX_STRING_SIZE = 256;

typedef struct {
  char state[MAX_STRING_SIZE];
  bool alpha_test;
  bool blend;
  bool lighting;
  bool two_sided_lighting;
  bool textures;
  bool colour_material;
  bool depth_test;
  bool cull_face;
  bool cull_face_cw;
  bool stencil;
  bool fog;
  bool rescale_normals;
} StateProperties;

class StateLoader {
public:
  StateLoader();
  ~StateLoader();

  void init();
  void shutdown();
  void readFiles(const std::string &);
  std::map<std::string, StateProperties*> getMap() { return _state_properties; }

  StateProperties *readRecord(FILE *);
  StateProperties *getStateProperties(const std::string &state) {
    StateProperties *sp = _state_properties[state];
    return ((sp) ? (sp) : (_state_properties["default"]));
  }

protected:
  std::map<std::string, StateProperties*> _state_properties;
  bool _initialised;

};

} /* namespace Sear */
#endif /* SEAR_STATELOADER_H */
