// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall, University of Southampton

// $Id: System.cpp,v 1.174 2008-10-07 19:33:14 simon Exp $

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <sigc++/object_slot.h>

#include <varconf/varconf.h>
#include <Eris/Exceptions.h>
#include <Eris/DeleteLater.h>
#include <Eris/Avatar.h>
#include <Eris/View.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "guichan/Workarea.h"
#include "renderers/Camera.h"
#include "renderers/CameraSystem.h"
#include "renderers/Graphics.h"
#include "renderers/RenderSystem.h"
#include "environment/Environment.h"
#include "loaders/ModelSystem.h"

#include "ActionHandler.h"
#include "Bindings.h"
#include "Calendar.h"
#include "Character.h"
#include "client.h"
#include "Console.h"
#include "FileHandler.h"
//#include "Sound.h"
#include "ScriptEngine.h"
#include "MediaManager.h"
#include "System.h"
#include "WorldEntity.h"
#include "Editor.h"
#include "CacheManager.h"
#include "Metaserver.h"

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
  static const std::string CMD_EXIT = "exit";
  static const std::string CMD_QUIT = "quit";

  static const std::string CMD_GET_ATTRIBUTE = "getat";
  static const std::string CMD_SET_ATTRIBUTE = "setat";

  static const std::string CMD_LOAD_GENERAL_CONFIG = "load_general";
  static const std::string CMD_LOAD_KEY_BINDINGS_USER = "load_bindings";
  static const std::string CMD_LOAD_KEY_BINDINGS_GLOBAL = "load_global_bindings";
  static const std::string CMD_SAVE_GENERAL_CONFIG = "save_general";
  static const std::string CMD_SAVE_KEY_BINDINGS = "save_bindings";
  static const std::string CMD_READ_CONFIG = "read_config";
  static const std::string CMD_BIND_KEY = "bind";
  static const std::string CMD_KEY_PRESS = "keypress";
  static const std::string CMD_TOGGLE_MLOOK = "toggle_mlook";
  static const std::string CMD_ADD_EVENT = "event";
  static const std::string CMD_IDENTIFY_ENTITY = "identify";
  static const std::string CMD_DUMP_ATTRIBUTES = "dump_attributes";
  static const std::string CMD_reload_configs = "reload_configs";

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
  static const std::string KEY_delay = "render_delay";
 
  //Config default values
  static const int DEFAULT_window_width = 800;
  static const int DEFAULT_window_height = 600;  
  static const bool DEFAULT_mouse_move_select = false;
  static const double DEFAULT_max_click_time = 0.3;
  static const int DEFAULT_joystick_touch_button = 1;
  static const int DEFAULT_joystick_pickup_button = 2;
  static const int DEFAULT_key_repeat_delay = 1000;
  static const int DEFAULT_key_repeat_rate = 500;
  static const bool DEFAULT_fullscreen = false;
  static const unsigned int DEFAULT_delay = 0;
  static const bool DEFAULT_disable_joystick = true;
 
System *System::m_instance = NULL;

System::System() :
  m_action(ACTION_DEFAULT),
  m_mouseLook(0),
  m_screen(NULL),
  m_width(0),
  m_height(0),
  m_KeyRepeatDelay(DEFAULT_key_repeat_delay),
  m_KeyRepeatRate(DEFAULT_key_repeat_rate),
  m_click_on(false),
  m_click_x(0),
  m_click_y(0),
  m_controller(NULL),
  m_mouse_move_select(false),
  m_seconds(0.0),
  m_elapsed(0.0),
  m_current_ticks(0),
  m_system_running(false),
  m_initialised(false),
  m_startFullscreen(DEFAULT_fullscreen),
  m_delay(0)
{
  m_instance = this;
  // Initialise system states
  for (unsigned int i = 0; i < SYS_LAST_STATE; ++i) m_systemState[i] = false;
    
  // create the filehandler early, so we can call addSearchPath on it
  m_file_handler = std::auto_ptr<FileHandler>(new FileHandler());
}

