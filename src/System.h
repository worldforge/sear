// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: System.h,v 1.34 2004-04-12 15:28:50 alriddoch Exp $

#ifndef SEAR_SYSTEM_H
#define SEAR_SYSTEM_H 1

#include <string>
#include <list>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <sigc++/object_slot.h>

#include "interfaces/ConsoleObject.h"
#include <varconf/Config.h>

namespace Sear {
	
// Forward declarations
class ActionHandler;
class Render;
class Calendar;
class Client;
class EventHandler;
class FileHandler;
class ModelHandler;
class ObjectHandler;
class ScriptEngine;
//class StateLoader;
class Console;
class Character;
class Graphics;
class Sound;

typedef enum {
  SYS_UNKNOWN = 0,
  SYS_CONNECTED,
  SYS_LOGGED_IN,
  SYS_IN_WORLD,
  SYS_LAST_STATE
} SystemState;

/// \todo replace defines for cursor mode
#define ACTION_DEFAULT (0)
#define ACTION_PICKUP (1)
#define ACTION_TOUCH (2)

/**
 * This class is the root object for Sear. All the higher level classes will be created by
 * a single system object.
 * \todo Make System class completly singular (make constructor private)
 */ 
class System : public ConsoleObject, public SigC::Object{
public:
  /**
   * Default constructor
   */ 
  System();

  /**
   * Destructor. Will call shutdown if still initialised when called.
   * @see shutdown()
   */ 
  ~System();

  /** 
   * Initialise the system object
   * @return True on success
   */ 
  bool init();

  /**
   * Shutdown the system object. This will destroy any objects created by the system
   * object, save any settings, and run any shutdown scripts found.
   */ 
  void shutdown();

  /**
   * Creates a window at pre-determined size
   * @param fullscreen Flag indicating whether to create window in fullscreen mode
   */ 
  void createWindow(bool fullscreen);

  /**
   * This is the main program loop. Will loop until an exit condition has been met.
   */ 
  void mainLoop();

  /**
   * Sets the Window and icon title
   * @param title Window title
   * @param icon Icon title
   */
  void setCaption(const std::string &title, const std::string &icon); 

  /**
   * This toggles the fullscreen flag of the window
   */ 
  void toggleFullscreen();

  /**
   * This toggles the mlook flag
   */
  void toggleMouselook();

  /**
   * This passes a command string to the console to run. The first word encountered
   * will be sent as the command, the remainder will be sent as args.
   * @see Console::runCommand()
   * @param command Command string
   */ 
  void runCommand(const std::string &command);

  /**
   * This function will display a message on the console, the top of the screen, or both.
   * An optional message duration can be set, or the default value will be used.
   * \todo DOC message type
   * @param msg Message to be displayed
   * @param type where message is to be displayed
   * @param duration Length of time to display message
   */ 
  void pushMessage(const std::string &msg, int type, int duration = MESSAGE_LIFE);

  /**
   * Function checks for the existance of the given file
   * @param filename File to look for
   * @return True if file exists
   */ 
  bool fileExists(const std::string &filename);
  
  /**
   * Processes a string converting any '~' characters found to the 
   * users home directory.
   * @param path String to convert
   * @return Converted string
   */ 
  std::string processHome(const std::string &path);

  /**
   * Adds a list of paths to use for searching to the existing list
   * @param l Additional list of paths
   */ 
  void addSearchPaths(std::list<std::string> l);
  
  /**
   * Gets the time in milliseconds since SDL was initialised
   * @return Time in milliseconds
   */ 
  unsigned int getTime() const { return SDL_GetTicks(); }

  /**
   * Gets the time in seconds since SDL was initialised
   * @return Time in seconds
   */ 
  float getTimef() const { return (float)SDL_GetTicks() / 1000.0f; }

  /**
   * Set a system state
   * @param ss Sytem state to set
   * @param state Value of state
   */ 
  void setState(SystemState ss, bool state) { _systemState[ss] = state; }
  
  /**
   * Get value of a system state
   * @param ss System state to query
   * @return Value of query state
   */ 
  bool checkState(SystemState ss) { return _systemState[ss]; }

  const std::string getHomePath() { return home_path; }
  const std::string getInstallPath() { return install_path; }
  void setInstallDir(const std::string &install_dir) { install_path = install_dir; }
  

  Graphics *getGraphics() const { return _graphics; }
  
