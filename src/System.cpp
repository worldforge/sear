// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "System.h"
#include "Render.h"
#include "../renderers/GL.h"
#include "client.h"
#include "Camera.h"
#include "Bindings.h"
#include "Console.h"
#include "EventHandler.h"
#include "ModelHandler.h"
#include "Config.h"
#include "Character.h"
#include "conf.h"
#include "ObjectLoader.h"
#include "StateLoader.h"
#include "Utility.h"
#include "WorldEntity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>

#include <unistd.h>
#include "cursors.h"

#include "Exception.h"
#include "Log.h"
#include <Eris/Types.h>

namespace Sear {

System *System::_instance = NULL;
const std::string System::SCRIPTS_DIR = "scripts";
const std::string System::STARTUP_SCRIPT = "startup.script";
const std::string System::SHUTDOWN_SCRIPT = "shutdown.script";

System::System() :
  repeat(false),
  window_width(0),
  window_height(0),
  fullscreen(0),
  screen(NULL),
  renderer(NULL),
  _client(NULL),
  _icon(NULL),
  _event_handler(NULL),
  _ol(NULL),
  _console(NULL),
  _character(NULL),
  _prefix_cwd(false),
  _system_running(false)
{
  int i;
  _instance = this;
  // Initialise system states
  for (i = 1; i < SYS_LAST_STATE; i++) _systemState[i] = false;
}

System::~System() {
}

bool System::init() {
  if (!initVideo()) return false;
  _event_handler = new EventHandler(this);
  _model_handler = new ModelHandler();
  _client = new Client(this, CLIENT_NAME);
  if(!_client->init()) {
    Log::writeLog("Error initializing Eris", Log::LOG_ERROR);
    throw Exception("ERIS INIT");
  }
  
  SDL_EnableUNICODE(1);

#ifdef __WIN32__
  const char *homedrive;
  const char *homepath;
  homedrive = getenv("HOMEDRIVE");
  homepath = getenv("HOMEPATH");

  if (homedrive == 0) homedrive = ".";
  if (homepath == 0) homepath = "";
  home_path = std::string(homedrive)+std::string(homepath);
  if (homedrive) delete homedrive;
  if (homepath) delete homepath;
#else
  home_path = std::string(getenv("HOME"));
  if (home_path.empty()) home_path = ".";
  else {
    home_path += "/.sear";
    mkdir(home_path.c_str(), 0755);
  }
#endif
  install_path = std::string(INSTALLDIR) + std::string("/share/sear");
  
  _ol = new ObjectLoader();
  _ol->init();

  _sl = new StateLoader();
  _sl->init();
  
  _general = new Config();
  _textures = new Config();
  _models = new Config();
  
  Bindings::init();
  
  _console = new Console(this);

  registerCommands(_console);
  _client->registerCommands(_console);
  
  
  Log::writeLog("Running startup scripts", Log::LOG_INFO);
  std::string install_location = install_path + "/" + SCRIPTS_DIR + "/" + STARTUP_SCRIPT;
  std::string home_location = home_path + "/" + STARTUP_SCRIPT;
  std::string current_location = "./" + STARTUP_SCRIPT;

  if (fileExists(install_location)) {
    runScript(install_location);
  } else {
//    No install
  }
  if (fileExists(home_location)) {
    runScript(home_location);
  } else {
//   No home
  }
  if (fileExists(current_location)) {
    runScript(current_location);
  } else {
// No current
  }
  readConfig();
  _system_running = true;

  _command_history_iterator = _command_history.begin();
  return true;
}

void System::shutdown() {
  Log::writeLog("Shutting Down Renderer", Log::LOG_INFO);
  
  if (_character) {
    _character->shutdown();
    delete _character;
    _character = NULL;
  }
  if (_client) {
    _client->shutdown();
    delete _client;
    _client = NULL;
  }
  if (_ol) {
    _ol->shutdown();
    delete _ol;
    _ol = NULL;
  }
  if (renderer) {
    renderer->shutdown();
    delete renderer;
    renderer = NULL;
  }

  writeConfig();
  Log::writeLog("Running shutdown scripts", Log::LOG_INFO);
  std::string install_location = install_path + "/" + SCRIPTS_DIR + "/" + SHUTDOWN_SCRIPT;
  std::string home_location = home_path + "/" + SHUTDOWN_SCRIPT;
  std::string current_location = "./" + SHUTDOWN_SCRIPT;
  if (fileExists(current_location)) runScript(current_location);
  if (fileExists(home_location)) runScript(home_location);
  if (fileExists(install_location)) runScript(install_location);

  
  Bindings::shutdown();

  if (_general) {
    _general->shutdown();
    delete _general;
    _general = NULL;
  }	  
  if (_textures) {
    _textures->shutdown();
    delete _textures;
    _textures = NULL;
  }	  
  if (_models) {
    _models->shutdown();
    delete _models;
    _models = NULL;
  }	  
  if (_console) {
    _console->shutdown();
    delete _console;
    _console = NULL;
  }
  
  if (_icon) delete _icon;
  if (_cursor_default) SDL_FreeCursor(_cursor_default);
  if (_cursor_pickup) SDL_FreeCursor(_cursor_pickup);
  if (_cursor_touch) SDL_FreeCursor(_cursor_touch);
  SDL_Quit();
}

bool System::initVideo() {
  Log::writeLog("Initialising Video", Log::LOG_INFO);
#ifdef DEBUG
  // NOPARACHUTE means SDL doesn't handle any errors allowing us to catch them in a debugger
  if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0 ) {
#else
  // We want release versions to die quietly
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
#endif
    Log::writeLog(std::string("Unable to init SDL: ") + string_fmt(SDL_GetError()), Log::LOG_ERROR);
    return false;
  } 
  return true;
}

