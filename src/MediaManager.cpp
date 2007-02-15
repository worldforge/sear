// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

// $Id: MediaManager.cpp,v 1.1 2007-02-15 20:20:22 simon Exp $

#include  <libwfut/WFUT.h>

#include "src/Console.h"
#include "src/FileHandler.h"
#include "src/System.h"

#include "MediaManager.h"


static const bool debug = true;

static const std::string WFUT_XML = "wfut.xml";

// Configuration file keys
static const std::string SECTION_media = "media";
static const std::string KEY_server_root = "server_root";
static const std::string KEY_channel_name = "channel";
static const std::string KEY_enable_updates = "enable_updates";

// Configuuration files default values.
static const std::string DEFAULT_local_root = "${SEAR_HOME}/";
static const std::string DEFAULT_system_root = "${SEAR_INSTALL}/share/sear/";
static const std::string DEFAULT_server_root = "http://white.worldforge.org/downloads/WFUT/";
static const std::string DEFAULT_channel_name = "sear-media-0.6";

static const bool DEFAULT_enable_updates = false;

// Console command identifiers
static const std::string CMD_check_for_updates = "check_for_updates";
static const std::string CMD_enable_updates = "enable_updates";
static const std::string CMD_disable_updates = "disable_updates";
static const std::string CMD_download_file = "download_file";

