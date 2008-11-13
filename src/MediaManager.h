// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 - 2008 Simon Goodall

#ifndef SEAR_MEDIA_MANAGER_H
#define SEAR_MEDIA_MANAGER_H 1

#include <string>

#include <sigc++/signal.h>

#include <varconf/config.h>

#include  <libwfut/WFUT.h>

#include "interfaces/ConsoleObject.h"

namespace Sear {

class Console;

class MediaManager : public ConsoleObject {
public:
  // This enum is used to identify the different types of file we are looking
  // at. This is meant to be used as a filter. E.g. The texture manager does
  // not need to take any notice of model files.
  typedef enum {
    MEDIA_UNKNOWN = 0,
    MEDIA_TEXTURE,
    MEDIA_MODEL,
    MEDIA_CONFIG
  } MediaType;

  // Enum listing status of media. Will be used with dynamic updates.
  typedef enum {
    STATUS_OK = 0,
    STATUS_USE_OLD,
    STATUS_USE_DEFAULT,
    STATUS_UNKNOWN_FILE
  } MediaStatus;


  MediaManager();
  virtual ~MediaManager();

  int init();
  void shutdown();

  bool isInitialised() const { return m_initialised; }

  void readConfig(varconf::Config &config);
  void writeConfig(varconf::Config &config);

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);

  /** Poll the media manager.
   * This method performs some downloading and updates internal structures when
   * a file download completes (sucessfully or not).
   * @return Number of outstanding file downloads. 
   */
  int poll();

  // This function is used to request the status of a particular file.
  // We could return a different value depending on what the status of the 
  // file is. 
  //  STATUS_FILE_UNKNOWN  -- To return when the updates no nothing of this file
  //  STATUS_FILE_MISSING  -- To return when a local version of the file does 
  //                          not exist
  //  STATUS_FILE_UPDATING -- To return when a local version exists, but a newer
  //                          version on the server will be downloaded.
  //  STATUS_FILE_OK       -- The local version is the latest version
  //  This function will then schedule an update if required.
  // TODO: We could return an int identifier for faster matching to requestor
  MediaStatus checkFile(const std::string &filename, MediaType type);

  // Look for updated files on the server.
  int checkForUpdates();

  // Signals fired when a download completes, or fails.
//  Signal<void, MediaType, const std::string &> onDownloadComplete;
//  Signal<void, MediaType, const std::string &> onDownloadFailed;
  sigc::signal<void, const std::string&, const std::string&> DownloadComplete;
  sigc::signal<void, const std::string&, const std::string&, const std::string&> DownloadFailed;
  sigc::signal<void, const std::string&, const WFUT::WFUTUpdateReason&> UpdateReason;

  size_t getNumUpdates() const { return m_updates_list.size(); }

  /** Save local list to disk
   */
  void saveList(const std::string &filename, const WFUT::ChannelFileList &list);

  /** Abort all current and pending updates.
   * Currently no feedback is given to the user.
   */ 
  void cancelAll();

private:
  // Internal handlers for libWFUT callbacks 
  void onDownloadComplete(const std::string &url, const std::string &filename);
  void onDownloadFailed(const std::string &url, const std::string &filename, const std::string &reason);
void onUpdateReason(const std::string &filename, const WFUT::WFUTUpdateReason &reason) ;

  // Flag indicating object initiialisation status
  bool m_initialised;
  // Flag indicating whether or not to download updates
  bool m_updates_enabled;

  WFUT::WFUTClient m_wfut;

  std::string m_server_root;
  std::string m_system_root;
  std::string m_local_root;
  std::string m_channel_name;

  WFUT::ChannelFileList m_server_list;
  WFUT::ChannelFileList m_system_list;
  WFUT::ChannelFileList m_local_list;
  WFUT::ChannelFileList m_updates_list;
};

} /* namespace Sear */

#endif /* SEAR_MEDIA_MANAGER_H */
