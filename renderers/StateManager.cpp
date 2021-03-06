// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall, University of Southampton

// $Id: StateManager.cpp,v 1.35 2008-04-06 14:21:40 simon Exp $

/*
 * TODO
 * custom of default settings
 * */

#include <sstream>

#include <sigc++/object_slot.h>

#include <varconf/varconf.h>

#include <sage/sage.h>
#include <sage/GL.h>

#include "StateManager.h"


#include "common/Log.h"
#include "common/Utility.h"

#include "src/Console.h"
#include "src/System.h"
#include "src/FileHandler.h"

#include "default_states.h"

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
static const std::string DEPTH_WRITE = "depth_write";
static const std::string CULL_FACE = "cull_face";
static const std::string CULL_FACE_CW = "cull_face_cw";
static const std::string STENCIL = "stencil";
static const std::string FOG = "fog";
static const std::string RESCALE_NORMALS = "rescale_normals";
static const std::string ALPHA_FUNCTION = "alpha_function";
static const std::string ALPHA_VALUE = "alpha_value";
static const std::string BLEND_SRC_FUNCTION = "blend_src_function";
static const std::string BLEND_DEST_FUNCTION = "blend_dest_function";
static const std::string NORMALISE = "normalise";

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
static const std::string CMD_reload_config_states = "reload_config_states";

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

  if (debug) std::cout << "State Loader: Initialising." << std::endl;

  m_current_state = -1;
  m_state_counter = 1;

  m_states.resize(256);
  m_name_state_vector.resize(256);
  m_state_change_vector.resize(256);
  for (unsigned int i = 0; i < 256; m_state_change_vector[i++].resize(256));
  
  varconf::Config config;
  config.sigsv.connect(sigc::mem_fun(*this, &StateManager::varconf_callback));
  config.sige.connect(sigc::mem_fun(*this, &StateManager::varconf_error_callback));

  std::istringstream ss (default_states);
  config.parseStream(ss, varconf::GLOBAL);

  m_initialised = true;
  return 0;
}

int StateManager::shutdown() {
  assert(m_initialised == true);

  m_states.clear();

  m_initialised = false;
  return 0;
}

void StateManager::readFiles(const std::string &file_name) {
  assert(m_initialised);
  varconf::Config config;
  config.sigsv.connect(sigc::mem_fun(*this, &StateManager::varconf_callback));
  config.sige.connect(sigc::mem_fun(*this, &StateManager::varconf_error_callback));
  config.readFromFile(file_name);
}

void StateManager::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  StateID sID = m_state_name_map[section];

  SPtr<StateProperties> record = SPtr<StateProperties>();
  bool create_record = false;
  if (sID > 0) {
    // Record ID already exists, lets see if the record is valid.
    record = m_states[sID];
    if (record.isValid()) {
      // Record already exists, all good
    } else {
      // Record does not exist, lets make a new one.
      create_record = true;
    }
  } else {
    // Not seen this record name yet, so need to fill in all the data structures.
    sID = m_state_counter++;
    create_record = true;
  }
  
  // If record does not exist, create it.
  if (create_record) {
      record = SPtr<StateProperties> (new StateProperties());
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
      record->depth_write = true;
      record->cull_face = false;
      record->cull_face_cw = false;
      record->stencil = false;
      record->fog = false;
      record->rescale_normals = false;
      record->normalise = false;
      record->alpha_function = GL_GREATER;
      record->alpha_value = 0.1f;
      record->blend_src_function = GL_SRC_ALPHA;
      record->blend_dest_function = GL_ONE_MINUS_SRC_ALPHA;
  
      m_states[sID] = record;
      m_state_name_map[record->state] = sID;
      m_name_state_vector[sID] = record->state;

      if (debug) printf("[StateManager] Adding State: %s\n", section.c_str());
  }

  if (key == ALPHA_TEST) record->alpha_test = (bool)config.getItem(section, key);
  else if (key == BLEND) record->blend = (bool)config.getItem(section, key);
  else if (key == LIGHTING) record->lighting = (bool)config.getItem(section, key);
  else if (key == TWO_SIDED_LIGHTING) record->two_sided_lighting = (bool)config.getItem(section, key);
  else if (key == COLOUR_MATERIAL) record->colour_material = (bool)config.getItem(section, key);
  else if (key == DEPTH_TEST) record->depth_test = (bool)config.getItem(section, key);
  else if (key == DEPTH_WRITE) record->depth_write = (bool)config.getItem(section, key);
  else if (key == CULL_FACE) record->cull_face = (bool)config.getItem(section, key);
  else if (key == CULL_FACE_CW) record->cull_face_cw = (bool)config.getItem(section, key);
  else if (key == STENCIL) record->stencil = (bool)config.getItem(section, key);
  else if (key == FOG) record->fog = (bool)config.getItem(section, key);
  else if (key == RESCALE_NORMALS) record->rescale_normals = (bool)config.getItem(section, key);
  else if (key == NORMALISE) record->normalise = (bool)config.getItem(section, key);
  else if (key == ALPHA_FUNCTION) record->alpha_function = getAlphaFunction((std::string)config.getItem(section, key));
  else if (key == ALPHA_VALUE) record->alpha_value = (double)config.getItem(section, key);
  else if (key == BLEND_SRC_FUNCTION) record->blend_src_function = getBlendFunction((std::string)config.getItem(section, key));
  else if (key == BLEND_DEST_FUNCTION) record->blend_dest_function = getBlendFunction((std::string)config.getItem(section, key));
  else if (key.substr(0, TEXTURE.size()) == TEXTURE) {
    unsigned int unit;
    cast_stream(key.substr(TEXTURE.size()), unit);
    if (unit < MAX_UNITS) {
      record->textures[unit] = (bool)config.getItem(section, key);
    }
  }
}

