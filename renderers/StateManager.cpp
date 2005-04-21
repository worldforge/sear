// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: StateManager.cpp,v 1.21 2005-04-21 21:43:49 simon Exp $

/*
 * TODO
 * custom of default settings
 * */

#include <sage/sage.h>
#include <sage/GL.h>

#include "StateManager.h"

#include <varconf/varconf.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "src/Console.h"
#include "src/System.h"
#include "src/FileHandler.h"


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
static const std::string TEXTURE = "texture_";
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

static const std::string CMD_LOAD_STATE_CONFIG = "load_state_file";

StateManager::StateManager() :
  m_initialised(false),
  m_current_state(-1),
  m_state_counter(1)
{}

StateManager::~StateManager() {
  if (m_initialised) shutdown();
}

int StateManager::init() {
  assert(m_initialised == false);
  if (m_initialised) shutdown();
  if (debug) std::cout << "State Loader: Initialising." << std::endl;

  m_current_state = -1;
  m_state_counter = 1;

  m_states.resize(256);
  m_name_state_vector.resize(256);
  m_state_change_vector.resize(256);
  for (unsigned int i = 0; i < 256; m_state_change_vector[i++].resize(256));
  
  StateProperties *default_state = new StateProperties;
  StateProperties *font_state = new StateProperties;
  StateProperties *select_state = new StateProperties;
  StateProperties *cursor_state = new StateProperties;
  // Create a default record
  default_state->state = "default";
  default_state->alpha_test = false;
  default_state->blend = false;
  default_state->lighting = false;
  default_state->two_sided_lighting = false;
  default_state->textures[0] = true;
  for (unsigned int i = 1; i < MAX_UNITS; ++i)
    default_state->textures[i] = false;
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
  m_states[m_state_counter] = default_state;
  m_state_name_map[default_state->state] = m_state_counter;
  m_name_state_vector[m_state_counter] = default_state->state;
  ++m_state_counter;
  
  //Create a font state so we can still see text
  //even if no files have been loaded into Sear
  font_state->state = "font";
  font_state->alpha_test = true;
  font_state->blend = true;
  font_state->lighting = false;
  font_state->two_sided_lighting = false;
  font_state->textures[0] = true;
  for (unsigned int  i = 1; i < MAX_UNITS; ++i)
    font_state->textures[i] = false;
  font_state->colour_material = false;
  font_state->depth_test = false;
  font_state->cull_face = false;
  font_state->cull_face_cw = false;
  font_state->stencil = false;
  font_state->fog = false;
  font_state->rescale_normals = false;
  font_state->alpha_function = GL_GREATER;
  font_state->alpha_value = 0.1f;
  font_state->blend_src_function = GL_SRC_ALPHA;
  font_state->blend_dest_function = GL_ONE_MINUS_SRC_ALPHA;
  m_states[m_state_counter] = font_state;
  m_state_name_map[font_state->state] = m_state_counter;
  m_name_state_vector[m_state_counter] = font_state->state;
  ++m_state_counter;
 
  // Create default select mode state
  select_state->state = "select";
  select_state->alpha_test = false;
  select_state->blend = false;
  select_state->lighting = false;
  select_state->two_sided_lighting = false;
  for (unsigned int  i = 0; i < MAX_UNITS; ++i)
    select_state->textures[i] = false;
  select_state->colour_material = false;
  select_state->depth_test = true;
  select_state->cull_face = false;
  select_state->cull_face_cw = false;
  select_state->stencil = false;
  select_state->fog = false;
  select_state->rescale_normals = false;
  select_state->alpha_function = GL_GREATER;
  select_state->alpha_value = 0.1f;
  select_state->blend_src_function = GL_SRC_ALPHA;
  select_state->blend_dest_function = GL_ONE_MINUS_SRC_ALPHA;
  m_states[m_state_counter] = select_state;
  m_state_name_map[select_state->state] = m_state_counter;
  m_name_state_vector[m_state_counter] = select_state->state;
  ++m_state_counter;

  // Create default state for mouse cursor
  cursor_state->state = "cursor";
  cursor_state->alpha_test = true;
  cursor_state->blend = false;
  cursor_state->lighting = false;
  cursor_state->two_sided_lighting = false;
  for (unsigned int  i = 1; i < MAX_UNITS; ++i)
    cursor_state->textures[i] = false;
  cursor_state->textures[0] = true;
  cursor_state->colour_material = false;
  cursor_state->depth_test = false;
  cursor_state->cull_face = false;
  cursor_state->cull_face_cw = false;
  cursor_state->stencil = false;
  cursor_state->fog = false;
  cursor_state->rescale_normals = false;
  cursor_state->alpha_function = GL_GREATER;
  cursor_state->alpha_value = 0.1f;
  cursor_state->blend_src_function = GL_SRC_ALPHA;
  cursor_state->blend_dest_function = GL_ONE_MINUS_SRC_ALPHA;
  m_states[m_state_counter] = cursor_state;
  m_state_name_map[cursor_state->state] = m_state_counter;
  m_name_state_vector[m_state_counter] = cursor_state->state;
  ++m_state_counter;


  m_initialised = true;
  return 0;
}

