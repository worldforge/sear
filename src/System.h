// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: System.h,v 1.23 2002-11-12 23:59:22 simon Exp $

#ifndef SEAR_SYSTEM_H
#define SEAR_SYSTEM_H 1

#include <string>
#include <list>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <sigc++/object_slot.h>

#include "ConsoleObject.h"
#include <varconf/Config.h>


//namespace varconf {
//  class Config;
//}

namespace Sear {
class ActionHandler;
class Render;
class Client;
class EventHandler;
class FileHandler;
class ModelHandler;
class ObjectHandler;
class ScriptEngine;
class StateLoader;
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

#define ACTION_DEFAULT (0)
#define ACTION_PICKUP (1)
#define ACTION_TOUCH (2)



class System : public ConsoleObject, public SigC::Object{
public:
  System();
  ~System();

  bool init();
  void shutdown();

  void createWindow(bool fullscreen);
  void mainLoop();

  /*
   * Sets the Window and icon title
   */
  void setCaption(const std::string &title, const std::string &icon); 
  void toggleFullscreen();

  void runCommand(const std::string &);
  void pushMessage(const std::string &msg, int type, int = MESSAGE_LIFE);
  bool fileExists(const std::string &);
  std::string processHome(const std::string &);

  void addSearchPaths(std::list<std::string> l);
  
  unsigned int getTime() const { return SDL_GetTicks(); }
  float getTimef() const { return (float)SDL_GetTicks() / 1000.0f; }

  void setState(SystemState ss, bool state) { _systemState[ss] = state; }
  bool checkState(SystemState ss) { return _systemState[ss]; }

  const std::string getHomePath() { return home_path; }
  const std::string getInstallPath() { return install_path; }
  void setInstallDir(const std::string &install_dir) { install_path = install_dir; }
  

//  Render *getRenderer() const { return renderer; }
  Graphics *getGraphics() const { return _graphics; }
  
  varconf::Config &getGeneral() { return _general; }
  varconf::Config &getTexture() { return _textures; }
  varconf::Config &getModel() { return _models; } 
  varconf::Config &getModelRecords() { return _model_records; }
  varconf::Config &getObjectRecords() { return _object_records; }
 
  ScriptEngine *getScriptEngine() const { return _script_engine; }
  StateLoader *getStateLoader() const { return _state_loader; }
  EventHandler *getEventHandler() const { return _event_handler; }
  ModelHandler *getModelHandler() const { return _model_handler; }
  ActionHandler *getActionHandler() const { return _action_handler; }
  FileHandler *getFileHandler() const { return _file_handler; }
  ObjectHandler *getObjectHandler() const { return _object_handler; }
  
  Console *getConsole() { return _console; }
  Character *getCharacter() { return _character; }
  void setCharacter(Character *);
  
  static System *instance() { return _instance; }
  static Uint32 System::getPixel(SDL_Surface *surface, int x, int y);
  static SDL_Surface *loadImage(const std::string &);

  void switchCursor(int);
  void setAction(int);
 
  typedef enum {
    DAWN = 0,
    DAY,
    DUSK,
    NIGHT
  } TimeArea;
  
 void setTimeOfDay(float time) { _current_time = time; }
 float getTimeOfDay() { return _current_time; }
 TimeArea getTimeArea() { return _time_area; }

 float getDawnTime() { return _dawn_time; }
 float getDayTime() { return _day_time; }
 float getDuskTime() { return _dusk_time; }
 float getNightTime() { return _night_time; }
 
  static const int MESSAGE_LIFE = 5000;

  void registerCommands(Console *);
  void runCommand(const std::string &command, const std::string &args);

protected:
  bool repeat; 
  int action;
  bool initVideo();
  
  void handleEvents(const SDL_Event &);

  int window_width;
  int window_height;
  bool fullscreen;
  SDL_Surface *screen;
  Graphics *_graphics;
  Render *renderer;
  Client *_client;
  static System *_instance;
  std::string command;
  std::string _icon_file;
  SDL_Surface *_icon;
  
  bool button_down;
  int _width;
  int _height;
  int area;

