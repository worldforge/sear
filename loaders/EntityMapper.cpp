// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

// $Id: EntityMapper.cpp,v 1.2 2007-05-02 20:47:54 simon Exp $

/** The EntityMapper class aims to provide a mapping between an entity and it's state to
 * a object record. 
 * A mapping file needs to be defined which can check particular attributes of an entity
 * to allow for model and texture selection.
 * The mapper can also be used to randomly select a model from a pool, perhaps by seeding 
 * a rnd with the id.
 * This to some extend will need to interact with the cal3d loader, e.g. for face/head mesh selection.
 * Returns nothing if no entry available
 */

#include <inttypes.h>
#include <string.h>

#include "EntityMapper.h"

#include "common/Utility.h"

#include "src/Console.h"
#include "src/FileHandler.h"
#include "src/System.h"
#include "src/WorldEntity.h"

static const std::string CMD_load_entity_mappings = "load_entity_mappings";

static const std::string KEY_rule = "rule";
static const std::string KEY_options = "options";

static const std::string RULE_random = "random";

namespace Sear {

EntityMapper::EntityMapper() :
  m_initialised(false)
{
  
}

EntityMapper::~EntityMapper() {
  if (m_initialised) shutdown();
}

int EntityMapper::init() {
  assert(m_initialised == false);

  m_initialised = true;

  return 0;
}

void EntityMapper::shutdown() {
  assert(m_initialised == true);

  m_initialised = false;
}

void EntityMapper::registerCommands(Console *console) {
  console->registerCommand(CMD_load_entity_mappings, this);
}

void EntityMapper::runCommand(const std::string &command, const std::string &args) {
  if (command == CMD_load_entity_mappings) {
    varconf::Config config;
    config.sigsv.connect(sigc::mem_fun(this, &EntityMapper::varconf_callback));
    config.sige.connect(sigc::mem_fun(this, &EntityMapper::varconf_error_callback));
    std::string filename = args;
    System::instance()->getFileHandler()->getFilePath(filename);
    config.readFromFile(filename);
  }
}

std::string EntityMapper::getEntityMapping(WorldEntity *we) {
  assert(we != 0);

  const std::string &id = we->getId();
  const std::string &type = we->type();

  // Do we have a rule defined?
  StringMap::const_iterator I = m_rules_map.find(type);
  if (I != m_rules_map.end()) {
    if (I->second == RULE_random) {
      // The random rule picks a random entry from the options list
      StringListMap::const_iterator J = m_options_map.find(type);
      if (J != m_options_map.end()) {
        const StringList &options = J->second;
        // Setup seed based on entity ID.

        // To allow aliasing
        union { char c[4]; uint32_t i; } u; 
        // Blank array as ID might not fill it up
        memset(u.c, '\0', sizeof(char) * 4);
        // Take last 4 chars of ID if possible as these change more than the first few
        // I.e. entities created sequentially could have the same beginning for their ID.
        unsigned int idx = (id.size() > 4) ? (id.size() - 4) : (0);
        // Copy up to 4 chars of the id field
        strncpy(u.c, &id.c_str()[idx], 4);
        srand(u.i); 
        // Gen random index
        float r = (float)rand() / (float)RAND_MAX * (float)options.size();
        idx = (unsigned int)(r);

        if (idx == options.size()) --idx; // Bounds check
        return options[idx];
      }
    }
  }
  return "";
}

void EntityMapper::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  const std::string &value = (std::string)config.getItem(section, key);
printf("Item: %s\n", section.c_str());
  if (key == KEY_rule) {
    m_rules_map[section] = value;
  } else if (key == KEY_options) {
    Tokeniser tok(value, ' ');
    while (tok.hasRemainingTokens()) {
      const std::string &str = tok.nextToken();
      if (str.empty()) continue;
      m_options_map[section].push_back(str);
    }
  }
}

void EntityMapper::varconf_error_callback(const char *message) {
  fprintf(stderr, "[EntityMapper] %s\n", message);
}


} /* namespace Sear */
