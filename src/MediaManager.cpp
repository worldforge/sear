// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

// $Id: MediaManager.cpp,v 1.4 2007-04-12 13:42:22 simon Exp $

#include <sigc++/connection.h>
#include <sigc++/bind.h>

#include  <libwfut/WFUT.h>
#include  <libwfut/platform.h>
#include  <libwfut/Encoder.h>

#include "src/Console.h"
#include "src/FileHandler.h"
#include "src/System.h"

#include "MediaManager.h"



/*
  We need to record the updates files we have downloaded. If we assume there is only one channel, then we can 
  hook up a signal to fire when poll returns 0 -- but we need to store the previous poll value so it only fires on 
  the change.

  This can then be used to save the data file and fire off a /reload_configs command.

  We also need to hook up the temporary file list too.
 
  Is there a GUI we can use too? -- progress bars?
 
  Doing an update and reload is not very good if there were no files to begin with as
  the configs list will still be empty. Perhaps we need to;
   A) Provide a default empty media structure to load the configs, but require an update.
   B) Have a single file listing all the config files which can be re-read first.
   C) Standard file structure for media (bad!!)

*/
static const bool debug = true;

static const std::string STR_wfut_xml = "wfut.xml";
static const std::string STR_tempwfut = "tempwfut.xml";

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
static const std::string CMD_download_updates = "download_updates";


static void recordUpdate(const WFUT::FileObject &fo, const std::string &tmpfile) {
  FILE *fp = 0;
  if (!WFUT::os_exists(tmpfile)) {
    // Write header 
    fp = fopen(tmpfile.c_str(), "wt");
    if (!fp) {
      //error
      return;
    }
    fprintf(fp, "<?xml version=\"1.0\"?>\n");
    fprintf(fp, "<fileList dir=\"\">\n");
  } else {
    fp = fopen(tmpfile.c_str(), "at");
    if (!fp) {
      //error
      return;
    }
  }
  fprintf(fp, "<file filename=\"%s\" version=\"%d\" crc32=\"%lu\" size=\"%ld\" execute=\"%s\"/>\n", WFUT::Encoder::encodeString(fo.filename).c_str(), fo.version, fo.crc32, fo.size, (fo.execute) ? ("true") : ("false"));
  fclose(fp);
}

// We use this to update the local file list with the updated details
void onDownloadComplete(const std::string &u, const std::string &f, const WFUT::ChannelFileList &updates, WFUT::ChannelFileList *local, const std::string &tmpfile)  {
  printf("Downloaded: %s\n", f.c_str());

  const WFUT::FileMap &ulist = updates.getFiles();
  WFUT::FileMap::const_iterator I = ulist.find(f);
  // The fileobject should exist, otherwise how did we get here?
  assert (I != ulist.end());

  // Add the updated file record to the local list.
  // addFile will overwrite any existing fileobject with the same
  // filename
  local->addFile(I->second);

  // We store in a tmp file the fact that we sucessfully downloaded
  // this file, incase of a crash.
  recordUpdate(I->second, tmpfile);
}

// Signal handler called when a download fails.
//static void onDownloadFailed(const std::string &u, const std::string &f, const std::string &r)  {
//  fprintf(stderr, "Error downloading: %s\n", u.c_str());
//}

namespace Sear {

MediaManager::MediaManager() :
  m_initialised(false),
  m_updates_enabled(DEFAULT_enable_updates),
  m_server_root(DEFAULT_server_root),
  m_system_root(DEFAULT_system_root),
  m_local_root(DEFAULT_local_root),
  m_channel_name(DEFAULT_channel_name)
{
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
  console->registerCommand(CMD_download_updates, this);
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
  } else if (command == CMD_download_updates) {
    if (m_updates_enabled) {
      std::string path = m_local_root + "/" + m_channel_name;
      System::instance()->getFileHandler()->expandString(path);

      // Connect up the signal handlers
      // TODO: This is a bit of a signal handler leak here....
      m_wfut.DownloadComplete.connect(sigc::bind(sigc::ptr_fun(::onDownloadComplete), m_updates_list, &m_local_list, path + "/" + STR_tempwfut));

      m_wfut.updateChannel(m_updates_list, m_server_root + "/" + m_channel_name, path);
    }
  }
}

int MediaManager::poll() {
  assert(m_initialised == true);
  return m_wfut.poll();
}

MediaManager::MediaStatus MediaManager::checkFile(const std::string &filename, MediaType type) {
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
  std::string path = m_local_root + "/" + m_channel_name + "/" + fo.filename;

  System::instance()->getFileHandler()->expandString(path);

  m_wfut.updateFile(fo, url, path);

  // TODO: Store fileobject and type somewhere.

  return (has_local) ? STATUS_USE_OLD : STATUS_USE_DEFAULT;
}

int MediaManager::checkForUpdates() {
  assert(m_initialised == true);

  std::string local_wfut = m_local_root + "/" + m_channel_name + "/" + STR_wfut_xml; 
  System::instance()->getFileHandler()->getFilePath(local_wfut);
  if (debug) printf("[MediaManager] Local WFUT: %s\n", local_wfut.c_str());
  
  m_wfut.getLocalList(local_wfut, m_local_list);


  // Look for tmpwfut file. If it exists, update the local files list.
  std::string local_path = m_local_root + "/" + m_channel_name + "/";
  System::instance()->getFileHandler()->getFilePath(local_path);

  const std::string &tmp_wfut = local_path  + "/" + STR_tempwfut;
  if (debug) printf("Tmp wfut: %s\n", tmp_wfut.c_str());

  WFUT::ChannelFileList tmplist;
  if (WFUT::os_exists(tmp_wfut)) {
    if (m_wfut.getLocalList(tmp_wfut, tmplist)) {
      fprintf(stderr, "Error reading tmpwfut.xml file\n");
    } else {
      const WFUT::FileMap &fm = tmplist.getFiles();
      WFUT::FileMap::const_iterator I = fm.begin();
      WFUT::FileMap::const_iterator Iend = fm.end();
      for (; I != Iend; ++I) {
        m_local_list.addFile(I->second);
      }
    }
  }

  std::string system_wfut = m_system_root + "/" + m_channel_name + "/" + STR_wfut_xml; 
  System::instance()->getFileHandler()->getFilePath(system_wfut);
  if (debug) printf("[MediaManager] System WFUT: %s\n", system_wfut.c_str());
  
  m_wfut.getLocalList(system_wfut, m_system_list) ;

  // No need to download server list, or calculate updates if we do not want any
  if (!m_updates_enabled) return 0;
 
  const std::string &server_wfut = m_server_root + "/" + m_channel_name + "/" + STR_wfut_xml;
  if (debug) printf("[MediaManager] Server WFUT: %s\n", server_wfut.c_str());
  if (m_wfut.getFileList(server_wfut, m_server_list)) {
    // Error getting server list
    return 1;
  }

  if (debug) printf("[MediaManager] Calculating Updates\n");
  m_updates_list.clear();
  if (m_wfut.calculateUpdates(m_server_list, m_system_list, m_local_list, m_updates_list, local_path)) {
    // Error!
    fprintf(stderr, "[MediaManager] Error Calculating Updates\n");
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