  ScriptEngine *_script_engine;
  EventHandler *_event_handler;
  FileHandler *_file_handler;
  ModelHandler *_model_handler;
  StateLoader *_state_loader;
  ActionHandler *_action_handler;
  ObjectHandler *_object_handler;

  std::list<std::string> additional_paths;
  
  std::string home_path;

  static const std::string SCRIPTS_DIR;
  static const std::string STARTUP_SCRIPT;
  static const std::string SHUTDOWN_SCRIPT;
 
  varconf::Config _general;
  varconf::Config _textures;
  varconf::Config _models;

  varconf::Config _model_records;
  varconf::Config _object_records;
  
  Console *_console;
  Character *_character;
 
  static SDL_Cursor *buildCursor(const char *image[]);
	  
  static const float DEFAULT_seconds_per_minute = 1.0f;
  static const float DEFAULT_minutes_per_hour = 2.0f;
  static const float DEFAULT_hours_per_day = 24.0f;
  
  static const char * const KEY_seconds_per_minute = "seconds_per_minute";
  static const char * const KEY_minutes_per_hour = "minutes_per_hour";
  static const char * const KEY_hours_per_day = "hours_per_day";
  
  static const char * const KEY_icon_file = "iconfile";
  static const char * const KEY_mouse_move_select = "mouse_move_select";
  
  static const char * const KEY_dawn_time = "dawn_time";
  static const char * const KEY_day_time = "day_time";
  static const char * const KEY_dusk_time = "dusk_time";
  static const char * const KEY_night_time = "night_time";

  static const char * const KEY_render_use_stencil = "render_use_stencil";

  static const char * const KEY_window_width = "width";
  static const char * const KEY_window_height = "height";
  
  static const int DEFAULT_window_width = 640;
  static const int DEFAULT_window_height = 480;
  
  static const bool DEFAULT_mouse_move_select = true;

  static const float DEFAULT_dawn_time = 6.0f;
  static const float DEFAULT_day_time = 9.0f;
  static const float DEFAULT_dusk_time = 18.0f;
  static const float DEFAULT_night_time = 21.0f;
  
  float _seconds_per_day;
  float _seconds_per_minute;
  float _minutes_per_hour;
  float _hours_per_day;
 
  void readConfig();
  void writeConfig();
  std::string install_path;  
  SDL_Cursor *_cursor_default;
  SDL_Cursor *_cursor_pickup;
  SDL_Cursor *_cursor_touch;

  bool _mouse_move_select;

  std::string _current_dir;

  float _current_time;
  float _seconds;

  float _dawn_time;
  float _day_time;
  float _dusk_time;
  float _night_time;
  TimeArea _time_area;

  typedef struct {
    char *section;
    char *key;
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
  
private:
  bool _systemState[SYS_LAST_STATE];
  bool _system_running;

  std::list<std::string> _command_history;
  std::list<std::string>::iterator _command_history_iterator;

  bool _initialised;

  static const char * const EXIT = "exit";
  static const char * const QUIT = "quit";

  static const char * const GET_ATTRIBUTE = "getat";
  static const char * const SET_ATTRIBUTE = "setat";

  static const char * const LOAD_MODEL_RECORDS = "load_model_records";
  static const char * const LOAD_OBJECT_RECORDS = "load_object_records";
  static const char * const LOAD_STATE_FILE = "load_state_file";
  static const char * const LOAD_GENERAL_CONFIG = "load_general";
  static const char * const LOAD_KEY_BINDINGS = "load_bindings";
  static const char * const LOAD_TEXTURE_CONFIG = "load_textures";
  static const char * const LOAD_MODEL_CONFIG = "load_models";
  static const char * const SAVE_GENERAL_CONFIG = "save_general";
  static const char * const SAVE_KEY_BINDINGS = "save_bindings";
  static const char * const READ_CONFIG = "read_config";
  static const char * const BIND_KEY = "bind";
  static const char * const KEY_PRESS = "keypress";
  static const char * const TOGGLE_FULLSCREEN = "toggle_fullscreen";
  static const char * const ADD_EVENT = "event";
  static const char * const IDENTIFY_ENTITY = "identify";
  static const char * const GET_TIME = "get_time";
  static const char * const SET_TIME = "set_time";
};

} /* namespace Sear */
#endif /* SEAR_SYSTEM_H */