void System::createWindow(bool fullscreen) {
  Log::writeLog("Creating Window", Log::LOG_INFO);
  //Request Open GL window attributes
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5 );
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5 );
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5 );
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1 );
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1 );
  const SDL_VideoInfo *info;
  info = SDL_GetVideoInfo();
  if (!info) {
    Log::writeLog("Error quering video", Log::LOG_DEFAULT);
  }
#ifndef __WIN32__

  Log::writeLog(std::string("hw_available: ") + string_fmt(info->hw_available), Log::LOG_DEFAULT);
  Log::writeLog(std::string("wm_available: ") + string_fmt(info->wm_available), Log::LOG_DEFAULT);
  Log::writeLog(std::string("blit_hw: ") + string_fmt(info->blit_hw), Log::LOG_DEFAULT);
  Log::writeLog(std::string("blit_hw_CC: ") + string_fmt(info->blit_hw_CC), Log::LOG_DEFAULT);
  Log::writeLog(std::string("blit_hw_A: ") + string_fmt(info->blit_hw_A), Log::LOG_DEFAULT);
  Log::writeLog(std::string("blit_sw: ") + string_fmt(info->blit_sw), Log::LOG_DEFAULT);
  Log::writeLog(std::string("blit_sw_CC: ") + string_fmt(info->blit_sw_CC), Log::LOG_DEFAULT);
  Log::writeLog(std::string("blit_sw_A: ") + string_fmt(info->blit_sw_A), Log::LOG_DEFAULT);
  Log::writeLog(std::string("video_mem: ") + string_fmt(info->video_mem), Log::LOG_DEFAULT);
  Log::writeLog(std::string("hw_available: ") + string_fmt(info->hw_available), Log::LOG_DEFAULT);
