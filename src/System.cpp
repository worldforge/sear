// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: System.cpp,v 1.128 2005-06-19 22:52:22 alriddoch Exp $

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <sigc++/slot.h>
#include <sage/sage.h>
#include <sage/GL.h>
#include <varconf/varconf.h>
#include <Eris/Exceptions.h>
#include <Eris/View.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "ActionHandler.h"
#include "Bindings.h"
#include "Calendar.h"
#include "renderers/Camera.h"
#include "renderers/CameraSystem.h"
#include "Character.h"
#include "client.h"
#include "Console.h"
#include "Exception.h"
#include "FileHandler.h"
#include "renderers/Graphics.h"
#include "loaders/ModelSystem.h"
#include "renderers/Render.h"
#include "renderers/GL.h"
#include "Sound.h"
#include "src/ScriptEngine.h"
#include "System.h"
#include "WorldEntity.h"
#include "renderers/RenderSystem.h"
#include "environment/Environment.h"
#include "Editor.h"

#include "guichan/Workarea.h"

#include "CacheManager.h"
#include "renderers/StaticObject.h"

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

  static const std::string LOAD_GENERAL_CONFIG = "load_general";
  static const std::string LOAD_KEY_BINDINGS = "load_bindings";
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
  static const std::string KEY_mouse_move_select = "mouse_move_select";
  static const std::string KEY_render_use_stencil = "render_use_stencil";
  static const std::string KEY_window_width = "width";
  static const std::string KEY_window_height = "height";
  static const std::string KEY_media_root = "media_root";
  
  static const std::string SECTION_INPUT = "input";
  static const std::string KEY_STRAFE_AXIS = "strafe_axis";
  static const std::string KEY_MOVE_AXIS = "move_axis";
  static const std::string KEY_PAN_AXIS = "pan_axis";
  static const std::string KEY_DISABLE_JOYSTICK = "disable_joystick";
  static const std::string KEY_ELEVATION_AXIS = "elevation_axis";
  static const std::string KEY_key_repeat_delay = "key_repeat_delay";
  static const std::string KEY_key_repeat_rate = "key_repeat_rate";
 
  static const std::string KEY_fullscreen = "start_fullscreen";
  
  //Config default values
  static const int DEFAULT_window_width = 640;
  static const int DEFAULT_window_height = 480;  
  static const bool DEFAULT_mouse_move_select = false;
  static const double DEFAULT_max_click_time = 0.3;
  static const int DEFAULT_joystick_touch_button = 1;
  static const int DEFAULT_joystick_pickup_button = 2;
  static const int DEFAULT_key_repeat_delay = 1000;
  static const int DEFAULT_key_repeat_rate = 500;
 
  static const bool DEFAULT_fullscreen = false;
 
System *System::m_instance = NULL;

System::System() :
  m_action(ACTION_DEFAULT),
  m_mouseLook(0),
  m_screen(NULL),
  m_client(NULL),
  m_width(0),
  m_height(0),
  m_KeyRepeatDelay(DEFAULT_key_repeat_delay),
  m_KeyRepeatRate(DEFAULT_key_repeat_rate),
  m_click_on(false),
  m_click_x(0),
  m_click_y(0),
  m_script_engine(NULL),
  m_action_handler(NULL),
  m_calendar(NULL),
  m_controller(NULL),
  m_console(NULL),
  m_character(NULL),
  m_mouse_move_select(false),
  m_seconds(0.0),
  m_process_records(false),
  m_sound(NULL),
  m_system_running(false),
  m_editor(NULL),
  m_initialised(false),
  m_startFullscreen(DEFAULT_fullscreen)
{
  m_instance = this;
  // Initialise system states
  for (unsigned int i = 0; i < SYS_LAST_STATE; ++i) m_systemState[i] = false;
    
  // create the filehandler early, so we can call addSearchPath on it
  m_file_handler = new FileHandler();
}

