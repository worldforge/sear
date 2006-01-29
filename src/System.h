// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: System.h,v 1.67 2006-01-29 18:33:56 alriddoch Exp $

#ifndef SEAR_SYSTEM_H
#define SEAR_SYSTEM_H 1

#include <string>
#include <list>
#include <SDL/SDL.h>

#include <sigc++/object_slot.h>

#include "interfaces/ConsoleObject.h"
#include <varconf/Config.h>
#include <wfmath/point.h>

namespace Sear {
	
// Forward declarations
class ActionHandler;
class Calendar;
class Client;
class FileHandler;
class ScriptEngine;
class Console;
class Workarea;
class Character;
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
#define ACTION_USE (3)
#define ACTION_ATTACK (4)

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
   * Sets the Window size
   * @param w Window width
   * @param h Window height
   */
  void resizeScreen(int w, int h);

  /**
   * This toggles the mlook flag
   */
  void toggleMouselook();

  /**
   * Return if mouselook is currently set
   */
  bool isMouselookEnabled() const;

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

  /**
   * Gets the time in seconds from the last frame start to the current
   * frame start.
   * @return Elapsed time in seconds
   */
  double getTimeElapsed() const { return m_elapsed; }

  /**
   * Set a system state
   * @param ss Sytem state to set
   * @param state Value of state
   */ 
  void setState(SystemState ss, bool state);// { m_systemState[ss] = state; }
  
  /**
   * Get value of a system state
   * @param ss System state to query
   * @return Value of query state
   */ 
  bool checkState(SystemState ss) const { return m_systemState[ss]; }

  /**
   * Get the General Config object
   * @return Reference to general config object
   */ 
  varconf::Config &getGeneral() { return m_general; }
  
  /**
   * Enable or disable key repeating.
   * @param bEnable The state of the key repeating.
   **/
  void vEnableKeyRepeat(bool bEnable = true);
 
  ScriptEngine *getScriptEngine() const { return m_script_engine; }
  ActionHandler *getActionHandler() const { return m_action_handler; }
  FileHandler *getFileHandler() const { return m_file_handler; }
  Calendar *getCalendar() const { return m_calendar; }
  
  Console *getConsole() const { return m_console; }
  Workarea *getWorkarea() const { return m_workarea; }
  Character *getCharacter() const { return m_character; }
  
  static System *instance() { return m_instance; }

  void switchCursor(int);
  void setAction(int);
 
  static const int MESSAGE_LIFE = 5000;

  void registerCommands(Console *);
  void runCommand(const std::string &command, const std::string &args);
  Client *getClient() const { return m_client; }

  int getWidth() const { return m_width; }
  int getHeight() const { return m_height; }
protected:
  bool initVideo();
  
  void handleEvents(const SDL_Event &);
  void handleAnalogueControllers();
  void handleJoystickMotion(Uint8 axis, Sint16 value);  
  void processRecords();

  int m_action;
  bool m_mouseLook;
  SDL_Surface *m_screen;
  Client *m_client;
  static System *m_instance;
  
  int m_width;
  int m_height;
  int m_KeyRepeatDelay;
  int m_KeyRepeatRate;

  bool m_click_on;
  int m_click_x;
  int m_click_y;
  std::string m_click_id;
  double m_click_seconds;
  WFMath::Point<3> m_click_pos;

  ScriptEngine *m_script_engine; ///< Pointer to scripting engine object
  FileHandler *m_file_handler; ///< Pointer to file handler object
  ActionHandler *m_action_handler; ///< Pointer to action handler object
  Calendar *m_calendar; ///< Pointer to calender object
   
  varconf::Config m_general;

  SDL_Joystick *m_controller;
  Console *m_console;
  Workarea *m_workarea;
  Character *m_character;
 
  void readConfig(varconf::Config &config);
  void writeConfig(varconf::Config &config);

  bool m_mouse_move_select;

  double m_seconds;
  double m_elapsed;

  typedef struct {
    std::string section;
    std::string key;
    varconf::Config *config;
  } VarconfRecord;

  std::list<VarconfRecord*> m_record_list;
  bool m_process_records;
  Sound *m_sound;
  
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
  
  SigC::Signal3<void, std::string, int, int> pushedMessage;

  sigc::signal<void> EnteredWorld;
  sigc::signal<void> LeftWorld;
private:
  bool m_systemState[SYS_LAST_STATE]; ///< Array storing various system states
  bool m_system_running; ///< Flag determining when mainLoop terminates (setting to false terminates)

  Editor *m_editor;
  bool m_initialised; ///< Initialisation state of System
  bool m_startFullscreen;
};

} /* namespace Sear */
#endif /* SEAR_SYSTEM_H */

