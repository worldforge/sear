// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall, University of Southampton

// $Id: StateManager.cpp,v 1.1 2003-03-15 17:18:49 simon Exp $

/*
 * TODO
 * Allow states flushing
 * custom of default settings
 * */

#include <sage/sage.h>
#include <GL/gl.h>

#include "StateManager.h"

#include <varconf/varconf.h>

#include "common/Log.h"

#ifdef HAVE_CONFIG
  #include "config.h"
#endif

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif


#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

// Varconf Key names.
static const std::string ALPHA_TEST = "alpha_test";
static const std::string BLEND = "blend";
static const std::string LIGHTING = "lighting";
static const std::string TWO_SIDED_LIGHTING = "two_sided_lighting";
static const std::string TEXTURES = "textures";
static const std::string COLOUR_MATERIAL = "colour_material";
static const std::string DEPTH_TEST = "depth_test";
static const std::string CULL_FACE = "cull_face";
static const std::string CULL_FACE_CW = "cull_face_cw";
static const std::string STENCIL = "stencil";
static const std::string FOG = "fog";
static const std::string RESCALE_NORMALS = "rescale_normals";
static const std::string ALPHA_FUNCTION = "alpha_function";
static const std::string ALPHA_VALUE = "alpha_value";
static const std::string BLEND_SRC_FUNCTION = "blend_src_function";
static const std::string BLEND_DEST_FUNCTION = "blend_dest_function";

static const std::string ALPHA_greater = "greater";
static const std::string ALPHA_less = "less";
static const std::string ALPHA_equal = "equal";
static const std::string ALPHA_notequal = "notequal";
static const std::string ALPHA_gequal = "greater_or_equal";
static const std::string ALPHA_lequal = "less_or_equal";
static const std::string ALPHA_always = "always";
static const std::string ALPHA_never = "never";
static const std::string BLEND_zero = "zero";
static const std::string BLEND_one = "one";
static const std::string BLEND_dst_color = "dst_colour";
static const std::string BLEND_one_minus_dst_color = "one_minus_dst_colour";
static const std::string BLEND_src_alpha = "src_alpha";
static const std::string BLEND_one_minus_src_alpha = "one_minus_src_alpha";
static const std::string BLEND_dst_alpha = "dst_alpha";
static const std::string BLEND_one_minus_dst_alpha = "one_minus_dst_alpha";
static const std::string BLEND_constant_color_ext = "constant_colour_ext";
static const std::string BLEND_one_minus_constant_color_ext = "one_minus_constant_color_ext";
static const std::string BLEND_constant_alpha_ext = "constant_alpha_ext";
static const std::string BLEND_one_minus_constant_alpha_ext = "one_minus_constant_alpha_ext";
static const std::string BLEND_src_alpha_saturate = "src_alpha_saturate";

StateManager::StateManager() :
  _initialised(false),
  _current_state(-1),
  _state_counter(0)
{}

StateManager::~StateManager() {
  if (_initialised) shutdown();
}

void StateManager::init() {
  if (_initialised) shutdown();
  if (debug) std::cout << "State Loader: Initialising." << std::endl;

  StateProperties *default_state = new StateProperties;
  StateProperties *font_state = new StateProperties;
  // Create a default record
  default_state->state = "default";
  default_state->alpha_test = false;
  default_state->blend = false;
  default_state->lighting = false;
  default_state->two_sided_lighting = false;
  default_state->textures = false;
  default_state->colour_material = false;
  default_state->depth_test = false;
  default_state->cull_face = false;
  default_state->cull_face_cw = false;
  default_state->stencil = false;
  default_state->fog = false;
  default_state->rescale_normals = false;
  default_state->alpha_function = GL_GREATER;
  default_state->alpha_value = 0.1f;
  default_state->blend_src_function = GL_SRC_ALPHA;
  default_state->blend_dest_function = GL_ONE_MINUS_SRC_ALPHA;
  _states[_state_counter] = default_state;
  _state_name_map[default_state->state] = _state_counter;
  _name_state_vector[_state_counter] = default_state->state;
  ++_state_counter;
  
  //Create a font state so we can still see text
  //even if no files have been loaded into Sear
  font_state->state = "font";
  font_state->alpha_test = true;
  font_state->blend = false;
  font_state->lighting = false;
  font_state->two_sided_lighting = false;
  font_state->textures = true;
  font_state->colour_material = false;
  font_state->depth_test = false;
  font_state->cull_face = false;
  font_state->cull_face_cw = false;
  font_state->stencil = false;
  font_state->fog = false;
  font_state->rescale_normals = false;
  default_state->alpha_function = GL_GREATER;
  default_state->alpha_value = 0.1f;
  default_state->blend_src_function = GL_SRC_ALPHA;
  default_state->blend_dest_function = GL_ONE_MINUS_SRC_ALPHA;
  _states[_state_counter] = font_state;
  _state_name_map[font_state->state] = _state_counter;
  _name_state_vector[_state_counter] = font_state->state;
  ++_state_counter;

  _initialised = true;
}