#endif
  
  //Create Window
  int flags = SDL_OPENGL;
  int bpp = 0;
  bpp = info->vfmt->BitsPerPixel;
  if (fullscreen) flags |= SDL_FULLSCREEN;
  screen = SDL_SetVideoMode(_width, _height, 0, flags);
  if (screen == NULL ) {
    Log::writeLog(std::string("Unable to set ") + string_fmt(_width) + std::string(" x ") + string_fmt(_height) + std::string(" video: ") + string_fmt(SDL_GetError()), Log::LOG_ERROR);
    _system_running = false;
    exit(1);
  }

  // Check OpenGL flags
  int value = 0;
  SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &value);
  Log::writeLog(std::string("Red Size: ") + string_fmt(value), Log::LOG_DEFAULT);
  SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &value);
  Log::writeLog(std::string("Blue Size: ") + string_fmt(value), Log::LOG_DEFAULT);
  SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &value);
  Log::writeLog(std::string("Green Size: ") + string_fmt(value), Log::LOG_DEFAULT);
  SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &value);
  Log::writeLog(std::string("Depth Size: ") + string_fmt(value), Log::LOG_DEFAULT);
  SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &value);

  if (value < 1) _general->setAttribute("render_use_stencil", "false");
  
  Log::writeLog(std::string("Stencil Size: ") + string_fmt(value), Log::LOG_DEFAULT);
  SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &value);
  Log::writeLog(std::string("Double Buffer: ") + string_fmt(value), Log::LOG_DEFAULT);
 
  if (!_icon) _icon = IMG_Load(_icon_file.c_str());
  // Should set the icon transparency but it does not work.
  if (_icon) {
    if (SDL_SetColorKey(_icon, SDL_SRCCOLORKEY, SDL_MapRGB(_icon->format, 0x0, 0x0, 0x0)) == -1) {
      Log::writeLog("ERROR icon setting transparency", Log::LOG_ERROR);
    }
    SDL_WM_SetIcon(_icon, NULL);
  } 
  if (!_cursor_default) _cursor_default = buildCursor(CURSOR_DEFAULT);
  if (!_cursor_pickup)  _cursor_pickup = buildCursor(CURSOR_PICKUP); 
  if (!_cursor_touch)   _cursor_touch = buildCursor(CURSOR_TOUCH);
  Log::writeLog("Creating Renderer", Log::LOG_INFO);
  if (renderer) {
//    renderer->initWindow(_width, _height);
    renderer->shutdown();
    delete renderer;
  }// else {
    renderer = new GL(this);
    _console->init();
    renderer->init();
    renderer->initWindow(_width, _height);
//    renderer->buildDisplayLists();
    pushMessage("Loading, Please wait...", 2, 100);
    renderer->drawScene("", false); // Render scene one before producing colour set
    renderer->buildColourSet();
//  }
}

void System::mainLoop() {
  SDL_Event event;
  while (_system_running) {
    try {
      _seconds = (float)SDL_GetTicks() / 1000.0f;
      _current_time = _seconds / _seconds_per_minute / _minutes_per_hour;
      while (_current_time > _seconds_per_day) _current_time -= _seconds_per_day;
      _current_time = _current_time / _minutes_per_hour / _seconds_per_minute;
      if (_current_time < _dawn_time) _time_area = NIGHT;
      else if (_current_time < _day_time) _time_area = DAWN;
      else if (_current_time < _dusk_time) _time_area = DAY;
      else if (_current_time < _night_time) _time_area = DUSK;
      else if (_current_time < _hours_per_day) _time_area = NIGHT;
      
      while (SDL_PollEvent(&event)  ) {
        handleEvents(event);
        // Stop processing events if we are quiting
        if (!_system_running) break;
      }
      _event_handler->poll();
      _client->poll();
      renderer->drawScene(command, false);
    } catch (ClientException ce) {
      Log::writeLog(ce.getMessage(), Log::LOG_ERROR);
      pushMessage(ce.getMessage(), CONSOLE_MESSAGE);
    } catch (Exception e) {
      Log::writeLog(e.getMessage(), Log::LOG_ERROR);
      pushMessage(e.getMessage(), CONSOLE_MESSAGE);
    } catch (Eris::InvalidOperation io) {
      Log::writeLog(io._msg, Log::LOG_ERROR);
      pushMessage(io._msg, CONSOLE_MESSAGE);
    } catch (Eris::BaseException be) {
      Log::writeLog(be._msg, Log::LOG_ERROR);
      pushMessage(be._msg, CONSOLE_MESSAGE);
    } catch (...) {
      Log::writeLog("Caught Unknown Exception", Log::LOG_ERROR);
      pushMessage("Caught Unknown Exception", CONSOLE_MESSAGE);
    }
  }
}

