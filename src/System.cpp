// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: System.cpp,v 1.72 2004-04-12 15:28:50 alriddoch Exp $

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sear_icon.xpm"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <sigc++/slot.h>
#include <sage/GL.h>
#include <varconf/varconf.h>
#include <Eris/Exceptions.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "ActionHandler.h"
#include "Bindings.h"
#include "Calendar.h"
#include "Camera.h"
#include "Character.h"
#include "client.h"
#include "conf.h"
#include "Console.h"
#include "cursors.h"
#include "EventHandler.h"
#include "Exception.h"
#include "FileHandler.h"
#include "Graphics.h"
#include "ModelHandler.h"
#include "ObjectHandler.h"
#include "Render.h"
#include "Sound.h"
#include "src/ScriptEngine.h"
//#include "StateLoader.h"
#include "System.h"
#include "WorldEntity.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif


#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

// Config section keyword
static const std::string SYSTEM = "system";
  
namespace Sear {


  static const std::string SCRIPTS_DIR = "scripts";
  static const std::string STARTUP_SCRIPT = "startup.script";
  static const std::string SHUTDOWN_SCRIPT = "shutdown.script";

  // Console commands
  static const std::string EXIT = "exit";
  static const std::string QUIT = "quit";

  static const std::string GET_ATTRIBUTE = "getat";
  static const std::string SET_ATTRIBUTE = "setat";

  static const std::string LOAD_MODEL_RECORDS = "load_model_records";
  static const std::string LOAD_OBJECT_RECORDS = "load_object_records";
//  static const std::string LOAD_STATE_FILE = "load_state_file";
  static const std::string LOAD_GENERAL_CONFIG = "load_general";
  static const std::string LOAD_KEY_BINDINGS = "load_bindings";
  static const std::string LOAD_MODEL_CONFIG = "load_models";
  static const std::string SAVE_GENERAL_CONFIG = "save_general";
  static const std::string SAVE_KEY_BINDINGS = "save_bindings";
  static const std::string READ_CONFIG = "read_config";
  static const std::string BIND_KEY = "bind";
  static const std::string KEY_PRESS = "keypress";
  static const std::string TOGGLE_FULLSCREEN = "toggle_fullscreen";
  static const std::string TOGGLE_MLOOK = "toggle_mlook";
  static const std::string ADD_EVENT = "event";
  static const std::string IDENTIFY_ENTITY = "identify";

  // Config key values  
  static const std::string KEY_icon_file = "iconfile";
  static const std::string KEY_mouse_move_select = "mouse_move_select";
  static const std::string KEY_render_use_stencil = "render_use_stencil";
  static const std::string KEY_window_width = "width";
  static const std::string KEY_window_height = "height";
  static const std::string KEY_media_root = "media_root";
  //Config default values
  static const int DEFAULT_window_width = 640;
  static const int DEFAULT_window_height = 480;  
  static const bool DEFAULT_mouse_move_select = true;

System *System::_instance = NULL;

System::System() :
  repeat(false),
  action(ACTION_DEFAULT),
  fullscreen(0),
  mouseLook(0),
  screen(NULL),
  _graphics(NULL),
  renderer(NULL),
  _client(NULL),
  _icon(NULL),
  _width(0),
  _height(0),
  _script_engine(NULL),
  _event_handler(NULL),
  _file_handler(NULL),
  _model_handler(NULL),
//  _state_loader(NULL),
  _action_handler(NULL),
  _object_handler(NULL),
  _calendar(NULL),
  _controller(NULL),
  _console(NULL),
  _character(NULL),
  _cursor_default(NULL),
  _cursor_pickup(NULL),
  _cursor_touch(NULL),
  _mouse_move_select(false),
  _seconds(0.0f),
  _process_records(false),
  sound(NULL),
  _system_running(false),
  _initialised(false)
{
  _instance = this;
  // Initialise system states
  for (unsigned int i = 0; i < SYS_LAST_STATE; ++i) _systemState[i] = false;
}

System::~System() {
  if (_initialised) shutdown();
}


bool System::init() {
  if (_initialised) shutdown();
  if (!initVideo()) return false;


  _event_handler = new EventHandler();
  _event_handler->init();
  _script_engine = new ScriptEngine();
  _script_engine->init();
  _model_handler = new ModelHandler();
  _model_handler->init();
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
    home_path += "/.sear/";
    mkdir(home_path.c_str(), 0755);
  }
#endif
  // This should not be hardcoded!!
  install_path = std::string(INSTALLDIR) + std::string("/share/sear/");
 
