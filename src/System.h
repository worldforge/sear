// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _SYSTEM_H_
#define _SYSTEM_H_ 1

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <string>

namespace Sear {

class GL_Render;
class Render;
class Client;
class EventHandler;
class ObjectLoader;
class Config;
class Console;
class Character;

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

class System {
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
  void runScript(const std::string&);
  void pushMessage(const std::string &msg, int type, int = MESSAGE_LIFE);
  bool fileExists(const std::string &);
  std::string processHome(std::string);

  unsigned int getTime() const { return SDL_GetTicks(); }

  void setState(SystemState ss, bool state) { _systemState[ss] = state; }
  bool checkState(SystemState ss) { return _systemState[ss]; }

  const std::string getHomePath() { return home_path; }
  const std::string getInstallPath() { return install_path; }
  void setInstallDir(const std::string &install_dir) { install_path = install_dir; }
  

  Render *getRenderer() const { return renderer; }
  Config *getGeneral() { return _general; }
  Config *getTexture() { return _textures; }
  Config *getModel() { return _models; } 
  ObjectLoader *getObjectLoader() { return _ol; }
  EventHandler *getEventHandler() { return _event_handler; }
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
protected:
 
  int action;
  bool initVideo();
  
  void handleEvents(const SDL_Event &);

  int window_width;
  int window_height;
  bool fullscreen;
  SDL_Surface *screen;
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

  EventHandler *_event_handler;
  ObjectLoader *_ol;

  std::string home_path;

  static const std::string SCRIPTS_DIR;
  static const std::string STARTUP_SCRIPT;
  static const std::string SHUTDOWN_SCRIPT;
 
  Config *_general;
  Config *_textures;
  Config *_models;
  Console *_console;
  Character *_character;
 
  static SDL_Cursor *buildCursor(const char *image[]);
	  
  static const float DEFAULT_seconds_per_minute = 1.0f;
  static const float DEFAULT_minutes_per_hour = 2.0f;
  static const float DEFAULT_hours_per_day = 24.0f;
  
  static const char * const KEY_seconds_per_minute = "seconds_per_minute";
  static const char * const KEY_minutes_per_hour = "minutes_per_hour";
  static const char * const KEY_hours_per_day = "hours_per_day";
  
  static const char * const KEY_icon_file = "IconFile";
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
bool prefix_cwd;

  float _current_time;
  float _seconds;

  float _dawn_time;
  float _day_time;
  float _dusk_time;
  float _night_time;
  TimeArea _time_area;
private:
  bool _systemState[SYS_LAST_STATE];
  bool _system_running;
};

} /* namespace Sear */
#endif /* _SYSTEM_H_ */