void System::handleEvents(const SDL_Event &event) {
  switch (event.type) {
    case SDL_MOUSEBUTTONDOWN: {
      switch (event.button.button) {
        case (SDL_BUTTON_LEFT):   { 
          renderer->procEvent(event.button.x, event.button.y);
	  switch (action) {
            case (ACTION_DEFAULT): break;
            case (ACTION_PICKUP):  if (_character) _character->getEntity(renderer->getActiveID()); break;
            case (ACTION_TOUCH): if (_character) _character->touchEntity(renderer->getActiveID()); break;
	  }
	  setAction(ACTION_DEFAULT);
	  break;
        }
        break;
      }
      break;
    }
    case SDL_MOUSEMOTION: {
      if (_mouse_move_select) renderer->procEvent(event.button.x, event.button.y);
      break;
    } 
    case SDL_KEYDOWN: {
//      if (Bindings::getBinding(Bindings::idToString((int)event.key.keysym.sym)) == CMD_TOGGLE_CONSOLE) {
//        _console->toggleConsole();
//  	if (_console->consoleStatus()) {
//	  }else {	  
//	  }
//        command = "";
//	break;
//      }
     // Keys that still execute bindings with console open 
      if (_console->consoleStatus()) {
        if (!repeat) {
	  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	  command = "";
	  repeat = true;
	}
	if (event.key.keysym.sym == SDLK_UP) {
          // Previous command
//	  Log::writeLog("Previous Command", Log::LOG_INFO);
	  if (_command_history_iterator != _command_history.begin()) {
	    _command_history_iterator--;
            command = (*_command_history_iterator);
	  }
	}
	else if (event.key.keysym.sym == SDLK_DOWN) {
//	  Log::writeLog("Next Command", Log::LOG_INFO);
          // next command
	  if (_command_history_iterator != _command_history.end()) {
	    _command_history_iterator++;
            if (_command_history_iterator != _command_history.end()) command = (*_command_history_iterator);
	    else command = "";
	  }
	}
	else if ((event.key.keysym.sym == SDLK_BACKQUOTE) ||
	  (event.key.keysym.sym == SDLK_F1) ||
	  (event.key.keysym.sym == SDLK_F2) ||
	  (event.key.keysym.sym == SDLK_F3) ||
	  (event.key.keysym.sym == SDLK_F4) ||
	  (event.key.keysym.sym == SDLK_F5) ||
	  (event.key.keysym.sym == SDLK_F6) ||
	  (event.key.keysym.sym == SDLK_F7) ||
	  (event.key.keysym.sym == SDLK_F8) ||
	  (event.key.keysym.sym == SDLK_F9) ||
	  (event.key.keysym.sym == SDLK_F10) ||
	  (event.key.keysym.sym == SDLK_F11) ||
	  (event.key.keysym.sym == SDLK_F12) ||
	  (event.key.keysym.sym == SDLK_F13) ||
	  (event.key.keysym.sym == SDLK_F14) ||
	  (event.key.keysym.sym == SDLK_F15) ||
	  (event.key.keysym.sym == SDLK_ESCAPE))
	{
          runCommand(Bindings::getBinding(Bindings::idToString((int)event.key.keysym.sym)));
	} else if (event.key.keysym.sym == SDLK_RETURN) {
	  if (command.empty()) break;
          runCommand(command);
	  _command_history.push_back(command);
	  _command_history_iterator = _command_history.end();
          command = "";
        } else if (event.key.keysym.sym == SDLK_DELETE || event.key.keysym.sym == SDLK_BACKSPACE) {
          if (command.length() > 0) {
            command = command.substr(0, command.length() - 1); 
          }
        } else if (event.key.keysym.unicode < 0x80 && event.key.keysym.unicode > 0) {
          command = command + (char)event.key.keysym.unicode;
	}
      } else {
        if (repeat) {
          SDL_EnableKeyRepeat(0, 0);
	  command = "";
	  repeat = false;
	}
        runCommand(Bindings::getBinding(Bindings::idToString((int)event.key.keysym.sym)));
      }
      break;
    }
    case SDL_KEYUP: {
      if (!_console->consoleStatus()) {
        char *binding = (char *) Bindings::getBinding(Bindings::idToString((int)event.key.keysym.sym)).c_str();
        if (binding[0] == '+') {
          runCommand("-" + std::string(++binding));
        }
      }
      break;
    }
    case SDL_QUIT:
      _system_running = false;
      break;
    }
}