  _action_handler = new ActionHandler(this);
  _action_handler->init();

  _file_handler = new FileHandler();
  _file_handler->addSearchPath(install_path);
  _file_handler->addSearchPath(install_path + "/scripts");
  _file_handler->addSearchPath(home_path);
  _file_handler->addSearchPath(".");

  _file_handler->setVariable("SEAR_INSTALL", install_path);
  _file_handler->setVariable("SEAR_MEDIA", install_path + "/sear-media/");
  _file_handler->setVariable("SEAR_HOME", home_path);

  for (std::list<std::string>::const_iterator I = additional_paths.begin(); I != additional_paths.end(); ++I) {
    _file_handler->addSearchPath(*I);
  }
//  _state_loader = new StateLoader();
//  _state_loader->init();
  
  _object_handler = new ObjectHandler();
  _object_handler->init();
 
  _calendar = new Calendar();
  _calendar->init();
 
  // Connect signals for record processing 
  _general.sigsv.connect(SigC::slot(*this, &System::varconf_callback));
  _textures.sigsv.connect(SigC::slot(*this, &System::varconf_callback));
  _models.sigsv.connect(SigC::slot(*this, &System::varconf_callback));
  _model_records.sigsv.connect(SigC::slot(*this, &System::varconf_callback));
  
  // Connect signals for error messages
  _general.sige.connect(SigC::slot(*this, &System::varconf_error_callback));
  _textures.sige.connect(SigC::slot(*this, &System::varconf_error_callback));
  _models.sige.connect(SigC::slot(*this, &System::varconf_error_callback));
  _model_records.sige.connect(SigC::slot(*this, &System::varconf_error_callback));
  
  Bindings::init();
  Bindings::bind("escape", "/" + QUIT);
  Bindings::bind("backquote", "/toggle_console");
  Bindings::bind("caret", "/toggle_console");
  
  _console = new Console(this);
  _console->init();
  registerCommands(_console);
  _client->registerCommands(_console);
  _script_engine->registerCommands(_console);
  _action_handler->registerCommands(_console);
  _file_handler->registerCommands(_console);
  _object_handler->registerCommands(_console);
  _calendar->registerCommands(_console);

  int sticks = SDL_NumJoysticks();
  if (sticks > 0) {
    SDL_JoystickEventState(SDL_ENABLE);
    _controller = SDL_JoystickOpen(0);
    int axes = SDL_JoystickNumAxes(_controller);
    if (axes < 4) {
      std::cout << "Joystick has less than 4 axis" << std::endl << std::flush;
    }
    int buttons = SDL_JoystickNumButtons(_controller);
    std::cout << "Got a joystick with " << axes << " axes, and " <<
              buttons << " buttons." << std::endl << std::flush;
  }

  try { 
    sound = new Sound();
    sound->init();
    sound->registerCommands(_console);
  } catch (Exception &e) {
    Log::writeLog(e.getMessage(), Log::LOG_ERROR);
  }
  
  _width = 640; _height = 480;
  //_width = 800; _height = 600;
  createWindow(false);
  if (debug) Log::writeLog("Running startup scripts", Log::LOG_INFO);
  std::list<std::string> startup_scripts = _file_handler->getAllinSearchPaths(STARTUP_SCRIPT);
  for (std::list<std::string>::const_iterator I = startup_scripts.begin(); I != startup_scripts.end(); ++I) {
    _script_engine->runScript(*I);
  }
  readConfig();
  _general.sigsv.connect(SigC::slot(*this, &System::varconf_general_callback));
  