System::~System() {
  assert (m_initialised == false);
  if (m_initialised) shutdown();
}


bool System::init(int argc, char *argv[]) {
  assert (m_initialised == false);

  if (!initVideo()) return false;

  m_script_engine = new ScriptEngine();
  m_script_engine->init();

  m_client = new Client(this, "Sear");
  if(!m_client->init()) {
    Log::writeLog("Error initializing Eris", Log::LOG_ERROR);

    delete m_client;
    m_client = NULL;

    m_script_engine->shutdown();
    delete m_script_engine;
    m_script_engine = NULL;

    return false;
  }
  
  m_action_handler = new ActionHandler(this);
  m_action_handler->init();

  m_calendar = new Calendar();
  m_calendar->init();
 
  // Connect signals for record processing 
  m_general.sigsv.connect(SigC::slot(*this, &System::varconf_callback));
  
  // Connect signals for error messages
  m_general.sige.connect(SigC::slot(*this, &System::varconf_error_callback));
  
  Bindings::init();
  Bindings::bind("escape", "/" + QUIT);
  Bindings::bind("backquote", "/toggle_console");
  Bindings::bind("caret", "/toggle_console");
  
  m_console = new Console(this);
  m_console->init();
  registerCommands(m_console);

  m_client->registerCommands(m_console);
  m_script_engine->registerCommands(m_console);
  m_action_handler->registerCommands(m_console);
  m_file_handler->registerCommands(m_console);
  m_calendar->registerCommands(m_console);

  m_character = new Character();
  m_character->init();
  m_character->registerCommands(m_console);

  m_editor = new Editor();
  m_editor->registerCommands(m_console);

  m_workarea = new Workarea(this);
  m_workarea->registerCommands(m_console);

  int sticks = SDL_NumJoysticks();

  if (m_general.findItem(SECTION_INPUT, KEY_MOVE_AXIS))  {
    if ((bool)m_general.getItem(SECTION_INPUT, KEY_DISABLE_JOYSTICK)) {
      sticks = 0;
    }
  }

  if (sticks > 0) {
    SDL_JoystickEventState(SDL_ENABLE);
    m_controller = SDL_JoystickOpen(0);
    int axes = SDL_JoystickNumAxes(m_controller);
    if (axes < 4) {
      std::cout << "Joystick has less than 4 axis" << std::endl << std::flush;
    }
    int buttons = SDL_JoystickNumButtons(m_controller);
    std::cout << "Got a joystick with " << axes << " axes, and " <<
              buttons << " buttons." << std::endl << std::flush;
              
    if (m_general.findItem(SECTION_INPUT, KEY_STRAFE_AXIS))
        m_axisBindings[m_general.getItem(SECTION_INPUT, KEY_STRAFE_AXIS)] = AXIS_STRAFE;
    else
        m_axisBindings[0] = AXIS_STRAFE;
        
    if (m_general.findItem(SECTION_INPUT, KEY_MOVE_AXIS))
        m_axisBindings[m_general.getItem(SECTION_INPUT, KEY_MOVE_AXIS)] = AXIS_MOVE;
    else
        m_axisBindings[1] = AXIS_MOVE;
        
    if (m_general.findItem(SECTION_INPUT, KEY_PAN_AXIS))
        m_axisBindings[m_general.getItem(SECTION_INPUT, KEY_PAN_AXIS)] = AXIS_PAN;
    else
        m_axisBindings[2] = AXIS_PAN;
    
    if (m_general.findItem(SECTION_INPUT, KEY_ELEVATION_AXIS))
        m_axisBindings[m_general.getItem(SECTION_INPUT, KEY_ELEVATION_AXIS)] = AXIS_ELEVATE;
    else
        m_axisBindings[3] = AXIS_ELEVATE;
  }

/*
    sound = new Sound();
    if (sound->init()) {
      delete sound;
      sound = NULL;
    } else { 
      sound->registerCommands(m_console);
    }
*/
  RenderSystem::getInstance().init();
  RenderSystem::getInstance().registerCommands(m_console);
  ModelSystem::getInstance().init();
  ModelSystem::getInstance().registerCommands(m_console);
  CacheManager::getInstance().init();

  // Register StaticObject with CacheManager
//  StaticObject *so = new StaticObject();
//  CacheManager::getInstance().addType(so);
//  delete so;
  
 
  Environment::getInstance().init();

  if (debug) Log::writeLog("Running startup scripts", Log::LOG_INFO);
  FileHandler::FileList startup_scripts = m_file_handler->getAllinSearchPaths(STARTUP_SCRIPT);
  for (FileHandler::FileList::const_iterator I = startup_scripts.begin(); I != startup_scripts.end(); ++I) {
    m_script_engine->runScript(*I);
  }

  // Pass command line into general varconf object for processing
  m_general.getCmdline(argc, argv);
  readConfig(m_general);
  RenderSystem::getInstance().readConfig(m_general);
  m_general.sigsv.connect(SigC::slot(*this, &System::varconf_general_callback));

  // Try and create the window
  bool success;
  if (!(success = RenderSystem::getInstance().createWindow(m_width, m_height, m_startFullscreen))) {
    // Only try again if stencil buffer was enabled first time round
    if (RenderSystem::getInstance().getState(RenderSystem::RENDER_STENCIL)) {
      // Disable stencil buffer and try again
      RenderSystem::getInstance().setState(RenderSystem::RENDER_STENCIL, false);
      success = RenderSystem::getInstance().createWindow(m_width, m_height, m_startFullscreen);
    }
  }
  if (!success) return false;

  // Hide cursor
  SDL_ShowCursor(0);

  RenderSystem::getInstance().initContext();

  m_workarea->init();

  m_system_running = true;
  m_initialised = true;
  return true;
}