void System::setCaption(const std::string &title, const std::string &icon) {
  SDL_WM_SetCaption(title.c_str(), icon.c_str());
}

void System::toggleFullscreen() {
  fullscreen = ! fullscreen;
  // If fullscreen fails, create a new window with the fullscreen flag (un)set
  if (!SDL_WM_ToggleFullScreen(screen)) createWindow(fullscreen);
}


Uint32 System::getPixel(SDL_Surface *surface, int x, int y) {
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
  switch(bpp) {
    case 1:
      return *p;
    case 2:
      return *(Uint16 *)p;
    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        return p[0] << 16 | p[1] << 8 | p[2];
      else
        return p[0] | p[1] << 8 | p[2] << 16;
     case 4:
       return *(Uint32 *)p;
    default:
       return 0;       /* shouldn't happen, but avoids warnings */
  }
}


SDL_Surface *System::loadImage(const  std::string &filename) {
  Uint8 *rowhi, *rowlo;
  Uint8 *tmpbuf /*, tmpch*/;
  SDL_Surface *image;
  int i/*, j*/;
  image = IMG_Load(filename.c_str());
  if ( image == NULL ) { 
    Log::writeLog(std::string("Unable to load ") + filename + std::string(": ") + string_fmt( SDL_GetError()), Log::LOG_ERROR);
    return(NULL);
  }
  /* GL surfaces are upsidedown and RGB, not BGR :-) */
  tmpbuf = (Uint8 *)malloc(image->pitch);
  if ( tmpbuf == NULL ) {  
    Log::writeLog("Out of memory", Log::LOG_ERROR);
    return(NULL);
  }
  rowhi = (Uint8 *)image->pixels;
  rowlo = rowhi + (image->h * image->pitch) - image->pitch;
  for ( i=0; i<image->h/2; ++i ) {
    //commented out for use with png
/*
    for ( j=0; j<image->w; ++j ) {
      tmpch = rowhi[j*3];
      rowhi[j*3] = rowhi[j*3+2];
      rowhi[j*3+2] = tmpch;
      tmpch = rowlo[j*3];
      rowlo[j*3] = rowlo[j*3+2];
      rowlo[j*3+2] = tmpch;
    }
*/
    memcpy(tmpbuf, rowhi, image->pitch);
    memcpy(rowhi, rowlo, image->pitch);
    memcpy(rowlo, tmpbuf, image->pitch);
    rowhi += image->pitch;
    rowlo -= image->pitch;
  }
  free(tmpbuf);
  return(image);
}

void System::pushMessage(const std::string &msg, int type, int duration) {
  if(_console) _console->pushMessage(msg, type, duration);
}