  _command_history_iterator = _command_history.begin();
  _graphics->initST();
  _system_running = true;
  _initialised = true;
  return true;
}

void System::shutdown() {
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
  
  if (_action_handler) {
    _action_handler->shutdown();
    delete _action_handler;
    _action_handler = NULL;
  }
  if (_object_handler) {
    _object_handler->shutdown();
    delete _object_handler;
    _object_handler = NULL;
  }
 
  if (_model_handler) {
    _model_handler->shutdown();
    delete _model_handler;
    _model_handler = NULL;
  }  
  
  if (_graphics) {
    _graphics->writeConfig();
    _graphics->writeComponentConfig();
    _graphics->shutdown();
    delete _graphics;
    _graphics = NULL;
  }

  if (_calendar) {
    _calendar->shutdown();
    delete _calendar;
    _calendar = NULL;
  }
  writeConfig();
  if (debug) Log::writeLog("Running shutdown scripts", Log::LOG_INFO);
  std::list<std::string> shutdown_scripts = _file_handler->getAllinSearchPaths(SHUTDOWN_SCRIPT);
  for (std::list<std::string>::const_iterator I = shutdown_scripts.begin(); I != shutdown_scripts.end(); ++I) {
    _script_engine->runScript(*I);
  }
  Bindings::shutdown();
 if (_event_handler) {
    _event_handler->shutdown();
    delete _event_handler;
    _event_handler = NULL;
  }  
  if (_script_engine) {
    _script_engine->shutdown();
    delete _script_engine;
    _script_engine = NULL;
  }  
  if (_file_handler) {
//    _file_handler->shutdown();
    delete _file_handler;
    _file_handler = NULL;
  }
  if (_console) {
    _console->shutdown();
    delete _console;
    _console = NULL;
  }
 
//  if (_state_loader) {
//    _state_loader->shutdown();
//    delete _state_loader;
//    _state_loader = NULL;
//  }

  // Are these actually needed ? or does SDL clean then up too? 
//  if (_icon) SDL_FreeSurface(_icon);
//  if (_cursor_default) SDL_FreeCursor(_cursor_default);
//  if (_cursor_pickup) SDL_FreeCursor(_cursor_pickup);
//  if (_cursor_touch) SDL_FreeCursor(_cursor_touch);
  if (sound) {
    sound->shutdown();
    delete sound;
    sound = NULL;
  }
  SDL_Quit();
  _initialised = false;
}

bool System::initVideo() {
  if (debug) Log::writeLog("Initialising Video", Log::LOG_INFO);
#ifdef DEBUG
//#warning "PARACHUTE IS DISABLED"
  // NOPARACHUTE means SDL doesn't handle any errors allowing us to catch them in a debugger
  if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE) < 0 ) {
#else
  // We want release versions to die quietly
  if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0 ) {
#endif
    Log::writeLog(std::string("Unable to init SDL: ") + string_fmt(SDL_GetError()), Log::LOG_ERROR);
    return false;
  } 
  return true;
}