void StateManager::varconf_error_callback(const char *message) {
  Log::writeLog(message, Log::LOG_ERROR);
}

StateID StateManager::requestState(const std::string &state_name) {
  assert(m_initialised);

  StateNameMap::const_iterator S = m_state_name_map.find(state_name);
  StateID stateId;

  if (S == m_state_name_map.end()) {
    // Create empty dummy state record to be filled in later
    stateId = m_state_counter++;

    m_state_name_map[state_name] = stateId;
    m_name_state_vector[stateId] = state_name;
    m_states[stateId] = SPtr<StateProperties>();
    return stateId;
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

  assert (state < (int)m_states.size());
  SPtr<StateProperties> sp = m_states[state];
  // If state doesn't exist, take first one

  if (!sp) {
    std::cerr << "bad state found - " << state <<  std::endl;
    sp = m_states[1];
    state = 1;
  }
  assert(sp.isValid());
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
    glDepthMask(sp->depth_write);
        
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
    if (sp->normalise) glEnable(GL_NORMALIZE);
    else glDisable(GL_NORMALIZE);
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

void StateManager::buildStateChange(unsigned int &list, SPtr<StateProperties> previous_state, SPtr<StateProperties> next_state) {
  assert(m_initialised);
  assert (previous_state.isValid());
  assert (next_state.isValid());
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
  
  if (previous_state->depth_write != next_state->depth_write) {
    glDepthMask(next_state->depth_write);
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
  if (previous_state->normalise != next_state->normalise) {
    if (next_state->normalise) glEnable(GL_NORMALIZE);
    else glDisable(GL_NORMALIZE);
  }
//  if ((next_state->alpha_function != previous_state->alpha_function) || (next_state->alpha_value != previous_state->alpha_value)) 
  glAlphaFunc(next_state->alpha_function, next_state->alpha_value);
  glBlendFunc(next_state->blend_src_function, next_state->blend_dest_function);
  glEndList();
}

void StateManager::registerCommands(Console *console) {
  assert(m_initialised);
  assert(console != NULL);
  console->registerCommand(CMD_LOAD_STATE_CONFIG, this);
  console->registerCommand(CMD_reload_config_states, this);
}

void StateManager::runCommand(const std::string &command, const std::string &arguments) {
  assert(m_initialised);
  if (command == CMD_LOAD_STATE_CONFIG) {
    std::string a = arguments;
    m_state_configs.push_back(a);
    System::instance()->getFileHandler()->getFilePath(a);
    readFiles(a);
  }
  else if (command == CMD_reload_config_states) {
    // Destroy current display lists
    contextDestroyed(true);
    // We can safely re-read the config files to overwrite existing data
    // This only breaks if the config file does no specify all fields
    std::list<std::string>::const_iterator I = m_state_configs.begin();
    std::list<std::string>::const_iterator Iend = m_state_configs.end();
    while (I != Iend) {
      std::string args_cpy = *I++;
      System::instance()->getFileHandler()->getFilePath(args_cpy);
      readFiles(args_cpy);
    }

    contextCreated();
  }
}

void StateManager::contextCreated() {
  assert(m_initialised);
}
void StateManager::contextDestroyed(bool check) {
  assert(m_initialised);
  for (unsigned int i = 0; i < m_state_change_vector.size(); ++i) {
    for (unsigned int j = 0; j < m_state_change_vector[i].size(); ++j) {
      // Delete display list if its still valid
      if (check && glIsList(m_state_change_vector[i][j])) {
        glDeleteLists(m_state_change_vector[i][j], 1);
      }
      // reset list value
      m_state_change_vector[i][j] = 0; 
    }
  }
  m_current_state = -1;
}

} /* namespace Sear */