void System::runScript(const std::string &file_name) {
  FILE *script_file = NULL;
  const int MAX_LINE_SIZE = 256;
  char string_data[MAX_LINE_SIZE];
  Log::writeLog(std::string("System: Running script - ") + file_name, Log::LOG_DEFAULT);
  script_file = fopen(processHome(file_name).c_str(), "r");
  char cur_dir[257];
  memset(cur_dir, '\0', 257);
  getcwd(cur_dir, 256);
  Log::writeLog(std::string("Current Directory: ") + cur_dir, Log::LOG_DEFAULT);
  bool pre_cwd = _prefix_cwd; // Store current setting
  if (!script_file) {
    Log::writeLog(std::string("System: Error opening script file: ") + file_name, Log::LOG_ERROR);
    return;
  }
//  try {
    while (!feof(script_file)) {
      fscanf(script_file, "%[^\n]\n", &string_data[0]);
      runCommand(std::string(string_data));
    }
//  } catch (...) {
    // TODO be more discriminate on errors
//    Log::writeLog("Caught an Exception while running script. Script aborted", Log::LOG_ERROR);
//  }
  chdir(cur_dir);
  _prefix_cwd = pre_cwd; // Restore setting
  fclose(script_file);
}

bool System::fileExists(const std::string &file_name) {
  FILE *test_file = NULL;
  test_file = fopen(file_name.c_str(), "r");
  if (test_file) {
    fclose(test_file);
    return true;
  }
  return false;
}

std::string System::processHome(const std::string &input) {
  std::string output;
  int i = input.find("~");
  if (i == -1) return input;
  output = input.substr(0, i);
  output += home_path;
  output += input.substr(i + 1);
  return output;
}

void System::setCharacter(Character *character) {
  if (_character) { // get rid of old instance if it exists
    _character->shutdown();
    delete _character;
    _character = NULL;
  }
  _character = character;
  // Assuming init has no bee performed
  _character->init();
  _character->registerCommands(_console);
}

void System::readConfig() {
  std::string temp;
  if (!_general) {
    Log::writeLog("System: Error - General config object not created", Log::LOG_ERROR);
    return;
  }
  temp = _general->getAttribute(KEY_dawn_time);
  _dawn_time = (temp.empty()) ? (DEFAULT_dawn_time) : (atof(temp.c_str()));
  temp = _general->getAttribute(KEY_day_time);
  _day_time = (temp.empty()) ? (DEFAULT_day_time) : (atof(temp.c_str()));
  temp = _general->getAttribute(KEY_dusk_time);
  _dusk_time = (temp.empty()) ? (DEFAULT_dusk_time) : (atof(temp.c_str()));
  temp = _general->getAttribute(KEY_night_time);
  _night_time = (temp.empty()) ? (DEFAULT_night_time) : (atof(temp.c_str()));

  temp = _general->getAttribute(KEY_seconds_per_minute);
  _seconds_per_minute = (temp.empty()) ? (DEFAULT_seconds_per_minute) : (atof(temp.c_str()));
  temp = _general->getAttribute(KEY_minutes_per_hour);
  _minutes_per_hour = (temp.empty()) ? (DEFAULT_minutes_per_hour) : (atof(temp.c_str()));
  temp = _general->getAttribute(KEY_hours_per_day);
  _hours_per_day = (temp.empty()) ? (DEFAULT_hours_per_day) : (atof(temp.c_str()));

  _seconds_per_day = _seconds_per_minute * _minutes_per_hour * _hours_per_day;
  
  _icon_file = _general->getAttribute(KEY_icon_file);
  temp = _general->getAttribute(KEY_mouse_move_select);
  _mouse_move_select =  (temp.empty()) ? (DEFAULT_mouse_move_select) : (temp == "true");

  temp = _general->getAttribute(KEY_window_width);
  _width = (temp.empty()) ? (DEFAULT_window_width) : (atoi(temp.c_str()));
  temp = _general->getAttribute(KEY_window_height);
  _height = (temp.empty()) ? (DEFAULT_window_height) : (atoi(temp.c_str()));
}