void StateManager::shutdown() {
  if (debug) std::cout << "State Loader: Shutting Down" << std::endl;
  while (!_states.empty()) {
    if (*_states.begin()) delete(*_states.begin());
    _states.erase(_states.begin());
  }
  _initialised = false;
}

void StateManager::readFiles(const std::string &file_name) {
  varconf::Config config;
  config.sigsv.connect(SigC::slot(*this, &StateManager::varconf_callback));
  config.sige.connect(SigC::slot(*this, &StateManager::varconf_error_callback));
  config.readFromFile(file_name);
}

void StateManager::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  StateProperties *record = _states[_state_name_map[section]];
  // If record does not exist, create it.
  if (!record) {
      record = new StateProperties();
      record->state = section;
      // Setup default values
      record->alpha_test = false;
      record->blend = false;
      record->lighting = false;
      record->two_sided_lighting = false;
      record->textures = false;
      record->colour_material = false;
      record->depth_test = false;
      record->cull_face = false;
      record->cull_face_cw = false;
      record->stencil = false;
      record->fog = false;
      record->rescale_normals = false;
      record->alpha_function = GL_GREATER;
      record->alpha_value = 0.1f;
      record->blend_src_function = GL_SRC_ALPHA;
      record->blend_dest_function = GL_ONE_MINUS_SRC_ALPHA;
  
      _states[_state_counter] = record;
      _state_name_map[record->state] = _state_counter;
      _name_state_vector[_state_counter] = record->state;
      ++_state_counter;
      if (debug)std::cout << "Adding State: " << section << std::endl;
  }

  if (key == ALPHA_TEST) record->alpha_test = (bool)config.getItem(section, key);
  else if (key == BLEND) record->blend = (bool)config.getItem(section, key);
  else if (key == LIGHTING) record->lighting = (bool)config.getItem(section, key);
  else if (key == TWO_SIDED_LIGHTING) record->two_sided_lighting = (bool)config.getItem(section, key);
  else if (key == TEXTURES) record->textures = (bool)config.getItem(section, key);
  else if (key == COLOUR_MATERIAL) record->colour_material = (bool)config.getItem(section, key);
  else if (key == DEPTH_TEST) record->depth_test = (bool)config.getItem(section, key);
  else if (key == CULL_FACE) record->cull_face = (bool)config.getItem(section, key);
  else if (key == CULL_FACE_CW) record->cull_face_cw = (bool)config.getItem(section, key);
  else if (key == STENCIL) record->stencil = (bool)config.getItem(section, key);
  else if (key == FOG) record->fog = (bool)config.getItem(section, key);
  else if (key == RESCALE_NORMALS) record->rescale_normals = (bool)config.getItem(section, key);
  else if (key == ALPHA_FUNCTION) record->alpha_function = getAlphaFunction((std::string)config.getItem(section, key));
  else if (key == ALPHA_VALUE) record->alpha_value = (double)config.getItem(section, key);
  else if (key == BLEND_SRC_FUNCTION) record->blend_src_function = getBlendFunction((std::string)config.getItem(section, key));
  else if (key == BLEND_DEST_FUNCTION) record->blend_dest_function = getBlendFunction((std::string)config.getItem(section, key));
}

void StateManager::varconf_error_callback(const char *message) {
  Log::writeLog(message, Log::LOG_ERROR);
}

unsigned int StateManager::getAlphaFunction(const std::string &alpha_function) {
  if (alpha_function == ALPHA_greater) return GL_GREATER;
  if (alpha_function == ALPHA_less) return GL_LESS;
  if (alpha_function == ALPHA_equal) return GL_EQUAL;
  if (alpha_function == ALPHA_notequal) return GL_NOTEQUAL;
  if (alpha_function == ALPHA_gequal) return GL_GEQUAL;
  if (alpha_function == ALPHA_lequal) return GL_LEQUAL;
  if (alpha_function == ALPHA_always) return GL_ALWAYS;
  if (alpha_function == ALPHA_never) return GL_NEVER;
}

unsigned int StateManager::getBlendFunction(const std::string &blend_function) {
  if (blend_function == BLEND_zero) return GL_ZERO;
  if (blend_function == BLEND_one) return GL_ONE;
  if (blend_function == BLEND_dst_color) return GL_DST_COLOR;
  if (blend_function == BLEND_one_minus_dst_color) return GL_ONE_MINUS_DST_COLOR;
  if (blend_function == BLEND_src_alpha) return GL_SRC_ALPHA;
  if (blend_function == BLEND_one_minus_src_alpha) return GL_ONE_MINUS_SRC_ALPHA;
  if (blend_function == BLEND_dst_alpha) return GL_DST_ALPHA;
  if (blend_function == BLEND_one_minus_dst_alpha) return GL_ONE_MINUS_DST_ALPHA;
  if (blend_function == BLEND_constant_color_ext) return GL_CONSTANT_COLOR_EXT;
  if (blend_function == BLEND_one_minus_constant_color_ext) return GL_ONE_MINUS_CONSTANT_COLOR_EXT;
  if (blend_function == BLEND_constant_alpha_ext) return GL_CONSTANT_ALPHA_EXT;
  if (blend_function == BLEND_one_minus_constant_alpha_ext) return GL_ONE_MINUS_CONSTANT_ALPHA_EXT;
  if (blend_function == BLEND_src_alpha_saturate) return GL_SRC_ALPHA_SATURATE;
}