void System::shutdown() {

  assert (m_initialised == true);
  std::cout << "System: Starting Shutdown" << std::endl;
  // Save config
  writeConfig(m_general);

  m_client->shutdown();
  delete m_client;
  m_client = NULL;
  
  m_character->shutdown();
  delete m_character;
  m_character = NULL;

  m_action_handler->shutdown();
  delete m_action_handler;
  m_action_handler = NULL;

  CacheManager::getInstance().shutdown();
  Environment::getInstance().shutdown();
  ModelSystem::getInstance().shutdown(); 
  RenderSystem::getInstance().destroyWindow();
  RenderSystem::getInstance().shutdown();

  delete m_editor;
  m_editor = NULL;

  delete m_workarea;
  m_workarea = NULL;

  m_calendar->shutdown();
  delete m_calendar;
  m_calendar = NULL;

  if (debug) Log::writeLog("Running shutdown scripts", Log::LOG_INFO);
  FileHandler::FileList shutdown_scripts = m_file_handler->getAllinSearchPaths(SHUTDOWN_SCRIPT);
  for (FileHandler::FileList::const_iterator I = shutdown_scripts.begin(); I != shutdown_scripts.end(); ++I) {
    m_script_engine->runScript(*I);
  }
  Bindings::shutdown();

  m_script_engine->shutdown();
  delete m_script_engine;
  m_script_engine = NULL;

  delete m_file_handler;
  m_file_handler = NULL;

  m_console->shutdown();
  delete m_console;
  m_console = NULL;
 
  if (m_sound) {
    m_sound->shutdown();
    delete m_sound;
    m_sound = NULL;
  }

  SDL_Quit();

  m_initialised = false;
  std::cout << "System: Finished Shutdown" << std::endl;
}

