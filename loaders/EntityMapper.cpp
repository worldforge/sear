// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 - 2008  Simon Goodall

/** The EntityMapper class aims to provide a mapping between an entity and it's
 *  state to an object record. 
 * A mapping file needs to be defined which can check particular attributes of
 * an entity to allow for model and texture selection.
 * The mapper can also be used to randomly select a model from a pool, perhaps
 * by seeding a rnd with the id.
 * This to some extend will need to interact with the cal3d loader, e.g. for
 * face/head mesh selection.
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
#include "IEntityMapperRule.h"

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

  m_type_rule_map.clear();
  m_options_map.clear();

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

std::string EntityMapper::getEntityMapping(const WorldEntity *we) {
  assert(we != 0);

  //const std::string &id = we->getId();
  const std::string &type = we->type();

  // Do we have a rule defined?
  StringMap::const_iterator I = m_type_rule_map.find(type);
  if (I != m_type_rule_map.end()) {

    RuleMap::const_iterator R = m_rules_map.find(I->second);
    if (R != m_rules_map.end()) {
      const SPtr<IEntityMapperRule> rule = R->second; 

      StringListMap::const_iterator J = m_options_map.find(type);
      if (J != m_options_map.end()) {
        const StringList &options = J->second;

        return rule->getEntityMapping(we, options);
      }
    }
  }
  return "";
}

void EntityMapper::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  const std::string &value = (std::string)config.getItem(section, key);
  if (key == KEY_rule) {
    m_type_rule_map[section] = value;
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