void StateManager::stateChange(StateID state) {
  assert(_initialised == true);
  if (_current_state == state) return; // No need to do anything
  StateProperties *sp = _states[state];
  if (!sp) {
    sp = _states[0];
    state = 0;
  }
  // First time round, we have no states
  if (_current_state != -1) {
//    std::string change = std::string(_cur_state->state) + std::string("_to_") + std::string(sp->state);
    GLuint list = _state_change_vector[_current_state][state];
    if (!glIsList(list)) {
      list = glGenLists(1);
      buildStateChange(list, _states[_current_state], sp);
      _state_change_vector[_current_state][state] = list;
    }
    glCallList(list);
  } else {
    // We can't build up a transition, so we just go ahead directly	  
    if (sp->alpha_test) glEnable(GL_ALPHA_TEST);
    else glDisable(GL_ALPHA_TEST);
    if (sp->blend) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
    if (sp->lighting && checkState(RENDER_LIGHTING)) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);
    if (sp->two_sided_lighting && checkState(RENDER_LIGHTING)) glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    else glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    if (sp->textures && checkState(RENDER_TEXTURES)) glEnable(GL_TEXTURE_2D);
    else glDisable(GL_TEXTURE_2D);
    if (sp->colour_material) glEnable(GL_COLOR_MATERIAL);
    else glDisable(GL_COLOR_MATERIAL);
    if (sp->depth_test) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
    if (sp->cull_face) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
    if (sp->cull_face_cw) glFrontFace(GL_CW);
    else glFrontFace(GL_CCW);
    if (sp->stencil) glEnable(GL_STENCIL_TEST);
    else glDisable(GL_STENCIL_TEST);
    if (sp->fog) glEnable(GL_FOG);
    else glDisable(GL_FOG);
    if (sp->rescale_normals) glEnable(GL_RESCALE_NORMAL);
    else glDisable(GL_RESCALE_NORMAL);
    glAlphaFunc(sp->alpha_function, sp->alpha_value);
    glBlendFunc(sp->blend_src_function, sp->blend_dest_function);
  }
  _current_state = state;
}

void StateManager::buildStateChange(unsigned int &list, StateProperties *previous_state, StateProperties *next_state) {
  assert (previous_state != NULL);
  assert (next_state != NULL);
  glNewList(list, GL_COMPILE);
  if (previous_state->alpha_test != next_state->alpha_test) {
    if (next_state->alpha_test) glEnable(GL_ALPHA_TEST);
    else glDisable(GL_ALPHA_TEST);
  }
  if (previous_state->blend != next_state->blend) {
    if (next_state->blend) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
  }
  if (previous_state->lighting != next_state->lighting) {
    if (next_state->lighting && checkState(RENDER_LIGHTING)) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);
  }
  if (previous_state->two_sided_lighting != next_state->two_sided_lighting) {
    if (next_state->lighting && checkState(RENDER_LIGHTING)) glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    else glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
  }
  if (previous_state->textures != next_state->textures) {
    if (next_state->textures && checkState(RENDER_TEXTURES)) glEnable(GL_TEXTURE_2D);
    else glDisable(GL_TEXTURE_2D);
  }
  if (previous_state->colour_material != next_state->colour_material) {
    if (next_state->colour_material) glEnable(GL_COLOR_MATERIAL);
    else glDisable(GL_COLOR_MATERIAL);
  }
  if (previous_state->depth_test != next_state->depth_test) {
    if (next_state->depth_test) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
  }
  if (previous_state->cull_face != next_state->cull_face) {
    if (next_state->cull_face) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
  }
  if (previous_state->cull_face_cw != next_state->cull_face_cw) {
    if (next_state->cull_face_cw) glFrontFace(GL_CW);
    else glFrontFace(GL_CCW);
  }
  if (previous_state->stencil != next_state->stencil) {
    if (next_state->stencil) glEnable(GL_STENCIL_TEST);
    else glDisable(GL_STENCIL_TEST);
  }
  if (previous_state->fog != next_state->fog) {
    if (next_state->fog) glEnable(GL_FOG);
    else glDisable(GL_FOG);
  }
  if (previous_state->rescale_normals != next_state->rescale_normals) {
    if (next_state->rescale_normals) glEnable(GL_RESCALE_NORMAL);
    else glDisable(GL_RESCALE_NORMAL);
  }
  glAlphaFunc(next_state->alpha_function, next_state->alpha_value);
  glBlendFunc(next_state->blend_src_function, next_state->blend_dest_function);
  
  glEndList();
}

} /* namespace Sear */