bool System::initVideo() {
  if (debug) Log::writeLog("Initialising Video", Log::LOG_INFO);
#ifdef DEBUG
//#warning "PARACHUTE IS DISABLED"
  // NOPARACHUTE means SDL doesn't handle any errors allowing us to catch them in a debugger
  if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE) < 0 ) {
#else
  // We want release versions to die quietly
  if (SDL_Init(SDL_INIT_JOYSTICK) < 0 ) {
#endif
    Log::writeLog(std::string("Unable to init SDL: ") + string_fmt(SDL_GetError()), Log::LOG_ERROR);
    return false;
  } 
  return true;
}

void System::mainLoop() {
  SDL_Event event;
  static double last_time = 0.0;
  m_action_handler->handleAction("system_start", NULL);
  while (m_system_running) {
    try {
      m_seconds = (double)SDL_GetTicks() / 1000.0f;
      double time_elapsed = m_seconds - last_time;
      last_time = m_seconds;
      while (SDL_PollEvent(&event)  ) {
        handleEvents(event);
        // Stop processing events if we are quiting
        if (!m_system_running) break;
      }
      // Handle mouse and joystick
      handleAnalogueControllers();
      // poll network
      m_client->poll();
      if (m_client->getAvatar() && m_client->getAvatar()->getView()) {
        m_client->getAvatar()->getView()->update();
      }
      // Update Calendar
      if (checkState(SYS_IN_WORLD)) {
        m_calendar->update(m_client->getAvatar()->getWorldTime());
      }
      // draw scene
      RenderSystem::getInstance().drawScene(false, time_elapsed);
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
  Render *renderer = RenderSystem::getInstance().getRenderer();

  assert(m_character != NULL);
  assert(renderer != NULL);

  if (m_workarea->handleEvent(event)) {
    return;
  }

  switch (event.type) {
    case SDL_MOUSEBUTTONDOWN: {
      switch (event.button.button) {
        case (SDL_BUTTON_LEFT):   { 
          m_click_on = true;
          m_click_x = event.button.x;
          m_click_y = event.button.y;
          m_click_seconds = m_seconds;
          float x,y,z;
          if (dynamic_cast<GL*>(renderer)->getWorldCoords(m_click_x, m_click_y, x,y,z)) {
              m_click_pos = WFMath::Point<3>(x,y,z);
          }
          renderer->procEvent(event.button.x, event.button.y);
          m_click_id = renderer->getActiveID();
          break;
        }
        break;
        case (SDL_BUTTON_RIGHT):   { 
//          if (m_character != NULL) {
            m_character->moveForward(1);
  //        }
        }
        break;
      }
      break;
    }
    case SDL_MOUSEBUTTONUP: {
      switch (event.button.button) {
        case (SDL_BUTTON_LEFT):   {
          switch (m_action) {
            case (ACTION_DEFAULT): {
              double period = m_seconds - m_click_seconds;
              if ((period > DEFAULT_max_click_time) &&
                  ((event.button.x != m_click_x) ||
                   (event.button.y != m_click_y))) {
                  std::cout << "DRAG" << std::endl << std::flush;
                  m_character->getEntity(m_click_id);
              } else {
                  std::cout << "CLICK" << std::endl << std::flush;
                  m_character->touchEntity(m_click_id);
              }
            }
            break;
            case (ACTION_PICKUP): m_character->getEntity(m_click_id); break;
            case (ACTION_TOUCH): m_character->touchEntity(m_click_id); break;
            case (ACTION_USE): m_character->useToolOnEntity(m_click_id, m_click_pos); break;
          }
          setAction(ACTION_DEFAULT);
          m_click_on = false;
          break;
        }
        break;
        case (SDL_BUTTON_RIGHT):   { 
//          if (m_character != NULL) {
            m_character->moveForward(-1);
  //        }
        }
        break;
      }
      break;
    }
    case SDL_MOUSEMOTION: {
      if (m_mouse_move_select && !m_mouseLook) renderer->procEvent(event.button.x, event.button.y);
      break;
    } 
    case SDL_KEYDOWN: {
      if (m_console->consoleStatus()) {
        // Keys that still execute bindings with console open 
        if ((event.key.keysym.sym == SDLK_BACKQUOTE) ||
            (event.key.keysym.sym == SDLK_CARET) ||
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
        } else {
          m_console->vHandleInput(event.key.keysym.sym, event.key.keysym.unicode);
        }
      } else {
        runCommand(Bindings::getBindingForKeysym(event.key.keysym));
      }
      break;
    }
    case SDL_KEYUP: {
      if (!m_console->consoleStatus()) {
        char *binding = (char *) Bindings::getBindingForKeysym(event.key.keysym).c_str();
        if (binding[0] == '+') {
          runCommand("-" + std::string(++binding));
        }
      }
      break;
    }
    
    case SDL_JOYAXISMOTION: {
      handleJoystickMotion(event.jaxis.axis, event.jaxis.value);
      break;
    }
    
    case SDL_JOYBUTTONDOWN: {
      if (event.jbutton.button == DEFAULT_joystick_touch_button) {
        renderer->procEvent(m_width / 2, m_height / 2);
        m_character->touchEntity(renderer->getActiveID());
      }
      if (event.jbutton.button == DEFAULT_joystick_pickup_button) {
        renderer->procEvent(m_width / 2, m_height / 2);
	m_character->getEntity(renderer->getActiveID());
      }
      break;
    }
    case SDL_VIDEORESIZE: {
      RenderSystem::getInstance().resize(event.resize.w, event.resize.h);
      m_workarea->resize(event.resize.w, event.resize.h);
      break;
    }
    case SDL_QUIT: {
      m_system_running = false;
      break;
    }
  }
}

void System::handleAnalogueControllers() {
  if (m_mouseLook) {
    // We should still be ok if the user wants to drag something
    int mx = m_width / 2,
        my = m_height / 2;
    int dx, dy;
    SDL_GetMouseState(&dx, &dy);
    dx -= mx;
    dy -= my;
    if (dx != 0) {
      float rotation = dx / 4.f;
      m_character->rotateImmediate(rotation);
    }
    if (dy != 0) {
      float elevation = -dy / 4.f;
      CameraSystem * g = RenderSystem::getInstance().getCameraSystem();
      Camera * c = NULL;
      if (g != NULL) {
        c = g->getCurrentCamera();
        if (c != NULL) {
          c->elevateImmediate(elevation);
        }
      }
    }
    
    if ((dx != 0) || (dy != 0)) {
      SDL_WarpMouse(mx, my);
    }
  }
  
/*  if (m_controller) {
    for (AxisBindingMap::const_iterator B=m_axisBindings.begin(); B != m_axisBindings.end(); ++B) {
        Sint16 av = SDL_JoystickGetAxis(m_controller, B->second);
        handleJoystickMotion(B->second, av);
    }
  }
    
    std::cout << "Upd "
              << SDL_JoystickGetAxis(m_controller, 0) << ":"
              << SDL_JoystickGetAxis(m_controller, 1) << ":"
              << SDL_JoystickGetAxis(m_controller, 2) << ":"
              << SDL_JoystickGetAxis(m_controller, 3) << ":"
              << SDL_JoystickGetAxis(m_controller, 4) << ":"
              << SDL_JoystickGetAxis(m_controller, 5) << ":"
              << SDL_JoystickGetAxis(m_controller, 6) << ":"
              << std::endl << std::flush;
  } */
}

void System::handleJoystickMotion(Uint8 axis, Sint16 value)
{
  assert (m_character != NULL);
  if (!m_axisBindings.count(axis)) return;
    
    std::cout << "got joy motion for axis " << (int)axis << ", value=" << value << std::endl;
    
    switch (m_axisBindings[axis]) {
    case AXIS_STRAFE: // Left right move
        if (abs(value) > 3200) {
          m_character->setStrafeSpeed(value / 10000.f);
        } else {
            std::cout << "X too small" << std::endl << std::flush;
          m_character->setStrafeSpeed(0);
        }
        break;
          
    case AXIS_MOVE: // For back move
        if (abs(value) > 3200) {
          m_character->setMovementSpeed(value / -10000.f);
        } else {
            std::cout << "Y too small" << std::endl << std::flush;
          m_character->setMovementSpeed(0);
        }
      break;
          
    case AXIS_PAN: // Left right view
        if (abs(value) > 3200) {
          m_character->setRotationRate(value / 10000.f);
        } else {
            std::cout << "X too small" << std::endl << std::flush;
          m_character->setRotationRate(0);
        }
        break;
          
    case AXIS_ELEVATE: // Up down view
          CameraSystem * g = RenderSystem::getInstance().getCameraSystem();
          if (g != NULL) {
            Camera * c = g->getCurrentCamera();
            if (c != NULL) {
              if (abs(value) > 3200) {
                c->setElevationSpeed(value / 10000.f);
              } else {
                std::cout << "Y too small" << std::endl << std::flush;
                c->setElevationSpeed(0);
              }
            }
          }
          break;
    } // of axis switch
}

void System::setCaption(const std::string &title, const std::string &icon) {
  // Set window and icon title
  SDL_WM_SetCaption(title.c_str(), icon.c_str());
}

void System::toggleMouselook() {
  if (debug) std::cout << "System::toggleMouselook()" << std::endl << std::flush;
  m_mouseLook = ! m_mouseLook;
  if (m_mouseLook) {
    RenderSystem::getInstance().setMouseVisible(false);
    SDL_WarpMouse(m_width / 2, m_height / 2);
  } else {
    RenderSystem::getInstance().setMouseVisible(true);
  }
}

void System::pushMessage(const std::string &msg, int type, int duration) {
  assert(m_console);
  m_console->pushMessage(msg, type, duration);
}

void System::readConfig(varconf::Config &config) {
  varconf::Variable temp;
  
  if (config.findItem(SYSTEM, KEY_mouse_move_select)) {
    temp = config.getItem(SYSTEM, KEY_mouse_move_select);
    m_mouse_move_select = (!temp.is_bool()) ? (DEFAULT_mouse_move_select) : ((bool)temp);
  } else {
    m_mouse_move_select = DEFAULT_mouse_move_select;
  }
  if (config.findItem(SYSTEM, KEY_window_width)) {
    temp = config.getItem(SYSTEM, KEY_window_width);
    m_width = (!temp.is_int()) ? (DEFAULT_window_width) : ((int)temp);
  } else {
    m_width = DEFAULT_window_width;
  }
  if (config.findItem(SYSTEM, KEY_window_height)) {
    temp = config.getItem(SYSTEM, KEY_window_height);
    m_height = (!temp.is_int()) ? (DEFAULT_window_height) : ((int)temp);
  } else {
    m_height = DEFAULT_window_height;
  }
  if(config.findItem(SECTION_INPUT, KEY_key_repeat_delay)) {
    temp = config.getItem(SECTION_INPUT, KEY_key_repeat_delay);
    m_KeyRepeatDelay = (temp.is_int() == true) ? ((int)(temp)) : (DEFAULT_key_repeat_delay);
  }
  if(config.findItem(SECTION_INPUT, KEY_key_repeat_rate)) {
    temp = config.getItem(SECTION_INPUT, KEY_key_repeat_rate);
    m_KeyRepeatRate = (temp.is_int() == true) ? ((int)(temp)) : (DEFAULT_key_repeat_rate);
  }
 
  if (config.findItem(SYSTEM, KEY_fullscreen)) {
    temp = config.getItem(SYSTEM, KEY_fullscreen);
    m_startFullscreen = (!temp.is_bool()) ? (DEFAULT_fullscreen) : ((bool)(temp));
  } else {
    m_startFullscreen = DEFAULT_fullscreen;
  }


  RenderSystem::getInstance().readConfig(config);
  ModelSystem::getInstance().readConfig(config);
  m_character->readConfig(config);
  m_calendar->readConfig(config);
  m_workarea->readConfig(m_general);
}

void System::writeConfig(varconf::Config &config) {
  config.setItem(SYSTEM, KEY_mouse_move_select,  m_mouse_move_select);
  config.setItem(SYSTEM, KEY_window_width, m_width);
  config.setItem(SYSTEM, KEY_window_height, m_height);
  config.setItem(SECTION_INPUT, KEY_key_repeat_delay, m_KeyRepeatDelay);
  config.setItem(SECTION_INPUT, KEY_key_repeat_rate, m_KeyRepeatRate);

  // Write Other config objects
  RenderSystem::getInstance().writeConfig(config);
  ModelSystem::getInstance().writeConfig(config);
  m_character->writeConfig(config);
  m_calendar->writeConfig(config);
  m_workarea->writeConfig(m_general);
}

void System::vEnableKeyRepeat(bool bEnable) {
  if(bEnable == true) {
    SDL_EnableKeyRepeat(m_KeyRepeatDelay, m_KeyRepeatRate);
  } else {
    SDL_EnableKeyRepeat(0, 0);
  }
}

void System::setAction(int new_action) {
  switch (new_action) {
    case (ACTION_DEFAULT): switchCursor(RenderSystem::CURSOR_DEFAULT); break;
    case (ACTION_PICKUP): switchCursor(RenderSystem::CURSOR_PICKUP); break;
    case (ACTION_TOUCH): switchCursor(RenderSystem::CURSOR_TOUCH); break;
    case (ACTION_USE): switchCursor(RenderSystem::CURSOR_USE); break;
  }
  m_action = new_action;
}

void System::switchCursor(int cursor) {
  RenderSystem::getInstance().setMouseState(cursor);
}

void System::registerCommands(Console *console) {
  console->registerCommand(QUIT, this);
  console->registerCommand(EXIT, this);
  console->registerCommand(GET_ATTRIBUTE, this);
  console->registerCommand(SET_ATTRIBUTE, this);
  console->registerCommand(LOAD_GENERAL_CONFIG, this);
  console->registerCommand(LOAD_KEY_BINDINGS, this);
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
    m_console->runCommand(command);
  } catch (Exception e) {
    Log::writeLog(e.getMessage(), Log::LOG_ERROR);
  }
}

void System::runCommand(const std::string &command, const std::string &args_t) {
  Tokeniser tokeniser;
  std::string args = args_t;
  m_file_handler->expandString(args);
  tokeniser.initTokens(args);
  if (command == EXIT || command == QUIT) m_system_running = false;
  else if (command == GET_ATTRIBUTE) {
    std::string section = tokeniser.nextToken();
    std::string key = tokeniser.remainingTokens();
     pushMessage(m_general.getItem(section, key), CONSOLE_MESSAGE);
  }
  else if (command == SET_ATTRIBUTE) {
    std::string section = tokeniser.nextToken();
    std::string key = tokeniser.nextToken();
    std::string value = tokeniser.remainingTokens();
    m_general.setItem(section, key, value);
  }
  else if (command == LOAD_GENERAL_CONFIG) {
    m_process_records = m_script_engine->prefixEnabled();
  System::instance()->getFileHandler()->expandString(args);
    m_general.readFromFile(args);
    if (m_process_records) {
      m_process_records = false;
      processRecords();
    }
  }
  else if (command == LOAD_KEY_BINDINGS) {
    Bindings::loadBindings(args);
  }
  else if (command == SAVE_GENERAL_CONFIG) {
  System::instance()->getFileHandler()->expandString(args);
    m_general.writeToFile(args);
  }
  else if (command == SAVE_KEY_BINDINGS) {
  System::instance()->getFileHandler()->expandString(args);
    Bindings::saveBindings(args);
  }
  else if (command == READ_CONFIG) {
    readConfig(m_general);
    m_character->readConfig(m_general);
  }
  else if (command == BIND_KEY) {
    std::string key = tokeniser.nextToken();
    std::string value = tokeniser.remainingTokens();
    Bindings::bind(key, value);
  }
  else if (command == KEY_PRESS) {
    runCommand(Bindings::getBinding(args));
  }
  else if (command == TOGGLE_FULLSCREEN) RenderSystem::getInstance().toggleFullscreen();
  else if (command == TOGGLE_MLOOK) toggleMouselook();
  else if (command == IDENTIFY_ENTITY) {
    if (!m_client->getAvatar()) return;
    Render *renderer = RenderSystem::getInstance().getRenderer();
    WorldEntity *we = (dynamic_cast<WorldEntity*>(m_client->getAvatar()->getView()->getEntity(renderer->getActiveID())));
    if (we) we->displayInfo();  
  }

  else if (command == "normalise_on") glEnable(GL_NORMALIZE);
  else if (command == "normalise_off") glDisable(GL_NORMALIZE);

  else if (command == "setvar") {
    std::string key = tokeniser.nextToken();
    std::string value = tokeniser.remainingTokens();
    m_file_handler->setVariable(key, value);
  }
  else if (command == "getvar") {
    std::string key = tokeniser.nextToken();
    pushMessage(m_file_handler->getVariable(key), CONSOLE_MESSAGE);
  }
  
  else Log::writeLog(std::string("Command not found: - ") + command, Log::LOG_ERROR);
}

void System::varconf_error_callback(const char *error) {
  Log::writeLog(std::string("Varconf Error: ") + error, Log::LOG_ERROR);
}

void System::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  // TODO this should be specific to the model loader config only.
  if (key == "state") {
    config.setItem(section, "state_num", RenderSystem::getInstance().requestState(config.getItem(section, key)));
  }
  if (key == "select_state") {
    config.setItem(section, "select_state_num", RenderSystem::getInstance().requestState(config.getItem(section, key)));
  }
  if (m_process_records && m_script_engine->prefixEnabled()) {
    varconf::Variable v = config.getItem(section, key);
    if (v.is_string()) {
      VarconfRecord *r = new VarconfRecord();
      r->section = section;
      r->key = key; 
      r->config = &config;
      m_record_list.push_back(r);
    }
  }
}