void System::createWindow(bool fullscreen) {
  if (debug) Log::writeLog("Creating Window", Log::LOG_INFO);
  //Request Open GL window attributes
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5 );
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5 );
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5 );
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1 );
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1 );
  const SDL_VideoInfo *info = SDL_GetVideoInfo();
  if (!info) {
    Log::writeLog("Error quering video", Log::LOG_DEFAULT);
  }
  //Create Window
  int flags = SDL_OPENGL;
  int bpp = info->vfmt->BitsPerPixel;
  if (fullscreen) flags |= SDL_FULLSCREEN;
  if (!(_width && _height)) {
    Log::writeLog(std::string("Invalid resolution: ") + string_fmt(_width) + std::string(" x ") + string_fmt(_height), Log::LOG_ERROR);
    _system_running = false;
    exit(1);
  }
  if (debug) Log::writeLog(std::string("Setting video to ") + string_fmt(_width) + std::string(" x ") + string_fmt(_height), Log::LOG_INFO);

  //Is this the correct way to free a window?
  if (screen) SDL_FreeSurface(screen);
  screen = SDL_SetVideoMode(_width, _height, bpp, flags);
  if (screen == NULL ) {
    Log::writeLog(std::string("Unable to set ") + string_fmt(_width) + std::string(" x ") + string_fmt(_height) + std::string(" video: ") + string_fmt(SDL_GetError()), Log::LOG_ERROR);
    _system_running = false;
    exit(1);
  }

  // Check OpenGL flags
  int value;
  if (debug) {
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &value);
    Log::writeLog(std::string("Red Size: ") + string_fmt(value), Log::LOG_DEFAULT);
    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &value);
    Log::writeLog(std::string("Blue Size: ") + string_fmt(value), Log::LOG_DEFAULT);
    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &value);
    Log::writeLog(std::string("Green Size: ") + string_fmt(value), Log::LOG_DEFAULT);
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &value);
    Log::writeLog(std::string("Depth Size: ") + string_fmt(value), Log::LOG_DEFAULT);
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &value);
    Log::writeLog(std::string("Stencil Size: ") + string_fmt(value), Log::LOG_DEFAULT);
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &value);
    Log::writeLog(std::string("Double Buffer: ") + string_fmt(value), Log::LOG_DEFAULT);
  }

  SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &value);
  if (value < 1) _general.setItem("render_options", "use_stencil_buffer", false);
  if (!_icon) _icon = IMG_ReadXPMFromArray(sear_icon_xpm);
  SDL_WM_SetIcon(_icon, NULL);
  if (!_cursor_default) _cursor_default = buildCursor(CURSOR_DEFAULT);
  if (!_cursor_pickup)  _cursor_pickup = buildCursor(CURSOR_PICKUP); 
  if (!_cursor_touch)   _cursor_touch = buildCursor(CURSOR_TOUCH);
  if (debug) Log::writeLog("Creating Renderer", Log::LOG_INFO);
  
  // Delete grpahics object if it already exists
  if (_graphics) {
    _graphics->shutdown();
    delete _graphics;
  }
  _graphics = new Graphics(this);
  _graphics->init();

  _graphics->registerCommands(_console);
  _graphics->getRender()->initWindow(_width, _height);

  renderer = _graphics->getRender();
  pushMessage("Loading, Please wait...", 2, 100);

  // Don't think we need this anymore
//  _graphics->drawScene("", false, 0); // Render scene one before producing colour set
  renderer->buildColourSet();
}