int StateManager::shutdown() {
  assert(m_initialised);
  if (debug) std::cout << "State Loader: Shutdown" << std::endl;
  while (!m_states.empty()) {
//    if (*m_states.begin()) {
      StateProperties *sp = *(m_states.begin());
      delete sp;
  //  }
    m_states.erase(m_states.begin());
  }
  m_initialised = false;
  return 0;
}

void StateManager::readFiles(const std::string &file_name) {
  assert(m_initialised);
  varconf::Config config;
  config.sigsv.connect(SigC::slot(*this, &StateManager::varconf_callback));
  config.sige.connect(SigC::slot(*this, &StateManager::varconf_error_callback));
  config.readFromFile(file_name);
}

void StateManager::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  assert(m_initialised);
  int in = m_state_name_map[section];
  StateProperties *record = m_states[m_state_name_map[section]];
  // If record does not exist, create it.
//  if (!record) {
  if (in == 0) {
      record = new StateProperties();
      record->state = section;
      // Setup default values
      record->alpha_test = false;
      record->blend = false;
      record->lighting = false;
      record->two_sided_lighting = false;
      for (unsigned int i = 0; i < MAX_UNITS; ++i)
        record->textures[i] = false;
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
  
      m_states[m_state_counter] = record;
      m_state_name_map[record->state] = m_state_counter;
      m_name_state_vector[m_state_counter] = record->state;
      ++m_state_counter;
      if (debug) std::cout << "Adding State: " << section << std::endl;
  }

  if (key == ALPHA_TEST) record->alpha_test = (bool)config.getItem(section, key);
  else if (key == BLEND) record->blend = (bool)config.getItem(section, key);
  else if (key == LIGHTING) record->lighting = (bool)config.getItem(section, key);
  else if (key == TWO_SIDED_LIGHTING) record->two_sided_lighting = (bool)config.getItem(section, key);
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
  else if (key.substr(0, TEXTURE.size()) == TEXTURE) {
    unsigned int unit;
    cast_stream(key.substr(TEXTURE.size()), unit);
    if (unit < MAX_UNITS) {
      record->textures[unit] = (bool)config.getItem(section, key);
//      std::cout << "Setting " << section << " texture unit " << unit << " to " << record->textures[unit] << std::endl;
    }
  }
//  try {
//  std::cout << key << " - " << key.substr(0, TEXTURE.size()) << " - " << key.substr(TEXTURE.size()) << std::endl;
//  } catch (...) {}
}

void StateManager::varconf_error_callback(const char *message) {
  Log::writeLog(message, Log::LOG_ERROR);
}

StateID StateManager::getState(const std::string &state_name) const {
  assert(m_initialised);
  StateNameMap::const_iterator S = m_state_name_map.find(state_name);
  if (S == m_state_name_map.end()) {
    std::cerr << "state " << state_name << " is unknown" << std::endl;
    return 1;
  }
  assert(S->second > 0); 
  return S->second;
}
  
int StateManager::getAlphaFunction(const std::string &alpha_function) {
  if (alpha_function == ALPHA_greater) return GL_GREATER;
  if (alpha_function == ALPHA_less) return GL_LESS;
  if (alpha_function == ALPHA_equal) return GL_EQUAL;
  if (alpha_function == ALPHA_notequal) return GL_NOTEQUAL;
  if (alpha_function == ALPHA_gequal) return GL_GEQUAL;
  if (alpha_function == ALPHA_lequal) return GL_LEQUAL;
  if (alpha_function == ALPHA_always) return GL_ALWAYS;
  if (alpha_function == ALPHA_never) return GL_NEVER;
  return 0;
}