System::~System() {
  if (m_initialised) shutdown();

  m_file_handler.release();

  // Explicity Tell sigc to disconnect signals
  notify_callbacks();
}


bool System::init(int argc, char *argv[]) {
  assert (m_initialised == false);

  if (!initVideo()) return false;

  m_script_engine = std::auto_ptr<ScriptEngine>(new ScriptEngine());
  m_script_engine->init();

  // Pass client name and version when creating a connection
  m_client = std::auto_ptr<Client>(new Client(this, PACKAGE_VERSION));
  if (!m_client->init()) {
    Log::writeLog("Error initializing Eris", Log::LOG_ERROR);

    m_client.release();
    m_script_engine.release();

    return false;
  }
  
  m_action_handler = std::auto_ptr<ActionHandler>(new ActionHandler(this));
  m_action_handler->init();

  m_calendar = std::auto_ptr<Calendar>(new Calendar());
  m_calendar->init();

  m_media_manager = std::auto_ptr<MediaManager>(new MediaManager());
  m_media_manager->init();

  m_meta_server = std::auto_ptr<Metaserver>(new Metaserver());
  m_meta_server->init();
 
  // Connect signals for record processing 
//  m_general.sigsv.connect(sigc::mem_fun(this, &System::varconf_callback));
  
  // Connect signals for error messages
  m_general.sige.connect(sigc::mem_fun(this, &System::varconf_error_callback));
  
  Bindings::init();

  Bindings::bind("backquote", "/toggle_console");
  Bindings::bind("caret", "/toggle_console");
  
  m_console = std::auto_ptr<Console>(new Console(this));
  m_console->init();
  registerCommands(m_console.get());

  m_client->registerCommands(m_console.get());
  m_script_engine->registerCommands(m_console.get());
  m_action_handler->registerCommands(m_console.get());
  m_file_handler->registerCommands(m_console.get());
  m_calendar->registerCommands(m_console.get());
  m_media_manager->registerCommands(m_console.get());
  m_meta_server->registerCommands(m_console.get());

  m_character = std::auto_ptr<Character>(new Character());
  m_character->init();
  m_character->registerCommands(m_console.get());

  m_editor = std::auto_ptr<Editor>(new Editor());
  m_editor->registerCommands(m_console.get());

  m_workarea = std::auto_ptr<Workarea>(new Workarea(this));

#if 0
  m_sound = std::auto_ptr<Sound>(new Sound());
  if (m_sound->init()) {
    m_sound.release();
  } else { 
    m_sound->registerCommands(m_console.get());
  }
#else
 // #warning Sound Disabled
#endif
  RenderSystem::getInstance().init();
  RenderSystem::getInstance().registerCommands(m_console.get());
  ModelSystem::getInstance().init();
  ModelSystem::getInstance().registerCommands(m_console.get());
//  CacheManager::getInstance().init();

  // Register StaticObject with CacheManager
//  StaticObject *so = new StaticObject();
//  CacheManager::getInstance().addType(so);
//  delete so;
  
 
  Environment::getInstance().init();
  Environment::getInstance().registerCommands(m_console.get());

  if (debug) Log::writeLog("Running startup scripts", Log::LOG_INFO);

  FileHandler::FileSet startup_scripts = m_file_handler->getAllinSearchPaths(STARTUP_SCRIPT);
  FileHandler::FileSet::const_iterator I = startup_scripts.begin();
  FileHandler::FileSet::const_iterator Iend = startup_scripts.end();
  for (; I != Iend; ++I) {
    m_script_engine->runScript(*I);
  }

  // Pass command line into general varconf object for processing
  m_general.getCmdline(argc, argv);
  readConfig(m_general);
  RenderSystem::getInstance().readConfig(m_general);
  m_general.sigsv.connect(sigc::mem_fun(this, &System::varconf_general_callback));

  // Enable Joysticks if requested
  int sticks = SDL_NumJoysticks();

  if (m_general.findItem(SECTION_INPUT, KEY_DISABLE_JOYSTICK))  {
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




  // Try and create the window
  bool success;
  if (!(success = RenderSystem::getInstance().createWindow(m_width, m_height, m_startFullscreen))) {
    // Only try again if stencil buffer was enabled first time round
    if (RenderSystem::getInstance().getState(RenderSystem::RENDER_STENCIL)) {
      printf("[System] Creating window with stencil buffer failed. Trying again without stencil buffer.\n");
      // Disable stencil buffer and try again
      RenderSystem::getInstance().setState(RenderSystem::RENDER_STENCIL, false);
      success = RenderSystem::getInstance().createWindow(m_width, m_height, m_startFullscreen);
    }
  }
  if (!success) {
    // TODO lots more cleaning up required!
    m_client.release();
    m_calendar.release();
    m_action_handler.release();
    m_script_engine.release();
    m_editor.release();
    m_console.release();
    m_workarea.release();
    m_character.release();
    m_media_manager.release();
    //m_sound.release();

    Bindings::shutdown();

    Environment::getInstance().shutdown();
    ModelSystem::getInstance().shutdown(); 
    RenderSystem::getInstance().shutdown();

    SDL_Quit();

    Eris::execDeleteLaters();

    return false;
  }

  m_workarea->init();

  m_workarea->registerCommands(m_console.get());

  m_system_running = true;
  m_initialised = true;
  return true;
}

int System::reinit() {
  // Restart these engines
  m_script_engine->shutdown();
  m_script_engine->init();
  m_action_handler->shutdown();
  m_action_handler->init();

  RenderSystem::getInstance().reinit();
  ModelSystem::getInstance().reinit();
  Environment::getInstance().reinit();

  // Re-run all the startup scripts.
  FileHandler::FileSet startup_scripts = m_file_handler->getAllinSearchPaths(STARTUP_SCRIPT);
  FileHandler::FileSet::const_iterator I = startup_scripts.begin();
  FileHandler::FileSet::const_iterator Iend = startup_scripts.end();
  for (; I != Iend; ++I) {
    m_script_engine->runScript(*I);
  }

  // Re-read configs
  readConfig(m_general);
  RenderSystem::getInstance().readConfig(m_general);

  return 0;
}

void System::shutdown() {

  assert (m_initialised == true);
  printf("[System] Starting Shutdown\n");


  // Save config
  writeConfig(m_general);

  m_client.release();
  
  m_character.release();

  m_action_handler.release();

  m_media_manager.release();

  if (debug) Log::writeLog("Running shutdown scripts", Log::LOG_INFO);
  FileHandler::FileSet shutdown_scripts = m_file_handler->getAllinSearchPaths(SHUTDOWN_SCRIPT);
  for (FileHandler::FileSet::const_iterator I = shutdown_scripts.begin(); I != shutdown_scripts.end(); ++I) {
    m_script_engine->runScript(*I);
  }

  Bindings::shutdown();

  m_script_engine.release();

  m_file_handler.release();

  m_console.release();
 
  //m_sound.release();

//  CacheManager::getInstance().shutdown();
  Environment::getInstance().shutdown();
  ModelSystem::getInstance().shutdown(); 
  RenderSystem::getInstance().destroyWindow();
  RenderSystem::getInstance().shutdown();

  m_editor.release();

  m_workarea.release();

  m_calendar.release();

  // Explicity Tell sigc to disconnect signals
  notify_callbacks();

  SDL_Quit();

  Eris::execDeleteLaters();

  m_initialised = false;
  printf("[System] Finished Shutdown\n");
}

bool System::initVideo() {
  if (debug) Log::writeLog("Initialising Video", Log::LOG_INFO);
#ifdef DEBUG
  // NOPARACHUTE means SDL doesn't handle any errors allowing us to catch them in a debugger. However, this can cause other problems!
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

      SDL_Delay(m_delay);
      // Store GetTicks so we only call it once per framee
      m_current_ticks = SDL_GetTicks();
      m_seconds = (double)m_current_ticks / 1000.0;
      m_elapsed = m_seconds - last_time;
      last_time = m_seconds;
      while (SDL_PollEvent(&event)) {
        handleEvents(event);
        // Stop processing events if we are quiting
        if (!m_system_running) break;
      }
      // Handle mouse and joystick
      handleAnalogueControllers();
      // poll network
      m_client->poll();

      m_meta_server->poll();

      m_media_manager->poll();

      if (m_client->getAvatar() && m_client->getAvatar()->getView()) {
        m_client->getAvatar()->getView()->update();
      }
      // Update Calendar
      if (checkState(SYS_IN_WORLD)) {
        m_calendar->update();
      }
      // draw scene
      RenderSystem::getInstance().drawScene(false, m_elapsed);
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

  if (!m_console->consoleStatus()) {
    try {
      if (m_workarea->handleEvent(event)) {
        return;
      }
    } catch (gcn::Exception &e) {
      fprintf(stderr, "[System] Guichan Exception: %s\n", e.getMessage().c_str());
    }
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
          if (RenderSystem::getInstance().getWorldCoords(m_click_x, m_click_y, x,y,z)) {
              m_click_pos = WFMath::Point<3>(x,y,z);
          }
          RenderSystem::getInstance().processMouseClick(event.button.x, event.button.y);
          m_click_id = RenderSystem::getInstance().getActiveEntityID();
          break;
        }
        break;
        case (SDL_BUTTON_RIGHT):   { 
          m_character->moveForward(1);
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
//                  if (debug) printf("[System] DRAG\n"); fflush(stdout);
                  m_character->getEntity(m_click_id);
              } else {
//                  if (debug) printf("[System] CLICK\n"); fflush(stdout);
                  m_character->touchEntity(m_click_id);
              }
            }
            break;
            case (ACTION_PICKUP): m_character->getEntity(m_click_id); break;
            case (ACTION_TOUCH): m_character->touchEntity(m_click_id); break;
            case (ACTION_USE): m_character->useToolOnEntity(m_click_id, m_click_pos); break;
            case (ACTION_ATTACK): m_character->attackEntity(m_click_id); break;
          }
          setAction(ACTION_DEFAULT);
          m_click_on = false;
          break;
        }
        break;
        case (SDL_BUTTON_RIGHT):   { 
            m_character->moveForward(-1);
        }
        break;
      }
      break;
    }
    case SDL_MOUSEMOTION: {
      if (m_mouse_move_select && !m_mouseLook) RenderSystem::getInstance().processMouseClick(event.button.x, event.button.y);
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
        RenderSystem::getInstance().processMouseClick(m_width / 2, m_height / 2);
        m_character->touchEntity(RenderSystem::getInstance().getActiveEntityID());
      }
      if (event.jbutton.button == DEFAULT_joystick_pickup_button) {
        RenderSystem::getInstance().processMouseClick(m_width / 2, m_height / 2);
	m_character->getEntity(RenderSystem::getInstance().getActiveEntityID());
      }
      break;
    }
    case SDL_VIDEORESIZE: {
      resizeScreen(event.resize.w, event.resize.h);
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
      m_character->rotateImmediate(-rotation);
    }
    if (dy != 0) {
      float elevation = -dy / 4.f;
      CameraSystem * g = RenderSystem::getInstance().getCameraSystem();
      Camera * c = NULL;
      if (g != NULL) {
        c = g->getCurrentCamera();
        if (c != NULL) {
          c->elevateImmediate(-elevation);
          // m_character->setPitch(c->getElevation());
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

  if (!m_axisBindings.count(axis)) return;
    
    if (debug) printf("[System] got joy motion for axis %d, value =%d\n", (int)axis, value);
    
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

void System::resizeScreen(int w, int h)
{
  RenderSystem::getInstance().resize(w, h);
  m_workarea->resize();
  m_width = w;
  m_height = h;
}

void System::toggleMouselook() {
  if (debug) printf("[System] toggleMouselook()\n"); fflush(stdout);
  m_mouseLook = ! m_mouseLook;
  if (m_mouseLook) {
    RenderSystem::getInstance().setMouseVisible(false);
    SDL_WarpMouse(m_width / 2, m_height / 2);
  } else {
    RenderSystem::getInstance().setMouseVisible(true);
  }
}

bool System::isMouselookEnabled() const {
  return m_mouseLook;
}

void System::pushMessage(const std::string &msg, int type, int duration) {
  assert(m_console.get() != 0);
  m_console->pushMessage(msg, type, duration);
  pushedMessage.emit(msg, type, duration);
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

  if (config.findItem(SYSTEM, KEY_delay)) {
    temp = config.getItem(SYSTEM, KEY_delay);
    m_delay = (!temp.is_int()) ? (DEFAULT_delay) : ((int)(temp));
  } else {
    m_delay = DEFAULT_delay;
  }

  // This parameter is read during init, so we are just making sure
  // a value is set
  if (config.findItem(SECTION_INPUT, KEY_DISABLE_JOYSTICK)) {
    temp = config.getItem(SECTION_INPUT, KEY_DISABLE_JOYSTICK);
    if (!temp.is_bool()) {
      config.setItem(SECTION_INPUT, KEY_DISABLE_JOYSTICK, DEFAULT_disable_joystick);
    }
  } else {
    config.setItem(SECTION_INPUT, KEY_DISABLE_JOYSTICK, DEFAULT_disable_joystick);
  }

  m_media_manager->readConfig(m_general);
  m_client->readConfig(config);
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
  config.setItem(SYSTEM, KEY_delay, (int)m_delay);

  // Write Other config objects
  m_client->writeConfig(config);
  RenderSystem::getInstance().writeConfig(config);
  ModelSystem::getInstance().writeConfig(config);
  m_character->writeConfig(config);
  m_calendar->writeConfig(config);
  m_workarea->writeConfig(m_general);
  m_media_manager->writeConfig(m_general);
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
    case (ACTION_ATTACK): switchCursor(RenderSystem::CURSOR_ATTACK); break;
  }
  m_action = new_action;
}

void System::switchCursor(int cursor) {
  RenderSystem::getInstance().setMouseState(cursor);
}

void System::registerCommands(Console *console) {
  console->registerCommand(CMD_QUIT, this);
  console->registerCommand(CMD_EXIT, this);
  console->registerCommand(CMD_GET_ATTRIBUTE, this);
  console->registerCommand(CMD_SET_ATTRIBUTE, this);
  console->registerCommand(CMD_LOAD_GENERAL_CONFIG, this);
  console->registerCommand(CMD_LOAD_KEY_BINDINGS_USER, this);
  console->registerCommand(CMD_LOAD_KEY_BINDINGS_GLOBAL, this);
  console->registerCommand(CMD_SAVE_GENERAL_CONFIG, this);
  console->registerCommand(CMD_SAVE_KEY_BINDINGS, this);
  console->registerCommand(CMD_READ_CONFIG, this);
  console->registerCommand(CMD_BIND_KEY, this);
  console->registerCommand(CMD_KEY_PRESS, this);
  console->registerCommand(CMD_TOGGLE_MLOOK, this);
  console->registerCommand(CMD_ADD_EVENT, this);
  console->registerCommand(CMD_IDENTIFY_ENTITY, this);
  console->registerCommand(CMD_DUMP_ATTRIBUTES, this);
  console->registerCommand(CMD_reload_configs, this);

  console->registerCommand("reinit", this);
}

void System::runCommand(const std::string &command) {
//  if (debug) Log::writeLog(command, Log::LOG_INFO);
  m_console->runCommand(command);
}

void System::runCommand(const std::string &command, const std::string &args_t) {
  Tokeniser tokeniser;
  std::string args = args_t;
  m_file_handler->expandString(args);
  tokeniser.initTokens(args);
  if (command == CMD_EXIT || command == CMD_QUIT) m_system_running = false;
  else if (command == CMD_GET_ATTRIBUTE) {
    std::string section = tokeniser.nextToken();
    std::string key = tokeniser.remainingTokens();
     pushMessage(m_general.getItem(section, key), CONSOLE_MESSAGE);
  }
  else if (command == CMD_SET_ATTRIBUTE) {
    std::string section = tokeniser.nextToken();
    std::string key = tokeniser.nextToken();
    std::string value = tokeniser.remainingTokens();
    m_general.setItem(section, key, value);
  }
  else if (command == CMD_LOAD_GENERAL_CONFIG) {
    System::instance()->getFileHandler()->expandString(args);
    m_general.readFromFile(args);
  }
  else if (command == CMD_LOAD_KEY_BINDINGS_USER) {
    System::instance()->getFileHandler()->expandString(args);
    Bindings::loadBindings(args, true);
  }
  else if (command == CMD_LOAD_KEY_BINDINGS_GLOBAL) {
    System::instance()->getFileHandler()->expandString(args);
    Bindings::loadBindings(args, false);
  }
  else if (command == CMD_SAVE_GENERAL_CONFIG) {
    System::instance()->getFileHandler()->expandString(args);
    m_general.writeToFile(args);
  }
  else if (command == CMD_SAVE_KEY_BINDINGS) {
    System::instance()->getFileHandler()->expandString(args);
    Bindings::saveBindings(args);
  }
  else if (command == CMD_READ_CONFIG) {
    readConfig(m_general);
    m_character->readConfig(m_general);
  }
  else if (command == CMD_BIND_KEY) {
    std::string key = tokeniser.nextToken();
    std::string value = tokeniser.remainingTokens();
    Bindings::bind(key, value);
  }
  else if (command == CMD_KEY_PRESS) {
    runCommand(Bindings::getBinding(args));
  }
  else if (command == CMD_TOGGLE_MLOOK) toggleMouselook();
  else if (command == CMD_IDENTIFY_ENTITY) {
    if (!m_client->getAvatar()) return;
    WorldEntity *we = RenderSystem::getInstance().getActiveEntity();
    if (we) we->displayInfo();  
  }
   else if (command == CMD_DUMP_ATTRIBUTES) {
    if (!m_client->getAvatar()) return;
    WorldEntity *we = RenderSystem::getInstance().getActiveEntity();
    if (we) we->dumpAttributes();  
  }
  else if (command == CMD_reload_configs) {
    runCommand("/reload_config_textures");
    runCommand("/reload_config_sprites");
    runCommand("/reload_config_objects");
    runCommand("/reload_config_models");
    runCommand("/reload_config_states");
  } 
  else if (command == "reinit") reinit();
  else fprintf(stderr, "[System] Command not found: %s\n", command.c_str());
}

void System::varconf_error_callback(const char *error) {
  fprintf(stderr, "[System] Varconf: %s\n", error);
}

void System::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  // Nothing to do.
}

void System::addSearchPaths(std::list<std::string> l) {
  std::list<std::string>::const_iterator I = l.begin();
  std::list<std::string>::const_iterator Iend = l.end();
  for (; I != Iend; ++I) {        
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

void System::setState(SystemState ss, bool state) {
  m_systemState[ss] = state;
  if (ss == SYS_IN_WORLD) {
    if (state) EnteredWorld.emit();
    else LeftWorld.emit();
  }
}
    
} /* namespace Sear */