void System::mainLoop() {
  SDL_Event event;
  static float last_time = 0.0f;
  _action_handler->handleAction("system_start", NULL);
  while (_system_running) {
    try {
      _seconds = (float)SDL_GetTicks() / 1000.0f;
      float time_elapsed = _seconds - last_time;
      last_time = _seconds;
      while (SDL_PollEvent(&event)  ) {
        handleEvents(event);
        // Stop processing events if we are quiting
        if (!_system_running) break;
      }
      // Handle mouse and joystick
      handleAnalogueControllers();
      // Update Calendar
      _calendar->update(time_elapsed);
      // Poll event handler
      _event_handler->poll();
      // poll network
      _client->poll();
      // draw scene
      _graphics->drawScene(command, false, time_elapsed);
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
    case SDL_MOUSEBUTTONUP: {
      switch (event.button.button) {
        case (SDL_BUTTON_LEFT):   { 
          break;
        }
        break;
      }
      break;
    }
    case SDL_MOUSEMOTION: {
      if (_mouse_move_select && !mouseLook) renderer->procEvent(event.button.x, event.button.y);
      break;
    } 
    case SDL_KEYDOWN: {
     // Keys that still execute bindings with console open 
      if (_console->consoleStatus()) {
        if (!repeat) {
          SDL_EnableKeyRepeat(1000,500);
          //SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
          command = "";
          repeat = true;
        }
        if (event.key.keysym.sym == SDLK_UP) {
          // Previous command
          if (_command_history_iterator != _command_history.begin()) {
            _command_history_iterator--;
            command = (*_command_history_iterator);
          }
        }
        else if (event.key.keysym.sym == SDLK_DOWN) {
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
    case SDL_JOYAXISMOTION: {
      switch (event.jaxis.axis) {
        case 0: // Left right move
          if (_character != NULL) {
            if (abs(event.jaxis.value) > 3200) {
              _character->setStrafeSpeed(event.jaxis.value / 10000.f);
            } else {
                std::cout << "X too small" << std::endl << std::flush;
              _character->setStrafeSpeed(0);
            }
          }
          break;
        case 1: // For back move
          if (_character != NULL) {
            if (abs(event.jaxis.value) > 3200) {
              _character->setMovementSpeed(event.jaxis.value / -10000.f);
            } else {
                std::cout << "Y too small" << std::endl << std::flush;
              _character->setMovementSpeed(0);
            }
          }
          break;
        case 4: // Left right view
          if (_character != NULL) {
            if (abs(event.jaxis.value) > 3200) {
              _character->setRotationRate(event.jaxis.value / 10000.f);
            } else {
                std::cout << "X too small" << std::endl << std::flush;
              _character->setRotationRate(0);
            }
          }
          break;
        case 5: // Up down view
          Graphics * g = getGraphics();
          if (g != NULL) {
            Camera * c = g->getCamera();
            if (c != NULL) {
              if (abs(event.jaxis.value) > 3200) {
                c->setElevationSpeed(event.jaxis.value / 10000.f);
              } else {
                std::cout << "Y too small" << std::endl << std::flush;
                c->setElevationSpeed(0);
              }
            }
          }
          break;
      }
      break;
    }
    case SDL_QUIT: {
      _system_running = false;
      break;
    }
  }
}

void System::handleAnalogueControllers() {
  if (mouseLook) {
    // We should still be ok if the user wants to drag something
    int mx = _width / 2,
        my = _height / 2;
    int dx, dy;
    SDL_GetMouseState(&dx, &dy);
    dx -= mx;
    dy -= my;
    Graphics * g = getGraphics();
    Camera * c = NULL;
    if (g != NULL) {
      c = g->getCamera();
    }
    if (dx != 0) {
      float rotation = dx / 4.f;
      if (c != NULL) {
        c->rotateImmediate(rotation);
      }
    }
    if (dy != 0) {
      float elevation = -dy / 4.f;
      std::cout << "E: " << elevation << std::endl << std::flush;
      if (c != NULL) {
        c->elevateImmediate(elevation);
      }
    }
    
    if ((dx != 0) || (dy != 0)) {
      SDL_WarpMouse(mx, my);
    }
  }
#if 0
  if (_controller != NULL) {
    Graphics * g = getGraphics();
    if (g != NULL) {
      Camera * c = g->getCamera();
      if (c != NULL) {
        int rot = SDL_JoystickGetAxis(_controller, 4);
        if (abs(rot) > 3200) {
          c->setRotation(rot / -10000);
        } else {
          c->setRotation(0);
        }

        int elev = SDL_JoystickGetAxis(_controller, 5);
        if (abs(elev) > 3200) {
          c->setElevation(elev / -10000);
        } else {
          c->setElevation(0);
        }
      }
    }
    std::cout << "Upd "
              << SDL_JoystickGetAxis(_controller, 0) << ":"
              << SDL_JoystickGetAxis(_controller, 1) << ":"
              << SDL_JoystickGetAxis(_controller, 2) << ":"
              << SDL_JoystickGetAxis(_controller, 3) << ":"
              << SDL_JoystickGetAxis(_controller, 4) << ":"
              << SDL_JoystickGetAxis(_controller, 5) << ":"
              << SDL_JoystickGetAxis(_controller, 6) << ":"
              << std::endl << std::flush;
  }
#endif
}

void System::setCaption(const std::string &title, const std::string &icon) {
  // Set window and icon title
  SDL_WM_SetCaption(title.c_str(), icon.c_str());
}

void System::toggleFullscreen() {
  fullscreen = ! fullscreen;
  // If fullscreen fails, create a new window with the fullscreen flag (un)set
  if (!SDL_WM_ToggleFullScreen(screen)) createWindow(fullscreen);
}

void System::toggleMouselook() {
  std::cout << "System::toggleMouselook()" << std::endl << std::flush;
  mouseLook = ! mouseLook;
  if (mouseLook) {
    SDL_ShowCursor(SDL_DISABLE);
    SDL_WarpMouse(_width / 2, _height / 2);
  } else {
    SDL_ShowCursor(SDL_ENABLE);
  }
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
  int i = input.find("~");
  if (i == -1) return input;
  std::string output = input.substr(0, i) + home_path + input.substr(i + 1);
  return output;
}

void System::setCharacter(Character *character) {
  if (_character) { // get rid of old instance if it exists
    _character->shutdown();
    delete _character;
    _character = NULL;
  }
  _character = character;
  // Assuming init has not been performed
  _character->init();
  _character->registerCommands(_console);
}

void System::readConfig() {
  varconf::Variable temp;

  temp = _general.getItem(SYSTEM, KEY_mouse_move_select);
  _mouse_move_select =  (!temp.is_bool()) ? (DEFAULT_mouse_move_select) : ((bool)temp);

  temp = _general.getItem(SYSTEM, KEY_window_width);
  _width = (!temp.is_int()) ? (DEFAULT_window_width) : ((int)temp);
  temp = _general.getItem(SYSTEM, KEY_window_height);
  _height = (!temp.is_int()) ? (DEFAULT_window_height) : ((int)temp);

  temp = _general.getItem(SYSTEM, KEY_media_root);
  _media_root = (!temp.is_string()) ? (INSTALLDIR) : ((std::string)temp);
}

void System::writeConfig() {
  _general.setItem(SYSTEM, KEY_mouse_move_select,  _mouse_move_select);

  _general.setItem(SYSTEM, KEY_window_width, _width);
  _general.setItem(SYSTEM, KEY_window_height, _height);
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
  console->registerCommand(LOAD_MODEL_RECORDS, this);
//  console->registerCommand(LOAD_STATE_FILE, this);
  console->registerCommand(LOAD_GENERAL_CONFIG, this);
  console->registerCommand(LOAD_KEY_BINDINGS, this);
  console->registerCommand(LOAD_MODEL_CONFIG, this);
  console->registerCommand(SAVE_GENERAL_CONFIG, this);
  console->registerCommand(SAVE_KEY_BINDINGS, this);
  console->registerCommand(READ_CONFIG, this);
  console->registerCommand(BIND_KEY, this);
  console->registerCommand(KEY_PRESS, this);
  console->registerCommand(TOGGLE_FULLSCREEN, this);
  console->registerCommand(TOGGLE_MLOOK, this);
  console->registerCommand(ADD_EVENT, this);
  console->registerCommand(IDENTIFY_ENTITY, this);
  console->registerCommand("normalise_on", this);
  console->registerCommand("normalise_off", this);
  console->registerCommand("setvar", this);
  console->registerCommand("getvar", this);
}

void System::runCommand(const std::string &command) {
  if (debug) Log::writeLog(command, Log::LOG_INFO);
  try {
    _console->runCommand(command);
  } catch (Exception e) {
    Log::writeLog(e.getMessage(), Log::LOG_ERROR);
  }// catch (...) {
//    Log::writeLog("Caught Console Command Exception", Log::LOG_ERROR);
//  }
}

void System::runCommand(const std::string &command, const std::string &args_t) {
  Tokeniser tokeniser;
  std::string args = args_t;
  _file_handler->expandString(args);
  tokeniser.initTokens(args);
  if (command == EXIT || command == QUIT) _system_running = false;
  else if (command == GET_ATTRIBUTE) {
    std::string section = tokeniser.nextToken();
    std::string key = tokeniser.remainingTokens();
     pushMessage(_general.getItem(section, key), CONSOLE_MESSAGE);
  }
  else if (command == SET_ATTRIBUTE) {
    std::string section = tokeniser.nextToken();
    std::string key = tokeniser.nextToken();
    std::string value = tokeniser.remainingTokens();
    _general.setItem(section, key, value);
  }
//  else if (command == LOAD_STATE_FILE) {
//    if (_state_loader) _state_loader->readFiles(processHome(args));
//  }
  else if (command == LOAD_GENERAL_CONFIG) {
    _process_records = _script_engine->prefixEnabled();
    _general.readFromFile(processHome(args));
    if (_process_records) {
      _process_records = false;
      processRecords();
    }
  }
  else if (command == LOAD_MODEL_RECORDS) {
    _process_records = _script_engine->prefixEnabled();
    _model_records.readFromFile(processHome(args));
    if (_process_records) {
      _process_records = false;
      processRecords();
    }
  }
 else if (command == LOAD_MODEL_CONFIG) {
    _process_records = _script_engine->prefixEnabled();
    _models.readFromFile(processHome(args));
    if (_process_records) {
      _process_records = false;
      processRecords();
    }
  }
  else if (command == LOAD_KEY_BINDINGS) {
    Bindings::loadBindings(processHome(args));
  }
  else if (command == SAVE_GENERAL_CONFIG) {
    _general.writeToFile(processHome(args));
  }
  else if (command == SAVE_KEY_BINDINGS) {
    Bindings::saveBindings(processHome(args));
  }
  else if (command == READ_CONFIG) {
    readConfig();
    if (_graphics) {
      _graphics->readConfig();
      _graphics->readComponentConfig();
    }
    if (renderer) {
      renderer->readConfig();
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
  else if (command == TOGGLE_MLOOK) toggleMouselook();
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

  else if (command == "normalise_on") glEnable(GL_NORMALIZE);
  else if (command == "normalise_off") glDisable(GL_NORMALIZE);

  else if (command == "setvar") {
    std::string key = tokeniser.nextToken();
    std::string value = tokeniser.remainingTokens();
    _file_handler->setVariable(key, value);
  }
  else if (command == "getvar") {
    std::string key = tokeniser.nextToken();
    pushMessage(_file_handler->getVariable(key), CONSOLE_MESSAGE);
  }
  
  else Log::writeLog(std::string("Command not found: - ") + command, Log::LOG_ERROR);
}

void System::varconf_error_callback(const char *error) {
  Log::writeLog(std::string("Varconf Error: ") + error, Log::LOG_ERROR);
}

void System::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  if (_process_records && _script_engine->prefixEnabled()) {
    varconf::Variable v = config.getItem(section, key);
    if (v.is_string()) {
      VarconfRecord *r = new VarconfRecord();//*)malloc(sizeof(VarconfRecord));
      r->section = section;
      r->key = key; 
      r->config = &config;
      record_list.push_back(r);
    }
  }
}

void System::processRecords() {
  while (!record_list.empty()) {
    VarconfRecord *r = *record_list.begin();
    std::string value = r->config->getItem(r->section, r->key);
    _file_handler->expandString(value);
    r->config->setItem(r->section, r->key, value);
    delete r;
    record_list.erase(record_list.begin());
  }
}

void System::addSearchPaths(std::list<std::string> l) {
  for (std::list<std::string>::const_iterator I = l.begin(); I != l.end(); ++I) {        
    additional_paths.push_back(*I);
  }
}


void System::varconf_general_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  varconf::Variable temp;
  if (section == SYSTEM) {
    if (key ==  KEY_mouse_move_select) {
      temp = config.getItem(SYSTEM, KEY_mouse_move_select);
      _mouse_move_select =  (!temp.is_bool()) ? (DEFAULT_mouse_move_select) : ((bool)temp);
    }
    else if (key == KEY_window_width) {
      temp = config.getItem(SYSTEM, KEY_window_width);
      _width = (!temp.is_int()) ? (DEFAULT_window_width) : ((int)temp);
    }
    else if (key == KEY_window_height) {
      temp = config.getItem(SYSTEM, KEY_window_height);
      _height = (!temp.is_int()) ? (DEFAULT_window_height) : ((int)temp);
    }
  }
}       
} /* namespace Sear */