int StateManager::getBlendFunction(const std::string &blend_function) {
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
  return 0;
}

void StateManager::stateChange(StateID state) {
  assert(m_initialised == true);
  if (m_current_state == state) return; // No need to do anything
  StateProperties *sp = m_states[state];
  // If state doesn't exist, take first one
  assert(sp);
  if (!sp) {
    std::cout << "bad state found - " << state <<  std::endl;
    sp = m_states[1];
    state = 1;
  }
  assert(sp != NULL);
  // First time round, we have no states
  if (m_current_state != -1) {
    unsigned int list = m_state_change_vector[m_current_state][state];
    // Check whether we need to generate a display list
    if (!glIsList(list)) {
      list = glGenLists(1);
      buildStateChange(list, m_states[m_current_state], sp);
      m_state_change_vector[m_current_state][state] = list;
    }
    glCallList(list);
  } else {
    // We can't build up a transition, so we just go ahead directly	  
    if (sp->alpha_test) glEnable(GL_ALPHA_TEST);
    else glDisable(GL_ALPHA_TEST);
    if (sp->blend) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
//    if (sp->lighting && checkState(RENDER_LIGHTING)) glEnable(GL_LIGHTING);
    if (sp->lighting) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);
//    if (sp->two_sided_lighting && checkState(RENDER_LIGHTING)) glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    if (sp->two_sided_lighting) glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    else glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
//    if (sp->textures && checkState(RENDER_TEXTURES)) glEnable(GL_TEXTURE_2D);
    for (int i = MAX_UNITS - 1; i >= 0; --i) {
      glActiveTextureARB(GL_TEXTURE0_ARB + i);
      if (sp->textures[i]) glEnable(GL_TEXTURE_2D);
      else glDisable(GL_TEXTURE_2D);
    }
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
    // TODO is this broken? 0 could be a valid state....
//    if (sp->alpha_function != 0) {
      glAlphaFunc(sp->alpha_function, sp->alpha_value);
//    }
//    if (sp->blend_src_function != 0) {
      glBlendFunc(sp->blend_src_function, sp->blend_dest_function);
  //  }
  }
  m_current_state = state;
}

void StateManager::buildStateChange(unsigned int &list, StateProperties *previous_state, StateProperties *next_state) {
  assert(m_initialised);
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
//    if (next_state->lighting && checkState(RENDER_LIGHTING)) glEnable(GL_LIGHTING);
    if (next_state->lighting) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);
  }
  if (previous_state->two_sided_lighting != next_state->two_sided_lighting) {
//    if (next_state->lighting && checkState(RENDER_LIGHTING)) glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    if (next_state->lighting) glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    else glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
  }
 
  for (int i = MAX_UNITS - 1; i >= 0; --i) {
    glActiveTextureARB(GL_TEXTURE0_ARB + i);
    if (previous_state->textures[i] != next_state->textures[i]) {
//      if (next_state->textures && checkState(RENDER_TEXTURES)) glEnable(GL_TEXTURE_2D);
      if (next_state->textures[i]) glEnable(GL_TEXTURE_2D);
      else glDisable(GL_TEXTURE_2D);
    }
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
//  if ((next_state->alpha_function != previous_state->alpha_function) || (next_state->alpha_value != previous_state->alpha_value)) 
  glAlphaFunc(next_state->alpha_function, next_state->alpha_value);
  glBlendFunc(next_state->blend_src_function, next_state->blend_dest_function);
  glEndList();
}

void StateManager::registerCommands(Console *console) {
  assert(m_initialised);
  console->registerCommand(CMD_LOAD_STATE_CONFIG, this);
}

void StateManager::runCommand(const std::string &command, const std::string &arguments) {
  assert(m_initialised);
  if (command == CMD_LOAD_STATE_CONFIG) {
    std::string a = arguments;
    System::instance()->getFileHandler()->expandString(a);
    readFiles(a);
  }
}

void StateManager::invalidate() {
  assert(m_initialised);
  for (unsigned int i = 0; i < m_state_change_vector.size(); ++i) {
    for (unsigned int j = 0; j < m_state_change_vector[i].size(); ++j) {
      // Delete display list if its still valid
      if (glIsList(m_state_change_vector[i][j])) {
        glDeleteLists(m_state_change_vector[i][j], 1);
      }
      // reset list value
      m_state_change_vector[i][j] = 0; 
    }
  }
  m_current_state = -1;
}

} /* namespace Sear */