  /**
   * Get the General Config object
   * @return Reference to general config object
   */ 
  varconf::Config &getGeneral() { return _general; }
  /**
   * Get the Texure Config object
   * @return Reference to texture config object
   */ 
  varconf::Config &getTexture() { return _textures; }
  /**
   * Get the Model Config object
   * @return Reference to model config object
   */ 
  varconf::Config &getModel() { return _models; } 
  /**
   * Get the ModelRecords Config object
   * @return Reference to ModelRecords config object
   */ 
  varconf::Config &getModelRecords() { return _model_records; }
  /**
   * Get the ObjectRecords Config object
   * @return Reference to ObjectRecords config object
   */ 
  varconf::Config &getObjectRecords() { return _object_records; }
 
  ScriptEngine *getScriptEngine() const { return _script_engine; }
//  StateLoader *getStateLoader() const { return _state_loader; }
  EventHandler *getEventHandler() const { return _event_handler; }
  ModelHandler *getModelHandler() const { return _model_handler; }
  ActionHandler *getActionHandler() const { return _action_handler; }
  FileHandler *getFileHandler() const { return _file_handler; }
  ObjectHandler *getObjectHandler() const { return _object_handler; }
  Calendar *getCalendar() const { return _calendar; }
  
  Console *getConsole() { return _console; }
  Character *getCharacter() { return _character; }
  void setCharacter(Character *);
  
  static System *instance() { return _instance; }
  static Uint32 getPixel(SDL_Surface *surface, int x, int y);
  static SDL_Surface *loadImage(const std::string &);

  void switchCursor(int);
  void setAction(int);
 
  static const int MESSAGE_LIFE = 5000;

  void registerCommands(Console *);
  void runCommand(const std::string &command, const std::string &args);

protected:
  bool repeat; 
  int action;
  bool initVideo();
  
  void handleEvents(const SDL_Event &);
  void handleAnalogueControllers();

  bool fullscreen;
  bool mouseLook;
  SDL_Surface *screen;
  Graphics *_graphics;
  Render *renderer;
  Client *_client;
  static System *_instance;
  std::string command;
  std::string _icon_file;
  SDL_Surface *_icon;
  
  int _width;
  int _height;

  ScriptEngine *_script_engine; ///< Pointer to scripting engine object
  EventHandler *_event_handler; ///< Pointer to event handler object
  FileHandler *_file_handler; ///< Pointer to file handler object
  ModelHandler *_model_handler; ///< Pointer to model handler object
//  StateLoader *_state_loader; ///< Pointer to state loader object
  ActionHandler *_action_handler; ///< Pointer to action handler object
  ObjectHandler *_object_handler; ///< Pointer to object handler object
  Calendar *_calendar; ///< Pointer to calender object

  std::list<std::string> additional_paths;
  
  std::string home_path;

//  static const std::string SCRIPTS_DIR;
//  static const std::string STARTUP_SCRIPT;
//  static const std::string SHUTDOWN_SCRIPT;
 
  varconf::Config _general;
  varconf::Config _textures;
  varconf::Config _models;

  varconf::Config _model_records;
  varconf::Config _object_records;
  
  SDL_Joystick *_controller;
  Console *_console;
  Character *_character;
 
  static SDL_Cursor *buildCursor(const char *image[]);

  void readConfig();
  void writeConfig();
  std::string install_path;  
  SDL_Cursor *_cursor_default;
  SDL_Cursor *_cursor_pickup;
  SDL_Cursor *_cursor_touch;

  bool _mouse_move_select;

  std::string _current_dir;

  float _seconds;

  typedef struct {
    std::string section;
    std::string key;
    varconf::Config *config;
  } VarconfRecord;

  std::list<VarconfRecord*> record_list;
  bool _process_records;
  void processRecords();
  Sound *sound;
  
public:
  void varconf_callback(const std::string &, const std::string &, varconf::Config &);
  void varconf_general_callback(const std::string &, const std::string &, varconf::Config &);
  void varconf_error_callback(const char *);
  
  std::string getMediaRoot() const { return _media_root; }

private:
  bool _systemState[SYS_LAST_STATE]; ///< Array storing various system states
  bool _system_running; ///< Flag determining when mainLoop terminates (setting to false terminates)

  std::list<std::string> _command_history;
  std::list<std::string>::iterator _command_history_iterator;

  bool _initialised; ///< Initialisation state of System

  std::string _media_root;

};

} /* namespace Sear */
#endif /* SEAR_SYSTEM_H */