namespace Sear {

MediaManager::MediaManager() :
  m_initialised(false),
  m_updates_enabled(DEFAULT_enable_updates)
{
  // NASTY hardcoded values.
  m_channel_name = DEFAULT_channel_name;
  m_local_root = DEFAULT_local_root;
  m_system_root = DEFAULT_system_root;
  m_server_root = DEFAULT_server_root;
}


MediaManager::~MediaManager() {
  assert(m_initialised == false);
}

int MediaManager::init() {
  assert(m_initialised == false);

  m_wfut.init();

  // Hook up signals
  m_wfut.DownloadComplete.connect(sigc::mem_fun(this, &MediaManager::onDownloadComplete));
  m_wfut.DownloadFailed.connect(sigc::mem_fun(this, &MediaManager::onDownloadFailed));

  m_initialised = true;

  return 0;
}

void MediaManager::shutdown() {
  assert(m_initialised == true);

  // TODO: Remove signals

  m_wfut.shutdown();

  m_initialised = false;
}


void MediaManager::readConfig(varconf::Config &config) {
 // TODO
}

void MediaManager::writeConfig(varconf::Config &config) {
 // TODO
}

void MediaManager::registerCommands(Console *console) {
  console->registerCommand(CMD_check_for_updates, this);
  console->registerCommand(CMD_enable_updates, this);
  console->registerCommand(CMD_disable_updates, this);
  console->registerCommand(CMD_download_file, this);
}

void MediaManager::runCommand(const std::string &command, const std::string &args) {
  if (command == CMD_check_for_updates) {
    checkForUpdates();
  } else if (command == CMD_enable_updates) {
    m_updates_enabled = true;
  } else if (command == CMD_disable_updates) {
    m_updates_enabled = false;
  } else if (command == CMD_download_file) {
    checkFile(args, MEDIA_UNKNOWN); 
  }
}

int MediaManager::poll() {
  assert(m_initialised == true);
  return m_wfut.poll();
}

int MediaManager::checkFile(const std::string &filename, MediaType type) {
  assert(m_initialised == true);

  /* TODO:  convert filename string into a wfut.xml filename
   * This can be tricky as we are likely to get filenames such as;
   * ${SEAR_MEDIA}/textures.cfg or even ${SEAR_CASTLE}/keep.vconf
   * The first case is easy, we can strip off the prefix. The second case
   * is more tricky as ${SEAR_CASTLE} should resolve to ${SEAR_MEDIA}/castle
   * but how can we find this out easily?
   * To make this work, we may have to build in a lot of assumptions into Sear
   * We should ensure that ${SEAR_MEDIA} is always used.
   * 
   * Perhaps we should combine this function with the getFilePath function. We
   * take in the unexpanded string, find all possible file paths. Check to see
   * if the prefix is the same as the local or system root. Then do some magic?
   */ 

  // Is it on the local list?
  const WFUT::FileMap &local_files = m_local_list.getFiles();
  WFUT::FileMap::const_iterator local_itr = local_files.find(filename);
  WFUT::FileMap::const_iterator local_itr_end = local_files.end();

  bool has_local = local_itr != local_itr_end;
  

  // Is it on the updates list?
  const WFUT::FileMap &updates_files = m_updates_list.getFiles();
  WFUT::FileMap::const_iterator update_itr = updates_files.find(filename);
  WFUT::FileMap::const_iterator update_itr_end = updates_files.end();

  bool has_update = update_itr != update_itr_end;

  if (!has_local && !has_update) {
  if (debug) printf("[MediaManager] File unknown\n");
    return STATUS_UNKNOWN_FILE;
  }

  if (!has_update) {
  if (debug) printf("[MediaManager] File OK\n");
    return STATUS_OK;
  }

  // Queue the file
  const WFUT::FileObject &fo = update_itr->second;
  const std::string &url = m_server_root + "/" + m_channel_name + "/" + fo.filename;
  const std::string &path = m_local_root + "/" + m_channel_name + "/" + fo.filename;
  m_wfut.updateFile(fo, url, path);

  // TODO: Store fileobject and type somewhere.


  return (has_local) ? STATUS_USE_OLD : STATUS_USE_DEFAULT;
}


int MediaManager::checkForUpdates() {
  assert(m_initialised == true);

  std::string local_wfut = m_local_root + "/" + m_channel_name + "/" + WFUT_XML; 
  System::instance()->getFileHandler()->getFilePath(local_wfut);
  if (debug) printf("[MediaManager] Local WFUT: %s\n", local_wfut.c_str());
  // We don't mind if the file is correctly parsed or if it does not exist.
  // We do mind if the file exists but is corrupt.
//  if (
m_wfut.getLocalList(local_wfut, m_local_list);
/// == WFUT::WFUT_PARSE_ERROR) {
//    // Print a warning message?
//    return 1;
//  }

  std::string system_wfut = m_system_root + "/" + m_channel_name + "/" + WFUT_XML; 
  System::instance()->getFileHandler()->getFilePath(system_wfut);
  if (debug) printf("[MediaManager] System WFUT: %s\n", system_wfut.c_str());
  // We don't mind if the file is correctly parsed or if it does not exist.
  // We do mind if the file exists but is corrupt.
//  if (
m_wfut.getLocalList(system_wfut, m_system_list) ;
//== WFUT::WFUT_PARSE_ERROR) {
//    // Warnings!
//    return 1;
//  }


  // No need to download server list, or calculate updates if we do not want any
  if (!m_updates_enabled) return 0;
 
  const std::string &server_wfut = m_server_root + "/" + m_channel_name + "/" + WFUT_XML;
  if (debug) printf("[MediaManager] Server WFUT: %s\n", server_wfut.c_str());
  if (m_wfut.getFileList(server_wfut, m_server_list)) {
    // Error getting server list
    return 1;
  }

  if (debug) printf("[MediaManager] Calculating Updates\n");
  if (m_wfut.calculateUpdates(m_server_list, m_system_list, m_local_list, m_updates_list, m_local_root + "/" + m_channel_name)) {
    // Error!
    return 1;
  }
 
  return 0;
}

void MediaManager::onDownloadComplete(const std::string &url, const std::string &filename) {
  printf("DownloadComplete: %s\n", filename.c_str());
}
void MediaManager::onDownloadFailed(const std::string &url, const std::string &filename, const std::string &reason) {
  printf("DownloadFailed: %s\n", filename.c_str());
}

} /* namespace Sear */