void System::writeConfig() {
  if (!_general) {
    Log::writeLog("System: Error- General config object not created", Log::LOG_ERROR);
    return;
  }
  _general->setAttribute(KEY_dawn_time, string_fmt(_dawn_time));
  _general->setAttribute(KEY_day_time, string_fmt(_day_time));
  _general->setAttribute(KEY_dusk_time, string_fmt(_dusk_time));
  _general->setAttribute(KEY_night_time, string_fmt(_night_time));
  
  _general->setAttribute(KEY_seconds_per_minute, string_fmt(_seconds_per_minute));
  _general->setAttribute(KEY_minutes_per_hour, string_fmt(_minutes_per_hour));
  _general->setAttribute(KEY_hours_per_day, string_fmt(_hours_per_day));
  
  _general->setAttribute(KEY_mouse_move_select,  (_mouse_move_select) ? ("true") : ("false"));

  _general->setAttribute(KEY_window_width, string_fmt(_width));
  _general->setAttribute(KEY_window_height, string_fmt(_height));
  
}

SDL_Cursor *System::buildCursor(const char *image[]) {
  int i, row, col;
  Uint8 data[4*32];
  Uint8 mask[4*32];
  int hot_x, hot_y;

  i = -1;
  for ( row=0; row<32; ++row ) {
    for ( col=0; col<32; ++col ) {
      if ( col % 8 ) {
        data[i] <<= 1;
        mask[i] <<= 1;
      } else {
        ++i;
        data[i] = mask[i] = 0;
     }
     switch (image[4+row][col]) {
       case '.':
         data[i] |= 0x01;
         mask[i] |= 0x01;
         break;
       case 'X':
         mask[i] |= 0x01;
         break;
       case ' ':
         break;
      }
    }	
  }
  sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
  return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}

void System::setAction(int new_action) {
  switch (new_action) {
    case (ACTION_DEFAULT): switchCursor(0); break;
    case (ACTION_PICKUP): switchCursor(1); break;
    case (ACTION_TOUCH): switchCursor(2); break;
  }
  action = new_action;
}

void System::switchCursor(int cursor) {
  switch (cursor) {
    case(0): SDL_SetCursor(_cursor_default); break;
    case(1): SDL_SetCursor(_cursor_pickup); break;
    case(2): SDL_SetCursor(_cursor_touch); break;
  }
}

void System::registerCommands(Console *console) {
  console->registerCommand(QUIT, this);
  console->registerCommand(EXIT, this);
  console->registerCommand(GET_ATTRIBUTE, this);
  console->registerCommand(SET_ATTRIBUTE, this);
  console->registerCommand(CHANGE_DIRECTORY, this);
  console->registerCommand(ENABLE_DIR_PREFIX, this);
  console->registerCommand(DISABLE_DIR_PREFIX, this);
  console->registerCommand(RUN_SCRIPT, this);
  console->registerCommand(LOAD_OBJECT_FILE, this);
  console->registerCommand(LOAD_STATE_FILE, this);
  console->registerCommand(LOAD_GENERAL_CONFIG, this);
  console->registerCommand(LOAD_TEXTURE_CONFIG, this);
  console->registerCommand(LOAD_KEY_BINDINGS, this);
  console->registerCommand(LOAD_MODEL_CONFIG, this);
  console->registerCommand(SAVE_GENERAL_CONFIG, this);
  console->registerCommand(SAVE_KEY_BINDINGS, this);
  console->registerCommand(READ_CONFIG, this);
  console->registerCommand(BIND_KEY, this);
  console->registerCommand(KEY_PRESS, this);
  console->registerCommand(TOGGLE_FULLSCREEN, this);
  console->registerCommand(ADD_EVENT, this);
  console->registerCommand(IDENTIFY_ENTITY, this);
}