void System::processRecords() {
  while (!m_record_list.empty()) {
    VarconfRecord *r = *m_record_list.begin();
    std::string value = r->config->getItem(r->section, r->key);
    m_file_handler->expandString(value);
    r->config->setItem(r->section, r->key, value);
    delete r;
    m_record_list.erase(m_record_list.begin());
  }
}

void System::addSearchPaths(std::list<std::string> l) {
  for (std::list<std::string>::const_iterator I = l.begin(); I != l.end(); ++I) {        
    m_file_handler->addSearchPath(*I);
  }
}

void System::varconf_general_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  varconf::Variable temp;
  if (section == SYSTEM) {
    if (key ==  KEY_mouse_move_select) {
      temp = config.getItem(SYSTEM, KEY_mouse_move_select);
      m_mouse_move_select =  (!temp.is_bool()) ? (DEFAULT_mouse_move_select) : ((bool)temp);
    }
    else if (key == KEY_window_width) {
      temp = config.getItem(SYSTEM, KEY_window_width);
      m_width = (!temp.is_int()) ? (DEFAULT_window_width) : ((int)temp);
    }
    else if (key == KEY_window_height) {
      temp = config.getItem(SYSTEM, KEY_window_height);
      m_height = (!temp.is_int()) ? (DEFAULT_window_height) : ((int)temp);
    }
  }
}
    
} /* namespace Sear */
