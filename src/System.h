// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: System.h,v 1.51 2005-02-21 14:16:46 simon Exp $

#ifndef SEAR_SYSTEM_H
#define SEAR_SYSTEM_H 1

#include <string>
#include <list>
#include <SDL/SDL.h>

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
class Workspace;
class Character;
//class Graphics;
class Sound;
class Editor;

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
  bool init(int argc, char *argv[]);

  /**
   * Shutdown the system object. This will destroy any objects created by the system
   * object, save any settings, and run any shutdown scripts found.
   */ 
  void shutdown();

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
  double getTimeD() const { return (double)SDL_GetTicks() / 1000.0; }

  void updateTime(double time);

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
  bool checkState(SystemState ss) const { return _systemState[ss]; }

  //Graphics *getGraphics() const { return _graphics; }
  
  /**
   * Get the General Config object
   * @return Reference to general config object
   */ 
  varconf::Config &getGeneral() { return m_general; }
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
  
  /**
   * Enable or disable key repeating.
   * @param bEnable The state of the key repeating.
   **/
  void vEnableKeyRepeat(bool bEnable = true);
 
  ScriptEngine *getScriptEngine() const { return _script_engine; }
  EventHandler *getEventHandler() const { return _event_handler; }
  ModelHandler *getModelHandler() const { return _model_handler; }
  ActionHandler *getActionHandler() const { return _action_handler; }
  FileHandler *getFileHandler() const { return _file_handler; }
  ObjectHandler *getObjectHandler() const { return _object_handler; }
  Calendar *getCalendar() const { return _calendar; }
  
  Console *getConsole() { return _console; }
  Workspace *getWorkspace() { return _workspace; }
  Character *getCharacter() { return _character; }
  void setCharacter(Character *);
  
  static System *instance() { return _instance; }

  void switchCursor(int);
  void setAction(int);
 
  static const int MESSAGE_LIFE = 5000;

  void registerCommands(Console *);
  void runCommand(const std::string &command, const std::string &args);
  Client *getClient() const { return _client; }
protected:
  int action;
  bool initVideo();
  
  void handleEvents(const SDL_Event &);
  void handleAnalogueControllers();
  void handleJoystickMotion(Uint8 axis, Sint16 value);
  
  bool mouseLook;
  SDL_Surface *screen;
  //Graphics *_graphics;
  Render *renderer;
  Client *_client;
  static System *_instance;
  std::string _icon_file;
  SDL_Surface *_icon;
  
  int m_width;
  int m_height;
  int m_KeyRepeatDelay;
  int m_KeyRepeatRate;

  bool _click_on;
  int _click_x;
  int _click_y;
  std::string _click_id;
  double _click_seconds;

  ScriptEngine *_script_engine; ///< Pointer to scripting engine object
  EventHandler *_event_handler; ///< Pointer to event handler object
  FileHandler *_file_handler; ///< Pointer to file handler object
  ModelHandler *_model_handler; ///< Pointer to model handler object
  ActionHandler *_action_handler; ///< Pointer to action handler object
  ObjectHandler *_object_handler; ///< Pointer to object handler object
  Calendar *_calendar; ///< Pointer to calender object
   
  varconf::Config m_general;
  varconf::Config _models;

  varconf::Config _model_records;
  varconf::Config _object_records;
  
  SDL_Joystick *_controller;
  Console *_console;
  Workspace *_workspace;
  Character *_character;
 
  void readConfig();
  void writeConfig();
  std::string install_path;  

  bool m_mouse_move_select;

  double _seconds;

  typedef struct {
    std::string section;
    std::string key;
    varconf::Config *config;
  } VarconfRecord;

  std::list<VarconfRecord*> record_list;
  bool _process_records;
  void processRecords();
  Sound *sound;
  
  typedef enum {
    AXIS_STRAFE,
    AXIS_MOVE,
    AXIS_PAN,
    AXIS_ELEVATE
  } InputAxis;
  
  typedef std::map<int, InputAxis> AxisBindingMap;
  AxisBindingMap m_axisBindings;
  
public:
  void varconf_callback(const std::string &, const std::string &, varconf::Config &);
  void varconf_general_callback(const std::string &, const std::string &, varconf::Config &);
  void varconf_error_callback(const char *);
  
private:
  bool _systemState[SYS_LAST_STATE]; ///< Array storing various system states
  bool _system_running; ///< Flag determining when mainLoop terminates (setting to false terminates)

  Editor *m_editor;
  bool _initialised; ///< Initialisation state of System
};

} /* namespace Sear */
#endif /* SEAR_SYSTEM_H */