void System::runCommand(const std::string &command) {
  try {
    _console->runCommand(command);
  } catch (Exception e) {
    Log::writeLog(e.getMessage(), Log::LOG_ERROR);
  }// catch (...) {
//    Log::writeLog("Caught Console Command Exception", Log::LOG_ERROR);
//  }
}

void System::runCommand(const std::string &command, const std::string &args) {
  Log::writeLog(command, Log::LOG_DEFAULT);
  Tokeniser tokeniser = Tokeniser();
  tokeniser.initTokens(args);
  if (command == EXIT || command == QUIT) _system_running = false;
  else if (command == GET_ATTRIBUTE) {
    if (_general) pushMessage(_general->getAttribute(args), CONSOLE_MESSAGE);
  }
  else if (command == SET_ATTRIBUTE) {
    std::string key = tokeniser.nextToken();
    std::string value = tokeniser.remainingTokens();
    if (_general) _general->setAttribute(key, value);
  }
  else if (command == CHANGE_DIRECTORY) {
    if (args.empty()) return;
    chdir(args.c_str());
  }
  else if (command == ENABLE_DIR_PREFIX) _prefix_cwd = true;  
  else if (command == DISABLE_DIR_PREFIX) _prefix_cwd = false;
  else if (command == RUN_SCRIPT) runScript(processHome(args));
  else if (command == LOAD_OBJECT_FILE) {
    if (_ol) _ol->readFiles(processHome(args));
  }
  else if (command == LOAD_STATE_FILE) {
    if (_sl) _sl->readFiles(processHome(args));
  }
  else if (command == LOAD_GENERAL_CONFIG) {
    if (_general) _general->loadConfig(processHome(args));
    else cout << "ARG" << endl;
  }
  else if (command == LOAD_TEXTURE_CONFIG) {
    if (_textures) _textures->loadConfig(processHome(args), _prefix_cwd);
    else cout << "ARG" << endl;
  }
  else if (command == LOAD_MODEL_CONFIG) {
    if (_models) _models->loadConfig(processHome(args));
  }
  else if (command == LOAD_KEY_BINDINGS) {
    Bindings::loadBindings(processHome(args));
  }
  else if (command == SAVE_GENERAL_CONFIG) {
    if (_general) {
      if (args.empty()) _general->saveConfig();
      else _general->saveConfig(processHome(args));
    }
  }
  else if (command == SAVE_KEY_BINDINGS) {
    if (args.empty()) Bindings::saveBindings();
    else Bindings::saveBindings(processHome(args));
  }
  else if (command == READ_CONFIG) {
    readConfig();
    if (renderer) {
      renderer->readConfig();
      renderer->readComponentConfig();
    }
    if (_character)_character->readConfig();
  }
  else if (command == BIND_KEY) {
    std::string key = tokeniser.nextToken();
    std::string value = tokeniser.remainingTokens();
    Bindings::bind(key, value);
  }
  else if (command == KEY_PRESS) {
    runCommand(Bindings::getBinding(args));
  }
  else if (command == TOGGLE_FULLSCREEN) toggleFullscreen();
  else if (command == ADD_EVENT) {
    std::string event_function = tokeniser.nextToken();
    std::string extra = tokeniser.nextToken();
    std::string event_condition = tokeniser.nextToken();
    std::string target = tokeniser.remainingTokens();
    getEventHandler()->addEvent(Event(event_function, target, event_condition, extra));
  }
  else if (command == IDENTIFY_ENTITY) {
    Eris::World *world = Eris::World::Instance();
    if (!world) return;
    WorldEntity *we = ((WorldEntity*)(world->lookup(renderer->getActiveID())));
    if (we) we->displayInfo();  
  }
  else Log::writeLog(std::string("Command not found: - ") + command, Log::LOG_ERROR);
}

} /* namespace Sear */
