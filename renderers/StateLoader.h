// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: StateLoader.h,v 1.1 2003-03-15 15:23:09 simon Exp $

#ifndef SEAR_STATELOADER_H
#define SEAR_STATELOADER_H 1

#include <string>
#include <map>

#include <sigc++/object_slot.h>


namespace varconf {
  class Config;
}

namespace Sear {

typedef struct {
  std::string state;
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
  float alpha_value;
  unsigned int alpha_function;
  unsigned int blend_src_function;
  unsigned int blend_dest_function;
} StateProperties;

class StateLoader : public SigC::Object {
public:
  StateLoader();
  ~StateLoader();

  void init();
  void shutdown();
  void readFiles(const std::string &);

  std::map<std::string, StateProperties*> getMap() const { return _state_properties; }

  StateProperties *getStateProperties(const std::string &state) {
    StateProperties *sp = _state_properties[state];
    return ((sp) ? (sp) : (_state_properties["default"]));
  }
  
protected:
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
  void varconf_error_callback(const char *message);
  
  std::map<std::string, StateProperties*> _state_properties;
  bool _initialised;

};

} /* namespace Sear */

#endif /* SEAR_STATELOADER_H */
